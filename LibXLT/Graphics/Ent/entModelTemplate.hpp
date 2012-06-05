/*****************************************************************************
**  entModelTemplate.hpp
**
**      A entModelTemplate contains the geometry information needed to
**	generate scObjects.  This only includes information about the model,
**	such as fragments, materials, and textures.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef ENT_MODELTEMPLATE_HPP
#error entModelTemplate.hpp multiply included
#endif
#define ENT_MODELTEMPLATE_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif
#ifndef MDL_MATINFOTABLE_HPP
#include "Graphics/mdl/mdlMatInfoTable.hpp"
#endif 

#include <vector>


//============================================================================
//============================================================================
class g3dFragment;
class matMaterial;
class matTexture;


//============================================================================
//============================================================================
class entModelTemplate
{
	public:
		//--------------------------------------------------------------------
		//	Constructor
		//	i_PathIndex is the base index for this entity template, under this
		// directory the Textures, Models, Sounds and Data directories can be found.
		//--------------------------------------------------------------------
		entModelTemplate();
		//explicit entModelTemplate( const fsLocator& i_TextureDir );
		//explicit entModelTemplate( int i_PathIndex );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~entModelTemplate();

		//--------------------------------------------------------------------
		//	Return texture diretory for this template.
		//--------------------------------------------------------------------
		//inline const fsLocator& GetTextureDirectory();

		//--------------------------------------------------------------------
		//	GetMaterials returns the list of materials used by the object.
		//--------------------------------------------------------------------
		inline std::vector<matMaterial*>& Materials();
		inline const std::vector<matMaterial*>& GetMaterials() const;

		//--------------------------------------------------------------------
		// Access to material table
		//--------------------------------------------------------------------
		inline mdlMatInfoTable& MaterialTable();
		inline const mdlMatInfoTable& GetMaterialTable() const;

		//--------------------------------------------------------------------
		//	GetFragments returns the list of fragments used by the object.
		//	This is not used for single-skin objects, for which each owns
		//	its own fragment.
		//--------------------------------------------------------------------
		inline std::vector<g3dFragment*>& Fragments();
		inline const std::vector<g3dFragment*>& GetFragments() const;

		////--------------------------------------------------------------------
		////	GetTextures returns the list of textures used by the object.
		////--------------------------------------------------------------------
		//inline std::vector<matTexture*>& Textures();
		//inline const std::vector<matTexture*>& GetTextures() const;

	protected:
		std::vector<matMaterial*> m_Materials;
		mdlMatInfoTable m_MaterialTable;
		std::vector<g3dFragment*> m_Fragments;
		//std::vector<matTexture*> m_Textures;
};


//--------------------------------------------------------------------
//	Return texture diretory for this template.
//--------------------------------------------------------------------
//inline const fsLocator& entModelTemplate::GetTextureDirectory()
//{
//	return m_TextureDir;
//}

//--------------------------------------------------------------------
//	GetMaterials returns the list of materials used by the object.
//--------------------------------------------------------------------
inline std::vector<matMaterial*>& entModelTemplate::Materials()
{
	return m_Materials;
}
inline const std::vector<matMaterial*>& entModelTemplate::GetMaterials() const
{
	return m_Materials;
}

//--------------------------------------------------------------------
// Access to material table
//--------------------------------------------------------------------
inline mdlMatInfoTable& entModelTemplate::MaterialTable()
{
	return m_MaterialTable;
}
inline const mdlMatInfoTable& entModelTemplate::GetMaterialTable() const
{
	return m_MaterialTable;
}

//--------------------------------------------------------------------
//	GetFragments returns the list of fragments used by the object.
//	This is not used for single-skin objects, for which each owns
//	its own fragment.
//--------------------------------------------------------------------
inline std::vector<g3dFragment*>& entModelTemplate::Fragments()
{
	return m_Fragments;
}
inline const std::vector<g3dFragment*>& entModelTemplate::GetFragments() const
{
	return m_Fragments;
}

////--------------------------------------------------------------------
////	GetTextures returns the list of textures used by the object.
////--------------------------------------------------------------------
//inline std::vector<matTexture*>& entModelTemplate::Textures()
//{
//	return m_Textures;
//}
//inline const std::vector<matTexture*>& entModelTemplate::GetTextures() const
//{
//	return m_Textures;
//}
