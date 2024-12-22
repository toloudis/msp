/*****************************************************************************
**	fgmtScriptObject.cpp
**
**	This class handles altering the fragment flags within a script object
**	after loading
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/fgmt/fgmtScriptObject.hpp"

#include "Support/fgmt/fgmtFragmentData.hpp"
#include "Support/fgmt/GUI/fgmtOperations.hpp"
#include "Support/fgmt/GUI/fgmtPropertyObject.hpp"
#include "Support/fgmt/wxGUI/fgmtUVEditor.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/Fs/fsFileEnum.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceFinderDir.hpp"
#include "Core/fs/fsResourceFinderJoin.hpp"
#include "Core/fs/fsResourceTrackerData.hpp"
#include "Core/gf/gfFileTxt.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/rel/relRelationshipMultiple.hpp"
#include "Graphics/eff/effBakeData.hpp"
#include "Graphics/eff/effNormalsData.hpp"
#include "Graphics/eff/effOcclusionData.hpp"
#include "Graphics/eff/effTexturedData.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dSceneTraverse.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Support/mtrl/mtrlScriptObject.hpp"
#include "Tool/api3d/api3dObject.hpp"
#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Tool/api3d/api3dObjectGeom.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiPropertyGrid.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"

#include <map>
#include <sstream>


//============================================================================
//============================================================================
namespace
{
	//====================================================================
	//====================================================================
	fsLocator find_texture_path(const fsLocator &i_Locator)
	{
		// just return directory from filename for now
		fsLocator dir = i_Locator;
		dir.Pop();

		fsLocator tex_dir = dir;
		tex_dir.Pop();
		tex_dir.Push("Textures");
		if (fsFileUtil::DirectoryExists(tex_dir))
			return tex_dir;

		return dir;
	}

	//====================================================================
	// gather fragments from scene node tree
	//====================================================================
	void gather_fragments(g3dSceneNode *i_Node, 
						  std::vector<fgmtFragmentData> &o_FragmentData,
						  std::vector<g3dFragment*> &o_Fragments,
						  std::vector<g3dSceneNode*> &o_FragmentNodes,
						  std::vector<matMaterial*> &o_Materials,
						  const std::string& i_LastStringName)
	{
		// Don't gather the low resolution fragments
		if (i_Node->GetContentResolution() == g3dSceneNode::e_LowRes)
			return;

		// Skip nodes with wireframe draw style because these flags
		// aren't relevant for non-solid draw styles, and because the 
		// joint display fragments shouldn't be displayed
		if ((i_Node->GetDrawStyle() != g3dSceneNode::e_Inherit) &&
			(i_Node->GetDrawStyle() != g3dSceneNode::e_Solid))
			return;

		// Track the last node name we encounter as we traverse down the tree
		// in order to share the node names when there are multiple materials
		std::string node_name = i_LastStringName;
		if (*i_Node->GetName())
			node_name = i_Node->GetName();

		if (i_Node->GetFragment())
		{
			// Gather fragment pointer and data
			g3dFragment *pFragment = i_Node->GetFragment();
			o_Fragments.push_back(pFragment);
			o_Materials.push_back(pFragment->GetMaterial());

			o_FragmentNodes.push_back(i_Node);

			fgmtFragmentData frg_data;
			frg_data.SetFragmentName( node_name );
			frg_data.m_bCastsShadow.SetValue( pFragment->GetCastsShadow() );
			frg_data.m_bReceivesShadow.SetValue( pFragment->GetReceivesShadow() );
			frg_data.m_bShadowHull.SetValue( pFragment->IsShadowHull() );
			frg_data.m_bDoubleSided.SetValue( pFragment->GetDoubleSided() );

			//effOcclusionData* aodata = pFragment->GetOcclusionData();
			//frg_data.m_bAOInherit.SetValue( aodata->m_bInheritParent );
			frg_data.m_bIsOccluder.SetValue( pFragment->GetCastsOcclusion() );
			frg_data.m_bReceivesOcclusion.SetValue( pFragment->GetReceivesOcclusion() );
			frg_data.m_bReceivesGI.SetValue( pFragment->GetReceivesGI() );
			//frg_data.m_bSiblingOccludeOnly.SetValue( aodata->m_bSiblingOcclude );
			//frg_data.m_bSelfOccludeOnly.SetValue( aodata->m_bSelfOcclude );
			//frg_data.m_bStaticAO.SetValue( aodata->m_bStatic );
			//frg_data.m_nSamples.SetValue( aodata->m_nSamples );
			//frg_data.m_SamplingResolution.SetValue( aodata->m_SamplingResolution );
			//frg_data.m_DepthBias.SetValue( aodata->m_DepthBias );
			//frg_data.m_AOTextureResolution.SetValue( aodata->m_Resolution );

			//frg_data.m_AOBlendFactor.SetValue( aodata->m_BlendFactor );
			//frg_data.m_AOUserTextureName.SetValue( itString(aodata->m_TextureName.c_str()) );

			o_FragmentData.push_back(frg_data);
		}

		int num_kids = i_Node->GetNumChildren();
		for (int k=0; k<num_kids; k++)
			gather_fragments(i_Node->GetChild(k), o_FragmentData, o_Fragments, o_FragmentNodes, o_Materials, node_name);
	}

	void get_fragment_strings(fgmtFragmentData& i_Data, 
		g3dFragment* i_pFragment, int i_Index,
		std::string* o_MaterialName, std::string* o_FragmentName,
		std::string* o_FragmentIndex)
	{
		if (o_MaterialName != NULL)
		{
			if (matMaterial *pMat = i_pFragment->GetMaterial())
			{
				*o_MaterialName = pMat->GetName();
			}
		}

		if (o_FragmentName != NULL)
		{
			if (!i_Data.GetFragmentName().empty())
			{
				*o_FragmentName = i_Data.GetFragmentName() + std::string("_Frag") + std::to_string(i_Index);
			}
			else
			{
				//static char local_buffer[256];
				//sprintf(local_buffer, "Fragment #%d", i_Index);
				// the code below always produce Fragment #0
				//std::ostringstream oss;
				//oss.setf(0, std::ios::floatfield);
				//oss.setf(std::ios::fixed, std::ios::floatfield);
				//oss<<"Fragment #"<<i_Index;
				//static std::string local_buffer(oss.str());
				//*o_FragmentName = local_buffer.c_str();
				*o_FragmentName = std::string("_Frag") + std::to_string(i_Index);
			}
		}

		if (o_FragmentIndex != NULL)
		{
			//static char local_buffer[256];
			//sprintf(local_buffer, "F%d", i_Index);
			// the code below always produce F#0
			/*std::ostringstream oss;
			oss.setf(0, std::ios::floatfield);
			oss.setf(std::ios::fixed, std::ios::floatfield);
			oss<<"F#"<<i_Index;
			static std::string local_buffer(oss.str());				
			*o_FragmentIndex = local_buffer.c_str();*/
			*o_FragmentIndex = std::string("F#") + std::to_string(i_Index);
		}

	}

	//====================================================================
	// construct a display name for the given fragment and data
	//====================================================================
	std::string construct_fragment_name(fgmtFragmentData& i_Data, 
										g3dFragment* i_pFragment,
										int i_Index)
	{
		std::string mat_name, frg_name, constructed_name;
		get_fragment_strings(i_Data, i_pFragment, i_Index, &mat_name, &frg_name, NULL);
		

		// Construct full name from fragment and material names
		constructed_name = frg_name + std::string(" : ") + mat_name;

		i_pFragment->SetFragmentName( frg_name );

		return constructed_name;
	}

	//====================================================================
	// construct a bake texture name for the given fragment and data
	//====================================================================
	std::string construct_fragment_bake_name(fgmtFragmentData& i_Data, 
										g3dFragment* i_pFragment,
										int i_Index,
										std::string i_modelName,
										std::string i_baseName)
	{
		std::string mat_name, frg_name, index_name;
		get_fragment_strings(i_Data, i_pFragment, i_Index, &mat_name, &frg_name, &index_name);
		

		// Construct full name from fragment and material names
		/*return i_baseName + std::string("-") + i_modelName + std::string("_") + 
				index_name + std::string("_") + mat_name;*/
		return frg_name + std::string("-") + mat_name;
	}

	//====================================================================
	// construct a file name for the given fragment and data
	//====================================================================
	std::string construct_fragment_texture_name(fgmtFragmentData& i_Data, 
		sel3dObject* i_pParent,
		g3dFragment* i_pFragment,
		int i_Index,
		bool i_bUseModelName,
		const std::string& i_ModelName)
	{
		std::string obj_name;
		if (i_bUseModelName && !i_ModelName.empty())
			obj_name = i_ModelName;
		else if (i_pParent != NULL)
			obj_name = i_pParent->GetDisplayName();
		else
			obj_name = "Unnamed";

		std::string mat_name, frg_name, frg_num;
		get_fragment_strings(i_Data, i_pFragment, i_Index, &mat_name, &frg_name, &frg_num);

		// Construct full name from fragment and material names
		return obj_name + std::string("_") + frg_num + std::string("_") + frg_name + std::string("_") + mat_name;
	}

	//====================================================================
	// construct a file name for the given fragment and data
	//====================================================================
	std::string construct_fragment_AOtexture_name(fgmtFragmentData& i_Data, 
		sel3dObject* i_pParent,
		g3dFragment* i_pFragment,
		int i_Index, 
		bool i_bSceneSpecific,
		const std::string& i_ModelName)
	{
		std::string name;
		name = "AO_" + construct_fragment_texture_name(i_Data, i_pParent, i_pFragment, 
			i_Index, !i_bSceneSpecific, i_ModelName) + ".dds";
		return name;
	}

	//====================================================================
	// gather fragments from scene node tree
	//====================================================================
	void gather_fragments(g3dSceneNode *i_Node, 
						  std::vector<g3dFragment*> &o_Fragments)
	{
		// Don't gather the low resolution fragments
		if (i_Node->GetContentResolution() == g3dSceneNode::e_LowRes)
			return;

		// Skip nodes with wireframe draw style because these flags
		// aren't relevant for non-solid draw styles, and because the 
		// joint display fragments shouldn't be displayed
		if ((i_Node->GetDrawStyle() != g3dSceneNode::e_Inherit) &&
			(i_Node->GetDrawStyle() != g3dSceneNode::e_Solid))
			return;

		if (i_Node->GetFragment())
		{
			// Gather fragment pointer and data
			g3dFragment *pFragment = i_Node->GetFragment();
			o_Fragments.push_back(pFragment);
		}

		int num_kids = i_Node->GetNumChildren();
		for (int k=0; k<num_kids; k++)
			gather_fragments(i_Node->GetChild(k), o_Fragments);
	}

}	// end of namespace

