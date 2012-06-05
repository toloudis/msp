/*****************************************************************************
**	ptltRangeIcon.hpp
**
**	3D Object that display range of falloff
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PTLT_RANGEICON_HPP
#error ptltRangeIcon.hpp multiply included
#endif
#define PTLT_RANGEICON_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class api3dObjectSimple;
class g3dFragment;


//============================================================================
//============================================================================
class ptltRangeIcon 
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	ptltRangeIcon();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~ptltRangeIcon();

	//--------------------------------------------------------------------
	// Update vertices of morphable fragment match view
	//--------------------------------------------------------------------
	void  Update(const maPoint3d &i_Pos, 
				 float i_Range,
				 float i_Percent);

	//----------------------------------------------------------------------------
	//	Renderable sets whether the object is visible
	//----------------------------------------------------------------------------
	void SetRenderable(bool i_Renderable);
	bool GetRenderable() const;

private:
	g3dFragment* m_pFragment;
	api3dObjectSimple *m_pObject;
};
