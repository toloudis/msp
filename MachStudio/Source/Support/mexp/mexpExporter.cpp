/****************************************************************************\
**	mexpExporter.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mexp/mexpExporter.hpp"

#include "Support/mexp/mexpChannelRecorder.hpp"

#include "Tool/cam3d/cam3dUtil.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Core/it/itString.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maRotation.hpp"
#include "Core/Ma/maTime.hpp"


#include <sstream>
//============================================================================
//============================================================================
using namespace std;


//============================================================================
//============================================================================
namespace
{
	const float c_FramesPerSecond = g3dConstants::c_fDefaultFrameRate;

	std::string convert_filename_to_name(const itString& i_Filename,
		std::map<itString, int> &io_InstanceCount)
	{
		itString no_ext(i_Filename);
		no_ext.StripExtension();

		const itString::CharType *str_ptr = no_ext.GetString();
		std::string ReturnString;
		for (int i = 0; i < no_ext.GetLength(); ++i)
		{
			// Can't have hyphens in Maya names, so we convert them 
			// to double-underscores
			if (str_ptr[i] == itString::CharType('-'))
				ReturnString += "__";
			else
				ReturnString.push_back((char)str_ptr[i]);
		}

		std::map<itString, int>::iterator it = io_InstanceCount.find(i_Filename);
		if (it != io_InstanceCount.end())
		{
			// Increment count
			it->second++;

			// Add _MS# extention to identify multiple instances with same asset name
			//char buffer[128];
			//::sprintf(buffer, "_MS%d", it->second);

			std::ostringstream oss;
			oss.setf(0, std::ios::floatfield);
			oss.setf(std::ios::fixed, std::ios::floatfield);
			oss << "_MS"<<it->second;
			std::string buffer(oss.str());
			ReturnString += buffer.c_str();
		}
		else
		{
			io_InstanceCount[i_Filename] = 1;
		}

		return ReturnString;
	}

	std::string convert_to_safe_name(const std::string& i_Name)
	{
		std::string ReturnString;
		for (int i = 0; i < i_Name.size(); ++i)
		{
			// Can't have hyphens in Maya names, so we convert them 
			// to double-underscores
			if (i_Name[i] == '-')
				ReturnString += "__";
			else
				ReturnString.push_back(i_Name[i]);
		}

		return ReturnString;
	}

	void export_translation_animation(std::ofstream &i_OutFile,
									  const std::string &i_Name,
									  const std::map<float, maPoint3d> &i_Keys)
	{
		const int num_keys = i_Keys.size();
		std::map<float, maPoint3d>::const_iterator it, end = i_Keys.end();

		// X translation
		i_OutFile << "createNode animCurveTL -n \"" << i_Name << "_translateX\";" << endl;
		i_OutFile << "\tsetAttr \".tan\" 2;" << endl;
		i_OutFile << "\tsetAttr \".wgt\" no;" << endl;
		i_OutFile << "\tsetAttr -s " << num_keys << " \".ktv[0:" << num_keys-1 << "]\"  ";
		for (it = i_Keys.begin(); it != end; ++it)
		{
			i_OutFile << it->first*c_FramesPerSecond << " " << it->second.m_X << " ";

		}
		i_OutFile << ";" << endl;

		// Y translation
		i_OutFile << "createNode animCurveTL -n \"" << i_Name << "_translateY\";" << endl;
		i_OutFile << "\tsetAttr \".tan\" 2;" << endl;
		i_OutFile << "\tsetAttr \".wgt\" no;" << endl;
		i_OutFile << "\tsetAttr -s " << num_keys << " \".ktv[0:" << num_keys-1 << "]\"  ";
		for (it = i_Keys.begin(); it != end; ++it)
		{
			i_OutFile << it->first*c_FramesPerSecond << " " << it->second.m_Y << " ";

		}
		i_OutFile << ";" << endl;

		// Z translation
		i_OutFile << "createNode animCurveTL -n \"" << i_Name << "_translateZ\";" << endl;
		i_OutFile << "\tsetAttr \".tan\" 2;" << endl;
		i_OutFile << "\tsetAttr \".wgt\" no;" << endl;
		i_OutFile << "\tsetAttr -s " << num_keys << " \".ktv[0:" << num_keys-1 << "]\"  ";
		for (it = i_Keys.begin(); it != end; ++it)
		{
			i_OutFile << it->first*c_FramesPerSecond << " " << it->second.m_Z << " ";

		}
		i_OutFile << ";" << endl;

		// add connections to drive translation from anim curve
		i_OutFile << "connectAttr \"" << i_Name << "_translateX.o\" \"" << i_Name << ".tx\";" << endl;
		i_OutFile << "connectAttr \"" << i_Name << "_translateY.o\" \"" << i_Name << ".ty\";" << endl;
		i_OutFile << "connectAttr \"" << i_Name << "_translateZ.o\" \"" << i_Name << ".tz\";" << endl;

	}

	// export lines to set translation and orientation of a transform node
	void export_transformation(std::ofstream &i_OutFile,
							   const maPoint3d& i_Position,
							   const maRotation& i_Orientation)
	{
		// output position
		i_OutFile << "\tsetAttr \".t\" -type \"double3\" ";
		i_OutFile << i_Position.m_X << " " << i_Position.m_Y << " ";
		i_OutFile << i_Position.m_Z << " " << ";" << endl;
		// output orientation as euler angles
		float angx = 0, angy = 0, angz = 0;
		i_Orientation.GetEuler(angx, angy, angz);
		i_OutFile << "\tsetAttr \".r\" -type \"double3\" ";
		i_OutFile << angx * maConstants::c_fRadToAngle << " ";
		i_OutFile << angy * maConstants::c_fRadToAngle << " ";
		i_OutFile << angz * maConstants::c_fRadToAngle << " " << ";" << endl;
	}

} // end of anonymous namespace

//--------------------------------------------------------------------
//	Constructor takes locator. Creates ascii file
//--------------------------------------------------------------------
mexpExporter::mexpExporter( const char* i_Filename,
						    bool i_bExportAnimation,
							bool i_bExportJoints)
: m_OutFile(i_Filename),
	m_bExportAnimation(i_bExportAnimation),
	m_bExportJoints(i_bExportJoints)
{
	m_OutFile << "//Maya ASCII 6.5 scene" << endl;
	m_OutFile << "requires maya \"6.5\";" << endl;
}
	
//--------------------------------------------------------------------
//--------------------------------------------------------------------
mexpExporter::~mexpExporter()
{
	envSTLHelpers::DeleteContainer(m_Recorders);
}

//--------------------------------------------------------------------
// Test output file to see if it opened correctly. 
//	Returns true if error
//--------------------------------------------------------------------
bool mexpExporter::OpenFailed()
{
	return (!m_OutFile);
}

//--------------------------------------------------------------------
// IsExportAnimation - if true, export interests should export 
//	animation channels. If false, just export the current position
//	of the object.
//--------------------------------------------------------------------
bool mexpExporter::IsExportAnimation()
{
	return m_bExportAnimation;
}

//--------------------------------------------------------------------
// IsExportJoints - if true, export hierarchy of joints/transforms
//	within object. If false, just export the MachStudio application
//	transformation.
//--------------------------------------------------------------------
bool mexpExporter::IsExportJoints()
{
	return m_bExportJoints;
}

//--------------------------------------------------------------------
//	Exports scene's time range.
//--------------------------------------------------------------------
void mexpExporter::ExportTimeRange(const maTime& i_Minimum, const maTime& i_Maximum)
{
	m_OutFile << "createNode script -n \"sceneConfigurationScriptNode\";" << endl;
	m_OutFile << "\tsetAttr \".b\" -type \"string\" \"playbackOptions";
	m_OutFile << " -min " << i_Minimum.AsFrame((int)c_FramesPerSecond) << " -max " << i_Maximum.AsFrame((int)c_FramesPerSecond);
	m_OutFile << " -ast " << i_Minimum.AsFrame((int)c_FramesPerSecond) << " -aet " << i_Maximum.AsFrame((int)c_FramesPerSecond);
	m_OutFile << "\";" << endl;
	m_OutFile << "\tsetAttr \".st\" 6;" << endl;
}

//--------------------------------------------------------------------
// Exports a locator in the position of a geometry asset.
// In Maya, the name of the locator will be used to position 
// the correct geometry in the correct position.
//--------------------------------------------------------------------
std::string mexpExporter::ExportGeometryLocation(const itString& i_Filename,
										  const maPoint3d& i_Position,
										  const maRotation& i_Orientation)
{
	// get asset name from filename without extension
	std::string asset_name = convert_filename_to_name(i_Filename, m_InstanceCount);

	m_OutFile << "createNode transform -n \"" << asset_name << "\";" << endl;
	export_transformation(m_OutFile, i_Position, i_Orientation);
	m_OutFile << "createNode locator -n \"" << asset_name << "Shape\" -p \"" << asset_name << "\";" << endl;

	return asset_name;
}

//--------------------------------------------------------------------
// Exports a locator in the position of a scene graph node
//--------------------------------------------------------------------
std::string mexpExporter::ExportNodeLocation(const std::string& i_NodeName,
											 const std::string& i_ParentName,
											 const maPoint3d& i_Position,
											 const maRotation& i_Orientation)
{
	// get maya safe name from node name
	std::string maya_name = convert_to_safe_name(i_NodeName);

	m_OutFile << "createNode transform -n \"" << maya_name << "\" -p \"" << i_ParentName << "\";" << endl;
	export_transformation(m_OutFile, i_Position, i_Orientation);
	m_OutFile << "createNode locator -n \"" << maya_name << "Joint\" -p \"" << maya_name << "\";" << endl;

	return maya_name;
}

//--------------------------------------------------------------------
// Export translation animation channels
//--------------------------------------------------------------------
void mexpExporter::ExportAnimationTranslation(const std::string& i_AssetName,
											  const std::map<float, maPoint3d> &i_Keys)
{
	export_translation_animation(m_OutFile, i_AssetName, i_Keys);}

//--------------------------------------------------------------------
// Export rotation animation channels
//--------------------------------------------------------------------
void mexpExporter::ExportAnimationRotation(const std::string& i_AssetName,
										   const std::map<float, maRotation> &i_Keys)
{
	const int num_keys = i_Keys.size();
	std::map<float, maRotation>::const_iterator it, end = i_Keys.end();

	// X rotation
	float angX = 0, angY = 0, angZ = 0;
	m_OutFile << "createNode animCurveTA -n \"" << i_AssetName << "_rotateX\";" << endl;
	m_OutFile << "\tsetAttr \".tan\" 2;" << endl;
	m_OutFile << "\tsetAttr \".wgt\" no;" << endl;
	m_OutFile << "\tsetAttr -s " << num_keys << " \".ktv[0:" << num_keys-1 << "]\"  ";
	for (it = i_Keys.begin(); it != end; ++it)
	{
		it->second.GetEuler(angX, angY, angZ);
		m_OutFile << it->first*c_FramesPerSecond << " " << angX*maConstants::c_fRadToAngle << " ";
	}
	m_OutFile << ";" << endl;

	// Y rotation
	m_OutFile << "createNode animCurveTA -n \"" << i_AssetName << "_rotateY\";" << endl;
	m_OutFile << "\tsetAttr \".tan\" 2;" << endl;
	m_OutFile << "\tsetAttr \".wgt\" no;" << endl;
	m_OutFile << "\tsetAttr -s " << num_keys << " \".ktv[0:" << num_keys-1 << "]\"  ";
	for (it = i_Keys.begin(); it != end; ++it)
	{
		it->second.GetEuler(angX, angY, angZ);
		m_OutFile << it->first*c_FramesPerSecond << " " << angY*maConstants::c_fRadToAngle << " ";
	}
	m_OutFile << ";" << endl;

	// Z rotation
	m_OutFile << "createNode animCurveTA -n \"" << i_AssetName << "_rotateZ\";" << endl;
	m_OutFile << "\tsetAttr \".tan\" 2;" << endl;
	m_OutFile << "\tsetAttr \".wgt\" no;" << endl;
	m_OutFile << "\tsetAttr -s " << num_keys << " \".ktv[0:" << num_keys-1 << "]\"  ";
	for (it = i_Keys.begin(); it != end; ++it)
	{
		it->second.GetEuler(angX, angY, angZ);
		m_OutFile << it->first*c_FramesPerSecond << " " << angZ*maConstants::c_fRadToAngle << " ";
	}
	m_OutFile << ";" << endl;

	// add connections to drive translation from anim curve
	m_OutFile << "connectAttr \"" << i_AssetName << "_rotateX.o\" \"" << i_AssetName << ".rx\";" << endl;
	m_OutFile << "connectAttr \"" << i_AssetName << "_rotateY.o\" \"" << i_AssetName << ".ry\";" << endl;
	m_OutFile << "connectAttr \"" << i_AssetName << "_rotateZ.o\" \"" << i_AssetName << ".rz\";" << endl;

}

//--------------------------------------------------------------------
// Exports a camera defined by position and target
//--------------------------------------------------------------------
std::string mexpExporter::ExportCamera(const std::string& i_Name,
								const maPoint3d& i_Position,
								const maPoint3d& i_Target,
								float i_FieldOfView)
{
	// get Maya safe name for camera
	std::string cam_name = convert_to_safe_name(i_Name);

	m_OutFile << "createNode lookAt -n \"" << cam_name << "_group\";" << endl;
	m_OutFile << "\tsetAttr \".a\" -type \"double3\" 0 0 -1 ;" << endl;
	m_OutFile << "createNode transform -n \"" << cam_name << "\" -p \"" << cam_name << "_group\";" << endl;
	// output camera position
	m_OutFile << "\tsetAttr \".t\" -type \"double3\" ";
	m_OutFile << i_Position.m_X << " " << i_Position.m_Y << " ";
	m_OutFile << i_Position.m_Z << " " << ";" << endl;

	m_OutFile << "createNode camera -n \"" << cam_name << "_shape\" -p \"" << cam_name << "\";" << endl;
	m_OutFile << "\tsetAttr -k off \".v\";" << endl;
	m_OutFile << "\tsetAttr \".rnd\" no;" << endl;
	m_OutFile << "\tsetAttr \".cap\" -type \"double2\" 1.41732 0.94488000000000005 ;" << endl;
	m_OutFile << "\tsetAttr \".ff\" 0;" << endl;
	m_OutFile << "\tsetAttr \".ncp\" 0.01;" << endl;
	
	// convert field of view into focal length based on 35mm film
	float focal_length = cam3dUtil::CalculateFocalLength(i_FieldOfView);
	m_OutFile << "\tsetAttr \".fl\" " << focal_length << ";" << endl;

	m_OutFile << "createNode transform -n \"" << cam_name << "_aim\" -p \"" << cam_name << "_group\";" << endl;
	m_OutFile << "\tsetAttr \".drp\" yes;" << endl;	
	// output camera target
	m_OutFile << "\tsetAttr \".t\" -type \"double3\" ";
	m_OutFile << i_Target.m_X << " " << i_Target.m_Y << " ";
	m_OutFile << i_Target.m_Z << " " << ";" << endl;

	m_OutFile << "createNode locator -n \"" << cam_name << "_aimShape\" -p \"" << cam_name << "_aim\";" << endl;
	m_OutFile << "\tsetAttr -k off \".v\" no;" << endl;
	m_OutFile << "connectAttr \"" << cam_name << "_aim.tx\" \"" << cam_name << "_group.tg[0].ttx\";" << endl;
	m_OutFile << "connectAttr \"" << cam_name << "_aim.ty\" \"" << cam_name << "_group.tg[0].tty\";" << endl;
	m_OutFile << "connectAttr \"" << cam_name << "_aim.tz\" \"" << cam_name << "_group.tg[0].ttz\";" << endl;
	m_OutFile << "connectAttr \"" << cam_name << "_aim.rp\" \"" << cam_name << "_group.tg[0].trp\";" << endl;
	m_OutFile << "connectAttr \"" << cam_name << "_aim.rpt\" \"" << cam_name << "_group.tg[0].trt\";" << endl;
	m_OutFile << "connectAttr \"" << cam_name << "_aim.pm\" \"" << cam_name << "_group.tg[0].tpm\";" << endl;
	m_OutFile << "connectAttr \"" << cam_name << ".pim\" \"" << cam_name << "_group.cpim\";" << endl;
	m_OutFile << "connectAttr \"" << cam_name << ".t\" \"" << cam_name << "_group.ct\";" << endl;
	m_OutFile << "connectAttr \"" << cam_name << ".rp\" \"" << cam_name << "_group.crp\";" << endl;
	m_OutFile << "connectAttr \"" << cam_name << ".rpt\" \"" << cam_name << "_group.crt\";" << endl;
	m_OutFile << "connectAttr \"" << cam_name << "_group.crx\" \"" << cam_name << ".rx\";" << endl;
	m_OutFile << "connectAttr \"" << cam_name << "_group.cry\" \"" << cam_name << ".ry\";" << endl;
	m_OutFile << "connectAttr \"" << cam_name << "_group.crz\" \"" << cam_name << ".rz\";" << endl;
	m_OutFile << "connectAttr \"" << cam_name << "_group.db\" \"" << cam_name << "_shape.coi\";" << endl;

	return cam_name;
}

//--------------------------------------------------------------------
// Export translation animation channels for camera's position
//--------------------------------------------------------------------
void mexpExporter::ExportCameraAnimationPosition(const std::string& i_Name,
									const std::map<float, maPoint3d> &i_Keys)
{
	export_translation_animation(m_OutFile, i_Name, i_Keys);
}

//--------------------------------------------------------------------
// Export translation animation channels for camera's target
//--------------------------------------------------------------------
void mexpExporter::ExportCameraAnimationTarget(const std::string& i_Name,
									const std::map<float, maPoint3d> &i_Keys)
{
	// add "_aim" to get transform for target
	export_translation_animation(m_OutFile, i_Name + "_aim", i_Keys);
}

//--------------------------------------------------------------------
// Export field of view animation channels into Maya's
// focal length channel
//--------------------------------------------------------------------
void mexpExporter::ExportCameraAnimationFOV(const std::string& i_Name,
								const std::map<float, float> &i_Keys)
{

	const int num_keys = i_Keys.size();
	std::map<float, float>::const_iterator it, end = i_Keys.end();

	// X rotation
	float angX = 0, angY = 0, angZ = 0;
	m_OutFile << "createNode animCurveTU -n \"" << i_Name << "_focalLength\";" << endl;
	m_OutFile << "\tsetAttr \".tan\" 2;" << endl;
	m_OutFile << "\tsetAttr \".wgt\" no;" << endl;
	m_OutFile << "\tsetAttr -s " << num_keys << " \".ktv[0:" << num_keys-1 << "]\"  ";
	for (it = i_Keys.begin(); it != end; ++it)
	{
		// Convert from field of view to focal length based on 35mm
		float focal_length = cam3dUtil::CalculateFocalLength(it->second);
		m_OutFile << it->first*c_FramesPerSecond << " " << focal_length << " ";
	}
	m_OutFile << ";" << endl;

	// add connection to drive focal length from anim curve
	m_OutFile << "connectAttr \"" << i_Name << "_focalLength.o\" \"" << i_Name << "_shape.fl\";" << endl;

}

//--------------------------------------------------------------------
// Add channel for recording animation into keys for export.
// Note: Ownership for the recorder passes to this exporter, it
// will be deleted when the exporter is finished.
//--------------------------------------------------------------------
void mexpExporter::AddChannelRecorder(mexpChannelRecorder *i_pRecorder)
{
	m_Recorders.push_back(i_pRecorder);
}

//--------------------------------------------------------------------
// If there are channel recorders, then it is necessary to run
// a simulation and bake the drivers into keys for exporting.
//--------------------------------------------------------------------
bool mexpExporter::HasChannelRecorders()
{
	return (!m_Recorders.empty());
}

//--------------------------------------------------------------------
//	Call RecordFrame on all recorders
//--------------------------------------------------------------------
void mexpExporter::RecordFrame(float i_CurrentTime)
{
	std::for_each(m_Recorders.begin(), m_Recorders.end(), 
		std::bind2nd(std::mem_fun(&mexpChannelRecorder::RecordFrame),i_CurrentTime));
}

//--------------------------------------------------------------------
//	Call FinishRecording on all recorders
//--------------------------------------------------------------------
void mexpExporter::FinishRecording()
{
	std::for_each(m_Recorders.begin(), m_Recorders.end(), 
		std::mem_fun(&mexpChannelRecorder::FinishRecording));
}

//--------------------------------------------------------------------
//	Call Export on all recorders
//--------------------------------------------------------------------
void mexpExporter::Export()
{
	//std::for_each(m_Recorders.begin(), m_Recorders.end(), 
	//	std::bind2nd(std::mem_fun(&mexpChannelRecorder::Export), (*this)));

	std::vector<mexpChannelRecorder*>::iterator it, end = m_Recorders.end();
	for (it = m_Recorders.begin(); it != end; ++it)
	{
		(*it)->Export(*this);
	}

}

