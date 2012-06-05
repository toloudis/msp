/****************************************************************************\
**	giMRayExportInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/GlobalIllumination/Object/giMRayExportInterest.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"
#include "Graphics/smdl/private/smdlSubdivSurface.hpp"

#include "Systems/GlobalIllumination/Object/giObjectMgr.hpp"

#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Support/mray/mrayExporter.hpp"
#include "Support/mray/mrayMgr.hpp"


#include <algorithm>


//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
const char* giMRayExportInterest::GetChunkDesc() const
{
	return "GI";
}

//--------------------------------------------------------------------
// Gather data for scene that will be exported
//--------------------------------------------------------------------
void giMRayExportInterest::GatherSceneData( mraySceneData &o_SceneData, mrayGlobalData &o_GlobalData ) const
{
	// put the names of the objects in the list
	const int num_objects = giObjectMgr::GetNumObjects();

	for (int i=0; i<num_objects; ++i)
	{
		giGIData objData = giObjectMgr::GetBaseData();

		mrayGIData data;
		maFloatRGBA col = objData.m_Color.GetValue();
		data.color = maPoint3d(col.GetRed(),col.GetGreen(),col.GetBlue());
		data.radiusNear = objData.m_GIRadius.GetValue();
		data.radiusFar = objData.m_GIRadiusFar.GetValue();
		data.angleBias = objData.m_AngleBias.GetValue();
		data.attenuation = objData.m_Attenuation.GetValue();
		data.contrast = objData.m_Contrast.GetValue();
		data.blurWidth = objData.m_BlurWidth.GetValue();
		data.blurSharpness = objData.m_BlurSharpness.GetValue();
		data.overscanPixels = objData.m_OverscanPixels.GetValue();
		data.name = objData.m_Name.GetValue().GetString();

		data.RSMGIScale = objData.m_RSMGIScale.GetValue();
		data.RSMGISampleRadius = objData.m_RSMGISampleRadius.GetValue();
		data.RSMGISampleNum = objData.m_RSMGISampleNum.GetValue();
		data.GILightScale = objData.m_GILightScale.GetValue();
		data.RSMSize = objData.m_RSMSize.GetValue();
		data.LPVScale = objData.m_LPVScale.GetValue();
		data.LPVIteration = objData.m_LPVIteration.GetValue();
		data.LPVVolumeSize = objData.m_LPVVolumeSize.GetValue();
		data.LPVGIFalloff = objData.m_LPVGIFalloff.GetValue();
		data.LPVRSMSize = objData.m_LPVRSMSize.GetValue();

		o_GlobalData.m_GIData = data;
	}
}

//--------------------------------------------------------------------
//	Export only the items in the given list.
//	The strings will be some subset of what was returned from the
//	call to GatherItemNames.
//--------------------------------------------------------------------
void giMRayExportInterest::Export( mrayExporter& i_Exporter,
										  const mraySceneData &i_SceneData)
{

}
