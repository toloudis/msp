/*****************************************************************************
**	mtrlShaderUtil.hpp
**
**	Routines for creating properties for material data
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef MTRL_SHADERUTIL_HPP
#error mtrlShaderUtil.hpp multiply included
#endif
#define MTRL_SHADERUTIL_HPP


//============================================================================
//============================================================================
class entModelInstance;
class fsLocator;
class matMaterial;
class mdlMaterialInfo;
class mtrlShaderObject;


//============================================================================
//============================================================================
namespace mtrlShaderUtil
{
	//--------------------------------------------------------------------
	// Create a property object for the current data
	//--------------------------------------------------------------------
	mtrlShaderObject* CreatePropertyObject(mdlMaterialInfo& i_Data, 
											int i_MaterialLayerIndex,
											matMaterial *i_pMaterial,
											const fsLocator& i_TextureDir,
											entModelInstance *i_pModelInstance);

	//--------------------------------------------------------------------
	// Create property object for glow aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateGlowObject(mdlMaterialInfo& i_Data, 
									   matMaterial *i_pMaterial,
									   const fsLocator& i_TextureDir,
										entModelInstance *i_pModelInstance);

	//--------------------------------------------------------------------
	// Create property object for outline aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateOutlineObject(mdlMaterialInfo& i_Data, 
		matMaterial *i_pMaterial,
		const fsLocator& i_TextureDir,
		entModelInstance *i_pModelInstance);

	//--------------------------------------------------------------------
	// Create property object for uv transform aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateUVTransformObject(mdlMaterialInfo& i_Data, 
									   int i_MaterialLayerIndex,
									   matMaterial *i_pMaterial,
									   const fsLocator& i_TextureDir,
										entModelInstance *i_pModelInstance);

	//--------------------------------------------------------------------
	// Create property object for reflection aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateReflectionObject(mdlMaterialInfo& i_Data, 
									   matMaterial *i_pMaterial,
									   const fsLocator& i_TextureDir,
										entModelInstance *i_pModelInstance);

	//--------------------------------------------------------------------
	// Create property object for displacement aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateDisplacementObject(mdlMaterialInfo& i_Data, 
		matMaterial *i_pMaterial,
		const fsLocator& i_TextureDir,
		entModelInstance *i_pModelInstance);

	//--------------------------------------------------------------------
	// Create property object for normals aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateNormalsObject(mdlMaterialInfo& i_Data, 
									   matMaterial *i_pMaterial,
									   const fsLocator& i_TextureDir,
										entModelInstance *i_pModelInstance);

	//--------------------------------------------------------------------
	// Create property object for normals aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateRendermanOverrideObject(mdlMaterialInfo& i_Data, 
													matMaterial *i_pMaterial,
													const fsLocator& i_TextureDir,
													entModelInstance *i_pModelInstance);

	//--------------------------------------------------------------------
	// Create property object for normals aspect of material
	//--------------------------------------------------------------------
	mtrlShaderObject* CreateTextureFilterObject(mdlMaterialInfo& i_Data, 
													matMaterial *i_pMaterial,
													const fsLocator& i_TextureDir,
													entModelInstance *i_pModelInstance);

}	// end of namespace
