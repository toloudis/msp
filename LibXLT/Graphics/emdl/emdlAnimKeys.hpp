/*****************************************************************************
**  emdlAnimKeys.hpp
**
**      emdlAnimKeys - class for maintaining shared keyframe data
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef EMDL_ANIMKEYS_HPP
#error emdlAnimKeys.hpp multiply included
#endif
#define EMDL_ANIMKEYS_HPP

#ifndef ENT_ANIMKEYS_HPP
#include "Graphics/ent/entAnimKeys.hpp"
#endif
#ifndef SMDL_KEYROOTMAP_HPP
#include "Graphics/smdl/smdlKeyRootMap.hpp"
#endif 


//============================================================================
//	emdlAnimKeys
//============================================================================
class emdlAnimKeys : public entAnimKeys
{
	public:
		//--------------------------------------------------------------------
		//	emdlAnimKeys constructor
		//--------------------------------------------------------------------
		emdlAnimKeys();

		//--------------------------------------------------------------------
		//	destructor
		//--------------------------------------------------------------------
		virtual ~emdlAnimKeys();

		//--------------------------------------------------------------------
		// Animation Roots are mappings from name to smdlTree
		//--------------------------------------------------------------------
		inline const smdlKeyRootMap& GetKeyRoots() const;
		inline smdlKeyRootMap& KeyRoots();

		//--------------------------------------------------------------------
		// Accessor functions
		//--------------------------------------------------------------------
		//inline const smdlTree<smdlGeoAnimKeys>& GetKeys() const;
		//inline smdlTree<smdlGeoAnimKeys>& Keys();
		inline std::vector<anKeyData<maPoint3d>*>&	TranslateChannels();
		inline std::vector<anKeyData<maRotation>*>&	RotateChannels();
		inline std::vector<anKeyData<maVector3d>*>&	ScaleChannels();
		inline std::vector<anKeyDataBase<bool>*>&	VisibleChannels();

		//--------------------------------------------------------------------
		// Name of joint that is root of this subanimation
		//--------------------------------------------------------------------
		//void SetNameOfRoot(const std::string& i_Name);
		//const std::string& GetNameOfRoot() const;

		//--------------------------------------------------------------------
		// AdditiveAnimation is true if the transformations are deltas
		//	from the base pose and can be used in additive subanimations.
		//--------------------------------------------------------------------
		void SetAdditiveAnimation(bool i_bDelta);
		inline bool GetAdditiveAnimation() const;

		//--------------------------------------------------------------------
		// Some animation that isn't coming from Maya should ignore the
		//	joint orientation built into the scene graph.
		//--------------------------------------------------------------------
		void SetIgnoreJointOrientation(bool i_bIgnore);
		inline bool GetIgnoreJointOrientation() const;

	private:
		smdlKeyRootMap m_KeyRoots;
		//smdlTree<smdlGeoAnimKeys> m_Keys;
		std::vector<anKeyData<maPoint3d>* > m_TranslateChannels;
		std::vector<anKeyData<maRotation>* > m_RotateChannels;
		std::vector<anKeyData<maVector3d>* > m_ScaleChannels;
		std::vector<anKeyDataBase<bool>* > m_VisibleChannels;
		std::string m_NameOfRoot;
		bool m_bAdditive;
		bool m_bIgnoreJointOrientation;
};


//--------------------------------------------------------------------
// Accessor functions
//--------------------------------------------------------------------
inline const smdlKeyRootMap& emdlAnimKeys::GetKeyRoots() const
{
	return m_KeyRoots;
}
inline smdlKeyRootMap& emdlAnimKeys::KeyRoots()
{
	return m_KeyRoots;
}

inline std::vector<anKeyData<maPoint3d>*>&	emdlAnimKeys::TranslateChannels()
{
	return m_TranslateChannels;
}

inline std::vector<anKeyData<maRotation>*>&	emdlAnimKeys::RotateChannels()
{
	return m_RotateChannels;
}

inline std::vector<anKeyData<maVector3d>*>&	emdlAnimKeys::ScaleChannels()
{
	return m_ScaleChannels;
}
inline std::vector<anKeyDataBase<bool>*>&	emdlAnimKeys::VisibleChannels()
{
	return m_VisibleChannels;
}

//--------------------------------------------------------------------
// AdditiveAnimation is true if the transformations are deltas
//	from the base pose and can be used in additive subanimations.
//--------------------------------------------------------------------
inline bool emdlAnimKeys::GetAdditiveAnimation() const
{
	return m_bAdditive;
}

//--------------------------------------------------------------------
// Some animation that isn't coming from Maya should ignore the
//	joint orientation built into the scene graph.
//--------------------------------------------------------------------
inline bool emdlAnimKeys::GetIgnoreJointOrientation() const
{
	return m_bIgnoreJointOrientation;
}

