/*****************************************************************************
**	smdlGeoAnimKeys.hpp
**
**		smdlGeoAnimKeys - animation information for a node
**	in hierarchical and jointed animations.
**	Translation data is stored in vector3d and
**	rotation data is stored as quaternion.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_GEOANIMKEYS_HPP
#error smdlGeoAnimKeys.hpp multiply included
#endif
#define SMDL_GEOANIMKEYS_HPP

#ifndef AN_KEYDATA_HPP
#include "Graphics/an/anKeyData.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif
#ifndef MA_VECTOR3D_HPP
#include "Core/ma/maVector3d.hpp"
#endif


//============================================================================
//============================================================================
class smdlGeoAnimKeys
{
	public:
		//--------------------------------------------------------------------
		//	Default constructor.
		//--------------------------------------------------------------------
		smdlGeoAnimKeys();

		//--------------------------------------------------------------------
		//	HasAnimation - returns true if some channel has animation
		//--------------------------------------------------------------------
		inline bool	HasAnimation() const;

		//--------------------------------------------------------------------
		// GetTranslation	- get linear interpolated translation key data
		//--------------------------------------------------------------------
		maVector3d		GetTranslation(float i_Time) const;

		//--------------------------------------------------------------------
		// GetRotation	- get slerp-ed rotation key data
		//--------------------------------------------------------------------
		maRotation		GetRotation(float i_Time) const;

		//--------------------------------------------------------------------
		// GetScale	- get linear interpolated scale key data
		//--------------------------------------------------------------------
		maVector3d		GetScale(float i_Time) const;

		//--------------------------------------------------------------------
		// GetVisible	- get non-interpolated visibility key data
		//--------------------------------------------------------------------
		bool GetVisible(float i_Time) const;

		//--------------------------------------------------------------------
		//	Accessors - const and non-const.
		//--------------------------------------------------------------------
		anKeyData<maVector3d>* GetTranslate();
		const anKeyData<maVector3d>* GetTranslate() const;
		anKeyData<maRotation>* GetRotate();
		const anKeyData<maRotation>* GetRotate() const;
		anKeyData<maVector3d>* GetScale();
		const anKeyData<maVector3d>* GetScale() const;
		anKeyDataBase<bool>* GetVisible();
		const anKeyDataBase<bool>* GetVisible() const;

		//--------------------------------------------------------------------
		//	Mutators.  The smdlGeoAnimKeys does not own the animations passed
		//	into these functions, nor does it copy them, so make sure you
		//	don't delete the objects out from under it.  Because of this,
		//	it is possible to reuse and share animation channels.
		//--------------------------------------------------------------------
		void SetTranslate(anKeyData<maVector3d>* i_Anim);
		void SetRotate(anKeyData<maRotation>* i_Anim);
		void SetScale(anKeyData<maVector3d>* i_Anim);
		void SetVisible(anKeyDataBase<bool>* i_Anim);

	private:
		anKeyData<maVector3d>* m_Translate;
		anKeyData<maRotation>* m_Rotate;
		anKeyData<maVector3d>* m_Scale;
		anKeyDataBase<bool>* m_Visible;
};


//--------------------------------------------------------------------
//	HasAnimation - returns true if some channel has animation
//--------------------------------------------------------------------
inline bool	smdlGeoAnimKeys::HasAnimation() const
{
	return ((m_Translate != NULL) 
		||  (m_Rotate != NULL) 
		||  (m_Scale != NULL) 
		||  (m_Visible != NULL));
}

//--------------------------------------------------------------------
//	Accessors - const and non-const.
//--------------------------------------------------------------------
inline anKeyData<maVector3d>*
smdlGeoAnimKeys::GetTranslate()
{
	return m_Translate;
}

inline const anKeyData<maVector3d>*
smdlGeoAnimKeys::GetTranslate() const
{
	return m_Translate;
}

inline anKeyData<maRotation>* smdlGeoAnimKeys::GetRotate()
{
	return m_Rotate;
}

inline const anKeyData<maRotation>*
smdlGeoAnimKeys::GetRotate() const
{
	return m_Rotate;
}

inline anKeyData<maVector3d>*
smdlGeoAnimKeys::GetScale()
{
	return m_Scale;
}

inline const anKeyData<maVector3d>*
smdlGeoAnimKeys::GetScale() const
{
	return m_Scale;
}

inline anKeyDataBase<bool>*
smdlGeoAnimKeys::GetVisible()
{
	return m_Visible;
}

inline const anKeyDataBase<bool>*
smdlGeoAnimKeys::GetVisible() const
{
	return m_Visible;
}
