/*****************************************************************************
**	smdlGeoAnimKeys.cpp
**
**		smdlGeoAnimKeys - animation information for a node
**	in hierarchical and jointed animations.
**	Translation data is stored in vector3d and
**	rotation data is stored as quaternion.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/smdlGeoAnimKeys.hpp"


//--------------------------------------------------------------------
//	Default constructor.
//--------------------------------------------------------------------
smdlGeoAnimKeys::smdlGeoAnimKeys()
:	m_Translate(NULL),
	m_Rotate(NULL),
	m_Scale(NULL),
	m_Visible(NULL)
{
}


//--------------------------------------------------------------------
// GetTranslation	- get linear interpolated translation key data
//--------------------------------------------------------------------
maVector3d
smdlGeoAnimKeys::GetTranslation(float i_Time) const
{
	if (m_Translate)
		return m_Translate->GetValue(i_Time);
	else
		return maVector3d(0,0,0);
}

//--------------------------------------------------------------------
// GetRotation	- get slerp-ed rotation key data
//--------------------------------------------------------------------
maRotation
smdlGeoAnimKeys::GetRotation(float i_Time) const
{
	if (!m_Rotate)
		return maRotation();

	int key1, key2;
	m_Rotate->GetBracketingKeyData(	i_Time,
								key1,
								key2);

	float t1, t2;
	maRotation prev_rot, next_rot;
	m_Rotate->GetKeyData(key1, t1, prev_rot);
	m_Rotate->GetKeyData(key2, t2, next_rot);

	// testing without interpolation
	//return prev_rot;

	if ( t1 == t2 )
		return prev_rot;

	float alpha = (i_Time - t1) / (t2 - t1);
	maRotation rot;
	rot.Slerp(prev_rot, next_rot, alpha);
	return rot;

}

//--------------------------------------------------------------------
// GetScale	- get linear interpolated sclae key data
//--------------------------------------------------------------------
maVector3d
smdlGeoAnimKeys::GetScale(float i_Time) const
{
	if (m_Scale)
		return m_Scale->GetValue(i_Time);
	else
		return maVector3d(1,1,1);
}

//--------------------------------------------------------------------
// GetVisible - interpolation between keys
// is based on a switch 95% of the way between keys. 
//--------------------------------------------------------------------
bool
smdlGeoAnimKeys::GetVisible(float i_Time) const
{
	// Non-interpolating version
	//if (m_Visible)
	//{
	//	int index = m_Visible->GetLowerKeyIndex(i_Time);
	//	if (index < m_Visible->GetNumKeys())
	//	{
	//		float time = 0;
	//		bool value = true;
	//		m_Visible->GetKeyData(index, time, value);
	//		return value;
	//	}
	//}

	// Switch visibility 95% between keys 
	if (m_Visible)
	{
		int key1, key2;
		m_Visible->GetBracketingKeyData(	i_Time,
									key1,
									key2);

		float t1, t2;
		bool prev_vis, next_vis;
		m_Visible->GetKeyData(key1, t1, prev_vis);
		m_Visible->GetKeyData(key2, t2, next_vis);

		if ( t1 == t2 )
			return prev_vis;

		// Change from one visibility setting to the somewhere
		// between the keys, this helps hide round off errors.
		float alpha = (i_Time - t1) / (t2 - t1);
		if (alpha > 0.95f)
			return next_vis;
		else
			return prev_vis;
	}

	return true;
}

//--------------------------------------------------------------------
//	Mutators.  The smdlGeoAnimKeys does not own the animations passed
//	into these functions, nor does it copy them, so make sure you
//	don't delete the objects out from under it.  Because of this,
//	it is possible to reuse and share animation channels.
//--------------------------------------------------------------------
void smdlGeoAnimKeys::SetTranslate(anKeyData<maVector3d>* i_Anim)
{
	m_Translate = i_Anim;
}
void smdlGeoAnimKeys::SetRotate(anKeyData<maRotation>* i_Anim)
{
	m_Rotate = i_Anim;
}
void smdlGeoAnimKeys::SetScale(anKeyData<maVector3d>* i_Anim)
{
	m_Scale = i_Anim;
}
void smdlGeoAnimKeys::SetVisible(anKeyDataBase<bool>* i_Anim)
{
	m_Visible = i_Anim;
}
