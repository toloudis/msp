/*****************************************************************************
**	prjltRangeIcon.hpp
**
**	3D Object that display range of falloff
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PRJLT_RANGEICON_HPP
#error prjltRangeIcon.hpp multiply included
#endif
#define PRJLT_RANGEICON_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class api3dObject;
class g3dFragment;
class gpxSceneObject;


//============================================================================
//============================================================================
class prjltRangeIcon 
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	prjltRangeIcon();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~prjltRangeIcon();

	//--------------------------------------------------------------------
	// Update vertices of morphable fragment match view
	//--------------------------------------------------------------------
	void  Update(const maPoint3d &i_Pos, 
				 const maVector3d &i_Dir, 
				 float i_Range,
				 float i_Percent);

	//----------------------------------------------------------------------------
	//	Renderable sets whether the object is visible
	//----------------------------------------------------------------------------
	void SetRenderable(bool i_Renderable);
	bool GetRenderable() const;

private:
	g3dFragment* m_pFragment;
	api3dObject *m_pObject;	// actual object	
	gpxSceneObject *m_pObjectProxy;	// thread-safe proxy to object	
};
