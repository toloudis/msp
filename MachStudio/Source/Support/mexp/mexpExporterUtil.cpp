/****************************************************************************\
**	mexpExporterUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mexp/mexpExporterUtil.hpp"

#include "Support/mexp/mexpExporter.hpp"
#include "Support/mexp/mexpRecorderCamera.hpp"
#include "Support/mexp/mexpRecorderNode.hpp"
#include "Support/mexp/mexpRecorderPosition.hpp"
#include "Support/mexp/mexpRecorderRotation.hpp"

#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelOrientation.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"

namespace
{
	// recursive export of hierarchy:
	void export_node(mexpExporter &io_Exporter,
			const std::string &i_ParentName,
			const g3dSceneNode* i_pNode)
	{
		std::string node_name(i_pNode->GetName());
		if (node_name.length() == 0) 
			return;	// stop export on no-name node (usually shape nodes)

		// Convert from matrix to translation and rotation
		const maMatrix4x4& matx = i_pNode->GetTransform();
		maPoint3d position = matx.GetTranslation();
		maRotation orientation;
		orientation.SetValue(matx);

		// Export this node
		std::string maya_name = io_Exporter.ExportNodeLocation(node_name, 
			i_ParentName, 
			position, 
			orientation);

		// If the exporter has requested animation data, then attach
		// channel recorders to the nodes also
		if (io_Exporter.IsExportAnimation())
		{
			io_Exporter.AddChannelRecorder( new mexpRecorderNodePosition(maya_name, *i_pNode) );
			io_Exporter.AddChannelRecorder( new mexpRecorderNodeRotation(maya_name, *i_pNode) );

		}

		// Export children recursively
		const int num_kids = i_pNode->GetNumChildren();
		for (int i=0; i<num_kids; i++)
		{
			export_node(io_Exporter, maya_name, i_pNode->GetChild(i));
		}
	}

}	// end of anonymous namespace

//--------------------------------------------------------------------
// Examine channel to see if animation is needed. If so, prepare 
//	exporter to record the animation.
// TODO: If the drivers on the channel are just static keys, then 
//	the keys could be exported directly without recording.
//--------------------------------------------------------------------
void mexpExporterUtil::PrepareTranslationAnimation(mexpExporter &io_Exporter,
	const std::string &i_MayaName,
	const tmlnChannelPosition &i_Channel,
	const prtyPoint3d &i_Property)
{
	if (i_Channel.GetNumDrivers() > 0)
	{
		io_Exporter.AddChannelRecorder( new mexpRecorderPosition(i_MayaName, i_Property) );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mexpExporterUtil::PrepareRotationAnimation(mexpExporter &io_Exporter,
	const std::string &i_MayaName,
	const tmlnChannelOrientation &i_Channel,
	const prtyRotation &i_Property)
{
	if (i_Channel.GetNumDrivers() > 0)
	{
		io_Exporter.AddChannelRecorder( new mexpRecorderRotation(i_MayaName, i_Property) );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mexpExporterUtil::PrepareCameraPositionAnimation(mexpExporter &io_Exporter,
	const std::string &i_MayaName,
	const tmlnChannelPosition &i_Channel,
	const prtyPoint3d &i_Property)
{
	if (i_Channel.GetNumDrivers() > 0)
	{
		io_Exporter.AddChannelRecorder( 
			new mexpRecorderCameraPoint3d(i_MayaName, i_Property, 
				mexpRecorderCameraPoint3d::e_Position) );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mexpExporterUtil::PrepareCameraTargetAnimation(mexpExporter &io_Exporter,
	const std::string &i_MayaName,
	const tmlnChannelPosition &i_Channel,
	const prtyPoint3d &i_Property)
{
	if (i_Channel.GetNumDrivers() > 0)
	{
		io_Exporter.AddChannelRecorder( 
			new mexpRecorderCameraPoint3d(i_MayaName, i_Property, 
				mexpRecorderCameraPoint3d::e_Target) );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mexpExporterUtil::PrepareCameraFOVAnimation(mexpExporter &io_Exporter,
	const std::string &i_MayaName,
	const tmlnChannelFloat &i_Channel,
	const prtyFloat &i_Property)
{
	if (i_Channel.GetNumDrivers() > 0)
	{
		io_Exporter.AddChannelRecorder( 
			new mexpRecorderCameraFloat(i_MayaName, i_Property, 
				mexpRecorderCameraFloat::e_FOV) );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mexpExporterUtil::ExportHierarchy(mexpExporter &io_Exporter,
		const std::string &i_MayaName,
		const g3dSceneNode* i_pNode)
{
	// recursive export of hierarchy:
	//export_node(io_Exporter, i_MayaName, i_pNode);
	
	// Actually, we want to skip the base node, because it has the 
	// the transformation from MachStudio that has already been exported.
	// So, just export the children of this node using the given
	// Maya name as the parent node.

	// Export children recursively
	const int num_kids = i_pNode->GetNumChildren();
	for (int i=0; i<num_kids; i++)
	{
		export_node(io_Exporter, i_MayaName, i_pNode->GetChild(i));
	}
}

