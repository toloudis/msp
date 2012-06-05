/*****************************************************************************
**	mtrlShaderUtil.cpp
**
**	Routines for creating properties for material data
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/mtrl/GUI/mtrlShaderUtil.hpp"
#include "Support/mtrl/GUI/mtrlFur.hpp"
#include "Support/mtrl/GUI/mtrlGlow.hpp"
#include "Support/mtrl/GUI/mtrlRefl.hpp"
#include "Support/mtrl/GUI/mtrlSurfaceShader.hpp"
#include "Support/mtrl/GUI/mtrlUVTransform.hpp"
#include "Support/mtrl/GUI/mtrlOutline.hpp"

#include "Graphics/eff/effReflData.hpp"
#include "Graphics/eff/effShaderParams.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"


//============================================================================
//============================================================================
namespace mtrlShaderUtil
{
	namespace
	{
	}	// end of namespace

	
	//--------------------------------------------------------------------
	// Create a property object for the current data
	//--------------------------------------------------------------------
	mtrlShaderObject* CreatePropertyObject(mdlMaterialInfo& i_Data, 
										   matMaterial *i_pMaterial,
										   const fsLocator& i_TextureDir,
										   entModelTemplate *i_pModelTemplate)
	{
		mtrlShaderObject* pRetVal = NULL;

		// first of all, let's look at the shader params.
		shared_ptr<effShaderParams> pParams = i_Data.GetShaderParams();

		// now, we need to get the shader.
		matShaderEffect* eff = pParams->GetShader();
		DBG_ASSERT0(eff != NULL, "shader did not load!");

		// now, if the current params have no shader bindings or gui, 
		// we need to create them. from then on, they can be carried along.
		if (eff)
		{
//				DBG_ASSERT0(eff != NULL, "shader did not load!");
			eff->BuildPrtyObject(pParams.get());
			i_pMaterial->SetShaderParams(pParams);
			pRetVal = new mtrlSurfaceShader(i_Data, i_pMaterial, pParams, i_TextureDir);
		}
	
		if (pRetVal)
		{
			// Give the shade object the model template pointer
			pRetVal->SetModelTemplate(i_pModelTemplate);
		}

		// Should we return a phong material or NULL?
		//return new mtrlPhong(i_Data);
		return pRetVal;
	}

	//--------------------------------------------------------------------
	// Create property object for fur aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateFurObject(mdlMaterialInfo& i_Data, 
									  matMaterial *i_pMaterial,
									  entModelTemplate *i_pModelTemplate)
	{
		mtrlShaderObject* pRetVal =  new mtrlFur(i_Data, &i_pMaterial->FurData().m_EffectData);
		// Give the shade object the model template pointer
		pRetVal->SetModelTemplate(i_pModelTemplate);
		return pRetVal;
	}

	//--------------------------------------------------------------------
	// Create property object for glow aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateGlowObject(mdlMaterialInfo& i_Data, 
									   matMaterial *i_pMaterial,
									   const fsLocator& i_TextureDir,
									   entModelTemplate *i_pModelTemplate)
	{
		mtrlShaderObject* pRetVal =  new mtrlGlow(i_Data, &i_pMaterial->GlowData(), i_TextureDir);
		// Give the shade object the model template pointer
		pRetVal->SetModelTemplate(i_pModelTemplate);
		return pRetVal;
	}

	//--------------------------------------------------------------------
	// Create property object for outline aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateOutlineObject(mdlMaterialInfo& i_Data, 
		matMaterial *i_pMaterial,
		const fsLocator& i_TextureDir,
		entModelTemplate *i_pModelTemplate)
	{
		mtrlShaderObject* pRetVal =  new mtrlOutline(i_Data, &i_pMaterial->OutlineData(), i_TextureDir);
		// Give the shade object the model template pointer
		pRetVal->SetModelTemplate(i_pModelTemplate);
		return pRetVal;
	}

	//--------------------------------------------------------------------
	// Create property object for uv transform aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateUVTransformObject(mdlMaterialInfo& i_Data, 
									   matMaterial *i_pMaterial,
									   const fsLocator& i_TextureDir,
									   entModelTemplate *i_pModelTemplate)
	{
		mtrlShaderObject* pRetVal =  new mtrlUVTransform(i_Data, &i_pMaterial->UVTransform(), i_TextureDir);
		// Give the shade object the model template pointer
		pRetVal->SetModelTemplate(i_pModelTemplate);
		return pRetVal;
	}

	//--------------------------------------------------------------------
	// Create property object for reflection aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateReflectionObject(mdlMaterialInfo& i_Data, 
									   matMaterial *i_pMaterial,
									   const fsLocator& i_TextureDir,
									   entModelTemplate *i_pModelTemplate)
	{
		mtrlShaderObject* pRetVal =  new mtrlRefl(i_Data, &i_pMaterial->ReflectionData(), i_TextureDir);
		// Give the shade object the model template pointer
		pRetVal->SetModelTemplate(i_pModelTemplate);
		return pRetVal;
	}

}	// end of namespace
