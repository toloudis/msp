/****************************************************************************\
**	icnIconScale.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Tool/icn/icnIconScale.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/Ma/maPoint4d.hpp"
#include "Graphics/Cam/camCamera.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace icnIconScale
{
	//============================================================================
	//============================================================================
	namespace
	{
		std::vector<icnIconScaleInterest*>	l_ScaleInterestList;

		IconScaleLevel	l_IconScaleLevel = e_Medium;

		//float l_GlobalScale = 1.0f;

		// global scale for all icons (should really base this off of average screen size)
		//const float l_IconsScaleModifier    = 0.0025f;	
	}

	//--------------------------------------------------------------------
	//	GlobalScale - a scaling factor for the scene, used to alter
	//		the size of icons and other "magic" numbers.
	//--------------------------------------------------------------------
	//void SetGlobalScale( float i_Scale )
	//{
	//	// Can't have a negative scale
	//	if (i_Scale > 0.0)
	//	{
	//		l_GlobalScale = i_Scale;
	//
	//		//	notify scale interests
	//		std::vector<icnIconScaleInterest*>::iterator it, end = l_ScaleInterestList.end();
	//		for (it  = l_ScaleInterestList.begin(); it != end ; ++it)
	//		{
	//			(*it)->GlobalScaleChanged( l_GlobalScale );
	//		}
	//	}
	//}
	//float GetGlobalScale()
	//{
	//	return l_GlobalScale;
	//}

	//--------------------------------------------------------------------
	// Set scale of icons based on an enumeration
	//--------------------------------------------------------------------
	void SetIconScaleLevel(IconScaleLevel i_IconScaleLevel)
	{
		l_IconScaleLevel = i_IconScaleLevel;
	}
	IconScaleLevel GetIconScaleLevel()
	{
		return l_IconScaleLevel;
	}

	//--------------------------------------------------------------------
	// Update scale of camera-relative icons to the given camera
	//--------------------------------------------------------------------
	void UpdateIconsScale( int i_IconLayerIndex, const camCamera& i_Camera, int i_ScreenWidth )
	{
		// Compute a modifier based on the current icon scale level.
		// You can think of these modifiers as groups of ten pixels.
		// So, 2.0 is a 20 pixel square for icon scale and 
		// 0.4 is just 4 pixels square. Keep in mind that each icon
		// varies from this square by its own internal scaling. So,
		// camera icons will get larger, but point lights are only about 
		// 70% of this square's size.
		float icon_scale_mod = 1.0f;
		switch (l_IconScaleLevel)
		{
		default:
		case e_Medium:
			icon_scale_mod = 1.0f;
			break;
		case e_Small:
			icon_scale_mod = 0.4f;
			break;
		case e_Large:
			icon_scale_mod = 2.0f;
			break;
		}

		// Compute a modifier based on the screen width versus a
		// standard 1000 pixel width.
		float screen_size_mod = 1.0f;

		//bga - If you comment out the following line, then the
		// icons will stay relative to camera space, meaning that the
		// icons will be smaller in smaller windows and larger in larger windows.
		if (i_ScreenWidth > 0) screen_size_mod = (1000.0f/(float)i_ScreenWidth);

		//	notify scale interests
		std::vector<icnIconScaleInterest*>::iterator it, end = l_ScaleInterestList.end();
		for (it  = l_ScaleInterestList.begin(); it != end ; ++it)
		{
			(*it)->UpdateIconScale( i_IconLayerIndex, i_Camera, icon_scale_mod * screen_size_mod );
		}
	}

	//--------------------------------------------------------------------
	// GetIconScaleForPosition - Computes scale for an icon at the
	// given position based on the given camera view. This scale should
	// keep the icon the same size in all camera views.
	//--------------------------------------------------------------------
	float GetIconScaleForPosition( const maPoint3d& i_Position,
											   const camCamera& i_Camera )
	{
		float fScale = 1.0f;

		// Really should be inverting camera matrix here to 
		// consider field of view in perspective cameras also.
		// There could also be a consideration of the screen size,
		// if that information was passed in.
		//if( i_Camera.IsOrthographic() )
		//{
		//	fScale = i_Camera.GetOrthoWidth() * l_IconsScaleModifier;
		//}
		//else
		//{
		//	float dist = (i_Camera.GetPosition() - i_Position).Length();
		//	if( dist > 0 ) fScale = dist * l_IconsScaleModifier;
		//}

		// Use the camera and projection matrix to get a scale value
		// that keeps the icon a fixed size in screen space.
		maMatrix4x4 cameraMat, projectionMat;
		i_Camera.GetProjectionMatrix(projectionMat);
		i_Camera.GetCameraMatrix(cameraMat);

		// Transform the icon center point to screen space
		maMatrix4x4 camprojmat = cameraMat * projectionMat;
		maPoint4d xformed_pos(i_Position.GetX(), i_Position.GetY(), i_Position.GetZ(), 1.0f);
		camprojmat.Transform(xformed_pos);
		
		float mtx_det = camprojmat.GetDeterminant();
		if (mtx_det != 0)
		{
			// Invert the matrix, move slightly to the side in screen space and then send the
			// position back into world space
			camprojmat.Invert();
			//const float c_NormScreenSize = 10 / 1000.0f; // 10 pixel diameter on screen width of 1000
			maPoint4d screen_offset = xformed_pos;
			//screen_offset.m_X += (c_NormScreenSize * xformed_pos.m_W);
			//camprojmat.Transform(screen_offset);

			//// The distance between the two points in world space is the
			//// scale. 
			//maVector3d diff(i_Position.GetX() - (screen_offset.GetX() / screen_offset.GetW()), 
			//				i_Position.GetY() - (screen_offset.GetY() / screen_offset.GetW()), 
			//				i_Position.GetZ() - (screen_offset.GetZ() / screen_offset.GetW()));
			//fScale = diff.Length();

			// Try to run just the screen space delta through the inverse matrix
			// to avoid round off errors. The delta in screen space is just the X value
			// so we can just pick that part off the top of the inverse matrix.
			// Use doubles because of round off error potential.
			double a = camprojmat.m_Mat[0];
			double b = camprojmat.m_Mat[1];
			double c = camprojmat.m_Mat[2];
			double d = camprojmat.m_Mat[3];
			double len = ::sqrt(a*a + b*b + c*c + d*d);
			const double c_NormScreenSize = 10 / 1000.0;
			double delta  = len * c_NormScreenSize * (double)xformed_pos.m_W;
			fScale = (float) delta;
		}

		return fScale;
	}


	//--------------------------------------------------------------------
	// Apply global scale to value, returning new scaled value
	//--------------------------------------------------------------------
	//float Scale(float i_Value)
	//{
	//	return l_GlobalScale * i_Value;
	//}

	//--------------------------------------------------------------------
	//	RegisterScaleInterest() - add a Scale interest to the system
	//--------------------------------------------------------------------
	void RegisterScaleInterest( icnIconScaleInterest* i_pInterest )
	{
		DBG_ASSERT( i_pInterest != 0, "Cannot register a NULL Scale Interest" );

		l_ScaleInterestList.push_back( i_pInterest );
	}

	//--------------------------------------------------------------------
	//	UnRegisterScaleInterest() - remove a Scale interest from the system.
	//
	//	Note: this will NOT delete the Scale interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterScaleInterest( icnIconScaleInterest* i_pInterest )
	{
		envSTLHelpers::RemoveOneValue( l_ScaleInterestList, i_pInterest );
	}

}	// end of namespace
