/*****************************************************************************\
**	mexpExporterUtil.hpp
**
**		Provides help in converting channels and properties into 
**	keys for exporting.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef MEXP_EXPORTERUTIL_HPP
#error mexpExporterUtil.hpp multiply included
#endif
#define MEXP_EXPORTERUTIL_HPP

#include <string>

//============================================================================
//	Forward References
//============================================================================
class g3dSceneNode;
class itString;
class mexpExporter;
class prtyFloat;
class prtyPoint3d;
class prtyRotation;
class tmlnChannelFloat;
class tmlnChannelOrientation;
class tmlnChannelPosition;


//============================================================================
//============================================================================
namespace mexpExporterUtil
{
	//--------------------------------------------------------------------
	// Examine channel to see if animation is needed. If so, prepare 
	//	exporter to record the animation.
	// Pass in the Maya name returned from the exporter functions
	//	the exporter the locator or camera initially.
	// TODO: If the drivers on the channel are just static keys, then 
	//	the keys could be exported directly without recording.
	//--------------------------------------------------------------------
	void PrepareTranslationAnimation(mexpExporter &io_Exporter,
		const std::string &i_MayaName,
		const tmlnChannelPosition &i_Channel,
		const prtyPoint3d &i_Property);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PrepareRotationAnimation(mexpExporter &io_Exporter,
		const std::string &i_MayaName,
		const tmlnChannelOrientation &i_Channel,
		const prtyRotation &i_Property);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PrepareCameraPositionAnimation(mexpExporter &io_Exporter,
		const std::string &i_MayaName,
		const tmlnChannelPosition &i_Channel,
		const prtyPoint3d &i_Property);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PrepareCameraTargetAnimation(mexpExporter &io_Exporter,
		const std::string &i_MayaName,
		const tmlnChannelPosition &i_Channel,
		const prtyPoint3d &i_Property);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PrepareCameraFOVAnimation(mexpExporter &io_Exporter,
		const std::string &i_MayaName,
		const tmlnChannelFloat &i_Channel,
		const prtyFloat &i_Property);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ExportHierarchy(mexpExporter &io_Exporter,
		const std::string &i_MayaName,
		const g3dSceneNode* i_pNode);
};
