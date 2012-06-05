/*****************************************************************************
**	scrImage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/scr/scrImage.hpp"

#include "Graphics/eff/effTexturedData.hpp"
#include "Graphics/g2d/g2dScreenUtil.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/g3d/g3dPackage.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"


//--------------------------------------------------------------------
// Construction
//--------------------------------------------------------------------
scrImage::scrImage()
:	m_pTexture(NULL),
	m_pFragment(NULL),
	m_pWindow(NULL),
	m_bInScreenSpace( true )
{
	m_pNode = new g3dSceneNode;

	m_pMaterial = new matMaterial("Billboard.fx");

	maFloatRGBA emissive(1.0f, 0, 0, GetAlpha());
	SetEmissive(emissive);
	m_pMaterial->SetHasSpecular(false);
}

//--------------------------------------------------------------------
// Destruction
//--------------------------------------------------------------------
scrImage::~scrImage()
{
	free_geometry();
	free_texture();
	free_model();
	delete m_pMaterial;
}

//--------------------------------------------------------------------
//	GetSceneNode
//--------------------------------------------------------------------
g3dSceneNode *scrImage::GetSceneNode()
{
	return m_pNode;
}

//--------------------------------------------------------------------
//	SetRenderable sets whether this is currently rendered
//--------------------------------------------------------------------
void scrImage::SetRenderable(bool i_bRenderable)
{
	scrPrimitive::SetRenderable( i_bRenderable );

	m_pNode->SetRenderable( i_bRenderable );
}

//--------------------------------------------------------------------
//	SetDiffuse sets the current emissive
//--------------------------------------------------------------------
//virtual 
void scrImage::SetEmissive(const maFloatRGBA& i_Color)
{
	scrPrimitive::SetEmissive(i_Color);
	m_pMaterial->TypedData<effTexturedData>()->m_Color = i_Color;
}

//--------------------------------------------------------------------
//	SetDiffuse sets the current diffuse (including alpha)
//--------------------------------------------------------------------
//virtual 
void scrImage::SetDiffuse(const maFloatRGBA& i_Color)
{
	scrPrimitive::SetAlpha( i_Color.GetAlpha() );

//	m_pMaterial->TypedData<effTexturedData>()->m_Color = i_Color;
//	m_pMaterial->TypedData<effTexturedData>()->m_Transparency = i_Color.GetAlpha();
}

//--------------------------------------------------------------------
//	SetAlpha sets the current alpha value
//--------------------------------------------------------------------
void scrImage::SetAlpha(float i_Alpha)
{
	scrPrimitive::SetAlpha( i_Alpha );

	m_pMaterial->TypedData<effTexturedData>()->m_Color.SetAlpha(i_Alpha);
//	m_pMaterial->TypedData<effTexturedData>()->m_ColorDiffuse = maFloatRGBA(0, 0, 0, i_Alpha);
//	m_pMaterial->TypedData<effTexturedData>()->m_Transparency = i_Alpha;
}

//--------------------------------------------------------------------
//	SetPosition sets the current screen coordinates
//--------------------------------------------------------------------
void scrImage::SetPosition(const maPoint3d& i_Position)
{
	scrPrimitive::SetPosition( i_Position );

	update_transform();
}

//--------------------------------------------------------------------
//	SetSize sets the current height and width
//--------------------------------------------------------------------
void scrImage::SetSize(int i_Width, int i_Height)
{
	scrPrimitive::SetSize(i_Width, i_Height);

	update_transform();
}

//--------------------------------------------------------------------
//	SetImage passes an fsLocator to the Image to use
//--------------------------------------------------------------------
void scrImage::SetImage(const fsLocator& i_Locator)
{
	//delete any old objects
	free_geometry();
	free_texture();

	m_pMaterial->TypedData<effTexturedData>()->m_Color = this->GetEmissive();
//	m_pMaterial->TypedData<effTexturedData>()->m_ColorEmissive = this->GetEmissive();
//	m_pMaterial->TypedData<effTexturedData>()->m_Transparency = this->GetDiffuse().GetAlpha();

	//	texture
	m_pTexture = matTextureMgr::LoadTexture(i_Locator);
	m_pMaterial->TypedData<effTexturedData>()->m_Texture = m_pTexture;

	// create the geometry
	maPoint3d vertices[4];
	vertices[0].Set( 0.0f,  0.0f, 0.0f );
	vertices[1].Set( 1.0f,  0.0f, 0.0f );
	vertices[2].Set( 0.0f, -1.0f, 0.0f );
	vertices[3].Set( 1.0f, -1.0f, 0.0f );

	maPoint3d normals[4];
	normals[0].Set( 0, 0, 1 );
	normals[1].Set( 0, 0, 1 );
	normals[2].Set( 0, 0, 1 );
	normals[3].Set( 0, 0, 1 );

	maPoint2d textureVertices[4];
	textureVertices[0].Set(  0,		0 );
	textureVertices[1].Set(  0.99f,	0 );
	textureVertices[2].Set(  0,		0.99f );
	textureVertices[3].Set(  0.99f,	0.99f );

	unsigned short indices[6];
	indices[0] = 2;
	indices[1] = 1;
	indices[2] = 0;
	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 1;

	// I didnt port it because it dealt directly with the fragments and ModelD3D stuff.  
	// It seems like that should use the generic Graphics API, not the Graphics/ModelD3D API.  
	// The g3dFragmentCreate API might be enough for it, but maybe the interfaces need to be extended.  
	// Or maybe scr should be a base API that gets filled in through GraphicsDX11/ModelD3D.
 
	m_pFragment = g3dFragmentCreate::CreateFragment(vertices,
													normals,
													textureVertices,
													4,
													indices,
													6,
													m_pMaterial );

	//size the model to the size of texture if a size has yet to be set
	int width, height;
	GetSize(width, height);
	if (!width)
	{
		width = m_pTexture->GetWidth();
		height = m_pTexture->GetHeight();
	}

	SetSize(width, height);

	SetPosition( GetPosition() );
}


//--------------------------------------------------------------------
//	SetWindow - set the window that this object will display to.
//--------------------------------------------------------------------
void scrImage::SetWindow( g2dWindow* i_pWindow )
{
	m_pWindow = i_pWindow;
}

//--------------------------------------------------------------------
//	free_texture
//--------------------------------------------------------------------
void scrImage::free_texture()
{
	//	release textures
	if (m_pTexture)
	{
		m_pMaterial->RemoveTextures();
		m_pMaterial->TypedData<effTexturedData>()->m_Texture = NULL;

		matTextureMgr::ReleaseTexture(m_pTexture);
		m_pTexture = NULL;
	}
}

//--------------------------------------------------------------------
//	free_geometry
//--------------------------------------------------------------------
void scrImage::free_geometry()
{
	//	cleanup fragments
	if (m_pFragment)
	{
		delete m_pFragment;
		m_pFragment = NULL;
	}
}

//--------------------------------------------------------------------
//	free_model()
//--------------------------------------------------------------------
void scrImage::free_model()
{
	//	cleanup model
	if (m_pNode)
	{
		if (m_pNode->GetParent())
		{
			m_pNode->GetParent()->RemoveChild(m_pNode);
		}
		delete m_pNode;
		m_pNode = NULL;
	}
}

//--------------------------------------------------------------------
//	update_transform
//--------------------------------------------------------------------
void scrImage::update_transform()
{
	//	make a matrix to apply the model transformation
	//	transformation order is scale, rotate, translate;
	//but first convert this virtual position into a screen position
	int nX, nY, width, height;

	DBG_ASSERT( m_pWindow != 0, "scrImage has no window associated to it" );
	if (m_pWindow == 0)
		return;

	m_pWindow->GetVirtualResolution(nX, nY);
	//DBG_LOG2( "-virt res (%d) height (%d)", nX, nY );

	GetSize(width, height);
	//DBG_LOG2( "-width (%d) height (%d)", width, height );

	maPoint3d pos = GetPosition();

	maMatrix4x4 transform = m_pNode->GetTransform();
	transform.MakeScale((2.0f * width + 1.0f) / nX, (2.0f * height + 1.0f) / nY, 1);
	transform.TranslateBy(pos.m_X, pos.m_Y, pos.m_Z);
	transform = g2dScreenUtil::WindowToScreenMatrix( *m_pWindow, transform );

	if ( !m_bInScreenSpace )
	{
		transform.m_Mat[12] += 1.0f;
		transform.m_Mat[13] -= 1.0f;
	}

	m_pNode->SetTransform( transform );
}
