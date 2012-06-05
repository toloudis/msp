/********************************************************************************************\
**  mtrLevel.cpp
**
**      Keeps track of viewed object.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#include "mtrLevel.hpp"

#include "mtrErrorHandler.hpp"
#include "mtrFaceMesh.hpp"
#include "mtrLightMgr.hpp"
#include "mtrMaterialSaver.hpp"
#include "mtrMaterialUtil.hpp"
#include "mtrMouseSelector.hpp"
#include "mtrPickUtil.hpp"

#include "an2StateAnimation.hpp"
#include "anKeyAnimation.hpp"
#include "api3dObjectGeom.hpp"
#include "api3dScene.hpp"
#include "appSimTime.hpp"
#include "cam3dMgr.hpp"
#include "dbgLog.hpp"
#include "effShaderArray.hpp"
#include "entImport.hpp"
#include "entModelTemplate.hpp"
#include "envSTLHelpers.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "fsResourceFinderDir.hpp"
#include "g3dFragment.hpp"
#include "g3dSceneNode.hpp"
#include "matMatAnim.hpp"
#include "matMaterial.hpp"
//#include "matShaderEffect.hpp"
#include "matTextureMgr.hpp"
#include "mayFragInfo.hpp"
#include "maySubdivInfo.hpp"
#include "itStringUtil.hpp"
#include "scObject.hpp"

#include <algorithm>
#include <memory>

//========================================================================
// Local variables and functions
//========================================================================
namespace
{
matMaterial*			l_HighlightMaterial = NULL;

api3dObjectGeom*			l_pObject = NULL;
fsLocator					l_FileLocator;
fsLocator					l_TextureDirectory;
fsLocator					l_GeneralTextureDirectory;
entModelTemplate*			l_ModelTemplate = NULL;
std::vector<g3dFragment*>	l_ObjFragments; // fragments to change with material highlight, not owned
mtrPickUtil::FaceMap			l_MeshMap; // for picking materials

std::vector<mtrMaterialTemplate> l_MatData;
int		l_SelMaterial = -1;
bool	l_bDoHighlight = false;

// clipboard for copy/paste of material info
mtrMaterialTemplate l_ClipboardMaterial;
bool bHaveClipboardData = false;

mtrMaterialSelectCallback *		l_SelectCallback = NULL;
mtrModelChangeCallback *		l_ChangeCallback = NULL;

// if not doing highlight, then flash highlight for short time
const float c_FlashTime = 1.5f;
bool	l_bFlashing = false; 
float	l_FlashEnd = 0.0f;


//========================================================================
//========================================================================
class FaceMeshSink : public entFragInfoSink
{
	public:
		FaceMeshSink(mtrPickUtil::FaceMap& o_Map)
			: m_Map(o_Map) {}

		//====================================================================
		//	i_Fragments is an array of fragments that was created for the
		//	given frag info.  It is possible the array will be empty
		//	for single skin and jointed objects.
		//====================================================================
		virtual void ReceiveFragInfo( const std::vector<g3dFragment*> &i_Fragments,
									  const entFragInfo& i_FragInfo )
		{
			const mayFragInfo *frag_info = dynamic_cast<const mayFragInfo*>(&i_FragInfo);
			if (frag_info)
			{
				int num_mats = frag_info->m_Materials.size();
				for (int mi=0; mi<num_mats; mi++)
				{
					g3dFragment *frag = i_Fragments.empty() ? NULL : i_Fragments[mi];
					matMaterial *mat = frag_info->m_Materials[mi].m_pMaterial;
					mtrPickUtil::FragmentMaterial info(frag, mat);
					
					int first_index = 0, num_indices = 0;
					frag_info->GetIndexRange(mi, first_index, num_indices);
					mtrFaceMesh mesh(&(frag_info->m_Vertices[0]), 
									frag_info->m_Vertices.size(), 
									&(frag_info->m_Indices[first_index]), 
									num_indices);
					m_Map[info] = mesh;
				}
			}

			//  handle subdivision fragments also now
			const maySubdivInfo *subdiv_info = dynamic_cast<const maySubdivInfo*>(&i_FragInfo);
			if (subdiv_info)
			{
					matMaterial *mat = subdiv_info->m_Material.m_pMaterial;
					g3dFragment *null_frag = NULL;
					mtrPickUtil::FragmentMaterial info(null_frag, mat);

					std::vector<envType::UInt32> tri_inds;
					subdiv_info->GenerateTriangleIndices(tri_inds);
					mtrFaceMesh mesh(&(subdiv_info->m_Vertices[0]), 
									subdiv_info->m_Vertices.size(), 
									&(tri_inds[0]), 
									tri_inds.size());
					m_Map[info] = mesh;
			}
		}

	private:
		mtrPickUtil::FaceMap& m_Map;
};

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

fsLocator find_general_texture_path(const fsLocator &i_Locator)
{
	// just return directory from filename for now
	fsLocator dir = i_Locator;
	dir.Pop();
	fsLocator tex_dir = dir;
	tex_dir.Pop();
	tex_dir.Push("General");
	tex_dir.Push("Textures");
	if (fsFileUtil::DirectoryExists(tex_dir))
		return tex_dir;

	return dir;
}


void restore_material()
{
	if (l_SelMaterial >= 0)
	{
		mtrMaterialUtil::ReplaceMaterial(l_HighlightMaterial, 
				l_ModelTemplate->GetMaterials()[l_SelMaterial], 
				l_ObjFragments);
	}
}

void highlight_material(int i)
{
	if (l_ModelTemplate)
	{
		mtrMaterialUtil::ReplaceMaterial(l_ModelTemplate->GetMaterials()[i], 
				l_HighlightMaterial, 
				l_ObjFragments);
	}
}

// sort_materials() - Get two material lists to match.
// This assumes that the template's materials came from the material table,
// which means they are in alphabetical order.  The l_MatData array is
// based on the order read from the file, and cannot be changed. So,
// we resort the template's material list to match the l_MatData list.
void sort_materials()
{
	if (l_MatData.empty()) return;
	if (l_MatData[0].GetMaterialName().empty()) return;

	std::map<std::string, int> remap;
	for (int i=0; i<l_MatData.size(); i++)
	{
		DBG_LOG2("l_MatData %d: %s", i, l_MatData[i].GetMaterialName().c_str());
		remap[l_MatData[i].GetMaterialName()] = i;

		int tex_num = l_MatData[i].GetNumTextureLayers();
		for (int t=0; t<tex_num; t++)
		{
			DBG_LOG1("  Texture: %s", l_MatData[i].GetTextureLayer(t).GetTextureName().c_str());
		}
		DBG_LOG1("  Glow Mask: %s", l_MatData[i].GetGlowMask().GetTextureName().c_str());
	}

	std::vector<matMaterial*> mat_list = l_ModelTemplate->GetMaterials();
	//l_ModelTemplate->GetMaterials().clear();

	std::map<std::string, int>::iterator it = remap.begin();
	for (int index = 0; it != remap.end(); ++it, ++index)
	{
		DBG_LOG2("Remap Material %d: %d", index, it->second);
		l_ModelTemplate->Materials()[it->second] = mat_list[index];	// old
		//l_ModelTemplate->GetMaterials()[index] = mat_list[it->second]; // bad
	}

}


} // end of namespace

//========================================================================
//	Initialize()
//========================================================================
void
mtrLevel::Initialize()
{
	// Set up highlight material
	l_HighlightMaterial = new matMaterial;
	l_HighlightMaterial->SetAmbient(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	l_HighlightMaterial->SetDiffuse(maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f));

	an2StateAnimation<maFloatRGBA>* color_anim = 
			new an2StateAnimation<maFloatRGBA>(	
					maFloatRGBA(0.7f, 0.2f, 0.2f, 1.0f),
					maFloatRGBA(0.7f, 0.7f, 0.7f, 1.0f),
					1.0f);
	color_anim->SetLooping(true);
	color_anim->SetReversing(true);

	matMatAnim* sel_box_anim = new matMatAnim(color_anim, matMatParamIndex::e_Ambient, 0.0f);
	l_HighlightMaterial->AddMatAnim(sel_box_anim);

}


//========================================================================
//	DeInitialize()
//========================================================================
void
mtrLevel::DeInitialize()
{
	Clear();

	delete l_HighlightMaterial;
	l_HighlightMaterial = NULL;
}



//============================================================================
//	Return number of Materials
//============================================================================
int		mtrLevel::GetNumMaterials()
{
	return l_MatData.size();
}

//============================================================================
//	Return name of Material with given index
//============================================================================
const char *	mtrLevel::GetMaterialName(int i_Index)
{
	if (!l_MatData[i_Index].GetMaterialName().empty())
		return l_MatData[i_Index].GetMaterialName().c_str();

	static char local_buffer[256];
	sprintf(local_buffer, "Material #%d", i_Index);

	return local_buffer;
}

//============================================================================
//	Return Material properties structure
//============================================================================
const mtrMaterialTemplate& mtrLevel::GetMaterialData(int i_Index)
{
	return l_MatData[i_Index];
}

//============================================================================
//	Set Material properties from structure
//============================================================================
void	mtrLevel::SetMaterialData(int i_Index,
						const mtrMaterialTemplate& i_Data)
{
	DBG_ASSERT0(i_Index<l_MatData.size(), "Material index out of range");

	// Note: handle this!
//	bool bTextureChanged = false;
//		(l_MatData[i_Index].GetTextureName() != i_Data.GetTextureName());
	bool bTextureChanged = mtrMaterialUtil::DidTextureChange(l_MatData[i_Index], i_Data);

	// preserve the material name
	std::string mat_name = l_MatData[i_Index].GetMaterialName();

	// Store values
	l_MatData[i_Index] = i_Data;

	l_MatData[i_Index].SetMaterialName(mat_name);

	try
	{
		mtrMaterialUtil::SetMaterialData(*(l_ModelTemplate->GetMaterials()[i_Index]),
					l_MatData[i_Index],
					*l_ModelTemplate,
					l_TextureDirectory,
					l_GeneralTextureDirectory,
					bTextureChanged);
	
	}
	catch( const fsFileDoesntExistX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		mtrErrorHandler *handler = mtrErrorHandler::GetErrorHandler();
		if (handler)
			handler->FileDoesntExist(filename.c_str());
	}
	catch( ... )
	{
		mtrErrorHandler *handler = mtrErrorHandler::GetErrorHandler();
		if (handler)
			handler->GeneralError();
		throw;
	}
}

//============================================================================
//	Select material
//============================================================================
void	mtrLevel::SelectMaterial(int i_Index)
{
	restore_material(); // restore highlight
	l_SelMaterial = i_Index;

	DBG_LOG2("Selecting Material %d: %s", i_Index, l_MatData[i_Index].GetMaterialName().c_str());

	// Call user callback to update interface
	if (l_SelectCallback)
		l_SelectCallback->SelectMaterial(i_Index);

	// always highlight material
	highlight_material(i_Index);
	if (!l_bDoHighlight) // if not always highlight, set time to stop
	{
		// set up flashing
		l_bFlashing = true;
		l_FlashEnd = appSimTime::GetTime() + c_FlashTime;
	}
}

//============================================================================
//	SetMaterialSelectCallback
//============================================================================
void	mtrLevel::SetMaterialSelectCallback(mtrMaterialSelectCallback *i_Callback)
{
	l_SelectCallback = i_Callback;
}
//============================================================================
//	SetModelChangeCallback
//============================================================================
void	mtrLevel::SetModelChangeCallback(mtrModelChangeCallback *i_Callback)
{
	l_ChangeCallback = i_Callback;
}

//============================================================================
//	SetDoHighlight
//============================================================================
void	mtrLevel::SetDoHighlight(bool i_bVal)
{
	if (i_bVal)
		highlight_material(l_SelMaterial);
	else
		restore_material();

	l_bDoHighlight = i_bVal;
}

//============================================================================
//	Think - Handle material animation timing
//============================================================================
void	mtrLevel::Think()
{
	mtrMouseSelector::Think();

	if (!l_bDoHighlight && l_bFlashing)
	{
		if (appSimTime::GetTime() > l_FlashEnd)
		{
			l_bFlashing = false;
			restore_material();
		}
	}
}

//========================================================================
//	Clear
//========================================================================
void
mtrLevel::Clear()
{
	restore_material();	// make sure highlight material is removed

	if (l_pObject)
		api3dScene::RemoveObject(l_pObject);
	delete l_pObject;
	l_pObject = NULL;
	l_ModelTemplate = NULL;	// owned by api3dObject


	l_SelMaterial = -1;
	l_MatData.clear();
	l_ObjFragments.clear(); // not owned
	l_MeshMap.clear();

	l_FileLocator.Clear();

	// Call user callback to update interface
	if (l_ChangeCallback)
		l_ChangeCallback->ModelChange();
}


//========================================================================
//	Save saves a level to a given locator
//========================================================================
void
mtrLevel::Save(const fsLocator& i_Locator)
{
	// Can't save empty file
	if (l_FileLocator.GetNumNames() == 0)
		return;

	try
	{
		if (!mtrMaterialSaver::Write( i_Locator, l_FileLocator, l_MatData ))
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Locator, filename);
			mtrErrorHandler *handler = mtrErrorHandler::GetErrorHandler();
			if (handler)
				handler->CantSaveFile(filename.c_str());
			return;
		}
	}
	catch( const fsReadOnlyX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		mtrErrorHandler *handler = mtrErrorHandler::GetErrorHandler();
		if (handler)
			handler->FileReadOnly(filename.c_str());
	}
	catch( ... )
	{
		mtrErrorHandler *handler = mtrErrorHandler::GetErrorHandler();
		if (handler)
			handler->GeneralError();
		throw;
	}

	l_FileLocator = i_Locator;
}



//========================================================================
//	LoadModel loads geometry from the given locator
//========================================================================
void 
mtrLevel::LoadModel(const fsLocator& i_Locator)
{
	Clear();

	try
	{
		// Load Model in new style
		l_TextureDirectory = find_texture_path(i_Locator);
		l_GeneralTextureDirectory = find_general_texture_path(l_TextureDirectory);

		fsResourceFinderDir finder(l_TextureDirectory);
		FaceMeshSink sink(l_MeshMap);
		std::auto_ptr<entModelTemplate> ent_template(
					entImport::LoadGeometry(i_Locator, finder, &sink) );
		scObject* obj_ptr = entImport::CreateObject( *ent_template );

		// gather fragments from object or template, because single skin and jointed
		//	objects have their own fragments that aren't in the model template.
		mtrMaterialUtil::GatherFragments(obj_ptr, *ent_template, l_ObjFragments);

		// pass ownership of pointers to new object
		l_ModelTemplate = ent_template.release();
		l_FileLocator = i_Locator;
		l_pObject = new api3dObjectGeom(l_ModelTemplate, obj_ptr);

		if (l_pObject)
		{
			api3dScene::AddObject(l_pObject);

			mtrLightMgr::SetCenter(l_pObject->GetWorldBox());
			FocusCamera();
		}

		//std::string dir;
		//fsFileUtil::LocatorToANSIFilename( i_Locator, dir );
		//DBG_LOG1( "texturedir %s", dir.c_str() );

		// read file again for the material info
		//
		mtrMaterialSaver::Read(i_Locator, l_MatData);
		int num_read_mats = l_MatData.size();
		int num_template_mats = l_ModelTemplate->GetMaterials().size();
		if (num_read_mats != num_template_mats)
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Locator, filename);
			mtrErrorHandler *handler = mtrErrorHandler::GetErrorHandler();
			if (handler)
				handler->CantParseMaterials(filename.c_str());
		}

		// get two materials lists to match
		if (mtrMaterialSaver::DidReadMaterialTable())
			sort_materials();

	}
	catch( const fsFileDoesntExistX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		mtrErrorHandler *handler = mtrErrorHandler::GetErrorHandler();
		if (handler)
			handler->FileDoesntExist(filename.c_str());
	}
	catch( ... )
	{
		mtrErrorHandler *handler = mtrErrorHandler::GetErrorHandler();
		if (handler)
			handler->GeneralError();
		throw;
	}

	// Call user callback to update interface
	if (l_ChangeCallback)
		l_ChangeCallback->ModelChange();
}


//========================================================================
//	DoMaterialPick - selects material from mouse pick
//========================================================================
void mtrLevel::DoMaterialPick(const maPoint3d &i_RayStart,
							 const maPoint3d &i_RayEnd)
{
	if (!l_pObject) return;

//	maPoint3d pts[2], norms[2];
//	pts[0] = i_RayStart;
//	pts[1] = i_RayEnd;
//	norms[0] = norms[1] = maPoint3d(0,1,0);
//	g3dFragmentManager::ModifyVertices(l_PickLine, 0, 2, pts, norms);

	//DBG_LOG2("Pick %d %d", i_MouseX, i_MouseY);
	matMaterial *pick_mat = mtrPickUtil::PickMaterial(l_pObject->Object(), l_MeshMap, 
											i_RayStart, i_RayEnd - i_RayStart);
	if (pick_mat)
	{
		for (int i=0; i<l_ModelTemplate->GetMaterials().size(); i++)
		{
			if (l_ModelTemplate->GetMaterials()[i] == pick_mat)
			{
				SelectMaterial(i);
				break;
			}
		}
	}

}


//========================================================================
//	Return directory from which to load textures
//========================================================================
const fsLocator&	mtrLevel::GetTextureDir()
{
	return l_TextureDirectory;
}


//========================================================================
// Focus camera on bounding box of object.
//========================================================================
void mtrLevel::FocusCamera()
{
	if (l_pObject)
	{
		cam3dMgr::FocusCamera(l_pObject->GetWorldBox());
	}
}

//============================================================================
//	Copy/Paste material info to/from clipboard
//============================================================================
void	mtrLevel::CopyMaterial()
{
	if (l_SelMaterial > -1 && l_SelMaterial < l_MatData.size())
	{
		l_ClipboardMaterial = l_MatData[l_SelMaterial];
		bHaveClipboardData = true;
	}
}
void	mtrLevel::PasteMaterial()
{
	if (bHaveClipboardData)
	{
		if (l_SelMaterial > -1 && l_SelMaterial < l_MatData.size())
		{
			SetMaterialData(l_SelMaterial, l_ClipboardMaterial);
			
			// Stop flashing so user can see material
			l_FlashEnd = appSimTime::GetTime();
		}
	}
}

bool	mtrLevel::HaveClipboardData()
{
	return bHaveClipboardData;
}

//========================================================================
//	ReloadTextures - forces reload of all textures
//========================================================================
void	mtrLevel::ReloadTextures()
{
	std::vector<matMaterial*> &mat_list = l_ModelTemplate->Materials();

	// Remove all old textures
	int i;
	for (i=0; i<mat_list.size(); i++)
		mtrMaterialUtil::RemoveTextures(*mat_list[i], *l_ModelTemplate);
	
	// Reset the material values
	for (i=0; i<mat_list.size(); i++)
		mtrMaterialUtil::SetMaterialData(*mat_list[i],
										l_MatData[i],
										*l_ModelTemplate,
										l_TextureDirectory,
										l_GeneralTextureDirectory,
										true);	// texture_changed == true
}

//========================================================================
//	ImportMaterials - read materials from file and apply
//	to current geometry by matching names
//========================================================================
int	mtrLevel::ImportMaterials(const fsLocator &i_Locator)
{
	std::vector<mtrMaterialTemplate> materials;

	try
	{
		mtrMaterialSaver::Read( i_Locator, materials );
	}
	catch( const fsFileDoesntExistX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		mtrErrorHandler *handler = mtrErrorHandler::GetErrorHandler();
		if (handler)
			handler->FileDoesntExist(filename.c_str());
		return 0;
	}
	catch( ... )
	{
		mtrErrorHandler *handler = mtrErrorHandler::GetErrorHandler();
		if (handler)
			handler->GeneralError();
		throw;
	}

	// Check to see if materials are named
	int num_mats = GetNumMaterials();
	bool bCanUseNames = true;
	int i;
	for (i=0; i<num_mats; i++)
	{
		if (l_MatData[i].GetMaterialName().empty())
			bCanUseNames = false;
	}

	// If not named and number of materials don't match
	// can't do anything.
	if (!bCanUseNames && (num_mats != materials.size()))
		return 0;

	// load material names from current model into 
	// set of strings
	//
	std::map<std::string, int> name_map;
	for (i=0; i<num_mats; i++)
	{
		std::string name = GetMaterialName(i);
		name_map[name] = i;
	}

	// Find if material names from given list exist in name map
	//
	int count = 0;
	for (i=0; i<materials.size(); i++)
	{
		std::string name = materials[i].GetMaterialName();
		if (bCanUseNames)
		{
			std::map<std::string, int>::iterator it = name_map.find(name);
			if (it != name_map.end())
			{
				// if found, set material data
				SetMaterialData(it->second, materials[i]);
				count++;
			}
		}
		else
		{
			// Use material index as match
			SetMaterialData(i, materials[i]);
			count++;
		}
	}

	return count;
}

//============================================================================
//	Use structure from template manager to set material with given index.
//	This applies an algorithm to find the appropriate texture names.
//============================================================================
void	mtrLevel::ApplyMaterialTemplate(int i_Index,
						const mtrMaterialTemplate& i_Data)
{
	DBG_ASSERT0(i_Index<l_MatData.size(), "Material index out of range");
	const mtrMaterialTemplate &old_data = l_MatData[i_Index];
	if (old_data.GetNumTextureLayers() > 0)
	{
		// Make copy of new data in order to alter texture filenames
		mtrMaterialTemplate custom = i_Data;

		// Get base file name, use this as the base for the other filename
		const mtrTextureLayer &base_layer = old_data.GetTextureLayer(0);
		std::string base_fname = base_layer.GetTextureName();
		if (base_fname.size() > 0)
		{
			// find position of extension
			int pos = base_fname.size()-1;
			while (pos >= 0)
			{
				if (base_fname[pos] == '.')
				{
					break;
				}
				pos--;
			}

			for (int i=0; i<custom.GetNumTextureLayers(); i++)
			{
				mtrTextureLayer &layer = custom.GetTextureLayer(i);
				switch (layer.GetTextureType())
				{
					// keep base filename from old material
				case mtrTextureLayer::e_Normal:
					layer.SetTextureName(base_fname);
					break;
					// add "_nmp" to filename
				case mtrTextureLayer::e_BumpHeight:
					if (pos > 0)
					{
						std::string nmp_fname = base_fname;
						nmp_fname.insert(pos, "_nmp");
						DBG_LOG1("Considering normal map filename: %s", nmp_fname.c_str());
						fsLocator nmp_loc = l_TextureDirectory;
						nmp_loc.Push(nmp_fname.c_str());
						if (fsFileUtil::FileExists(nmp_loc))
						{
							layer.SetTextureName(nmp_fname);
						}
					}
					break;
					// add "_spec" to filename
				case mtrTextureLayer::e_SpecularMap:
					if (pos > 0)
					{
						std::string spec_fname = base_fname;
						spec_fname.insert(pos, "_spec");
						DBG_LOG1("Considering specular map filename: %s", spec_fname.c_str());
						fsLocator spec_loc = l_TextureDirectory;
						spec_loc.Push(spec_fname.c_str());
						if (fsFileUtil::FileExists(spec_loc))
						{
							layer.SetTextureName(spec_fname);
						}
					}
					break;
					// add "_glos" to filename
				case mtrTextureLayer::e_GlossMap:
					if (pos > 0)
					{
						std::string glos_fname = base_fname;
						glos_fname.insert(pos, "_glos");
						DBG_LOG1("Considering gloss map filename: %s", glos_fname.c_str());
						fsLocator glos_loc = l_TextureDirectory;
						glos_loc.Push(glos_fname.c_str());
						if (fsFileUtil::FileExists(glos_loc))
						{
							layer.SetTextureName(glos_fname);
						}
					}
					break;
					// use template filename (by not doing anything)
				default:
					break;
				}
				
			}
		}

		// Now set altered data
		SetMaterialData(i_Index, custom);
	}
	else
	{
		// no base texture to use as root for texture filenames,
		// so just set material as is.
		SetMaterialData(i_Index, i_Data);
	}
}
