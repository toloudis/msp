/*****************************************************************************
**	scrImage.hpp
**
**		scrImage displays a single image on screen
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SCR_IMAGE_HPP
#error scrImage.hpp multiply included
#endif
#define SCR_IMAGE_HPP

#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef SCR_PRIMITIVE_HPP
#include "Graphics/scr/scrPrimitive.hpp"
#endif


//============================================================================
//============================================================================
class effPhongData;
class fsLocator;
class g2dWindow;
class g3dFragment;
class g3dSceneNode;
class maFloatRGBA;
class matTexture;


//============================================================================
//============================================================================
class scrImage : public scrPrimitive
{
	public:
		//--------------------------------------------------------------------
		// Construction
		//--------------------------------------------------------------------
		scrImage();

		//--------------------------------------------------------------------
		// Destruction
		//--------------------------------------------------------------------
		virtual ~scrImage();

		//--------------------------------------------------------------------
		//	GetSceneNode
		//--------------------------------------------------------------------
		g3dSceneNode *GetSceneNode();

		//--------------------------------------------------------------------
		//	SetRenderable sets whether this is currently rendered
		//--------------------------------------------------------------------
		virtual void SetRenderable(bool i_bRenderable);

		//--------------------------------------------------------------------
		//	SetEmissive sets the current emissive color
		//--------------------------------------------------------------------
		virtual void SetEmissive(const maFloatRGBA& i_Color);

		//--------------------------------------------------------------------
		//	SetDiffuse sets the current diffuse (including alpha)
		//--------------------------------------------------------------------
		virtual void SetDiffuse(const maFloatRGBA& i_Color);

		//--------------------------------------------------------------------
		//	SetAlpha sets the current alpha value
		//--------------------------------------------------------------------
		virtual void SetAlpha(float i_Alpha);

		//--------------------------------------------------------------------
		//	SetPosition sets the current screen coordinates
		//--------------------------------------------------------------------
		virtual void SetPosition(const maPoint3d& i_Position);

		//--------------------------------------------------------------------
		//	SetSize sets the current height and width
		//--------------------------------------------------------------------
		virtual void SetSize(int i_Width, int i_Height);

		//--------------------------------------------------------------------
		//	SetImage passes an fsLocator to the Image to use
		//--------------------------------------------------------------------
		void SetImage(const fsLocator& i_Locator);

		//--------------------------------------------------------------------
		//	SetWindow - set the window that this object will display to.
		//--------------------------------------------------------------------
		void SetWindow( g2dWindow* i_pWindow );

	protected:
		//--------------------------------------------------------------------
		//	free_texture
		//--------------------------------------------------------------------
		virtual void free_texture();

		//--------------------------------------------------------------------
		//	free_geometry
		//--------------------------------------------------------------------
		virtual void free_geometry();

		//--------------------------------------------------------------------
		//	free_model()
		//--------------------------------------------------------------------
		virtual void free_model();

	private:
		//--------------------------------------------------------------------
		//	update_transform
		//--------------------------------------------------------------------
		void update_transform();

	protected:
		matMaterial* m_pMaterial;

		matTexture*		m_pTexture;
		g3dFragment*	m_pFragment;
		g3dSceneNode*	m_pNode;
		g2dWindow*		m_pWindow;

		bool			m_bInScreenSpace;
};
