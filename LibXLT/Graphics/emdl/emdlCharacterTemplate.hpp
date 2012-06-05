/*****************************************************************************
**  emdlCharacterTemplate.hpp
**
**      A emdlCharacterTemplate contains the geometry information needed to
**	generate smdlCharacterObjects.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef EMDL_CHARACTERTEMPLATE_HPP
#error emdlCharacterTemplate.hpp multiply included
#endif
#define EMDL_CHARACTERTEMPLATE_HPP

#ifndef ENT_MODELTEMPLATE_HPP
#include "Graphics/ent/entModelTemplate.hpp"
#endif
#ifndef MDL_SKININFO_HPP
#include "Graphics/mdl/mdlSkinInfo.hpp"
#endif 
//#ifndef SMDL_CHARACTERSKIN_HPP
//#include "Graphics/smdl/smdlCharacterSkin.hpp"
//#endif
//#ifndef MDL_SUBDIVINFO_HPP
//#include "Graphics/mdl/mdlSubdivInfo.hpp"
//#endif
//#ifndef MDL_FRAGINFO_HPP
//#include "Graphics/mdl/mdlFragInfo.hpp"
//#endif


//============================================================================
//============================================================================
class g3dSceneNode;
struct mdlHairInfo;


//============================================================================
//============================================================================
class emdlCharacterTemplate : public entModelTemplate
{
	public:
		//--------------------------------------------------------------------
		//	Constructor
		//--------------------------------------------------------------------
		emdlCharacterTemplate();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~emdlCharacterTemplate();

		//--------------------------------------------------------------------
		//	GetModel returns the scene node representing root of the 
		//	scene graph hierarchy for this object.
		//--------------------------------------------------------------------
		inline const g3dSceneNode* GetModel() const;
		inline g3dSceneNode* GetModel();

		//--------------------------------------------------------------------
		//	SetModel sets the scene node representing the root of the
		//	object.
		//--------------------------------------------------------------------
		void SetModel( g3dSceneNode* i_pNode );

		//--------------------------------------------------------------------
		//	These functions are only used for character models - they
		//	return information needed for the skin.
		//--------------------------------------------------------------------
		//inline std::vector<mdlSubdivInfo>& SubdivInfos();
		//inline const std::vector<mdlSubdivInfo>& GetSubdivInfos() const;
		//inline std::vector<mdlFragInfo>& FragInfos();
		//inline const std::vector<mdlFragInfo>& GetFragInfos() const;
		//inline std::vector<smdlCharacterSkin>& CharacterSkins();
		//inline const std::vector<smdlCharacterSkin>& GetCharacterSkins() const;

		inline std::vector<mdlSkinInfo>& SkinnedSurfaces();
		inline const std::vector<mdlSkinInfo>& GetSkinnedSurfaces() const;

		inline std::vector< shared_ptr<mdlHairInfo> >& HairSurfaces();
		inline const std::vector< shared_ptr<mdlHairInfo> >& GetHairSurfaces() const;

		//inline const smdlJoint* GetJoints() const;
		//void SetJoints( smdlJoint* i_pHead );
		inline const fsLocator& GetFilename() const;
		void SetFilename( const fsLocator& i_Locator );

	private:
		g3dSceneNode* m_pModel;
		//smdlJoint* m_pJoints;
		std::vector<mdlSkinInfo> m_SkinnedSurfaces;
		std::vector< shared_ptr<mdlHairInfo> > m_HairSurfaces;
		fsLocator m_Filename;
};


//--------------------------------------------------------------------
//	GetModel returns the scene node representing the root of the
//	object.
//--------------------------------------------------------------------
inline const g3dSceneNode* emdlCharacterTemplate::GetModel() const
{
	return m_pModel;
}
inline g3dSceneNode* emdlCharacterTemplate::GetModel()
{
	return m_pModel;
}

//--------------------------------------------------------------------
//	GetSubdivInfos
//--------------------------------------------------------------------
//inline std::vector<mdlSubdivInfo>& emdlCharacterTemplate::SubdivInfos()
//{
//	return m_SubdivInfos;
//}
//inline const std::vector<mdlSubdivInfo>& emdlCharacterTemplate::GetSubdivInfos() const
//{
//	return m_SubdivInfos;
//}

//--------------------------------------------------------------------
//	GetFragInfos
//--------------------------------------------------------------------
//inline std::vector<mdlFragInfo>& emdlCharacterTemplate::FragInfos()
//{
//	return m_FragInfos;
//}
//inline const std::vector<mdlFragInfo>& emdlCharacterTemplate::GetFragInfos() const
//{
//	return m_FragInfos;
//}

//--------------------------------------------------------------------
//	GetJoints 
//--------------------------------------------------------------------
//inline const smdlJoint* emdlCharacterTemplate::GetJoints() const
//{
//	return m_pJoints;
//}

//--------------------------------------------------------------------
//	GetCharacterSkins 
//--------------------------------------------------------------------
//inline std::vector<smdlCharacterSkin>& emdlCharacterTemplate::CharacterSkins()
//{
//	return m_CharacterSkins;
//}
//inline const std::vector<smdlCharacterSkin>& emdlCharacterTemplate::GetCharacterSkins() const
//{
//	return m_CharacterSkins;
//}

//--------------------------------------------------------------------
//	GetSkinnedSurfaces 
//--------------------------------------------------------------------
inline std::vector<mdlSkinInfo>& emdlCharacterTemplate::SkinnedSurfaces()
{
	return m_SkinnedSurfaces;
}
inline const std::vector<mdlSkinInfo>& emdlCharacterTemplate::GetSkinnedSurfaces() const
{
	return m_SkinnedSurfaces;
}
//--------------------------------------------------------------------
//	GetHairSurfaces 
//--------------------------------------------------------------------
inline std::vector< shared_ptr<mdlHairInfo> >& emdlCharacterTemplate::HairSurfaces()
{
	return m_HairSurfaces;
}
inline const std::vector< shared_ptr<mdlHairInfo> >& emdlCharacterTemplate::GetHairSurfaces() const
{
	return m_HairSurfaces;
}

//--------------------------------------------------------------------
//	GetFilename 
//--------------------------------------------------------------------
inline const fsLocator& emdlCharacterTemplate::GetFilename() const
{
	return m_Filename;
}
