//****************************************************************************
//	ovrlyObject
//
//	An overlay object
//
//	StudioGPU
//	Copyright(C) 2010 - All Rights Reserved
//****************************************************************************
#ifdef OVRLY_OBJECT_HPP
#error ovrlyObject.hpp multiply included
#endif
#define OVRLY_OBJECT_HPP

#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif
#ifndef API3D_OBJECTSIMPLE_HPP
#include "Tool/api3d/api3dObjectSimple.hpp"
#endif

#include <vector>


//============================================================================
//	forward references
//============================================================================
class gpxSceneObject;
class matTexture;
class itString;


//============================================================================
//============================================================================
class ovrlyObject
{
	public:
		///-----------------------------------------------------------------------
		/// constructors
		///-----------------------------------------------------------------------
		ovrlyObject(itString& i_FileNormal,
					itString& i_FileHighlight,
					itString& i_FileDisable);

		///-----------------------------------------------------------------------
		/// destructors
		///-----------------------------------------------------------------------
		virtual ~ovrlyObject();

		//--------------------------------------------------------------------
		///	Get the width of the object
		//--------------------------------------------------------------------
		float GetWidth()
		{
			return m_Width;
		}
		//--------------------------------------------------------------------
		///	Get the height of the object
		//--------------------------------------------------------------------
		float GetHeight()
		{
			return m_Height;
		}

		//--------------------------------------------------------------------
		///	Set/Get the position
		//--------------------------------------------------------------------
		void SetPosition( maPoint2d& i_Position );
		maPoint2d GetPosition();

		//--------------------------------------------------------------------
		///	Set the color
		//--------------------------------------------------------------------
		void SetColor( maFloatRGBA i_Color );

		//--------------------------------------------------------------------
		//	Show() - Render the shape or not
		//--------------------------------------------------------------------
		virtual void Show(bool i_bVisible = true);

		//--------------------------------------------------------------------
		///	return true if the screen point is within the bounding area of
		///	this object.  If the object is not pickable this will also return
		///	false.
		//--------------------------------------------------------------------
		virtual bool CheckForPick( maPoint2d& i_ScreenPoint );

		//--------------------------------------------------------------------
		///	Inputs are the mouse movement
		//--------------------------------------------------------------------
		virtual void DoMovement( int i_X, int i_Y, int i_Z );

		//--------------------------------------------------------------------
		///	This overlay object normal state
		//--------------------------------------------------------------------
		virtual void State_Normal();

		//--------------------------------------------------------------------
		///	This overlay object has been picked, do what it needs to do.
		//--------------------------------------------------------------------
		virtual void State_Picked();

		//--------------------------------------------------------------------
		///	This overlay object has been rolled over, do what it needs to do.
		//--------------------------------------------------------------------
		virtual void State_Highlight();

		//--------------------------------------------------------------------
		///	This overlay object has been disabled
		//--------------------------------------------------------------------
		virtual void State_Disabled();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		bool IsVisible();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		bool IsPickable();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Resize();

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		struct ovrlyState
		{
			api3dObjectSimple*	m_pObject;
			gpxSceneObject*		m_pObjectProxy;
			matTexture*			m_pTexture;
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void create_object(itString& i_FileName, ovrlyState* o_pState);

		//--------------------------------------------------------------------
		///	Calculate the size of the object based on the view size and
		///	horizontal and vertical percentages.
		//--------------------------------------------------------------------
		void calculate_object_size(float& o_TargetX, float& o_TargetY);

		//--------------------------------------------------------------------
		///	Calculate the dimensions of the object assuming the center of the 
		///	image is (0,0)
		//--------------------------------------------------------------------
		void calculate_screen_dimensions(int i_TextureWidth, int i_TextureHeight, maPoint2d& o_Target);

		//--------------------------------------------------------------------
		///	Turn off all states, and keep just one
		//--------------------------------------------------------------------
		void show_state(int i_VisibleState);

	protected:
		bool m_bIsPickable;
		int m_CurrentState;
		float m_Width;
		float m_Height;
		maPoint2d m_Position;

		std::vector<ovrlyState> m_States;
};

