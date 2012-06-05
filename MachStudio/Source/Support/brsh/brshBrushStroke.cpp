/*****************************************************************************
**	brshBrushStroke.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Support/brsh/brshBrushStroke.hpp"

#include "Core/Gf/gfPaths.hpp"
#include "Graphics/eff/effTexturedData.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
brshBrushStroke::brshBrushStroke()
	: m_pStrokeRenderTarget(NULL),
	  m_pStrokeTexture(NULL),
	  m_pStrokeObject(NULL),
	  m_pStrokeMaterial(NULL),
	  m_pStrokeFrag(NULL)
{
	SetupStrokeTexture();
}

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
brshBrushStroke::~brshBrushStroke()
{
	gpxRenderControl::ConfirmSingleThread();

	if( m_pStrokeTexture != NULL ) 
		matTextureMgr::ReleaseTexture(m_pStrokeTexture);

	if( m_pStrokeRenderTarget != NULL )
		matTextureMgr::ReleaseTexture(m_pStrokeRenderTarget);

	if( m_pStrokeObject != NULL )
		delete m_pStrokeObject;

	m_pStrokeObject = NULL;
	m_pStrokeMaterial = NULL;
	m_pStrokeFrag = NULL;
}

//------------------------------------------------------------------------
// set the default values for the data
//------------------------------------------------------------------------
void brshBrushStroke::InitializeData()
{
	
}

//----------------------------------------------------------------------------
// Create the stroke texture for this brush stroke, define texture file,
// size, etc.
//----------------------------------------------------------------------------
void brshBrushStroke::SetupStrokeTexture()
{
	gpxRenderControl::ConfirmSingleThread();

	const bool lc_morphable = false;
	
	m_pStrokeFrag = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(0.05f, 0.05f, 1, 1, lc_morphable);
	m_pStrokeFrag->SetDoubleSided(true);

	// Material
	m_pStrokeMaterial = new matMaterial("Brushstroke.fx");
	effTexturedData* pData = dynamic_cast<effTexturedData*>(m_pStrokeMaterial->GetEffectData());
	if(pData == NULL)
	{
		DBG_ERROR("Paint brush error");
	}

	const float lc_TRANSPARENCY = 1.0f;
	pData->m_Color = maFloatRGBA( 1.0f, 1.0f, 1.0f, lc_TRANSPARENCY );

	//set the texture of the stroke
	fsLocator default_tex;
	GetDefaultTexture(default_tex);
	SetTexture(default_tex);
	m_pStrokeMaterial->SetHasSpecular( false );

	// Set up fragment in object
	m_pStrokeObject = new api3dObjectSimple(m_pStrokeFrag, m_pStrokeMaterial);
	m_pStrokeObject->SetGPUPickable(false);

	//create initial properties for brushstroke object
	m_pStrokeObject->SetPosition(maPoint3d(0,0,0)); 
	m_pStrokeObject->SetScale(maVector3d(0.2f, 0.2f, 1.0f));
	m_pStrokeObject->SetRenderable( true ); // not visible until we get a texture
}

//------------------------------------------------------------------------
// Assign the default texture map to the brush stroke
//------------------------------------------------------------------------
void brshBrushStroke::GetDefaultTexture(fsLocator& o_TextureFile)
{
	//create a rectangle texture
	fsLocator textureFile = gfPaths::GetPath(gfPaths::e_ExePath);
	textureFile.Push("Data");
	textureFile.Push("Effects");
	textureFile.Push("Textures");
	textureFile.Push("Spot-brush-001.dds");

	o_TextureFile = textureFile;
}

//------------------------------------------------------------------------
// Return the render target texture of the brush stroke.
//------------------------------------------------------------------------
matTexture* brshBrushStroke::GetStrokeRenderTarget()
{
	return m_pStrokeRenderTarget;
}

//------------------------------------------------------------------------
// Set the render target texture of the brush stroke.
//------------------------------------------------------------------------
void brshBrushStroke::SetStrokeRenderTarget(matTexture* i_pRenderTarget)
{
	m_pStrokeRenderTarget = i_pRenderTarget;
}

//----------------------------------------------------------------------------
// Set stroke scale/size
//----------------------------------------------------------------------------
void brshBrushStroke::SetScale(const maVector3d &i_Scale)
{
	m_pStrokeObject->SetScale(i_Scale);
}

//----------------------------------------------------------------------------
// Set the color value of the brushstroke
//----------------------------------------------------------------------------
void brshBrushStroke::SetColor(const maFloatRGBA &i_Color)
{
	gpxRenderControl::ConfirmSingleThread();

	//this will change the color of the textured rectangle
	effTexturedData* pData = dynamic_cast<effTexturedData*>(m_pStrokeMaterial->GetEffectData());
	if(pData)
		pData->m_Color = i_Color;
}

//------------------------------------------------------------------------
// Set the texture file for the brushstroke
//------------------------------------------------------------------------
void brshBrushStroke::SetTexture(const fsLocator& i_Texture)
{
	gpxRenderControl::ConfirmSingleThread();

	fsLocator tex_loc = i_Texture;
	if(i_Texture.GetNumNames() <= 0)
	{
		GetDefaultTexture(tex_loc);
	}

	//now update the texture
	if(m_pStrokeTexture)
	{
		matTextureMgr::ReleaseTexture(m_pStrokeTexture);
		m_pStrokeTexture = NULL;
	}

	//update the texture file associated to the brush stroke
	const bool l_MIPMAP = false;
	m_pStrokeTexture = matTextureMgr::LoadTexture( tex_loc, TEXTURE_TYPE_2D, l_MIPMAP );

	if(m_pStrokeMaterial)
	{
		effTexturedData* pData = dynamic_cast<effTexturedData*>(m_pStrokeMaterial->GetEffectData());
		if(pData)
			pData->m_Texture = m_pStrokeTexture;
	}
}

//------------------------------------------------------------------------
// Return the object associated with this brushstroke
//------------------------------------------------------------------------
api3dObjectSimple* brshBrushStroke::GetStrokeObject() const
{
	return m_pStrokeObject;
}