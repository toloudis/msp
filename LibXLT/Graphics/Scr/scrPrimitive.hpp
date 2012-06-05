/*****************************************************************************
**	scrPrimitive.hpp
**
**		scrPrimitive serves as a common pure virtual base class for scr objects
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SCR_PRIMITIVE_HPP
#error scrPrimitive.hpp multiply included
#endif
#define SCR_PRIMITIVE_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class g3dSceneNode;
class g2dWindow;
class fsLocator;


//============================================================================
//============================================================================
class scrPrimitive
{
	public:
		//--------------------------------------------------------------------
		// Construction
		//--------------------------------------------------------------------
		scrPrimitive();

		//--------------------------------------------------------------------
		// Destruction
		//--------------------------------------------------------------------
		virtual ~scrPrimitive();

	//
	//	Get/Sets
	//

		//--------------------------------------------------------------------
		//	GetRenderable returns whether this is currently rendered
		//--------------------------------------------------------------------
		bool GetRenderable() const;

		//--------------------------------------------------------------------
		//	SetRenderable sets whether this is currently rendered
		//--------------------------------------------------------------------
		virtual void SetRenderable(bool i_bRenderable);

		//--------------------------------------------------------------------
		//	GetAlpha returns the current alpha value
		//--------------------------------------------------------------------
		float GetAlpha() const;

		//--------------------------------------------------------------------
		//	SetAlpha sets the current alpha value
		//--------------------------------------------------------------------
		virtual void SetAlpha(float i_Alpha);

		//--------------------------------------------------------------------
		//	GetPosition returns the current screen coordinates
		//--------------------------------------------------------------------
		const maPoint3d& GetPosition() const;

		//--------------------------------------------------------------------
		//	SetPosition sets the current screen coordinates
		//--------------------------------------------------------------------
		virtual void SetPosition(const maPoint3d& i_Position);

		//--------------------------------------------------------------------
		//	GetSize returns the current height and width
		//--------------------------------------------------------------------
		void GetSize(int& o_Width, int& o_Height) const;

		//--------------------------------------------------------------------
		//	SetSize sets the current height and width
		//--------------------------------------------------------------------
		virtual void SetSize(int i_Width, int i_Height);

		//--------------------------------------------------------------------
		//	SetEmissive sets the current emissive color value
		//--------------------------------------------------------------------
		virtual void SetEmissive(const maFloatRGBA& i_Color);
		virtual maFloatRGBA& GetEmissive();

		//--------------------------------------------------------------------
		//	SetDiffuse sets the current Diffuse color value
		//--------------------------------------------------------------------
		virtual void SetDiffuse(const maFloatRGBA& i_Color);
		virtual maFloatRGBA& GetDiffuse();

	//
	//
	//

		//--------------------------------------------------------------------
		//	SetImage passes an fsLocator to the Image to use
		//--------------------------------------------------------------------
		virtual void SetImage(const fsLocator& i_Locator);

		//--------------------------------------------------------------------
		//	GetSceneNode
		//--------------------------------------------------------------------
		virtual g3dSceneNode *GetSceneNode() = 0;

		//--------------------------------------------------------------------
		//	SetWindow - set the window that this object will display to.
		//--------------------------------------------------------------------
		virtual void SetWindow( g2dWindow* i_pWindow ) = 0;

	private:
		bool		m_bRenderable;
		float		m_Alpha;
		maPoint3d	m_Position;
		int			m_Width, m_Height;
		maFloatRGBA	m_EmissiveColor;
		maFloatRGBA	m_DiffuseColor;
};
