/*****************************************************************************
**  entLODModelTemplate.hpp
**
**      Contains a group of templates that hold the geometry information needed to
**	generate scObjects.  
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef ENT_LODMODELTEMPLATE_HPP
#error entLODModelTemplate.hpp multiply included
#endif
#define ENT_LODMODELTEMPLATE_HPP

#ifndef ENT_MODELTEMPLATE_HPP
#include "Graphics/ent/entModelTemplate.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class g3dFragment;
class matMaterial;
class matTexture;


//============================================================================
//============================================================================
class entLODModelTemplate : public entModelTemplate
{
	public:
		//--------------------------------------------------------------------
		//	Constructor
		//--------------------------------------------------------------------
		entLODModelTemplate();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~entLODModelTemplate();

		//--------------------------------------------------------------------
		//	GetMaterials returns the list of materials used by the object.
		//--------------------------------------------------------------------
		//inline std::vector<matMaterial*>& Materials();
		//inline const std::vector<matMaterial*>& GetMaterials() const;

		//--------------------------------------------------------------------
		//	GetFragments returns the list of fragments used by the object.
		//	This is not used for single-skin objects, for which each owns
		//	its own fragment.
		//--------------------------------------------------------------------
		//inline std::vector<g3dFragment*>& Fragments();
		//inline const std::vector<g3dFragment*>& GetFragments() const;

		//--------------------------------------------------------------------
		//	GetTextures returns the list of textures used by the object.
		//--------------------------------------------------------------------
		//inline std::vector<matTexture*>& Textures();
		//inline const std::vector<matTexture*>& GetTextures() const;

		//--------------------------------------------------------------------
		//	add a model template.  the return value is the index of it.
		//--------------------------------------------------------------------
		int AddTemplate( entModelTemplate* i_pModelTemplate, std::string& i_ModelFile, float i_fLODDistance, bool i_bDefault );

		//--------------------------------------------------------------------
		//
		//--------------------------------------------------------------------
		int	GetNumberOfTemplates() const;

		//--------------------------------------------------------------------
		//
		//--------------------------------------------------------------------
		entModelTemplate* GetTemplate( int i_Index ) const;

		//--------------------------------------------------------------------
		//	delete the current template at i_Index and set the new one
		//--------------------------------------------------------------------
		void ReplaceTemplate( int i_Index, entModelTemplate* i_pModelTemplate );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		float GetLODDistance( int i_Index ) const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetLODDistance( int i_Index, float i_fDist );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		bool GetLODDefault( int i_Index ) const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetLODDefault( int i_Index, bool i_bDefault );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		const std::string& GetLODModelFile( int i_Index ) const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetLODModelFile( int i_Index, std::string& i_ModelFile );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
	private:

		struct lod_model_info
		{
			entModelTemplate*	m_pTemplate;
			std::string			m_ModelFile;
			float				m_fDistance;
			bool				m_bDefault;
			//bool				m_bModelNeedsToBeReloaded;	// not saved
		};

		int	m_CurrentTemplate;

		std::vector<lod_model_info> m_ModelTemplates;
};

////--------------------------------------------------------------------
////	GetMaterials returns the list of materials used by the object.
////--------------------------------------------------------------------
//inline std::vector<matMaterial*>& entLODModelTemplate::Materials()
//{
//	//DBG_ASSERT0( i_ModelTemplateIndex < m_ModelTemplates.size(), "invalid index" );
//
//	return m_ModelTemplates[ m_CurrentTemplate ].m_pTemplate->Materials();
//}
//inline const std::vector<matMaterial*>& entLODModelTemplate::GetMaterials() const
//{
//	return m_ModelTemplates[ m_CurrentTemplate ].m_pTemplate->GetMaterials();
//}
//
////--------------------------------------------------------------------
////	GetFragments returns the list of fragments used by the object.
////	This is not used for single-skin objects, for which each owns
////	its own fragment.
////--------------------------------------------------------------------
//inline std::vector<g3dFragment*>& entLODModelTemplate::Fragments()
//{
//	return m_ModelTemplates[ m_CurrentTemplate ].m_pTemplate->Fragments();
//}
//inline const std::vector<g3dFragment*>& entLODModelTemplate::GetFragments() const
//{
//	return m_ModelTemplates[ m_CurrentTemplate ].m_pTemplate->GetFragments();
//}
//
////--------------------------------------------------------------------
////	GetTextures returns the list of textures used by the object.
////--------------------------------------------------------------------
//inline std::vector<matTexture*>& entLODModelTemplate::Textures()
//{
//	return m_ModelTemplates[ m_CurrentTemplate ].m_pTemplate->Textures();
//}
//inline const std::vector<matTexture*>& entLODModelTemplate::GetTextures() const
//{
//	return m_ModelTemplates[ m_CurrentTemplate ].m_pTemplate->GetTextures();
//}
