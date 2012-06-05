/*****************************************************************************
**  entModelInstance.hpp
**
**      A entModelInstance contains the instance-specific allocated
**	objects used by scObjects.  The model template classes contain
**	the information to generate scObjects, but this class is used
**	to store the information allocated specifically to an instance
**	made from a template. It should be deleted when the scObject is freed.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef ENT_MODELINSTANCE_HPP
#error entModelInstance.hpp multiply included
#endif
#define ENT_MODELINSTANCE_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class g3dFragment;
class matMaterial;
class matTexture;


//============================================================================
//============================================================================
class entModelInstance
{
	public:
		//--------------------------------------------------------------------
		//	Constructor
		//--------------------------------------------------------------------
		entModelInstance();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~entModelInstance();

		//--------------------------------------------------------------------
		//	GetMaterials returns the list of materials used by the object.
		//--------------------------------------------------------------------
		inline std::vector<matMaterial*>& Materials();
		inline const std::vector<matMaterial*>& GetMaterials() const;

		//--------------------------------------------------------------------
		//	GetFragments returns the list of fragments used by the object.
		//	This is not used for single-skin objects, for which each owns
		//	its own fragment.
		//--------------------------------------------------------------------
		inline std::vector<g3dFragment*>& Fragments();
		inline const std::vector<g3dFragment*>& GetFragments() const;

		//--------------------------------------------------------------------
		//	GetTextures returns the list of textures used by the object.
		//--------------------------------------------------------------------
		inline std::vector<matTexture*>& Textures();
		inline const std::vector<matTexture*>& GetTextures() const;

	protected:
		std::vector<matMaterial*> m_Materials;
		std::vector<g3dFragment*> m_Fragments;
		std::vector<matTexture*> m_Textures;
};


//--------------------------------------------------------------------
//	GetMaterials returns the list of materials used by the object.
//--------------------------------------------------------------------
inline std::vector<matMaterial*>& entModelInstance::Materials()
{
	return m_Materials;
}
inline const std::vector<matMaterial*>& entModelInstance::GetMaterials() const
{
	return m_Materials;
}

//--------------------------------------------------------------------
//	GetFragments returns the list of fragments used by the object.
//	This is not used for single-skin objects, for which each owns
//	its own fragment.
//--------------------------------------------------------------------
inline std::vector<g3dFragment*>& entModelInstance::Fragments()
{
	return m_Fragments;
}
inline const std::vector<g3dFragment*>& entModelInstance::GetFragments() const
{
	return m_Fragments;
}

//--------------------------------------------------------------------
//	GetTextures returns the list of textures used by the object.
//--------------------------------------------------------------------
inline std::vector<matTexture*>& entModelInstance::Textures()
{
	return m_Textures;
}
inline const std::vector<matTexture*>& entModelInstance::GetTextures() const
{
	return m_Textures;
}
