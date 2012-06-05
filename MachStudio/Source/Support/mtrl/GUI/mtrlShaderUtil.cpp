/*****************************************************************************
**	mtrlShaderUtil.cpp
**
**	Routines for creating properties for material data
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/mtrl/GUI/mtrlShaderUtil.hpp"
#include "Support/mtrl/GUI/mtrlGlow.hpp"
#include "Support/mtrl/GUI/mtrlRefl.hpp"
#include "Support/mtrl/GUI/mtrlSurfaceShader.hpp"
#include "Support/mtrl/GUI/mtrlUVTransform.hpp"
#include "Support/mtrl/GUI/mtrlOutline.hpp"
#include "Support/mtrl/GUI/mtrlDisplacement.hpp"
#include "Support/mtrl/GUI/mtrlNormals.hpp"
#include "Support/mtrl/GUI/mtrlRendermanOverride.hpp"
#include "Support/mtrl/GUI/mtrlTextureFilter.hpp"

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
										   int i_MaterialLayerIndex,
										   matMaterial *i_pMaterial,
										   const fsLocator& i_TextureDir,
										   entModelInstance *i_pModelInstance)
	{
		mtrlShaderObject* pRetVal = NULL;

		// first of all, let's look at the shader params.
		shared_ptr<effShaderParams> pParams = i_Data.GetShaderParams(i_MaterialLayerIndex);

		// now, we need to get the shader.
		matShaderEffect* eff = pParams->GetShader();
		DBG_ASSERT(eff != NULL, "shader did not load!");

		// now, if the current params have no shader bindings or gui, 
		// we need to create them. from then on, they can be carried along.
		if (eff != NULL)
		{
			if (pParams->m_pShaderBindings == NULL)
				eff->BuildPrtyObject(pParams.get());
			i_pMaterial->SetShaderParams(i_MaterialLayerIndex, pParams);
			pRetVal = new mtrlSurfaceShader(i_Data, i_pMaterial, pParams, i_TextureDir);
		}
	
		if (pRetVal)
		{
			// Give the shade object the model template pointer
			pRetVal->SetModelInstance(i_pModelInstance);
		}

		// Should we return a phong material or NULL?
		//return new mtrlPhong(i_Data);
		return pRetVal;
	}

	//--------------------------------------------------------------------
	// Create property object for glow aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateGlowObject(mdlMaterialInfo& i_Data, 
									   matMaterial *i_pMaterial,
									   const fsLocator& i_TextureDir,
									   entModelInstance *i_pModelInstance)
	{
		mtrlShaderObject* pRetVal =  new mtrlGlow(i_Data, &i_pMaterial->GlowData(), i_TextureDir);
		// Give the shade object the model template pointer
		pRetVal->SetModelInstance(i_pModelInstance);
		return pRetVal;
	}

	//--------------------------------------------------------------------
	// Create property object for outline aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateOutlineObject(mdlMaterialInfo& i_Data, 
		matMaterial *i_pMaterial,
		const fsLocator& i_TextureDir,
		entModelInstance *i_pModelInstance)
	{
		mtrlShaderObject* pRetVal =  new mtrlOutline(i_Data, &i_pMaterial->OutlineData(), i_TextureDir);
		// Give the shade object the model template pointer
		pRetVal->SetModelInstance(i_pModelInstance);
		return pRetVal;
	}

	//--------------------------------------------------------------------
	// Create property object for uv transform aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateUVTransformObject(mdlMaterialInfo& i_Data, 
									   int i_MaterialLayerIndex,
									   matMaterial *i_pMaterial,
									   const fsLocator& i_TextureDir,
									   entModelInstance *i_pModelInstance)
	{
		mtrlShaderObject* pRetVal =  new mtrlUVTransform(i_Data, &i_pMaterial->UVTransform(i_MaterialLayerIndex), i_TextureDir);
		// Give the shade object the model template pointer
		pRetVal->SetModelInstance(i_pModelInstance);
		return pRetVal;
	}

	//--------------------------------------------------------------------
	// Create property object for reflection aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateReflectionObject(mdlMaterialInfo& i_Data, 
									   matMaterial *i_pMaterial,
									   const fsLocator& i_TextureDir,
									   entModelInstance *i_pModelInstance)
	{
		mtrlShaderObject* pRetVal =  new mtrlRefl(i_Data, &i_pMaterial->ReflectionData(), i_TextureDir);
		// Give the shade object the model template pointer
		pRetVal->SetModelInstance(i_pModelInstance);
		return pRetVal;
	}

	//--------------------------------------------------------------------
	// Create property object for displacement aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateDisplacementObject(mdlMaterialInfo& i_Data, 
		matMaterial *i_pMaterial,
		const fsLocator& i_TextureDir,
		entModelInstance *i_pModelInstance)
	{
		mtrlShaderObject* pRetVal =  new mtrlDisplacement(i_Data, &i_pMaterial->DisplacementData(), i_TextureDir);
		// Give the shade object the model template pointer
		pRetVal->SetModelInstance(i_pModelInstance);
		return pRetVal;
	}

	//--------------------------------------------------------------------
	// Create property object for normals aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateNormalsObject(mdlMaterialInfo& i_Data, 
									   matMaterial *i_pMaterial,
									   const fsLocator& i_TextureDir,
									   entModelInstance *i_pModelInstance)
	{
		mtrlShaderObject* pRetVal =  new mtrlNormals(i_Data, &i_pMaterial->NormalsData(), i_TextureDir);
		// Give the shade object the model template pointer
		pRetVal->SetModelInstance(i_pModelInstance);
		return pRetVal;
	}

	//--------------------------------------------------------------------
	// Create property object for normals aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateRendermanOverrideObject(mdlMaterialInfo& i_Data, 
													matMaterial *i_pMaterial,
													const fsLocator& i_TextureDir,
													entModelInstance *i_pModelInstance)
	{
		mtrlShaderObject* pRetVal =  new mtrlRendermanOverride(i_Data, &i_pMaterial->RendermanOverrideData(), i_TextureDir);
		// Give the shade object the model template pointer
		pRetVal->SetModelInstance(i_pModelInstance);
		return pRetVal;
	}

	//--------------------------------------------------------------------
	// Create property object for normals aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateTextureFilterObject(mdlMaterialInfo& i_Data, 
													matMaterial *i_pMaterial,
													const fsLocator& i_TextureDir,
													entModelInstance *i_pModelInstance)
	{
		mtrlShaderObject* pRetVal =  new mtrlTextureFilter(i_Data, &i_pMaterial->TextureFilter(), i_TextureDir);
		// Give the shade object the model template pointer
		pRetVal->SetModelInstance(i_pModelInstance);
		return pRetVal;
	}


}	// end of namespace