//--------------------------------------------------------------------
// Constructor takes object of which to override fragments and
//	also locator of model in order to read/write fragments.
//--------------------------------------------------------------------
fgmtScriptObject::fgmtScriptObject(api3dObject* i_pObject,
								   const fsLocator& i_ModelLocator,
								   std::string& i_BaseName)
: m_pObject(i_pObject),
	m_pParent(NULL)
	//m_LastSelectedFragmentIndex(-1),
	//m_PrtyBake("Enable Baking", false)
{
	m_ModelTextureLocator = find_texture_path(i_ModelLocator);
	DBG_ASSERT(i_pObject, "Null api3dObject.");
	
	itString modelName = i_ModelLocator.GetLastName();
	modelName.StripExtension();
	m_ModelName = itStringUtil::GetStdString(modelName);
	m_BaseName = i_BaseName;
	GatherFragments();
	for (int i = 0; i < m_FragmentUI.size(); i++)
	{
		m_FragmentUI[i]->SetModelTextureLocator(m_ModelTextureLocator);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
fgmtScriptObject::~fgmtScriptObject()
{
	// object and fragment pointers are not owned
	envSTLHelpers::DeleteContainer(m_FragmentUI);
	ReleaseBakedMaterials();
}

//--------------------------------------------------------------------
// Set Parent pointer to use when creating selectable 
// property objects
//--------------------------------------------------------------------
void fgmtScriptObject::SetParent(sel3dObject *i_pParent)
{
	m_pParent = i_pParent;

	// Create new multiple relationship connecting the property
	// objects to the new parent object
	//
	m_ParentRelationship.reset(
		new relRelationshipMultiple<fgmtPropertyObject>("Surfaces", *i_pParent, m_FragmentUI));
	i_pParent->AddRelationship(m_ParentRelationship);

	// update fragments if already "gathered".
	for (int i = 0; i < m_FragmentUI.size(); i++)
	{
		m_FragmentUI[i]->SetParentRelationship(m_ParentRelationship);
	}

}

//--------------------------------------------------------------------
// Set the ReceiveAO flag through context menu
//--------------------------------------------------------------------

void fgmtScriptObject::SetReceivesAO(int i_index,bool i_AO)
{
	try
	{
		// update fgmt prty data if it exists!
		int num_frags = m_FragmentData.size();

		if(i_index != -1)
		{
			fgmtFragmentData &frg_data = m_FragmentData[i_index];
			frg_data.m_bReceivesOcclusion.SetValue(i_AO);
		}
		else
		{

			for (int i=0; i<num_frags; ++i)
			{
				fgmtFragmentData& data = m_FragmentData[i];
				if (data.m_bAOInherit.GetValue())
				{
					data.m_bReceivesOcclusion.SetValue(i_AO);
				}
			}
		}
	}
	catch (const g2dOutOfVideoMemoryX& )
	{
		guiMessageBox::Show("Out of video memory creating Ambient Occlusion textures. Try reducing texture resolution or removing AO from some surfaces.", "Error", guiMessageBox::e_OKOnly);
	}
}

//--------------------------------------------------------------------
// Set the ShadowHull on/off flag through context menu
//--------------------------------------------------------------------
void fgmtScriptObject::SetShadowHull(int i_index,bool i_ShadowHull)
{
	int num_frags = m_FragmentData.size();
	fgmtFragmentData &frg_data = m_FragmentData[i_index];
	frg_data.m_bShadowHull.SetValue(i_ShadowHull);
}


//--------------------------------------------------------------------
// Set pointer of UI container for prty info.
//--------------------------------------------------------------------
void fgmtScriptObject::SetPrtyObject(prtyObject* i_pParent)
{
	m_pPrtyObject = i_pParent;

	// while we're in here, set up the AO prty's.

//	prtyPropertyUIInfo* pPUII;
//	m_pPrtyObject->AddProperty( new prtyCheckBoxUIInfo(&(m_AOData.m_bInherit), "Ambient Occlusion", "Inherit from global settings"));
//	m_pPrtyObject->AddProperty( new prtyCheckBoxUIInfo(&(m_AOData.m_bIsOccluder), "Ambient Occlusion", "Set as occluder for all AO computations."));
	m_pPrtyObject->AddProperty( new prtyCheckBoxUIInfo(&(m_AOData.m_bReceivesOcclusion), "Ambient Occlusion", "Set to have AO computed for this character"));
	m_pPrtyObject->AddProperty( new prtyCheckBoxUIInfo(&(m_AOData.m_bUseBakedTexture), "Baked Texture", "Set use baked texture"));
/*
	m_pPrtyObject->AddProperty( new prtyCheckBoxUIInfo(&(m_AOData.m_bSelfOccludeOnly), "Ambient Occlusion", "Receive AO only from fragments in this object"));
	m_pPrtyObject->AddProperty( new prtyCheckBoxUIInfo(&(m_AOData.m_bStatic), "Ambient Occlusion", "Do not compute per frame. Reuse initial result"));
	pPUII = new prtyNumericUpDownUIInfo(&(m_AOData.m_nSamples), "Ambient Occlusion", "Number of samples (higher=better,slower)");
	m_pPrtyObject->AddProperty( pPUII );
	prtyComboBoxUIInfo* pCBUII;
	pCBUII = new prtyComboBoxUIInfo(&(m_AOData.m_SamplingResolution), "Ambient Occlusion", "Resolution of sample texture (higher=better, slower)");
	pCBUII->AddItem(std::string("256"), 0);
	pCBUII->AddItem(std::string("512"), 1);
	pCBUII->AddItem(std::string("1024"), 2);
	pCBUII->AddItem(std::string("2048"), 3);
	pCBUII->AddItem(std::string("4096"), 4);
	m_pPrtyObject->AddProperty( pCBUII );
	prtyRangedFloatUIInfo* pRFUII  ;
	pRFUII  = new prtyRangedFloatUIInfo(&(m_AOData.m_DepthBias), "Ambient Occlusion", "Per-sample depth bias");
	pRFUII->SetMinimum(0);
	pRFUII->SetMaximum(0.5f);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(100);
	m_pPrtyObject->AddProperty( pRFUII );
	pCBUII = new prtyComboBoxUIInfo(&(m_AOData.m_TextureResolution), "Ambient Occlusion", "Default resolution of AO target texture");
	pCBUII->AddItem(std::string("256"), 0);
	pCBUII->AddItem(std::string("512"), 1);
	pCBUII->AddItem(std::string("1024"), 2);
	pCBUII->AddItem(std::string("2048"), 3);
	pCBUII->AddItem(std::string("4096"), 4);
	m_pPrtyObject->AddProperty( pCBUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_AOData.m_BlendFactor), "Ambient Occlusion", "AO Texture blend factor");
	pRFUII->SetMinimum(0);
	pRFUII->SetMaximum(1.0f);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(100);
	m_pPrtyObject->AddProperty( pRFUII );
	m_pPrtyObject->AddProperty( new prtyCheckBoxUIInfo(&(m_AOData.m_bTexturesInSceneFolder), "Ambient Occlusion", "Save AO textures to scene-specific folder"));
	pPUII = new prtyNumericUpDownUIInfo(&(m_AOData.m_DistanceCutoff), "Ambient Occlusion", "Distances greater will not occlude");
	m_pPrtyObject->AddProperty( pPUII );
*/
	// EON Composer... keep the property, just don't show the UI Info anymore
	// baking support
	//m_pPrtyObject->AddProperty( new prtyCheckBoxUIInfo(&(m_PrtyBake), "Baking", "Enable texture baking for this object"));


//	m_AOData.m_bIsOccluder.AddCallback(new prtyCallbackWrapper<fgmtScriptObject>(this, &fgmtScriptObject::AOChangedOccluder));
//	m_AOData.m_bSelfOccludeOnly.AddCallback(new prtyCallbackWrapper<fgmtScriptObject>(this, &fgmtScriptObject::AOChangedSelfOcclude));
//	m_AOData.m_bInherit.AddCallback(new prtyCallbackWrapper<fgmtScriptObject>(this, &fgmtScriptObject::AOChangedInherit));
	m_AOData.m_bReceivesOcclusion.AddCallback(new prtyCallbackWrapper<fgmtScriptObject>(this, &fgmtScriptObject::AOChangedReceive));
	m_AOData.m_bUseBakedTexture.AddCallback(new prtyCallbackWrapper<fgmtScriptObject>(this, &fgmtScriptObject::BakeChanged));

//	m_AOData.m_nSamples.AddCallback(new prtyCallbackWrapper<fgmtScriptObject>(this, &fgmtScriptObject::AOChangedNSamples));
//	m_AOData.m_SamplingResolution.AddCallback(new prtyCallbackWrapper<fgmtScriptObject>(this, &fgmtScriptObject::AOChangedSamplingRes));
//	m_AOData.m_DepthBias.AddCallback(new prtyCallbackWrapper<fgmtScriptObject>(this, &fgmtScriptObject::AOChangedDepthBias));
//	m_AOData.m_TextureResolution.AddCallback(new prtyCallbackWrapper<fgmtScriptObject>(this, &fgmtScriptObject::AOChangedTexRes));
//	m_AOData.m_BlendFactor.AddCallback(new prtyCallbackWrapper<fgmtScriptObject>(this, &fgmtScriptObject::AOChangedBlendFactor));
//	m_AOData.m_bStatic.AddCallback(new prtyCallbackWrapper<fgmtScriptObject>(this, &fgmtScriptObject::AOChangedStatic));
//	m_AOData.m_DistanceCutoff.AddCallback(new prtyCallbackWrapper<fgmtScriptObject>(this, &fgmtScriptObject::AOChangedDistCutoff));

	// no callback for this one, as the value is simply used directly:
	//m_AOData.m_bTexturesInSceneFolder

	//m_PrtyBake.AddCallback(new prtyCallbackWrapper<fgmtScriptObject>(this, &fgmtScriptObject::ChangedBake));
}

//--------------------------------------------------------------------
//	Read the model file and get fragment data structures out
//	in order to override them.
//--------------------------------------------------------------------
void fgmtScriptObject::GatherFragments()
{
	api3dObjectSingle* pObject = dynamic_cast<api3dObjectSingle*>(m_pObject);
	std::string node_name("");
	std::vector<fgmtFragmentData> fragment_data;
	std::vector<g3dFragment*> fragments;
	std::vector<g3dSceneNode*> frag_nodes;
	std::vector<matMaterial*> frag_mats;
	gather_fragments(pObject->Object()->GetBase(), fragment_data, fragments, frag_nodes, frag_mats, node_name);
	//gather_fragments(pObject->Object()->GetBase(), m_FragmentData, m_Fragments, m_FragmentNodes, m_Materials, node_name);

	// Assuming the old data is empty
	DBG_ASSERT(m_FragmentData.empty(), "GatherFragments() should only be called when fragment data is empty.");

	// gather_fragments() gathers all instances separately, but we want to group them
	// so that one fgmtPropertyObject contains a vector of all the instances so that
	// they behave like a single shared fragment. Use the constructed fragment name
	// to distinguish when the fragments are shared.
	std::map<std::string, int> reuse_map;
	std::vector<std::string> frag_names;
	for (int i = 0; i < fragments.size(); i++)
	{
		std::string frag_name = construct_fragment_name(fragment_data[i], fragments[i], i);
		std::string frag_bake_name = construct_fragment_bake_name(fragment_data[i], fragments[i], i, m_ModelName, m_BaseName);
		std::map<std::string, int>::iterator it = reuse_map.find(frag_name);
		
		frag_nodes[i]->SetBakeName(frag_bake_name.c_str());
		if (it == reuse_map.end())
		{
			// Not found, construct a new fragment data
			reuse_map[frag_name] = m_FragmentData.size();
			frag_names.push_back( frag_name );
			m_FragmentData.push_back( fragment_data[i] );
			m_Materials.push_back( frag_mats[i] );

			// Begin vectors of fragments and nodes with single entry, 
			// the second instance will add to these lists.
			std::vector<g3dFragment*> frag_vec(1, fragments[i]);
			m_Fragments.push_back( frag_vec );
			std::vector<g3dSceneNode*> node_vec(1, frag_nodes[i]);
			m_FragmentNodes.push_back( node_vec );
		}
		else
		{
			// Add fragment and its node to a vector per fgmtPropertyObject
			int instance_index = it->second;
			m_Fragments[instance_index].push_back( fragments[i] );
			m_FragmentNodes[instance_index].push_back( frag_nodes[i] );
		}
	}

	/*if (m_Fragments[0][0])
	{
		if (m_Fragments[0][0]->GetUseBakedTexture())
		{
			CreateBakedMaterials();
			ReplaceBakedMaterials();
		}
	}*/

	// after the instances have been sorted out, created the fgmtPropertyObjects
	for (int i = 0; i < m_FragmentData.size(); i++)
	{
		m_FragmentUI.push_back(new fgmtPropertyObject(frag_names[i],
													  m_FragmentData[i], 
													  m_Fragments[i], 
													  m_FragmentNodes[i],
													  this->m_AOTextureLocator, this));
		m_FragmentUI[i]->SetParentRelationship(m_ParentRelationship);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//void fgmtScriptObject::CleanUpFragments()
//{
//	// gather AO textures from each fragment, in order to reset after.
//	for (int i = 0; i < m_FragmentUI.size(); i++)
//	{
//		m_FragmentUI[i]->CleanUp();
//	}
//}

//--------------------------------------------------------------------
//	Get new fragment list and refresh state of flags. Used
//	when chaning resolution of model.
//--------------------------------------------------------------------
//void fgmtScriptObject::RefreshFragments()
//{
//	// assumption : order and number of fragments will stay the same 
//
//	// remember the selected index of the fragment.
//	int index = -1;
//	if (sel3dCastUtil::CastSelectedObject<fgmtScriptObject>() == this)
//	{
//		if ( fgmtPropertyObject *pPropObj = sel3dCastUtil::CastSelectedObject<fgmtPropertyObject>() )
//		{
//			index = GetIndexForName(pPropObj->GetName());
//			if (index > -1)
//				sel3dMgr::ClearSelection();
//		}
//	}
//
//	api3dObjectSingle* pObject = dynamic_cast<api3dObjectSingle*>(m_pObject);
//	std::string node_name("");
//	std::vector<fgmtFragmentData> old_data = m_FragmentData; // copy old data
//	fgmtFragmentsAOData old_ao_data = m_AOData; // copy old data
//	envSTLHelpers::DeleteContainer(m_FragmentUI);
//	m_FragmentData.clear();
//	m_Fragments.clear();
//	m_FragmentNodes.clear();
//	m_Materials.clear();
//	gather_fragments(pObject->Object()->GetBase(), m_FragmentData, m_Fragments, m_FragmentNodes, m_Materials, node_name);
//	for (int i = 0; i < m_Fragments.size(); i++)
//	{
//		std::string frag_name = construct_fragment_name(m_FragmentData[i], m_Fragments[i], i);
//		m_FragmentUI.push_back(new fgmtPropertyObject(frag_name,
//													  m_FragmentData[i], 
//													  m_Fragments[i], 
//													  m_FragmentNodes[i],
//													  this->m_AOTextureLocator));
//		m_FragmentUI[i]->SetParentRelationship(m_ParentRelationship);
//	}
//
//	for (int i = 0; i < m_FragmentUI.size(); i++)
//	{
//		m_FragmentUI[i]->SetModelTextureLocator(m_ModelTextureLocator);
//	}
//
//	// restore old data
//	this->SetFragmentData(old_data, old_ao_data, m_ModelName);
//
//	// now that data is restored, make sure the AO textures are up to date.
//	for (int i = 0; i < m_FragmentUI.size(); i++)
//	{
//// disable baked AO textures in favor of SSAO
////		m_FragmentUI[i]->ResetAOTexture();
//	}
//
//	// restore selection?
//	if (index > -1)
//		sel3dMgr::Select(m_FragmentUI[index]);
//
//}

//--------------------------------------------------------------------
//	Returns number of fragments. May return 0 if no fragments are
//	overriden (if GatherFragments has not been called).
//--------------------------------------------------------------------
int fgmtScriptObject::GetNumFragments() const
{
	return m_FragmentData.size();
}

//--------------------------------------------------------------------
//	Return name of Fragment with given index
//--------------------------------------------------------------------
std::string fgmtScriptObject::GetFragmentName(int i_Index) const
{
	DBG_ASSERT(i_Index<m_FragmentUI.size(), "Fragment index out of range");
	return i_Index < 0 ? "" : m_FragmentUI[i_Index]->GetName();
}

//--------------------------------------------------------------------
//	Return g3dFragment with given index
//--------------------------------------------------------------------
//g3dFragment* fgmtScriptObject::GetFragment(int i_Index) const
//{
//	DBG_ASSERT(i_Index<m_Fragments.size(), "Fragment index out of range");
//	return i_Index < 0 ? NULL : m_Fragments[i_Index];
//}

//--------------------------------------------------------------------
//	Return g3dSceneNode with given index
//
//	Note that with instancing, this is only going to return the
//	first node found for the given instance, which is fine
//	for the purposes of the UV Viewer.
//--------------------------------------------------------------------
g3dSceneNode* fgmtScriptObject::GetFragmentNode(int i_Index) const
{
	DBG_ASSERT(i_Index<m_FragmentNodes.size(), "Fragment index out of range");
	return (i_Index < 0) ? NULL : m_FragmentNodes[i_Index][0];
}

//--------------------------------------------------------------------
//	Return names of all Fragments at in one array
//--------------------------------------------------------------------
void  fgmtScriptObject::GetFragmentNames(std::vector<std::string> &o_Names) const
{
	int num_frags = m_FragmentData.size();
	o_Names.resize(num_frags);
	for (int i=0; i<num_frags; ++i)
	{
		o_Names[i] = this->GetFragmentName(i);
	}
}

//--------------------------------------------------------------------
// Return index for surface with given name. Returns -1
//	if nto found.
//--------------------------------------------------------------------
int fgmtScriptObject::GetIndexForName(const std::string &i_Name) const
{
	for (int i=0; i<m_FragmentUI.size(); i++)
	{
		if (i_Name == m_FragmentUI[i]->GetName()) 
			return i;
	}
	return -1;
}

//--------------------------------------------------------------------
//	Return Fragment properties structure
//--------------------------------------------------------------------
const fgmtFragmentData& fgmtScriptObject::GetFragmentData(int i_Index) const
{
	DBG_ASSERT(i_Index<m_Fragments.size(), "Fragment index out of range");

	return m_FragmentData[i_Index];
}

//--------------------------------------------------------------------
// Get vector of fragments in order to store info to file
//--------------------------------------------------------------------
void fgmtScriptObject::GetFragmentData(std::vector<fgmtFragmentData> &o_Data,
									   fgmtFragmentsAOData& o_AOData) const
{
	o_Data = m_FragmentData;
	o_AOData = m_AOData;
}

//--------------------------------------------------------------------
// Set vector of fragments from data from file. This uses the name
//	of the fragments to match up data in order to handle cases where
//	the model file has changed since the fragments were last 
//	overriden.
//--------------------------------------------------------------------
void fgmtScriptObject::SetFragmentData(const std::vector<fgmtFragmentData> &i_Data, 
									   const fgmtFragmentsAOData& i_AOData,
									   const std::string& i_ModelName,
									   const std::string& i_BaseName)
{
	m_AOData = i_AOData;
	m_ModelName = i_ModelName;
	m_BaseName = i_BaseName;

	if (i_Data.empty()) return;

	if (m_Fragments.empty())
	{
		this->GatherFragments();
	}

	// If the number of fragments are the same, just 
	// assume the fragment data matches
	if (i_Data.size() == m_FragmentData.size())
	{
		for (int i=0; i<i_Data.size(); ++i)
		{
			// need to let fgmtOperations know that i is the current fragment.
//			fgmtOperations::SetSelectedFragmentIndex(this, i);

			ChangeFragmentData(i, i_Data[i]);
		}
	}
	else
	{
		// Otherwise, go through each each fragment passed in and try to find the fragment by name
		// that matches
		for (int i=0; i<i_Data.size(); ++i)
		{
			std::string frg_name = i_Data[i].GetFragmentName();
			if (frg_name.empty())
			{
				//DBG_WARNING("Fragment is not named, cannot set values.");
			}
			else
			{
				bool bFound = false;
				for (int j=0; j<m_FragmentData.size(); ++j)
				{
					if (m_FragmentData[j].GetFragmentName() == frg_name)
					{
						bFound = true;
						ChangeFragmentData(j, i_Data[i]);
						break;
					}
				}

				if (!bFound)
				{
					DBG_WARNING("Could not find fragment named, " << frg_name.c_str());
				}
			}
		}
	}
}

//--------------------------------------------------------------------
//	Change the fragment at a given index, using the fragment
//	data structure.
//--------------------------------------------------------------------
void fgmtScriptObject::ChangeFragmentData(int i_Index, 
										  const fgmtFragmentData& i_Data)
{
	DBG_ASSERT(i_Index<m_FragmentData.size(), "Fragment index out of range");

	// preserve the fragment name
	fgmtFragmentData &frg_data = m_FragmentData[i_Index];
	std::string frg_name = frg_data.GetFragmentName();

	// set local data structure
	frg_data = i_Data;

	// put old name back
	frg_data.SetFragmentName(frg_name);

	if (frg_data.m_bUseBakedTexture.GetValue())
	{
		CreateBakedMaterials();
		ReplaceBakedMaterials();
	}
}

//--------------------------------------------------------------------
//	Change all fragments' surface flags to match the fragment at a given index.
//--------------------------------------------------------------------
void fgmtScriptObject::ChangeFragmentDataFlags(int i_Index)
{
	DBG_ASSERT(i_Index<m_FragmentData.size(), "Fragment index out of range");

	fgmtFragmentData &frg_data = m_FragmentData[i_Index];
	for (int i = 0; i < m_FragmentData.size(); i++)
	{
		if (i != i_Index)
		{
			m_FragmentData[i].m_bCastsShadow.SetValue(frg_data.m_bCastsShadow.GetValue());
			m_FragmentData[i].m_bReceivesShadow.SetValue(frg_data.m_bReceivesShadow.GetValue());
			m_FragmentData[i].m_bShadowHull.SetValue(frg_data.m_bShadowHull.GetValue());
			m_FragmentData[i].m_bDoubleSided.SetValue(frg_data.m_bDoubleSided.GetValue());
		}
	}
}

//--------------------------------------------------------------------
//	Change all fragments' AO data to match the fragment at a given index.
//--------------------------------------------------------------------
void fgmtScriptObject::ChangeFragmentDataAO(int i_Index)
{
	DBG_ASSERT(i_Index<m_FragmentData.size(), "Fragment index out of range");

	fgmtFragmentData &frg_data = m_FragmentData[i_Index];

	for (int i = 0; i < m_FragmentData.size(); i++)
	{
		if (i != i_Index)
		{
			m_FragmentData[i].m_bIsOccluder.SetValue( frg_data.m_bIsOccluder.GetValue() );
			m_FragmentData[i].m_bReceivesOcclusion.SetValue( frg_data.m_bReceivesOcclusion.GetValue() );
			m_FragmentData[i].m_bAOInherit.SetValue(frg_data.m_bAOInherit.GetValue());
			m_FragmentData[i].m_nSamples.SetValue(frg_data.m_nSamples.GetValue());
			m_FragmentData[i].m_DepthBias.SetValue(frg_data.m_DepthBias.GetValue());
			m_FragmentData[i].m_SamplingResolution.SetValue(frg_data.m_SamplingResolution.GetValue());
			m_FragmentData[i].m_AOBlendFactor.SetValue(frg_data.m_AOBlendFactor.GetValue());
		}
	}

	try
	{
		for (int i = 0; i < m_FragmentData.size(); i++)
		{
			if (i != i_Index)
			{
				m_FragmentData[i].m_AOTextureResolution.SetValue(frg_data.m_AOTextureResolution.GetValue());
			}
		}
	}
	catch (const g2dOutOfVideoMemoryX& )
	{
		guiMessageBox::Show("Out of video memory creating Ambient Occlusion textures. Try reducing texture resolution or removing AO from some surfaces.", "Error", guiMessageBox::e_OKOnly);
	}
}

//--------------------------------------------------------------------
//	Store selected index in order to maintain it when re-selecting
//--------------------------------------------------------------------
//void fgmtScriptObject::SetLastSelectedFragmentIndex(int i_Index)
//{
//	m_LastSelectedFragmentIndex = i_Index;
//}
//int fgmtScriptObject::GetLastSelectedFragmentIndex() const
//{
//	return m_LastSelectedFragmentIndex;
//}


//--------------------------------------------------------------------
// Get fragment index by looking for the node with the given 
//	pick code.
//--------------------------------------------------------------------
int fgmtScriptObject::GetFragmentIndexFromPickCode(envType::UInt32 i_PickCode) const
{
	const g3dSceneNode *pPickedNode = m_pObject->GetPickedNode(i_PickCode);
	if (pPickedNode)
	{
		const g3dFragment *pFragment = pPickedNode->GetFragment();
		if (pFragment)
		{
			for (int i=0; i<m_Fragments.size(); ++i)
			{
				if (envSTLHelpers::Contains(m_Fragments[i], pFragment))
					return i;
			}
		}
	}
	return -1;
}

//--------------------------------------------------------------------
// Is there a better way to do this besides storing every fragment's original material?
//--------------------------------------------------------------------
void fgmtScriptObject::HighlightFragment(int i_Index, matMaterial* i_MatHilight, bool i_On)
{
	DBG_ASSERT(i_Index<m_Fragments.size(), "Fragment index out of range");

	std::vector<g3dFragment*> &fragments = m_Fragments[i_Index];
	if (api3dObjectSingle* pObject = dynamic_cast<api3dObjectSingle*>(m_pObject))
	{
		//matMaterial* pExisting = NULL;
		matMaterial* pReplace = NULL;
		if (i_On)
		{
			// replace existing with highlight
			//pExisting = m_Materials[i_Index];
			pReplace = i_MatHilight;
		}
		else
		{
			// replace highlight with original
			//pExisting = i_MatHilight;
			pReplace = m_Materials[i_Index];
		}

		for (int i=0; i<fragments.size(); i++)
		{
			//if (!fragments[i]->GetUseBakedTexture())
			if (!m_FragmentData[i_Index].m_bUseBakedTexture.GetValue())
			{
			//DBG_ASSERT(pExisting == fragments[i]->GetMaterial(), "HighlightFragment: original fragment material does not match");
				fragments[i]->SetMaterial(pReplace);
			}
		}
	}
}

//--------------------------------------------------------------------
// auto-save a single fragment's AO texture.
//--------------------------------------------------------------------
//void fgmtScriptObject::SaveAOTexture(int i_Index)
//{
	//DBG_ASSERT(i_Index<m_Fragments.size(), "Fragment index out of range");

	//g3dFragment *pFragment = m_Fragments[i_Index];
	//effOcclusionData* aodata = pFragment->GetOcclusionData();

	//if (aodata->m_TextureDiffuse == NULL)
	//	return;

	//if (aodata->m_TextureName == "")
	//{
	//	fgmtFragmentData &frg_data = m_FragmentData[i_Index];

	//	// this is where we auto-generate a new texture filename in this folder.
	//	// if we are storing to a scene-specific folder, use the instance-specific object name.
	//	// if we are storing to the model folder, use the model name.
	//	std::string name = construct_fragment_AOtexture_name(frg_data, this->m_pParent, pFragment, 
	//		i_Index, m_AOData.m_bTexturesInSceneFolder.GetValue(), m_ModelName);
	//	//fsFileUtil::GenerateFileName(m_AOTextureLocator, std::string("AO_"), std::string(".dds"), name);
	//	// store texture name in the effOcclusionData
	//	aodata->m_TextureName = name;
	//	// store texture name in the object data
	//	frg_data.m_AOUserTextureName.SetValue(itString(name.c_str()));
	//}
	//fsLocator loc = m_AOData.m_bTexturesInSceneFolder.GetValue() ? m_AOTextureLocator : m_ModelTextureLocator;
	//loc.Push(aodata->m_TextureName.c_str());

	//try
	//{
	//	matTextureMgr::SaveTextureToFile(aodata->m_TextureDiffuse, loc);
	//}
	//catch(g2dImageSaveX& /*ex*/)
	//{
	//	int x = 0;
	//}
//}

//--------------------------------------------------------------------
// auto-import a single fragment's AO texture.
//--------------------------------------------------------------------
//void fgmtScriptObject::AutoImportAOTexture(int i_Index)
//{
//	DBG_ASSERT(i_Index<m_Fragments.size(), "Fragment index out of range");
//
//	g3dFragment *pFragment = m_Fragments[i_Index];
//	effOcclusionData* aodata = pFragment->GetOcclusionData();
//
//	fgmtFragmentData &frg_data = m_FragmentData[i_Index];
//
//	// search for files that match fgmt name, fgmt index, and mat name.
//	std::string mat_name, frg_name, frg_num;
//	get_fragment_strings(frg_data, pFragment, i_Index, &mat_name, &frg_name, &frg_num);
//
////	std::string fgmtInfoString = construct_fragment_info_string(pFragment, frg_data, i_Index);
//	std::vector<itString> matches;
//	matches.push_back(itString(frg_name.c_str()));
//
//	fsLocator loc = m_AOData.m_bTexturesInSceneFolder.GetValue() ? m_AOTextureLocator : m_ModelTextureLocator;
//	fsFileEnum::fsFileList filelist;
//	fsFileEnum::EnumerateFiles( loc, filelist, matches );
//
//	std::string name;
//	if (filelist.size() > 0)
//	{
//		itString name;
//		// try to match modelname next.
//		itString modelName(m_ModelName.c_str());
//		for (int i = 0; i < filelist.size(); i++)
//		{
//			name = filelist[i].GetLastName();
//			if (name.HasSubString(modelName))
//				break;
//		}
//		if (name.GetLength() == 0)
//			name = filelist[0].GetLastName();
//
//		// store texture name in the object data
//		// let the prty change handler update everything and load the texture.
//		frg_data.m_AOUserTextureName.SetValue(name);
//	}
//	else
//	{
//		// this is where we auto-generate a new texture filename in this folder.
//		// if we are storing to a scene-specific folder, use the instance-specific object name.
//		// if we are storing to the model folder, use the model name.
//		name = construct_fragment_AOtexture_name(frg_data, this->m_pParent, pFragment, 
//			i_Index, m_AOData.m_bTexturesInSceneFolder.GetValue(), m_ModelName);
//
//		// store texture name in the object data
//		// let the prty change handler update everything and load the texture.
//		frg_data.m_AOUserTextureName.SetValue(itString(name.c_str()));
//	}
//
//
//
//}

//--------------------------------------------------------------------
// auto-save all AO textures to file, generating filenames when needed.
//--------------------------------------------------------------------
//void fgmtScriptObject::SaveAOTextures()
//{
//	for (int i = 0; i < m_Fragments.size(); i++)
//	{
//		SaveAOTexture(i);
//	}
//}

//--------------------------------------------------------------------
// auto-import all AO textures from file, using generated filenames 
//--------------------------------------------------------------------
//void fgmtScriptObject::AutoImportAOTextures()
//{
//	for (int i = 0; i < m_Fragments.size(); i++)
//	{
//		AutoImportAOTexture(i);
//	}
//}

//--------------------------------------------------------------------
// set all texture filenames to empty string
//--------------------------------------------------------------------
//void fgmtScriptObject::ClearAOTextures()
//{
//	for (int i = 0; i < m_Fragments.size(); i++)
//	{
//		fgmtFragmentData &frg_data = m_FragmentData[i];
//		frg_data.m_AOUserTextureName.SetValue(itString(""));
//	}
//}

void fgmtScriptObject::SetAOTextureLocator(fsLocator& i_Locator)
{
	m_AOTextureLocator = i_Locator;
	for (int i = 0; i < m_FragmentUI.size(); i++)
	{
		m_FragmentUI[i]->SetAOTextureLocator(i_Locator);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void fgmtScriptObject::SetBakedTextureLocator(const fsLocator& i_Locator, const std::string& i_Extension)
{
	for(int i = 0; i < m_FragmentData.size(); i++)
	{
		m_FragmentData[i].m_bHasBeenBaked = true;
		m_FragmentData[i].m_BakedTextureLocation = i_Locator;
		m_FragmentData[i].m_BakedTextureFormat = i_Extension;
	}
	
	for (int i = 0; i < m_FragmentUI.size(); i++)
	{
		m_FragmentUI[i]->SetBakedTextureLocator(i_Locator, i_Extension);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void fgmtScriptObject::CreateBakedMaterials()
{
	std::vector<matMaterial*> frag_mats;

	ReleaseBakedMaterials();
	for (int i = 0; i < m_FragmentNodes.size(); i++)
	{
		// Loop through each fragment and create material for each of them and
		// its instance
		frag_mats.clear();
		for( int j = 0; j < m_FragmentNodes[i].size(); j++)
		{
			matMaterial* mat = new matMaterial("Bake.fx");
			effBakeData* pData = dynamic_cast<effBakeData*>(mat->GetEffectData());
			DBG_ASSERT(pData != NULL, "Create baked materials not using effBake");
			mat->SetName(m_FragmentNodes[i][j]->GetBakeName());
			pData->m_ColorEmissive = maFloatRGBA(0,0,0,0);

			// Load baked color texture if there is one
			fsLocator path = m_FragmentData[i].m_BakedTextureLocation.GetValue();
			std::string nameStr(m_FragmentNodes[i][j]->GetBakeName());
			nameStr.append("_Baked_Color.");
			nameStr.append(m_FragmentData[i].m_BakedTextureFormat.GetValue().c_str());
			path.Push(nameStr.c_str());
			if (fsFileUtil::FileExists(path))
			{
				matTexture* pBakedTex = matTextureMgr::LoadTexture(path);
				pData->m_TextureEmissive = pBakedTex;
				fsFileUtil::LocatorToANSIFilename(path, pData->m_NameEmissive);
			}

			nameStr = m_FragmentNodes[i][j]->GetBakeName();
			nameStr.append("_Baked_Normals.");
			nameStr.append(m_FragmentData[i].m_BakedTextureFormat.GetValue().c_str());
			itString nameItString(nameStr.c_str());
			path.ReplaceLastName( nameItString );
			if (fsFileUtil::FileExists(path))
			{
				matTexture* pBakedTex = matTextureMgr::LoadTexture(path);
				pData->m_TextureNormalMap = pBakedTex;
				fsFileUtil::LocatorToANSIFilename(path, pData->m_NameNormalMap);

				effNormalsData& normalData = (effNormalsData&)(mat->GetNormalsData());
				normalData.m_BumpScale = 1.0f;
				normalData.m_NameNormalMap = path;
				normalData.m_pNormalMap = pBakedTex;
			}

			frag_mats.push_back(mat);
		}
		
		m_BakedMaterials.push_back(frag_mats);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void fgmtScriptObject::ReleaseBakedMaterials()
{
	for (int i = 0; i < m_BakedMaterials.size(); i++)
	{
		for (int j = 0; j < m_BakedMaterials[i].size(); j++)
		{
			effBakeData* effData = dynamic_cast<effBakeData*>(m_BakedMaterials[i][j]->GetEffectData());
			if (effData->m_TextureNormalMap)
			{
				matTextureMgr::ReleaseTexture(effData->m_TextureNormalMap);
			}

			if (effData->m_TextureEmissive)
			{
				matTextureMgr::ReleaseTexture(effData->m_TextureEmissive);
			}
			//delete m_BakedMaterials[i][j]
		}
		envSTLHelpers::DeleteContainer(m_BakedMaterials[i]);
	}
	m_BakedMaterials.clear();
}

//--------------------------------------------------------------------
// Set frag material according to flag
//--------------------------------------------------------------------
void fgmtScriptObject::SetMaterialForFrag(g3dFragment* i_Frag, 
										  g3dSceneNode* i_Node,
										  matMaterial* i_newMaterial)
{
	matMaterial* originalMat = i_Frag->GetMaterial();
	if ( originalMat != i_newMaterial)
	{
		i_Frag->SetMaterial(i_newMaterial);
		mtrlScriptObject* mtrlObject = dynamic_cast<mtrlScriptObject*>(this);
		if (mtrlObject)
		{
			mtrlObject->IncreaseMaterialRefCount(i_newMaterial);
			mtrlObject->DecreaseMaterialRefCount(originalMat);
		}
	}
}

//--------------------------------------------------------------------
// ReplaceBakedMaterials: replace fragments that has baked material
// and with useBakedFlag on
//--------------------------------------------------------------------
void fgmtScriptObject::ReplaceBakedMaterials()
{
	for (int i = 0; i < m_Fragments.size(); i++)
	{
		for(int j = 0; j < m_Fragments[i].size(); j++)
		{
			if (m_FragmentData[i].m_bUseBakedTexture.GetValue())
			{
				SetMaterialForFrag(m_Fragments[i][j], m_FragmentNodes[i][j], m_BakedMaterials[i][j]);
				//m_Fragments[i][j]->SetMaterial(m_BakedMaterials[i][j]);
			}
			else
			{
				SetMaterialForFrag(m_Fragments[i][j], m_FragmentNodes[i][j], m_Materials[i]);
				//m_Fragments[i][j]->SetMaterial(m_Materials[i]);
			}
		}
	}
}

//--------------------------------------------------------------------
// Set frag material according to flag
//--------------------------------------------------------------------
void fgmtScriptObject::SwitchMaterial(bool i_IsBake, const std::string& i_FragName)
{
	if (i_IsBake && m_BakedMaterials.size() == 0)
		return;	// bake material is not initialized yet
	int frag_index = GetIndexForName(i_FragName);
	if (frag_index >= 0)
	{
		if (i_IsBake)
		{
			for(int i = 0; i < m_Fragments[frag_index].size(); i++)
			{
				if (m_BakedMaterials[frag_index][i])
				{
					SetMaterialForFrag(m_Fragments[frag_index][i], 
									m_FragmentNodes[frag_index][i], 
									m_BakedMaterials[frag_index][i]);
					//m_Fragments[frag_index][i]->SetMaterial(m_BakedMaterials[frag_index][i]);
				}
				else
				{
					SetMaterialForFrag(m_Fragments[frag_index][i], 
									m_FragmentNodes[frag_index][i], 
									m_Materials[frag_index]);
					//m_Fragments[frag_index][i]->SetMaterial(m_Materials[frag_index]);
				}
			}
		}
		else
		{
			for(int i = 0; i < m_Fragments[frag_index].size(); i++)
			{
				SetMaterialForFrag(m_Fragments[frag_index][i], 
									m_FragmentNodes[frag_index][i], 
									m_Materials[frag_index]);
				//m_Fragments[frag_index][i]->SetMaterial(m_Materials[frag_index]);
			}
		}
	}

	gpxRenderControl::SetNeedsNewRender();
}

//--------------------------------------------------------------------
// SetBakedFlag: Set all fragments' UseBakedFlag to i_bVal
//--------------------------------------------------------------------
void fgmtScriptObject::SetBakedFlag(bool i_bVal)
{
	for (int i = 0; i < m_FragmentData.size(); i++)
	{
		m_FragmentData[i].m_bUseBakedTexture = i_bVal;
	}

	/*for (int i = 0; i < m_Fragments.size(); i++)
	{
		for(int j = 0; j < m_Fragments[i].size(); j++)
		{
			m_Fragments[i][j]->SetUseBakedTexture(i_bVal);
		}
	}*/
}

//--------------------------------------------------------------------
// GetBakedFlagFromFrags: Get internal baked flag setting
//--------------------------------------------------------------------
void fgmtScriptObject::GetBakedFlagFromFrags()
{
	for ( int i = 0; i < m_FragmentData.size(); i++)
	{
		// FragmentData is always shared between instances. 
		// However they might have different material/baked texture
		// In the original structure it's impossible to have two instances set to
		// different baked flag value
		//m_FragmentData[i].m_bUseBakedTexture = m_Fragments[i][0]->GetUseBakedTexture();
		bool isUseBakedTex = true;
		g3dSceneNode* pParentNode = m_FragmentNodes[i][0];
		while(pParentNode)
		{
			isUseBakedTex = isUseBakedTex & pParentNode->GetActiveInRenderLayer() & pParentNode->GetRenderable();
			pParentNode = pParentNode->GetParent();
		}
		m_FragmentData[i].m_bUseBakedTexture = isUseBakedTex;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void fgmtScriptObject::ReportMemory(gfFileTxt& i_File)
{
	std::vector<fgmtFragmentData> fragmentData;
	std::vector<g3dFragment*> fragments;
	std::vector<g3dSceneNode*> fragmentNodes;
	std::vector<matMaterial*> materials;

	api3dObjectSingle* pObject = dynamic_cast<api3dObjectSingle*>(m_pObject);
	std::string node_name("");
	gather_fragments(pObject->Object()->GetBase(), fragmentData, fragments, fragmentNodes, materials, node_name);

	unsigned int fragMem = 0;
	for (int i = 0; i < fragments.size(); i++)
	{
		fragMem += fragments[i]->GetSize();
	}

    std::ostringstream stm;
	stm << "  " << fragments.size() << " fragmnts, size: " << fragMem/1024 << " KB\r\n";
	i_File.WriteLine(stm.str());
}

//--------------------------------------------------------------------
// Set flag for texture baking
//--------------------------------------------------------------------
void fgmtScriptObject::SetEnableBaking(bool i_bBake)
{
	// This could be switched so that the code in the property callback
	// is added here directly and then we could remove the property and
	// just use the checkbox interface for baking.
	//m_PrtyBake.SetValue(i_bBake);
}

//--------------------------------------------------------------------
// Access to the actual g3dFragments in object
//--------------------------------------------------------------------
//const std::vector<g3dFragment*>& fgmtScriptObject::GetG3dFragments() const
//{
//	return m_Fragments;
//}

//--------------------------------------------------------------------
//	Return Fragment ui properties
//--------------------------------------------------------------------
fgmtPropertyObject* fgmtScriptObject::GetFragmentUI(int i_Index) const
{
	DBG_ASSERT(i_Index<m_FragmentUI.size(), "Fragment index out of range");

	return m_FragmentUI[i_Index];
}
fgmtPropertyObject* fgmtScriptObject::GetFragmentUI(const std::string& i_SurfaceName) const
{
	int num_frags = m_FragmentData.size();
	for (int i=0; i<num_frags; ++i)
	{
		if (i_SurfaceName == m_FragmentUI[i]->GetName())
			return m_FragmentUI[i];
	}

	return NULL;
}

//--------------------------------------------------------------------
// Return the material used by the surface with the given index
//--------------------------------------------------------------------
matMaterial* fgmtScriptObject::GetMaterialForSurface(int i_Index) const
{
	DBG_ASSERT(i_Index<m_Materials.size(), "Fragment index out of range");

	return m_Materials[i_Index];
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void fgmtScriptObject::AOChangedOccluder(prtyProperty *i_pProperty, bool i_bDirty)
{
	// update fgmt prty data if it exists!
	int num_frags = m_FragmentData.size();
	for (int i=0; i<num_frags; ++i)
	{
		fgmtFragmentData& data = m_FragmentData[i];
		if (data.m_bAOInherit.GetValue())
		{
			data.m_bIsOccluder.SetValue(m_AOData.m_bIsOccluder.GetValue());
		}
	}
/*
	// update real fragments.
	if (num_frags == 0)
	{
		class ApplyFragData : public g3dSceneNodeProcessor
		{
		public:
			ApplyFragData(fgmtFragmentsAOData& i_AOData):m_AOData(i_AOData){}
			fgmtFragmentsAOData& m_AOData;
			virtual bool ProcessNode(g3dSceneNode* i_pNode)
			{
				g3dFragment* frag = i_pNode->GetFragment();
				if (frag != NULL)
				{
					if (frag->GetOcclusionData()->m_bInheritParent)
					{
						frag->SetCastsOcclusion( m_AOData.m_bIsOccluder.GetValue() );

						// any other frags that receive occlusion are now invalid.
						// we dont know which ones.
						g3dPrefs::CurrentPrefs().m_bAOInvalid = true;
					}
				}
				return true;
			}
		};
		ApplyFragData ao(m_AOData);
		api3dObjectSingle* pObject = dynamic_cast<api3dObjectSingle*>(m_pObject);
		g3dSceneTraverse::Traverse(pObject->Object()->GetBase(), &ao);
	}
*/
}
void fgmtScriptObject::AOChangedSelfOcclude(prtyProperty *i_pProperty, bool i_bDirty)
{
	// update fgmt prty data if it exists!
	int num_frags = m_FragmentData.size();
	for (int i=0; i<num_frags; ++i)
	{
		fgmtFragmentData& data = m_FragmentData[i];
		if (data.m_bAOInherit.GetValue())
		{
			data.m_bSiblingOccludeOnly.SetValue(m_AOData.m_bSelfOccludeOnly.GetValue());
		}
	}
}

void fgmtScriptObject::AOChangedInherit(prtyProperty *i_pProperty, bool i_bDirty)
{
	// update fgmt prty data if it exists!
	int num_frags = m_FragmentData.size();
	for (int i=0; i<num_frags; ++i)
	{
		fgmtFragmentData& data = m_FragmentData[i];
		data.m_bAOInherit.SetValue(m_AOData.m_bInherit.GetValue());
	}
}

void fgmtScriptObject::AOChangedNSamples(prtyProperty *i_pProperty, bool i_bDirty)
{
	// update fgmt prty data if it exists!
	int num_frags = m_FragmentData.size();
	for (int i=0; i<num_frags; ++i)
	{
		fgmtFragmentData& data = m_FragmentData[i];
		if (data.m_bAOInherit.GetValue())
		{
			data.m_nSamples.SetValue(m_AOData.m_nSamples.GetValue());
		}
	}
}

void fgmtScriptObject::AOChangedReceive(prtyProperty *i_pProperty, bool i_bDirty)
{
	try
	{
		// update fgmt prty data if it exists!
		int num_frags = m_FragmentData.size();
		for (int i=0; i<num_frags; ++i)
		{
			fgmtFragmentData& data = m_FragmentData[i];
			if (data.m_bAOInherit.GetValue())
			{
				data.m_bReceivesOcclusion.SetValue(m_AOData.m_bReceivesOcclusion.GetValue());
			}
		}
	}
	catch (const g2dOutOfVideoMemoryX& )
	{
		guiMessageBox::Show("Out of video memory creating Ambient Occlusion textures. Try reducing texture resolution or removing AO from some surfaces.", "Error", guiMessageBox::e_OKOnly);
	}
}

void fgmtScriptObject::AOChangedSamplingRes(prtyProperty *i_pProperty, bool i_bDirty)
{
	// update fgmt prty data if it exists!
	int num_frags = m_FragmentData.size();
	for (int i=0; i<num_frags; ++i)
	{
		fgmtFragmentData& data = m_FragmentData[i];
		if (data.m_bAOInherit.GetValue())
		{
			data.m_SamplingResolution.SetValue(m_AOData.m_SamplingResolution.GetValue());
		}
	}
}

void fgmtScriptObject::AOChangedDepthBias(prtyProperty *i_pProperty, bool i_bDirty)
{
	// update fgmt prty data if it exists!
	int num_frags = m_FragmentData.size();
	for (int i=0; i<num_frags; ++i)
	{
		fgmtFragmentData& data = m_FragmentData[i];
		if (data.m_bAOInherit.GetValue())
		{
			data.m_DepthBias.SetValue(m_AOData.m_DepthBias.GetValue());
		}
	}
}
void fgmtScriptObject::AOChangedTexRes(prtyProperty *i_pProperty, bool i_bDirty)
{
	try
	{
		// update fgmt prty data if it exists!
		int num_frags = m_FragmentData.size();
		for (int i=0; i<num_frags; ++i)
		{
			fgmtFragmentData& data = m_FragmentData[i];
			if (data.m_bAOInherit.GetValue())
			{
				data.m_AOTextureResolution.SetValue(m_AOData.m_TextureResolution.GetValue());
			}
		}
	}
	catch (const g2dOutOfVideoMemoryX& )
	{
		guiMessageBox::Show("Out of video memory creating Ambient Occlusion textures. Try reducing texture resolution or removing AO from some surfaces.", "Error", guiMessageBox::e_OKOnly);
	}
}
void fgmtScriptObject::AOChangedBlendFactor(prtyProperty *i_pProperty, bool i_bDirty)
{
	// update fgmt prty data if it exists!
	int num_frags = m_FragmentData.size();
	for (int i=0; i<num_frags; ++i)
	{
		fgmtFragmentData& data = m_FragmentData[i];
		if (data.m_bAOInherit.GetValue())
		{
			data.m_AOBlendFactor.SetValue(m_AOData.m_BlendFactor.GetValue());
		}
	}
}
void fgmtScriptObject::AOChangedStatic(prtyProperty *i_pProperty, bool i_bDirty)
{
	// update fgmt prty data if it exists!
	int num_frags = m_FragmentData.size();
	for (int i=0; i<num_frags; ++i)
	{
		fgmtFragmentData& data = m_FragmentData[i];
		if (data.m_bAOInherit.GetValue())
		{
			data.m_bStaticAO.SetValue(m_AOData.m_bStatic.GetValue());
		}
	}
}
void fgmtScriptObject::AOChangedDistCutoff(prtyProperty *i_pProperty, bool i_bDirty)
{
	// update fgmt prty data if it exists!
	int num_frags = m_FragmentData.size();
	for (int i=0; i<num_frags; ++i)
	{
		fgmtFragmentData& data = m_FragmentData[i];
		if (data.m_bAOInherit.GetValue())
		{
			data.m_AODistanceCutoff.SetValue(m_AOData.m_DistanceCutoff.GetValue());
		}
	}
}

void fgmtScriptObject::BakeChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// update fgmt prty data if it exists!
	int num_frags = m_FragmentData.size();
	for (int i=0; i<num_frags; ++i)
	{
		fgmtFragmentData& data = m_FragmentData[i];
		data.m_bUseBakedTexture.SetValue(m_AOData.m_bUseBakedTexture.GetValue());
	}
}

//void fgmtScriptObject::ChangedBake(prtyProperty *i_pProperty, bool i_bDirty)
//{
//	// non overridden fragments.
//	// change this code if we allow automatic internal override of fragments some day.
//	// then it can go through the m_Fragments or m_FragmentData lists.
//
//	class ApplyFragData : public g3dSceneNodeProcessor
//	{
//	public:
//		ApplyFragData(bool i_Value):m_Value(i_Value){}
//		bool m_Value;
//		virtual bool ProcessNode(g3dSceneNode* i_pNode)
//		{
//			g3dFragment* frag = i_pNode->GetFragment();
//			if (frag != NULL)
//			{
//				frag->SetReceivesBake(m_Value);
//			}
//			return true;
//		}
//	};
//	ApplyFragData bake(m_PrtyBake.GetValue());
//	api3dObjectSingle* pObject = dynamic_cast<api3dObjectSingle*>(m_pObject);
//	g3dSceneTraverse::Traverse(pObject->Object()->GetBase(), &bake);
//}

//--------------------------------------------------------------------
// display modal AO property grid
//--------------------------------------------------------------------
//void fgmtScriptObject::ShowAOGrid()
//{
//	std::vector<std::string> colnames;
//	colnames.push_back("Is Occluder");
//	colnames.push_back("Receive Occlusion");
//	colnames.push_back("Static");
//	colnames.push_back("Samples");
//	colnames.push_back("TextureRes");
//	colnames.push_back("Blend Factor"); 
//
//	std::vector<std::string> rownames;
//	std::vector<prtyObject*> rows;
//
//	// this GUI code can be factored out of the script object by exposing an 
//	// interface to get these bits of data:
//	rows.push_back(m_pPrtyObject);
//	rownames.push_back(m_pParent->GetDisplayName());
//	
//	for (int i = 0; i < GetNumFragments(); i++)
//	{
//		rows.push_back(GetFragmentUI(i));
//		rownames.push_back(GetFragmentName(i));
//	}
//	
//	if (guiPropertyGrid::ShowModal("Ambient Occlusion", rownames, colnames, rows) == guiPropertyGrid::e_OK)
//		g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
//}

//--------------------------------------------------------------------
//  Get a list of resources used by fragments.  
//  The resources will be appended to the passed in list.
//--------------------------------------------------------------------
void fgmtScriptObject::GetFragmentResources( fsResourceTrackerData& io_List )
{
	//std::vector<fsLocator> texture_list;
	//for (int i=0; i<m_FragmentData.size(); ++i)
	//{
	//	if (m_FragmentData[i].m_AOUserTextureName.GetValue().GetLength() > 0)
	//	{
	//		fsLocator f = m_FragmentUI[i]->LocateAOTexture(m_AOData.m_bTexturesInSceneFolder.GetValue());
	//		texture_list.push_back(f);
	//	}
	//}
	//
	//std::sort( texture_list.begin(), texture_list.end() );
	//std::unique( texture_list.begin(), texture_list.end() );

	//std::vector<fsLocator>::iterator it;
	//for (it = texture_list.begin(); it != texture_list.end(); ++it)
	//	io_List.Add( *it );

}

//--------------------------------------------------------------------
// GetFragmentNodes()
//--------------------------------------------------------------------
std::vector< std::vector<g3dSceneNode*> > & fgmtScriptObject::GetFragmentNodes()
{
	return m_FragmentNodes;
}

//--------------------------------------------------------------------
// GetFragmentDataElement()
//--------------------------------------------------------------------
fgmtFragmentData fgmtScriptObject::GetFragmentDataElement(int i_Idx)
{
	fgmtFragmentData retData;

	if ( i_Idx < m_FragmentData.size() )
	{
		return m_FragmentData[i_Idx];
	}
	return retData;
}