/*****************************************************************************
**  emdlImport.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/emdl/emdlImport.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/gf/gfFileX.hpp"
#include "Graphics/an/anKeyAnimation.hpp"
#include "Graphics/emdl/emdlCharacterAnimKeys.hpp"
#include "Graphics/emdl/emdlCharacterTemplate.hpp"
#include "Graphics/emdl/emdlHierTemplate.hpp"
#include "Graphics/emdl/emdlStaticTemplate.hpp"
#include "Graphics/emdl/emdlVertexAnimKeys.hpp"
#include "Graphics/emdl/emdlVertexTemplate.hpp"
#include "Graphics/ent/entLODModelTemplate.hpp"
#include "Graphics/Ent/entModelInstance.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/G3d/g3dFragmentCreate.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/hair/hairImport.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mdl/mdlAnimImport.hpp"
#include "Graphics/mdl/mdlHairInfo.hpp"
#include "Graphics/mdl/mdlImport.hpp"
#include "Graphics/sc/scAnimatableLODObject.hpp"
#include "Graphics/sc/scCompoundObject.hpp"
#include "Graphics/sc/scLODObject.hpp"
#include "Graphics/smdl/smdlCharacterAnimation.hpp"
#include "Graphics/smdl/smdlCharacterSkin.hpp"
#include "Graphics/smdl/smdlHierarchyObject.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"
#include "Graphics/smdl/smdlVertexAnimation.hpp"
#include "Graphics/smdl/smdlVertexObject.hpp"

#include <algorithm>
#include <iterator>
#include <memory>


//============================================================================
//============================================================================
namespace
{
	//----------------------------------------------------------------------------
	// find material by name in the vector of material overrides
	//----------------------------------------------------------------------------
	bool find_override(const std::string &i_MaterialName,
					   const std::vector< shared_ptr<mdlMaterialInfo> > &i_MaterialOverrides,
					   shared_ptr<mdlMatInfo> &o_MatInfo)
	{
		const int num_mats = i_MaterialOverrides.size();
		for (int mi=0; mi<num_mats; mi++)
		{
			if (i_MaterialName == i_MaterialOverrides[mi]->GetMaterialName())
			{
				o_MatInfo.reset(new mdlMatInfo());
				o_MatInfo->m_Info = *i_MaterialOverrides[mi];
				return true;
			}
		}
		return false;
	}
						
	//----------------------------------------------------------------------------
	// Load unique materials for this object using the material information 
	// in the template's material table.
	//----------------------------------------------------------------------------
	void load_unique_materials( const entModelTemplate& i_Template,
								const fsResourceFinder& i_TextureFinder,
								entModelInstance &o_Instance,
								const std::vector< shared_ptr<mdlMaterialInfo> > &i_MaterialOverrides,
								std::map<const matMaterial*,matMaterial*> &o_MaterialRemapping)
	{
		const mdlMatInfoTable &mat_table = i_Template.GetMaterialTable();
		
		const int num_mats = i_Template.GetMaterials().size();
		for (int mi=0; mi<num_mats; mi++)
		{
			const matMaterial *pTemplateMaterial = i_Template.GetMaterials()[mi];
			std::string mat_name = pTemplateMaterial->GetName();

			// This instance's material either comes from the material table
			// or from the material overrides passed in.
			shared_ptr<mdlMatInfo> mat_info;
			if (!find_override(mat_name, i_MaterialOverrides, mat_info))
			{
				mdlMatInfoTable::const_iterator it = mat_table.find(mat_name);
				if ( it != mat_table.end() )
				{
					mat_info = it->second;
				}
			}
			if (mat_info.get())
			{
				std::unique_ptr<matMaterial> new_material(new matMaterial());
				mat_info->LoadTextures(new_material.get(), i_TextureFinder, o_Instance.Textures());
				o_MaterialRemapping[pTemplateMaterial] = new_material.get();
				o_Instance.Materials().push_back( new_material.release() );
			}
		}
	}
	
	//----------------------------------------------------------------------------
	// When the fragments were copied, the are still pointing to the old 
	//	materials. To make them unique, we need to switch over to the
	//  new materials.
	//----------------------------------------------------------------------------
	void switch_fragment_materials(std::vector<g3dFragment*> &io_Fragments, 
								   const std::map<const matMaterial*,matMaterial*> &i_MaterialRemapping)
	{
		const int num_frags = io_Fragments.size();
		for (int fi=0; fi<num_frags; fi++)
		{
			const std::map<const matMaterial*,matMaterial*>::const_iterator it = 
				i_MaterialRemapping.find(io_Fragments[fi]->GetMaterial());
			if ( it != i_MaterialRemapping.end() )
			{
				io_Fragments[fi]->SetMaterial( it->second );
			}
		}
	}

}	// end of namespace


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
emdlImport::emdlImport()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
emdlImport::~emdlImport()
{

}

//----------------------------------------------------------------------------
//  LoadGeometry
//----------------------------------------------------------------------------
entModelTemplate* emdlImport::LoadGeometry( const fsLocator&	i_ModelLocator,
											  //const fsResourceFinder& i_TextureFinder,
											  entFragInfoSink* o_Sink)
{
	//	What kind of model is it?
	if( i_ModelLocator.GetLastName().HasSubString(itString(".mx")) )
	{
		fsResourceTracker::MarkBegin(i_ModelLocator);

		std::unique_ptr<emdlStaticTemplate> mdl_template(new emdlStaticTemplate());

		//	it's a static geometry model
		std::vector<g3dFragment*> low_res_fragments, high_res_fragments;
		mdlImport::LoadWorldFragments(	i_ModelLocator,
										//i_TextureFinder,
										mdl_template->Fragments(),
										low_res_fragments,
										high_res_fragments,
										mdl_template->MaterialTable(),
										mdl_template->Materials(),
										//mdl_template->Textures(),
										o_Sink );

		// If we loaded some low resolution fragments, 
		// put them at the end of the fragment list and set the
		// num low res field to the count.
		int num_low_res = low_res_fragments.size();
		if (num_low_res > 0)
		{
			std::copy(low_res_fragments.begin(), low_res_fragments.end(), 
						std::back_inserter(mdl_template->Fragments()));
		}
		mdl_template->SetNumLowRes(num_low_res);

		// Likewise with high res fragments
		int num_high_res = high_res_fragments.size();
		if (num_high_res > 0)
		{
			std::copy(high_res_fragments.begin(), high_res_fragments.end(), 
						std::back_inserter(mdl_template->Fragments()));
		}
		mdl_template->SetNumHighRes(num_high_res);

		fsResourceTracker::MarkEnd(i_ModelLocator);
		return mdl_template.release();
	}
	else if( (i_ModelLocator.GetLastName().HasSubString(itString(".gxb"))) ||
			 (i_ModelLocator.GetLastName().HasSubString(itString(".mhx"))) ||
			 (i_ModelLocator.GetLastName().HasSubString(itString(".chx"))))
	{
		fsResourceTracker::MarkBegin(i_ModelLocator);

		//	Use character template for all generalized hierarchical models now.
		std::unique_ptr<emdlCharacterTemplate> mdl_template(new emdlCharacterTemplate());

		g3dSceneNode* pBase;
		//std::vector<smdlJoint*> joint_roots;
		mdlImport::LoadGeneralHierarchicalModel(	i_ModelLocator,
													//i_TextureFinder,
													pBase,
													//joint_roots,
													mdl_template->SkinnedSurfaces(),
													mdl_template->HairSurfaces(),
													mdl_template->Fragments(),
													mdl_template->MaterialTable(),
													mdl_template->Materials(),
													//mdl_template->Textures(),
													o_Sink);

		mdl_template->SetModel( pBase );

		//TODO: Handle multiple joint chains read from a single file:
		//mdl_template->SetJoints( joint_roots );
		//DBG_ASSERT(joint_roots.size() <= 1, "Not ready for multiple joint chains yet!");
		//if (joint_roots.size() == 1)
		//	mdl_template->SetJoints( joint_roots[0] );

		mdl_template->SetFilename( i_ModelLocator );
		fsResourceTracker::MarkEnd(i_ModelLocator);
		return mdl_template.release();
	}
	//else if( i_ModelLocator.GetLastName().HasSubString(itString(".mhx")))
	//{
	//	fsResourceTracker::MarkBegin(i_ModelLocator);

	//	//	it's a hierarchical model
	//	std::auto_ptr<emdlHierTemplate> mdl_template(new emdlHierTemplate());

	//	g3dSceneNode* pBase;
	//	mdlImport::LoadHierarchicalModel(	i_ModelLocator,
	//										i_TextureFinder,
	//										pBase,
	//										mdl_template->Fragments(),
	//										mdl_template->Materials(),
	//										mdl_template->Textures(),
	//										o_Sink);

	//	mdl_template->SetModel( pBase );
	//	fsResourceTracker::MarkEnd(i_ModelLocator);
	//	return mdl_template.release();
	//}
	//else if( i_ModelLocator.GetLastName().HasSubString(itString(".chx")) )
	//{
	//	fsResourceTracker::MarkBegin(i_ModelLocator);

	//	//	it's a character model (single skeleton, multiple meshes, morph targets)
	//	std::auto_ptr<emdlCharacterTemplate> mdl_template(new emdlCharacterTemplate());

	//	smdlJoint* pJoint = NULL;
	//	mdlImport::LoadCharacterModel(	i_ModelLocator,
	//									i_TextureFinder,
	//									pJoint,
	//									mdl_template->CharacterSkins(),
	//									mdl_template->SubdivInfos(),
	//									mdl_template->FragInfos(),
	//									mdl_template->Fragments(),
	//									mdl_template->Materials(),
	//									mdl_template->Textures(),
	//									o_Sink);

	//	if (!mdl_template->Fragments().empty())
	//	{
	//		DBG_LOG("Loaded " << mdl_template->Fragments().size() << " embedded fragments from character model." );
	//	}

	//	mdl_template->SetJoints( pJoint );
	//	mdl_template->SetFilename( i_ModelLocator );
	//	fsResourceTracker::MarkEnd(i_ModelLocator);
	//	return mdl_template.release();
	//}
	else if( i_ModelLocator.GetLastName().HasSubString(itString(".jnx")) )
	{
		fsResourceTracker::MarkBegin(i_ModelLocator);

		//	it's a single-skin model
		//emdlSkinTemplate *mdl_template = new emdlSkinTemplate();

		// routing all old single skin files into the more general
		// character model type.
		std::unique_ptr<emdlCharacterTemplate> mdl_template( new emdlCharacterTemplate() );
		// create one skin to hold the single skin model
		mdl_template->SkinnedSurfaces().resize( 1 );
		mdlSkinInfo &single_skin_info = mdl_template->SkinnedSurfaces()[0];
		single_skin_info.m_SkinInfo.reset( new smdlCharacterSkin() );
		smdlCharacterSkin &skin_ref = *single_skin_info.m_SkinInfo;

		g3dSceneNode* pJoint = NULL;
		std::vector<mdlFragInfo> frag_infos;
		mdlImport::LoadSingleSkinModel(	i_ModelLocator,
										//i_TextureFinder,
										pJoint,
										skin_ref.m_BoneVertices,
										frag_infos,
										mdl_template->MaterialTable(),
										mdl_template->Materials(),
										//mdl_template->Textures(),
										o_Sink);

		mdl_template->SetModel( pJoint );

		DBG_ASSERT(frag_infos.size() == 1, "Old Single skin models had to have only a single fragment.");
		single_skin_info.m_MeshInfo.reset(new mdlFragInfo( frag_infos[0]));

		// leave the scene node name of the single_skin_info as empty, will attach to root.

		fsResourceTracker::MarkEnd(i_ModelLocator);
		return mdl_template.release();
	}
	else if( i_ModelLocator.GetLastName().HasSubString(itString(".vtx")) )
	{
		fsResourceTracker::MarkBegin(i_ModelLocator);

		std::unique_ptr<emdlVertexTemplate> mdl_template(new emdlVertexTemplate());

		//	multiple fragments, load just infos
		mdlImport::LoadFragInfos(	i_ModelLocator,
									//i_TextureFinder,
									mdl_template->FragInfos(),
									mdl_template->MaterialTable(),
									mdl_template->Materials(),
									//mdl_template->Textures(),
									o_Sink );
		fsResourceTracker::MarkEnd(i_ModelLocator);
		return mdl_template.release();
	}
#ifdef HAIR_SUPPORTED
	else if( i_ModelLocator.GetLastName().HasSubString(itString(".hair")) )
	{
		fsResourceTracker::MarkBegin(i_ModelLocator);

		std::unique_ptr<emdlCharacterTemplate> mdl_template(new emdlCharacterTemplate());

		shared_ptr<mdlHairInfo> hair_info(new mdlHairInfo);
		hairImport::LoadHair( i_ModelLocator,
							  *hair_info,
							  mdl_template->MaterialTable(),
							  mdl_template->Materials() );
		mdl_template->HairSurfaces().push_back(hair_info);

		fsResourceTracker::MarkEnd(i_ModelLocator);
		return mdl_template.release();
	}
#endif

	// return NULL and let next implementation try to handle it
	return NULL;
}


//----------------------------------------------------------------------------
// Creates scObject as instance of the template. Data to be stored
// by the instance are returned in the o_Instance argument.
// A list of material overrides can be passed in to alter the
// creation of materials for this specific instance.
//----------------------------------------------------------------------------
scObject* emdlImport::CreateObject( const entModelTemplate& i_Template,
								    entModelInstance &o_Instance,
									const fsResourceFinder& i_TextureFinder,
									const std::vector< shared_ptr<mdlMaterialInfo> > &i_MaterialOverrides )
{
	// Load unique materials for this object using the material information 
	// in the template's material table.
	std::map<const matMaterial*,matMaterial*> material_remapping;
	load_unique_materials(i_Template, i_TextureFinder, o_Instance, i_MaterialOverrides, material_remapping);

	//	hierarchical
	//
	const emdlHierTemplate *hier_template = dynamic_cast<const emdlHierTemplate*>(&i_Template);
	if ( hier_template )
	{
		std::unique_ptr<g3dSceneNode> new_root(hier_template->GetModel()->Clone(o_Instance.Fragments()));
		switch_fragment_materials(o_Instance.Fragments(), material_remapping);
		return new smdlHierarchyObject( new_root.release() );
	}

	//	vertex animation
	//
	const emdlVertexTemplate *vertex_template = dynamic_cast<const emdlVertexTemplate*>(&i_Template);
	if ( vertex_template )
	{
		return new smdlVertexObject( vertex_template->GetFragInfos(), material_remapping );
	}

	//	character - subdivs with morph targets on joints
	//
	const emdlCharacterTemplate *char_template = dynamic_cast<const emdlCharacterTemplate*>(&i_Template);
	if ( char_template )
	{
		//smdlJoint* pJointTree = char_template->GetJoints()->Clone();
		//DBG_ASSERT( pJointTree, "expected smdlJoint to be created");
		g3dSceneNode* pRootNode = (char_template->GetModel()) ? 
				char_template->GetModel()->Clone(o_Instance.Fragments()) : new g3dSceneNode();
		DBG_ASSERT( pRootNode, "expected root node to be cloned");
		if (!pRootNode)
			pRootNode = new g3dSceneNode();
		switch_fragment_materials(o_Instance.Fragments(), material_remapping);

		// pass just the frag_infos to the model, let it handle the fragments
		// needed to do the lod versions of the subdivision.
		smdlSubdivCharacter* jt_obj = new smdlSubdivCharacter(	pRootNode,
														char_template->GetSkinnedSurfaces(), 
														char_template->GetHairSurfaces(), 
														material_remapping,
														char_template->GetFilename() );
		return jt_obj;
	}

	//	LOD model
	//
	//const entLODModelTemplate *lod_template = dynamic_cast<const entLODModelTemplate*>(&i_Template);
	//if ( lod_template )
	//{
	//	//	loop through and call this function for each template
	//	//
	//	int size = lod_template->GetNumberOfTemplates();
	//	int i;

	//	scAnimatableLODObject* pALODObj = 0;
	//	scLODObject* pLODObj = 0;

	//	for ( i = 0 ; i < size ; i++ )
	//	{
	//		scObject* pObj = emdlImport::CreateObject( *(lod_template->GetTemplate( i )) );
	//		scAnimatableObject* pAObj = dynamic_cast<scAnimatableObject*>( pObj );

	//		if ( pAObj != 0 )
	//		{
	//			if ( pALODObj == 0 )
	//			{
	//				DBG_ASSERT( pLODObj == 0, "model choices trying to mix animatable + nonanimatable" );

	//				pALODObj = new scAnimatableLODObject();
	//			}

	//			float dist = lod_template->GetLODDistance(i);
	//			bool bDefault = lod_template->GetLODDefault(i);
	//			pALODObj->AddObject( pAObj, dist , bDefault );
	//		}
	//		else
	//		{
	//			if ( pLODObj == 0 )
	//			{
	//				DBG_ASSERT( pALODObj == 0, "model choices trying to mix animatable + nonanimatable" );

	//				pLODObj = new scLODObject();
	//			}

	//			float dist = lod_template->GetLODDistance(i);
	//			bool bDefault = lod_template->GetLODDefault(i);
	//			pLODObj->AddObject( pObj, dist , bDefault );
	//		}
	//	}

	//	if ( pALODObj != 0 )
	//		return pALODObj;
	//	else
	//		return pLODObj;
	//}

	// Do we know it is simple, or could it be another entity type?
	//
	{
		// Clone all the fragments and use the new ones when creating the object
		int num_frags = i_Template.GetFragments().size();
		o_Instance.Fragments().resize(num_frags);
		for (int i=0; i<num_frags; ++i)
		{
			g3dFragment* pCloneFrag = g3dFragmentCreate::CloneFragment(i_Template.GetFragments()[i]);
			o_Instance.Fragments()[i] = pCloneFrag;
		}
		// Switch materials on the new fragments
		switch_fragment_materials(o_Instance.Fragments(), material_remapping);

		// Create the scObject depending on how many fragments
		if (num_frags == 1)
		{
			return new scObject( o_Instance.GetFragments()[0] );
		}
		else
		{
			const emdlStaticTemplate *static_template = dynamic_cast<const emdlStaticTemplate*>(&i_Template);
			if ( static_template )
			{
				return new scCompoundObject( o_Instance.GetFragments(), 
											 static_template->GetNumLowRes(), 
											 static_template->GetNumHighRes() );
			}
			else
			{
				return new scCompoundObject( o_Instance.GetFragments() );
			}
		}
	}

	//return NULL;
}


//----------------------------------------------------------------------------
// Load shared animation key info from file
//----------------------------------------------------------------------------
entAnimKeys* emdlImport::LoadAnimKeys(const fsLocator& i_AnimLocator)
{
	//
	if ( i_AnimLocator.GetLastName().HasSubString(itString(".jna"))  ||
		 i_AnimLocator.GetLastName().HasSubString(itString(".mha")))
	{
		// Hierarchical animation
		std::unique_ptr< emdlAnimKeys > keys(new emdlAnimKeys);
		//std::string root_name;
		float fps = g3dConstants::c_fDefaultFrameRate;
		fsResourceTracker::MarkBegin(i_AnimLocator);
		mdlAnimImport::LoadAnimation(	i_AnimLocator,
									keys->KeyRoots(),
									keys->TranslateChannels(),
									keys->RotateChannels(),
									keys->ScaleChannels(),
									keys->VisibleChannels(),
									fps);
		//keys->SetNameOfRoot(root_name);
		keys->SetFramesPerSecond( fps );
		fsResourceTracker::MarkEnd(i_AnimLocator);
		return keys.release();
	}
	else if ( i_AnimLocator.GetLastName().HasSubString(itString(".gab")) ||
			  i_AnimLocator.GetLastName().HasSubString(itString(".cha")))
	{
		// Characters animation
		std::unique_ptr< emdlCharacterAnimKeys > keys(new emdlCharacterAnimKeys);
		//std::string root_name;
		bool delta_animation = false;
		float fps = g3dConstants::c_fDefaultFrameRate;
		float begin_frame = 0;
		fsResourceTracker::MarkBegin(i_AnimLocator);
		mdlAnimImport::LoadCharacterAnimation(	i_AnimLocator,
									keys->KeyRoots(),
									keys->TranslateChannels(),
									keys->RotateChannels(),
									keys->ScaleChannels(),
									keys->VisibleChannels(),
									delta_animation,
									fps,
									begin_frame,
									keys->MorphKeys(),
									keys->MorphChannels(),
									keys->VertexKeys(),
									keys->VertexFrames(),
									keys->SkinKeys());
		//keys->SetNameOfRoot(root_name);
		keys->SetAdditiveAnimation(delta_animation);
		keys->SetFramesPerSecond( fps );
		if (fps > 0) keys->SetStartTime( begin_frame / fps );
		// Old CHA files need to attach to root joint
		keys->SetAttachToRootJoint(i_AnimLocator.GetLastName().HasSubString(itString(".cha")));
		fsResourceTracker::MarkEnd(i_AnimLocator);

		if (!keys->VertexKeys().empty())
		{
			// Vertex animation was loaded, reallocate the way memory is used.
			vtxVertexAnimBudget::ReallocateBudget();
		}

		return keys.release();
	}
	else if ( i_AnimLocator.GetLastName().HasSubString(itString(".vta")) )
	{
		// Baked vertex animation
		std::unique_ptr< emdlVertexAnimKeys > keys(new emdlVertexAnimKeys);
		float fps = g3dConstants::c_fDefaultFrameRate;
		fsResourceTracker::MarkBegin(i_AnimLocator);
		mdlAnimImport::LoadVertexAnimation(	i_AnimLocator,
									keys->VertexKeys(),
									keys->VertexFrames(),
									fps);
		keys->SetFramesPerSecond( fps );
		fsResourceTracker::MarkEnd(i_AnimLocator);
		// Vertex animation was loaded, reallocate the way memory is used.
		vtxVertexAnimBudget::ReallocateBudget();
		return keys.release();
	}
	//else if ( i_AnimLocator.GetLastName().HasSubString(itString(".htr")) )
	//{
	//	// Motion capture ascii - Hierarchical animation
	//	std::auto_ptr< emdlAnimKeys > keys(new emdlAnimKeys);
	//	fsResourceTracker::MarkBegin(i_AnimLocator);
	//	mcpHTRData htr_data;
	//	mcpHTRParser::ReadData(i_AnimLocator, htr_data);
	//	mcpHTRParser::ConvertAnimation(	htr_data,
	//								keys->Keys(),
	//								keys->TranslateChannels(),
	//								keys->RotateChannels(),
	//								keys->ScaleChannels());
	//	// HTR files have full animation built in, and do not use
	//	//	the joint orientation in g3dSceneNode that comes from Maya.
	//	keys->SetIgnoreJointOrientation(true);
	//	keys->SetFramesPerSecond( (float) htr_data.m_HeaderData.m_FrameRate );
	//	fsResourceTracker::MarkEnd(i_AnimLocator);
	//	return keys.release();
	//}

	return NULL;
}

//----------------------------------------------------------------------------
// Create animation from key data
//----------------------------------------------------------------------------
entAnimation* emdlImport::CreateAnimation(const entAnimKeys &i_KeyData)
{

	const emdlCharacterAnimKeys *charkey_data = dynamic_cast<const emdlCharacterAnimKeys*>(&i_KeyData);
	if ( charkey_data )
	{
		smdlCharacterAnimation* ret_val = new smdlCharacterAnimation(charkey_data->GetKeyRoots(),
			charkey_data->GetMorphKeys(),
			charkey_data->GetVertexKeys(),
			charkey_data->GetSkinKeys());
		ret_val->SetAttachToRootJoint(charkey_data->GetAttachToRootJoint());
		ret_val->SetLooping(true);
		//ret_val->SetNameOfRoot( charkey_data->GetNameOfRoot() );
		ret_val->SetAdditiveAnimation( charkey_data->GetAdditiveAnimation() );
		ret_val->SetFrameRate( charkey_data->GetFramesPerSecond() );
		return ret_val;
	}

	const emdlAnimKeys *sckey_data = dynamic_cast<const emdlAnimKeys*>(&i_KeyData);
	if ( sckey_data )
	{
		// Can use smdlGeoAnimKeys for hierarchical and
		// single skin animations
		smdlGeoFrameAnimation* ret_val = new smdlGeoFrameAnimation(sckey_data->GetKeyRoots());
		ret_val->SetLooping(true);
		//ret_val->SetNameOfRoot( sckey_data->GetNameOfRoot() );
		ret_val->SetAdditiveAnimation( sckey_data->GetAdditiveAnimation() );
		ret_val->SetIgnoreJointOrientation( sckey_data->GetIgnoreJointOrientation() );
		ret_val->SetFrameRate( sckey_data->GetFramesPerSecond() );
		return ret_val;
	}

	const emdlVertexAnimKeys *vertkey_data = dynamic_cast<const emdlVertexAnimKeys*>(&i_KeyData);
	if ( vertkey_data )
	{
		// Baked vertex animation
		smdlVertexAnimation* ret_val = new smdlVertexAnimation(vertkey_data->GetVertexKeys());
		ret_val->SetLooping(true);
		ret_val->SetFrameRate( vertkey_data->GetFramesPerSecond() );
		return ret_val;
	}

	return NULL;
}

