/*****************************************************************************
**  cmpsWorldAxis.hpp
**
**      cmpsWorldAxis displays X,Y,Z axis in camera space to show 
**	world orientation of camera.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef CMPS_WORLDAXIS_HPP
#error cmpsWorldAxis.hpp multiply included
#endif
#define CMPS_WORLDAXIS_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class api3dObject;
class g3dSceneNode;
class gpxSceneObject;

//============================================================================
//============================================================================
class cmpsWorldAxis 
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cmpsWorldAxis();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~cmpsWorldAxis();

		//--------------------------------------------------------------------
		// Get root scene node in order to add and remove it from layers
		//--------------------------------------------------------------------
		g3dSceneNode* GetSceneNode();

		//--------------------------------------------------------------------
		//	SetPosition  moves the compass to the position passed in
		//--------------------------------------------------------------------
		void SetPosition(const maPoint3d& i_Position);

		//--------------------------------------------------------------------
		//	SetScale changes the scale of the compass.
		//--------------------------------------------------------------------
		virtual void SetScale(const maPoint3d& i_Scale);

		//--------------------------------------------------------------------
		//	SetOrientation changes the orientation of the compass.
		//--------------------------------------------------------------------
		virtual void SetOrientation(const maRotation& i_Orientation);

		//--------------------------------------------------------------------
		//	SetRenderable turns display of the compass on and off.
		//--------------------------------------------------------------------
		virtual void SetRenderable(bool i_bRender);

	protected:
		api3dObject* m_pObject;
		gpxSceneObject* m_pObjectProxy;
		g3dSceneNode *m_pRootNode;
};

