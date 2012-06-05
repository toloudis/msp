/********************************************************************************************\
**  mtrMaterialUtil.cpp
**
**      Routines for helping set material values from a mdlMaterialInfo.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#include "Graphics/mtr/mtrMaterialUtil.hpp"

#include "Core/app/appSimTime.hpp"
#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsResourceFinderDir.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Graphics/an/an2StateAnimation.hpp"
#include "Graphics/an/anKeyAnimation.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/eff/effShaderParams.hpp"
#include "Graphics/eff/effWaterData.hpp"
#include "Graphics/ent/entImport.hpp"
#include "Graphics/ent/entModelInstance.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mat/matMatAnim.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#include "Graphics/mtr/mtrTextureFinder.hpp"
#include "Graphics/sc/scObject.hpp"

#include <algorithm>
#include <memory>


//============================================================================
//============================================================================
namespace mtrMaterialUtil
{

	//------------------------------------------------------------------------
	// Local variables and functions
	//------------------------------------------------------------------------
	namespace
	{
		//--------------------------------------------------------------------
		// gather fragments from jointed object's scene node tree
		//--------------------------------------------------------------------
		void gather_fragments(g3dSceneNode *i_Node, std::vector<g3dFragment*> &o_Fragments)
		{
			if (i_Node->GetFragment())
				o_Fragments.push_back(i_Node->GetFragment());

			int num_kids = i_Node->GetNumChildren();
			for (int k=0; k<num_kids; k++)
				gather_fragments(i_Node->GetChild(k), o_Fragments);
		}

		//--------------------------------------------------------------------
		// gather nodes that have fragments from scene node tree
		//--------------------------------------------------------------------
		void gather_fragment_nodes(g3dSceneNode *i_Node, 
								   std::vector<g3dSceneNode*> &o_Nodes)
		{
			if (i_Node->GetFragment())
				o_Nodes.push_back(i_Node);

			int num_kids = i_Node->GetNumChildren();
			for (int k=0; k<num_kids; k++)
				gather_fragment_nodes(i_Node->GetChild(k), o_Nodes);
		}

		//--------------------------------------------------------------------
		// gather nodes using the given material from scene node tree
		//--------------------------------------------------------------------
		void gather_material_nodes(g3dSceneNode *i_Node, 
								   const matMaterial *i_pMaterial,
								   std::vector<g3dSceneNode*> &o_Nodes)
		{
			if (i_Node->GetFragment() &&
				i_Node->GetFragment()->GetMaterial() == i_pMaterial)
				o_Nodes.push_back(i_Node);

			int num_kids = i_Node->GetNumChildren();
			for (int k=0; k<num_kids; k++)
				gather_material_nodes(i_Node->GetChild(k), i_pMaterial, o_Nodes);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		matMatParamIndex::matMatParamIndexType get_param_index(int i_MatComponent, int i_TextureLayer)
		{
			switch (i_MatComponent)
			{
			case 0:
				return matMatParamIndex::e_Ambient;
			case 1:
				return matMatParamIndex::e_Diffuse;
			case 2:
				return matMatParamIndex::e_Specular;
			case 3:
				return matMatParamIndex::e_Emissive;
			case 4:
				return matMatParamIndex::e_SpecularPower;
			case 5:
				DBG_ASSERT(i_TextureLayer < 4, "Can't animate texture layer above fourth");
				if (i_TextureLayer >= 4)
					return matMatParamIndex::e_Ambient;
				return matMatParamIndex::matMatParamIndexType(matMatParamIndex::e_TextureTranslation0 + i_TextureLayer);
			case 6:
				DBG_ASSERT(i_TextureLayer < 4, "Can't animate texture layer above fourth");
				if (i_TextureLayer >= 4)
					return matMatParamIndex::e_Ambient;
				return matMatParamIndex::matMatParamIndexType(matMatParamIndex::e_TextureScale0 + i_TextureLayer);
			case 7:
				DBG_ASSERT(i_TextureLayer < 4, "Can't animate texture layer above fourth");
				if (i_TextureLayer >= 4)
					return matMatParamIndex::e_Ambient;
				return matMatParamIndex::matMatParamIndexType(matMatParamIndex::e_TextureRotation0 + i_TextureLayer);
			default:
				DBG_ASSERT(false, "Unknown material component in mtrMaterialUtil::get_param_index");
				return matMatParamIndex::e_Ambient;
			}
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		//matMatAnim * make_mat_anim(const mtrMatAnimInfo &i_Anim)
		//{
		//	int num_keys = i_Anim.m_KeyTimes.size();
		//	int key_data_size = i_Anim.m_KeyData.size() / num_keys;

		//	int index;
		//	if (key_data_size == 4)
		//	{
		//		anKeyAnimation<maFloatRGBA>* color_anim = 
		//			new anKeyAnimation<maFloatRGBA>(maFloatRGBA(i_Anim.m_KeyData[0], 
		//					i_Anim.m_KeyData[1], i_Anim.m_KeyData[2], i_Anim.m_KeyData[3]));
		//		index = key_data_size;
		//		for (int k=1; k<num_keys; k++, index+=key_data_size)
		//			color_anim->AddKey(i_Anim.m_KeyTimes[k], maFloatRGBA(i_Anim.m_KeyData[index], 
		//					i_Anim.m_KeyData[index+1], i_Anim.m_KeyData[index+2], i_Anim.m_KeyData[index+3]));

		//		color_anim->SetLooping(i_Anim.m_bLooping);
		//		color_anim->SetReversing(i_Anim.m_bReversing);

		//		matMatAnim *mat_anim = new matMatAnim( color_anim,
		//							get_param_index(i_Anim.m_Component, i_Anim.m_TextureLayer),
		//							appSimTime::GetTime() );
		//		mat_anim->SetActive(i_Anim.m_bInitialActive);
		//		return mat_anim;
		//	}
		//	else if (key_data_size == 2)
		//	{
		//		anKeyAnimation<maVector2d>* vec_anim = 
		//			new anKeyAnimation<maVector2d>(maVector2d(i_Anim.m_KeyData[0], i_Anim.m_KeyData[1]));
		//		index = key_data_size;
		//		for (int k=1; k<num_keys; k++, index+=key_data_size)
		//			vec_anim->AddKey(i_Anim.m_KeyTimes[k], maVector2d(i_Anim.m_KeyData[index], i_Anim.m_KeyData[index+1]));

		//		vec_anim->SetLooping(i_Anim.m_bLooping);
		//		vec_anim->SetReversing(i_Anim.m_bReversing);

		//		matMatAnim *mat_anim = new matMatAnim( vec_anim,
		//							get_param_index(i_Anim.m_Component, i_Anim.m_TextureLayer),
		//							appSimTime::GetTime() );
		//		mat_anim->SetActive(i_Anim.m_bInitialActive);
		//		return mat_anim;
		//	}
		//	else if (key_data_size == 1)
		//	{
		//		anKeyAnimation<float>* float_anim = new anKeyAnimation<float>(i_Anim.m_KeyData[0]);
		//		for (int k=1; k<num_keys; k++)
		//			float_anim->AddKey(i_Anim.m_KeyTimes[k], i_Anim.m_KeyData[k]);

		//		float_anim->SetLooping(i_Anim.m_bLooping);
		//		float_anim->SetReversing(i_Anim.m_bReversing);

		//		matMatAnim *mat_anim = new matMatAnim( float_anim,
		//							get_param_index(i_Anim.m_Component, i_Anim.m_TextureLayer),
		//							appSimTime::GetTime() );
		//		mat_anim->SetActive(i_Anim.m_bInitialActive);
		//		return mat_anim;
		//	}
		//	return NULL;
		//}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		//void set_blend_type(matMaterial &i_Mat, 
		//					int i_Index, 
		//					mtrTextureLayer::TextureType i_Type)
		//{
		//	switch (i_Type)
		//	{
		//	default:
		//		DBG_ASSERT(false, "Unrecognized texture blend type.");
		//		break;
		//	case mtrTextureLayer::e_Normal:
		//		{
		//			// If the previous level is bump height, then we have
		//			// to switch this level to be TexLayerModulate to 
		//			// receive the bump mapping
		//			if ((i_Index >= 1) && (i_Mat.GetTextureLayerType(i_Index-1) == matMaterial::e_TexLayerBumpMapHeightMap))
		//				i_Mat.SetTextureLayerType(i_Index, matMaterial::e_TexLayerModulate);
		//			else
		//				i_Mat.SetTextureLayerType(i_Index, matMaterial::e_TexLayerNormal);
		//		}
		//		break;
		//	case mayTexLayerInfo::e_BumpHeight:
		//		{
		//			i_Mat.SetTextureLayerType(i_Index, matMaterial::e_TexLayerBumpMapHeightMap);

		//			// Not sure what to do about bump map light
		//			maVector3d dir (3, 2, 1);
		//			dir.Normalize();
		//			i_Mat.SetBumpMapLight( i_Index, dir );
		//		}
		//		break;
		//	case mayTexLayerInfo::e_SpecularMap:
		//		i_Mat.SetTextureLayerType(i_Index, matMaterial::e_TexLayerSpecularMap);
		//		break;
		//	case mayTexLayerInfo::e_GlossMap:
		//		i_Mat.SetTextureLayerType(i_Index, matMaterial::e_TexLayerGlossMap);
		//		break;
		//	case mayTexLayerInfo::e_AlphaBlend:
		//		i_Mat.SetTextureLayerType(i_Index, matMaterial::e_TexLayerAlphaBlend);
		//		break;
		//	case mayTexLayerInfo::e_EnvMap:
		//		i_Mat.SetTextureLayerType(i_Index, matMaterial::e_TexLayerEnvMap);
		//		break;
		//	//case mayTexLayerInfo::e_BumpReflect:
		//	//	i_Mat.SetTextureLayerType(i_Index, matMaterial::e_TexLayerBumpMapEnvironment);
		//	//	break;

		//	}
		//}

	} // end of namespace

	//----------------------------------------------------------------------------
	//	Replace old material with new material in the given list of fragments
	//----------------------------------------------------------------------------
	void ReplaceMaterial( matMaterial* i_OldMaterial,
						  matMaterial* i_NewMaterial,
						  std::vector<g3dFragment*>& io_Fragments )
	{
		int i;
		int frag_num = io_Fragments.size();
		for ( i = 0; i < frag_num; ++i )
		{
			matMaterial* mat_ptr = io_Fragments[i]->GetMaterial();
			if ( mat_ptr == i_OldMaterial )
			{
				io_Fragments[i]->SetMaterial( i_NewMaterial );
			}
		}
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void LoadShaderTextures(const mdlMaterialInfo & i_MaterialTemplate, mtrTextureFinder& i_Finder)
	{
		std::vector<effParamTexture*> textureParams;
		i_MaterialTemplate.GetShaderParams()->GetAllTextureParams(textureParams);
		for (int i = 0; i < textureParams.size(); i++)
		{
			effParamTexture* param = textureParams[i];
			if( !param->Load(i_Finder))
			{
				param->SetTexture(NULL);
			}
		}
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void RemoveShaderTextures(matMaterial& io_Material,
						entModelInstance& i_ModelInstance)
	{
		std::vector<matTexture*> &tex_list = i_ModelInstance.Textures();

		// shader effect textures
/*
if (io_Material.GetEffectData() != NULL)
		{
			std::vector<matTexture*> shaderTextures;
			io_Material.GetEffectData()->GetTextures(shaderTextures);
			for (int i = 0; i < shaderTextures.size(); i++)
			{
				matTexture *texture = shaderTextures[i];
				envSTLHelpers::RemoveOneValue(tex_list, texture);
				matTextureMgr::ReleaseTexture(texture);
			}
			io_Material.GetEffectData()->RemoveTextures();
		}
*/
		
		if (io_Material.GetShaderParams())
		{
			std::vector<matTexture*> shaderTextures;
			io_Material.GetShaderParams()->GetTextures(shaderTextures);

			for (int i = 0; i < shaderTextures.size(); i++)
			{
				matTexture *texture = shaderTextures[i];
				envSTLHelpers::RemoveOneValue(tex_list, texture);
				matTextureMgr::ReleaseTexture(texture);
				/*while(envSTLHelpers::RemoveOneValue(tex_list, texture))
				{
					matTextureMgr::ReleaseTexture(texture);
				}*/
			}
			io_Material.GetShaderParams()->RemoveTextures();
		}
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void RemoveGlowTextures(matMaterial& io_Material,
						entModelInstance& i_ModelInstance)
	{
		std::vector<matTexture*> &tex_list = i_ModelInstance.Textures();

		// glow texture
		std::vector<matTexture*> glow_tex_list;
		io_Material.GetGlowData().GetTextures(glow_tex_list);
		for (int i = 0; i < glow_tex_list.size(); i++)
		{
			matTexture *texture = glow_tex_list[i];
			envSTLHelpers::RemoveOneValue(tex_list, texture);
			matTextureMgr::ReleaseTexture(texture);
		}
		io_Material.GlowData().RemoveTextures();
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void RemoveDisplacementTextures(matMaterial& io_Material,
		entModelInstance& i_ModelInstance)
	{
		std::vector<matTexture*> &tex_list = i_ModelInstance.Textures();

		// displacement texture
		std::vector<matTexture*> displacement_tex_list;
		io_Material.GetDisplacementData().GetTextures(displacement_tex_list);
		for (int i = 0; i < displacement_tex_list.size(); i++)
		{
			matTexture *texture = displacement_tex_list[i];
			envSTLHelpers::RemoveOneValue(tex_list, texture);
			matTextureMgr::ReleaseTexture(texture);
		}
		io_Material.DisplacementData().RemoveTextures();
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void RemoveNormalsTextures(matMaterial& io_Material,
						entModelInstance& i_ModelInstance)
	{
		std::vector<matTexture*> &tex_list = i_ModelInstance.Textures();

		// glow texture
		std::vector<matTexture*> normals_tex_list;
		io_Material.GetNormalsData().GetTextures(normals_tex_list);
		for (int i = 0; i < normals_tex_list.size(); i++)
		{
			matTexture *texture = normals_tex_list[i];
			envSTLHelpers::RemoveOneValue(tex_list, texture);
			matTextureMgr::ReleaseTexture(texture);
		}
		io_Material.NormalsData().RemoveTextures();
	}

	//------------------------------------------------------------------------
	// Remove textures from material and its template
	//------------------------------------------------------------------------
	void RemoveTextures(matMaterial& io_Material,
						entModelInstance& i_ModelInstance)
	{
		RemoveShaderTextures(io_Material, i_ModelInstance);
		RemoveGlowTextures(io_Material, i_ModelInstance);

		io_Material.RemoveTextures();
	}

	//------------------------------------------------------------------------
	// Remove all textures from material and its template
	//------------------------------------------------------------------------
	void RemoveAllTextures(matMaterial& io_Material,
						entModelInstance& i_ModelInstance)
	{
		RemoveShaderTextures(io_Material, i_ModelInstance);
		RemoveGlowTextures(io_Material, i_ModelInstance);
		RemoveNormalsTextures(io_Material, i_ModelInstance);
		RemoveDisplacementTextures(io_Material, i_ModelInstance);

		io_Material.RemoveTextures();
	}

	//--------------------------------------------------------------------
	// Gather fragments from template or object in order to
	//	have list of fragments for altering materials.
	//--------------------------------------------------------------------
	void GatherFragments(scObject *i_pObject, 
						 entModelInstance &i_Template,
						 std::vector<g3dFragment*> &o_Fragments)
	{
		// Check template for fragments first,
		if (i_Template.GetFragments().empty())
		{
			// fragments are not shared, so they will appear in the 
			// scene graph in or under the object's base
			gather_fragments(i_pObject->GetBase(), o_Fragments);
		}
		else
		{
			o_Fragments = i_Template.GetFragments();
		}
	}

	//--------------------------------------------------------------------
	// Gather nodes from the given object that have fragments
	//--------------------------------------------------------------------
	void GatherFragmentNodes(scObject *i_pObject, 
						 std::vector<g3dSceneNode*> &o_Nodes)
	{
		gather_fragment_nodes(i_pObject->GetBase(), o_Nodes);
	}

	//--------------------------------------------------------------------
	// Gather nodes from the given object that are using the 
	//	given material.
	//--------------------------------------------------------------------
	void GatherMaterialNodes(scObject *i_pObject, 
						 const matMaterial *i_pMaterial,
						 std::vector<g3dSceneNode*> &o_Nodes)
	{
		gather_material_nodes(i_pObject->GetBase(), i_pMaterial, o_Nodes);
	}

	//----------------------------------------------------------------------------
	//	Returns true if there are texture differences between the
	//	two material information data structures.
	//----------------------------------------------------------------------------
	//bool DidTextureChange(const mdlMaterialInfo &i_OldMatInfo,
	//					  const mdlMaterialInfo &i_NewMatInfo)
	//{
	//	int num_layers = i_OldMatInfo.GetNumTextureLayers();
	//	if (num_layers != i_NewMatInfo.GetNumTextureLayers())
	//		return true;

	//	for (int i=0; i<num_layers; i++)
	//	{
	//		if (!(i_OldMatInfo.GetTextureLayer(i) == i_NewMatInfo.GetTextureLayer(i)))
	//			return true;
	//	}
	//	// do diff on main shader data textures:

	//	if (!(i_OldMatInfo.GetGlowParams().m_NameGlowMask == i_NewMatInfo.GetGlowParams().m_NameGlowMask))
	//		return true;

	//	// If the material library path changed, have to reload the textures also
	//	// Because the new directory might have new textures with the same names
	//	if (i_OldMatInfo.GetLibraryFilename() != i_NewMatInfo.GetLibraryFilename())
	//		return true;

	//	return false;
	//}

	//------------------------------------------------------------------------
	// Merge Transparent textures from material and its template
	//------------------------------------------------------------------------
	void MergeTransparentTextures(matMaterial& io_Material,	const mdlMaterialInfo& i_MaterialTemplate)
	{
		effParamTexture* pParmTexi = i_MaterialTemplate.GetShaderParams()->FindTextureParam( "transparencyTex" );
		if( !pParmTexi ) pParmTexi = i_MaterialTemplate.GetShaderParams()->FindTextureParam( "transparencyMap" );	//try alternate param name
		effParamTexture* pParmTexo = io_Material.GetShaderParams()->FindTextureParam( "transparencyTex" );
		if( !pParmTexo ) pParmTexo = io_Material.GetShaderParams()->FindTextureParam( "transparencyMap" ); //try alternate param name
		if( pParmTexi && pParmTexo )
		{
			matTexture* pTexi = pParmTexi->GetTexture();
			matTexture* pTexo = pParmTexo->GetTexture();
			if( pTexo && pTexi )
			{
				g2dPFD format( pTexo->GetPixelFormat().GetPixelFormat(), 32 );	//override existing format to 32 Bit RGBA
				matTexture* pRenderTexture = matTextureMgr::CreateRenderTargetTexture( pTexo->GetWidth(), pTexo->GetHeight(), false, &format, true );
				g2dRenderTarget* pRenderTarget = pRenderTexture->GetRenderTargetAPI();
				DBG_ASSERT(pRenderTarget != NULL, "Invalid target texture for MergeTransparentTextures");
				
				if (pRenderTarget)
				{
					matTextureMgr::MergeTransparentTextures(pTexo, pTexi, pRenderTarget);

					matTextureMgr::ReleaseTexture( pTexi );
					pParmTexi->SetTexture( pRenderTexture );
				}
			}
		}
	}

	//----------------------------------------------------------------------------
	//	Set Material properties from structure
	//----------------------------------------------------------------------------
	void SetMaterialData(matMaterial &o_Material,
					const mdlMaterialInfo &i_MaterialTemplate,
					entModelInstance& i_ModelInstance,
					const fsLocator& i_TextureDirectory,
					const fsLocator& i_GeneralTextureDirectory,
					bool i_bForceReload)
	{
		if (!i_bForceReload)
		{
			DBG_ASSERT(i_MaterialTemplate.GetShaderParams() != o_Material.GetShaderParams(), 
				"material and template are sharing same shaderparams object");
			if (i_MaterialTemplate.GetShaderParams() == o_Material.GetShaderParams())
				return;
		}

		o_Material.SetName( i_MaterialTemplate.GetMaterialName() );
		o_Material.UVTransform() = i_MaterialTemplate.GetUVTransform();

		// Construct a fsResourceFinder to use to locate the textures 
		const bool bStrict = true; // has to return false if not found
		fsResourceFinderDir texture_finder(i_TextureDirectory, bStrict);

		// Use resource finder that also searches through material library
		mtrTextureFinder lib_texture_finder(texture_finder);

		//////////////////
		// SURFACE SHADER
		//////////////////
		if (i_MaterialTemplate.GetShaderParams())
		{
			if (i_MaterialTemplate.GetShaderParams() != o_Material.GetShaderParams())
			{
				// reload textures. new textures will come from disk, existing ones are cached.
				LoadShaderTextures(i_MaterialTemplate, lib_texture_finder);
				//Before removing existing textures, check if any alphas require merging.
				MergeTransparentTextures(o_Material, i_MaterialTemplate);
				// since the correct new textures (refcounted) are now loaded, we can safely release the old ones
				RemoveShaderTextures(o_Material, i_ModelInstance);
				// clone shader data into the material (including textures)
				o_Material.SetShaderParams(i_MaterialTemplate.GetShaderParams());
				// re-add textures to model container
				o_Material.GetShaderParams()->GetTextures(i_ModelInstance.Textures());
			}
			else
			{
				// no choice here but to remove first and force a reload.
				// the actual shared ptr data may have been totally overwritten.

				// since the correct new textures (refcounted) are now loaded, we can safely release the old ones
				RemoveShaderTextures(o_Material, i_ModelInstance);
				// reload textures. new textures will come from disk, existing ones are cached.
				LoadShaderTextures(i_MaterialTemplate, lib_texture_finder);

				o_Material.GetShaderParams()->GetTextures(i_ModelInstance.Textures());
			}
		}
		else
		{
			// since pNewData, has the correct new textures (refcounted), we can safely release the old ones
			RemoveShaderTextures(o_Material, i_ModelInstance);
			// clone shader data into the material (including textures)
			shared_ptr<effShaderParams> noParams;
			o_Material.SetShaderParams(noParams);
		}

		
		//////////
		// MATERIAL LAYERS
		//////////

		//TODO bga - This has to do something with the old material layers and their 
		// texture references.
		o_Material.RemoveExtraMaterialLayers();

		// Set shaders for the extra layers above the base material layer
		const int num_layers = i_MaterialTemplate.GetNumMaterialLayers();
		for (int li=1; li<num_layers; ++li)
		{
			// clone shader data into the material (including textures)
			o_Material.AddMaterialLayer();
			o_Material.SetShaderParams(li, i_MaterialTemplate.GetShaderParams(li) );

			// re-add textures to model container
			o_Material.GetShaderParams(li)->GetTextures(i_ModelInstance.Textures());
		}

		//////////
		// REFLECTION
		//////////
		o_Material.SetHasReflection(i_MaterialTemplate.GetHasReflection());
		if (i_MaterialTemplate.GetHasReflection())
		{
			if (!i_bForceReload)
				o_Material.ReflectionData() = i_MaterialTemplate.GetReflectionParams();
		}
		else
		{
		}

		//////////
		// GLOW
		//////////
		if (i_MaterialTemplate.GetHasGlow())
		{
			bool bGlowTextureChanged = (!o_Material.GetHasGlow() ||
				(o_Material.GlowData().m_NameGlowMask != i_MaterialTemplate.GetGlowParams().m_NameGlowMask) ||
				i_bForceReload);
			// remove old glow textures
			if (bGlowTextureChanged)
				RemoveGlowTextures(o_Material, i_ModelInstance);

			o_Material.SetHasGlow(i_MaterialTemplate.GetHasGlow());
			// copy glow data regardless of enabled state
			o_Material.GlowData().m_bConstantGlow = i_MaterialTemplate.GetGlowParams().m_bConstantGlow;
			o_Material.GlowData().m_GlowAmount = i_MaterialTemplate.GetGlowParams().m_GlowAmount;
			o_Material.GlowData().m_GlowScale = i_MaterialTemplate.GetGlowParams().m_GlowScale;
			o_Material.GlowData().m_GlowSize = i_MaterialTemplate.GetGlowParams().m_GlowSize;
			o_Material.GlowData().m_NameGlowMask = i_MaterialTemplate.GetGlowParams().m_NameGlowMask;

			// glow textures
			if (bGlowTextureChanged)
			{
				// load new glow textures
				o_Material.GlowData().ReloadTextures(lib_texture_finder);
				o_Material.GetGlowData().GetTextures(i_ModelInstance.Textures());
			}
		}
		else
		{
			RemoveGlowTextures(o_Material, i_ModelInstance);
			o_Material.SetHasGlow(false);
		}

		//////////
		// OUTLINE
		//////////
		if (i_MaterialTemplate.GetHasOutline())
		{
			o_Material.SetHasOutline(i_MaterialTemplate.GetHasOutline());

			// copy outline data regardless of enabled state
			o_Material.OutlineData().m_OutlineDepthScale = i_MaterialTemplate.GetOutlineParams().m_OutlineDepthScale;
			o_Material.OutlineData().m_OutlineMinAngle = i_MaterialTemplate.GetOutlineParams().m_OutlineMinAngle;
			o_Material.OutlineData().m_OutlineMaxAngle = i_MaterialTemplate.GetOutlineParams().m_OutlineMaxAngle;
			o_Material.OutlineData().m_OutlineThickness = i_MaterialTemplate.GetOutlineParams().m_OutlineThickness;
			o_Material.OutlineData().m_OutlineMinWidth = i_MaterialTemplate.GetOutlineParams().m_OutlineMinWidth;
			o_Material.OutlineData().m_OutlineMaxWidth = i_MaterialTemplate.GetOutlineParams().m_OutlineMaxWidth;
			o_Material.OutlineData().m_OutlineColor = i_MaterialTemplate.GetOutlineParams().m_OutlineColor;
		}
		else
		{
			o_Material.SetHasOutline(false);
		}

		//////////
		// DISPLACEMENT
		//////////
		if (i_MaterialTemplate.GetHasDisplacement())
		{
			bool bDisplacementTextureChanged = (!o_Material.GetHasDisplacement() ||
				(o_Material.DisplacementData().m_NameDisplacementMap != i_MaterialTemplate.GetDisplacementParams().m_NameDisplacementMap) || 
				i_bForceReload);
			// remove old displacement textures
			if( bDisplacementTextureChanged)
				RemoveDisplacementTextures(o_Material, i_ModelInstance);

			o_Material.SetHasDisplacement(i_MaterialTemplate.GetHasDisplacement());
			// copy displacement data regardless of enabled state
			o_Material.DisplacementData().m_Scale = i_MaterialTemplate.GetDisplacementParams().m_Scale;
			o_Material.DisplacementData().m_Bias = i_MaterialTemplate.GetDisplacementParams().m_Bias;
			o_Material.DisplacementData().m_Blur = i_MaterialTemplate.GetDisplacementParams().m_Blur;
//			o_Material.DisplacementData().m_bEnableTessellation = i_MaterialTemplate.GetDisplacementParams().m_bEnableTessellation;
			o_Material.DisplacementData().m_TessellationValue = i_MaterialTemplate.GetDisplacementParams().m_TessellationValue;
			o_Material.DisplacementData().m_NameDisplacementMap = i_MaterialTemplate.GetDisplacementParams().m_NameDisplacementMap;
			o_Material.DisplacementData().m_ObjUVScale = i_MaterialTemplate.GetDisplacementParams().m_ObjUVScale;

			// displacement texture
			if (bDisplacementTextureChanged)
			{
				// load new displacement texture
				o_Material.DisplacementData().ReloadTextures(lib_texture_finder);
				o_Material.GetDisplacementData().GetTextures(i_ModelInstance.Textures());
			}

		}
		else
		{
			o_Material.SetHasDisplacement(false);
			RemoveDisplacementTextures(o_Material, i_ModelInstance);
		}

		//////////
		// NORMAL MAPS
		//////////
		bool bNormalsTextureChanged = ( o_Material.NormalsData().m_NameNormalMap != i_MaterialTemplate.GetNormalsParams().m_NameNormalMap ||
										i_bForceReload);
		// remove old glow textures
		if (bNormalsTextureChanged)
			RemoveNormalsTextures(o_Material, i_ModelInstance);
				
		o_Material.NormalsData().m_BumpScale = i_MaterialTemplate.GetNormalsParams().m_BumpScale;
		o_Material.NormalsData().m_NameNormalMap = i_MaterialTemplate.GetNormalsParams().m_NameNormalMap;

		// glow textures
		if (bNormalsTextureChanged)
		{
			// load new glow textures
			o_Material.NormalsData().ReloadTextures(lib_texture_finder);
			o_Material.GetNormalsData().GetTextures(i_ModelInstance.Textures());
		}

	}

	//----------------------------------------------------------------------------
	//	Set Material properties from structure
	//----------------------------------------------------------------------------
	void SetMaterialLayerData(matMaterial &o_Material,
					int i_LayerIndex,
					const mdlMaterialInfo &i_MaterialTemplate,
					entModelInstance& i_ModelInstance,
					const fsLocator& i_TextureDirectory,
					const fsLocator& i_GeneralTextureDirectory)
	{
		DBG_ASSERT(i_MaterialTemplate.GetShaderParams() != o_Material.GetShaderParams(), 
			"material and template are sharing same shaderparams object");
		if (i_MaterialTemplate.GetShaderParams() == o_Material.GetShaderParams())
			return;

		DBG_ASSERT(i_LayerIndex >= 1, "SetMaterialLayerData incorrectly called on default material layer");
		if (i_LayerIndex < 1)
			return;

		// Construct a fsResourceFinder to use to locate the textures 
		const bool bStrict = true; // has to return false if not found
		fsResourceFinderDir texture_finder(i_TextureDirectory, bStrict);

		// Use resource finder that also searches through material library
		mtrTextureFinder lib_texture_finder(texture_finder);
		
		//////////
		// MATERIAL LAYERS
		//////////

		//TODO dmt - remove old layer texture references before adding new ones.
		// pseudocode:
		//i_ModelInstance.RemoveMaterialLayerTextures(o_Material.GetLayerTextures(i_LayerIndex));

		// clone shader data into the material (including textures)
		o_Material.SetShaderParams(i_LayerIndex, i_MaterialTemplate.GetShaderParams(i_LayerIndex) );

		// re-add textures to model container
		o_Material.GetShaderParams(i_LayerIndex)->GetTextures(i_ModelInstance.Textures());
	}
}	// end of namespace
