/*****************************************************************************
**  brshBrushStroke.hpp
**
**       Class to handle the brushstokes for each paint operation
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef BRSH_BRUSHSTROKE_HPP
#error brshBrushStroke.hpp multiply included
#endif

#ifndef BRSH_DATA_HPP
#include "Support/brsh/data/brshData.hpp"
#endif

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#ifndef MA_VECTOR3D_HPP
#include "Core/ma/maVector3d.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

//----------------------------------------------------------------------------
// Class Forwards
//----------------------------------------------------------------------------
class matMaterial;
class matTexture;
class g3dFragment;
class api3dObjectSimple;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class brshBrushStroke
{
public:
	//------------------------------------------------------------------------
	// Constructor
	//------------------------------------------------------------------------
	brshBrushStroke();

	//------------------------------------------------------------------------
	// Destructor
	//------------------------------------------------------------------------
	~brshBrushStroke();

	//------------------------------------------------------------------------
	// set the default values for the data
	//------------------------------------------------------------------------
	void InitializeData();

	//------------------------------------------------------------------------
	// Create the stroke texture for this brush stroke, define texture file,
	// size, etc.
	//------------------------------------------------------------------------
	void SetupStrokeTexture();

	//------------------------------------------------------------------------
	// Assign the default texture map to the brush stroke
	//------------------------------------------------------------------------
	void GetDefaultTexture(fsLocator& o_TextureFile);

	//------------------------------------------------------------------------
	// Return the render target texture of the brush stroke.
	//------------------------------------------------------------------------
	matTexture* GetStrokeRenderTarget();

	//------------------------------------------------------------------------
	// Set the render target texture of the brush stroke.
	//------------------------------------------------------------------------
	void SetStrokeRenderTarget(matTexture* i_pRenderTarget);

	//------------------------------------------------------------------------
	// Set stroke scale/size
	//------------------------------------------------------------------------
	void SetScale(const maVector3d &i_Scale);

	//------------------------------------------------------------------------
	// Set the color value of the brushstroke
	//------------------------------------------------------------------------
	void SetColor(const maFloatRGBA &i_Color);

	//------------------------------------------------------------------------
	// Set the texture file for the brushstroke
	//------------------------------------------------------------------------
	void SetTexture(const fsLocator& i_Texture);

	//------------------------------------------------------------------------
	// Return the object associated with this brushstroke
	//------------------------------------------------------------------------
	api3dObjectSimple* GetStrokeObject() const;
	

private:
	fsLocator			m_TextureLocation;
	matMaterial*		m_pStrokeMaterial;
	matTexture*			m_pStrokeRenderTarget;
	matTexture*			m_pStrokeTexture;
	g3dFragment*		m_pStrokeFrag;
	api3dObjectSimple*	m_pStrokeObject;
	brshData			m_Data;
};