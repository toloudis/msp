/****************************************************************************\
**	mexpExporter.hpp
**
**		mexpExporter provides an API for writing to Maya ascii files.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef MEXP_EXPORTER_HPP
#error mexpExporter.hpp multiply included
#endif
#define MEXP_EXPORTER_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <map>
#include <vector>
#include <fstream>

//============================================================================
//	Forward References
//============================================================================
class fsLocator;
class itString;
class maRotation;
class maTime;
class mexpChannelRecorder;

//============================================================================
//============================================================================
class mexpExporter
{
public:
	//--------------------------------------------------------------------
	//	Constructor takes filename. Creates ascii file.
	//	Flags configure what data the export interests should export.
	//--------------------------------------------------------------------
	mexpExporter( const char* i_Filename,
				  bool i_bExportAnimation = true,
				  bool i_bExportJoints = true );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~mexpExporter();

	//--------------------------------------------------------------------
	// Test output file to see if it opened correctly. 
	//	Returns true if error
	//--------------------------------------------------------------------
	bool OpenFailed();

	//--------------------------------------------------------------------
	// IsExportAnimation - if true, export interests should export 
	//	animation channels. If false, just export the current position
	//	of the object.
	//--------------------------------------------------------------------
	bool IsExportAnimation();

	//--------------------------------------------------------------------
	// IsExportJoints - if true, export hierarchy of joints/transforms
	//	within object. If false, just export the MachStudio application
	//	transformation.
	//--------------------------------------------------------------------
	bool IsExportJoints();

	//--------------------------------------------------------------------
	//	Exports scene's time range.
	//--------------------------------------------------------------------
	void ExportTimeRange(const maTime& i_Minimum, const maTime& i_Maximum);

	//--------------------------------------------------------------------
	// Exports a locator in the position of a geometry asset.
	// In Maya, the name of the locator will be used to position 
	// the correct geometry in the correct position.
	//
	// Returns assigned asset name for exporting purposes, pass this
	// name to the functions for animation export.
	//--------------------------------------------------------------------
	std::string ExportGeometryLocation(const itString& i_Filename,
								const maPoint3d& i_Position,
								const maRotation& i_Orientation);

	//--------------------------------------------------------------------
	// Exports a locator in the position of a scene graph node
	//--------------------------------------------------------------------
	std::string ExportNodeLocation(const std::string& i_NodeName,
								   const std::string& i_ParentName,
								   const maPoint3d& i_Position,
								   const maRotation& i_Orientation);

	//--------------------------------------------------------------------
	// Export translation animation channels
	//--------------------------------------------------------------------
	void ExportAnimationTranslation(const std::string& i_AssetName,
									const std::map<float, maPoint3d> &i_Keys);

	//--------------------------------------------------------------------
	// Export rotation animation channels
	//--------------------------------------------------------------------
	void ExportAnimationRotation(const std::string& i_AssetName,
								 const std::map<float, maRotation> &i_Keys);


	//--------------------------------------------------------------------
	// Exports a camera defined by position and target
	//
	// Returns assigned asset name for exporting purposes, pass this
	// name to the functions for animation export.
	//--------------------------------------------------------------------
	std::string ExportCamera(const std::string& i_Name,
					  const maPoint3d& i_Position,
					  const maPoint3d& i_Target,
					  float i_FieldOfView);

	//--------------------------------------------------------------------
	// Export translation animation channels for camera's position
	//--------------------------------------------------------------------
	void ExportCameraAnimationPosition(const std::string& i_Name,
									   const std::map<float, maPoint3d> &i_Keys);

	//--------------------------------------------------------------------
	// Export translation animation channels for camera's target
	//--------------------------------------------------------------------
	void ExportCameraAnimationTarget(const std::string& i_Name,
									 const std::map<float, maPoint3d> &i_Keys);

	//--------------------------------------------------------------------
	// Export field of view animation channels into Maya's
	// focal length channel
	//--------------------------------------------------------------------
	void ExportCameraAnimationFOV(const std::string& i_Name,
								  const std::map<float, float> &i_Keys);

	//--------------------------------------------------------------------
	// Add channel for recording animation into keys for export.
	// Note: Ownership for the recorder passes to this exporter, it
	// will be deleted when the exporter is finished.
	//--------------------------------------------------------------------
	void AddChannelRecorder(mexpChannelRecorder *i_pRecorder);

	//--------------------------------------------------------------------
	// If there are channel recorders, then it is necessary to run
	// a simulation and bake the drivers into keys for exporting.
	//--------------------------------------------------------------------
	bool HasChannelRecorders();

	//--------------------------------------------------------------------
	//	Call RecordFrame on all recorders
	//--------------------------------------------------------------------
	void RecordFrame(float i_CurrentTime);

	//--------------------------------------------------------------------
	//	Call FinishRecording on all recorders
	//--------------------------------------------------------------------
	void FinishRecording();

	//--------------------------------------------------------------------
	//	Call Export on all recorders
	//--------------------------------------------------------------------
	void Export();

private:
	std::ofstream m_OutFile;
	std::vector<mexpChannelRecorder*> m_Recorders;
	std::map<itString, int> m_InstanceCount;
	bool m_bExportAnimation;
	bool m_bExportJoints;
};

