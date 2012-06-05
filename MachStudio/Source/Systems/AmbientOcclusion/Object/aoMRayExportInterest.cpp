/****************************************************************************\
**	aoMRayExportInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/AmbientOcclusion/Object/aoMRayExportInterest.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"
#include "Graphics/smdl/private/smdlSubdivSurface.hpp"

#include "Systems/AmbientOcclusion/Object/aoObjectMgr.hpp"

#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Support/mray/mrayExporter.hpp"
#include "Support/mray/mrayMgr.hpp"


#include <algorithm>


//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
const char* aoMRayExportInterest::GetChunkDesc() const
{
	return "AO";
}

//--------------------------------------------------------------------
// Gather data for scene that will be exported
//--------------------------------------------------------------------
void aoMRayExportInterest::GatherSceneData( mraySceneData &o_SceneData, mrayGlobalData &o_GlobalData ) const
{
	// put the names of the objects in the list
	const int num_objects = aoObjectMgr::GetNumObjects();

	for (int i=0; i<num_objects; ++i)
	{
		aoAOData objData = aoObjectMgr::GetBaseData();

		mrayAOData data;
		maFloatRGBA col = objData.m_Color.GetValue();
		data.color = maPoint3d(col.GetRed(),col.GetGreen(),col.GetBlue());
		data.radiusNear = objData.m_AORadius.GetValue();
		data.radiusFar = objData.m_AORadiusFar.GetValue();
		data.angleBias = objData.m_AngleBias.GetValue();
		data.attenuation = objData.m_Attenuation.GetValue();
		data.contrast = objData.m_Contrast.GetValue();
		data.blurWidth = objData.m_BlurWidth.GetValue();
		data.blurSharpness = objData.m_BlurSharpness.GetValue();
		data.overscanPixels = objData.m_OverscanPixels.GetValue();
		data.name = objData.m_Name.GetValue().GetString();

		o_GlobalData.m_AOData = data;

	}
}

//--------------------------------------------------------------------
//	Export only the items in the given list.
//	The strings will be some subset of what was returned from the
//	call to GatherItemNames.
//--------------------------------------------------------------------
void aoMRayExportInterest::Export( mrayExporter& i_Exporter,
										  const mraySceneData &i_SceneData)
{

}
