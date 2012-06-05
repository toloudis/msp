/*****************************************************************************
**  mtrlIconUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mtrl/mtrlIconUtil.hpp"

#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#include "Systems/Cameras/Object/cmraScriptObject.hpp"

#include "Support/cams/camsCameraMgr.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"

#include "Graphics/mtr/mtrMaterialSaver.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"

//#define _CREATE_ICON

namespace mtrlIconUtil
{
	namespace
	{
		fsLocator l_TexturePath;
		mdlMaterialInfo l_matInfo;
		maFloatRGBA l_LowColor( 0.2f, 1.0f, 0.0f, 1.0f );
		bool isOrthoCam = false;
		//--------------------------------------------------------------------
		// loadMtrlSphere-- load a sphere to the scene and apply the specified material 
		//--------------------------------------------------------------------
		void loadMtrlSphere()
		{
			g3dFragment* fragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(5.0f,12,12);
			
			//fragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(5.0f, 5.0f, 1, 1, true);
			//fragment->SetDoubleSided(true);

			matMaterial *pTexMat = new matMaterial("Phong.fx");
			effPhongData* pData;
			pData = dynamic_cast<effPhongData*>(pTexMat->GetEffectData());
			DBG_ASSERT0(pData != NULL, "prjltIconObject not using effPhong");
			const float c_Transparency = 1.0f;
			//pSolidData->m_Color.Set( 1.0f, 1.0f, 1.0f, c_Transparency );
			/*
			pData->m_ColorEmissive = maFloatRGBA(l_matInfo.GetShaderData()->GetEmissive().GetRed(),
				l_matInfo.GetShaderData()->GetEmissive().GetBlue(), 
				l_matInfo.GetShaderData()->GetEmissive().GetGreen(),
				c_Transparency);
			pData->m_ColorDiffuse = maFloatRGBA(l_matInfo.GetShaderData()->GetDiffuse().GetRed(),
				l_matInfo.GetShaderData()->GetDiffuse().GetBlue(), 
				l_matInfo.GetShaderData()->GetDiffuse().GetGreen(),
				c_Transparency);
			pData->m_ColorAmbient = maFloatRGBA(l_matInfo.GetShaderData()->GetAmbient().GetRed(),
				l_matInfo.GetShaderData()->GetAmbient().GetBlue(), 
				l_matInfo.GetShaderData()->GetAmbient().GetGreen(),
				0.0f);
			DBG_WARNING1("Color Diffuse %d", l_matInfo.GetShaderData()->GetEmissive().GetRed());
			*/
			pData->m_ColorEmissive = maFloatRGBA(1.0f, 1.0f, 1.0f, c_Transparency);
			pData->m_ColorDiffuse = maFloatRGBA(0.0f, 0.0f, 0.0f, c_Transparency);
			pData->m_ColorAmbient = maFloatRGBA(0.0f, 0.0f, 0.0f, 0.0f);
			pTexMat->ForceTransparency( true );

			fsLocator tDir;
			fsFileUtil::ANSIFilenameToLocator("C:\\projects\\Test-Data\\Data\\STOCK\\Props\\General\\Textures\\CD_disc_LetsGetToIt.dds", tDir);
			matTexture* pMat = matTextureMgr::LoadTexture(tDir);
			pData->m_TextureDiffuse = pMat;
			
			api3dObjectSimple* pObject = new api3dObjectSimple(fragment, pTexMat);
			
			api3dScene::AddObject(pObject);

		}
		//--------------------------------------------------------------------
		// createMtrlCamera -- Create a camera so that the sphere can be rendered
		//--------------------------------------------------------------------
		void createMtrlCamera()
		{
			const bool editorCameraOrthographic = false;
			cmraCameraObject mtrlCamera(editorCameraOrthographic);
			//mtrlCamera.PropertyPosition;
			//mtrlCamera.PropertyTarget;
			//mtrlCamera.PropertyFieldOfView;
			//mtrlCamera.PropertyNearClip;
			//mtrlCamera.PropertyFarClip;
			nameString camName("mtrlIconCamera");
			camsCameraMgr::AddCamera(camName, "Camera to Render Mtrl Icon", mtrlCamera.GetCameraPtr());
		}

		//--------------------------------------------------------------------
		// renderSphere -- Render the new scene as a bmp
		//--------------------------------------------------------------------
		void renderSphere()
		{
			
		}

		//--------------------------------------------------------------------
		// writeImageData -- Write the raw data to the mtl file
		//--------------------------------------------------------------------
		void writeImageData()
		{
			
		}
	}

	//--------------------------------------------------------------------
	// createMtrlIcon -- Creates a material Icon from a specified material (mtl) file
	//--------------------------------------------------------------------
	void createMtrlIcon(const mdlMaterialInfo i_matInfo)
	{
		//l_TexturePath = i_TexturePath;
		l_matInfo = i_matInfo;
		//l_TexturePath = fsLocator(itString("C:\\projects\\Test-Data\\Data\\STOCK\\Materials\\initialShadingGroup2.mtl"));
#ifdef _CREATE_ICON	
		loadMtrlSphere();
		//createMtrlCamera();
		renderSphere();
		writeImageData();
#endif
	}

};