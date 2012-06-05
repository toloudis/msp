/*****************************************************************************
**  mtrlSurfaceShader.hpp
**
**      mtrlSurfaceShader is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_SURFACESHADER_HPP
#error mtrlSurfaceShader.hpp multiply included
#endif
#define MTRL_SURFACESHADER_HPP

#ifndef MTRL_SHADEROBJECT_HPP
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#endif

#ifndef RMP_DATA_HPP
#include "Support/rmp/rmpData.hpp"
#endif

class effParamTexture;
class effShaderParams;
class matMaterial;
class mdlMaterialInfo;
class gpxShaderParams;

//============================================================================
//============================================================================
class mtrlSurfaceShader : public mtrlShaderObject
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlSurfaceShader(mdlMaterialInfo& i_MatData,
		matMaterial* i_pMaterial,
		shared_ptr<effShaderParams> i_pShaderData,
		const fsLocator& i_TextureDir);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	virtual ~mtrlSurfaceShader();

	//------------------------------------------------------------------------
	// Set a new directory for the material in order to find the textures.
	// Called when the material has been exported to the material library.
	//------------------------------------------------------------------------
	virtual void UpdateTextureDirectory(const fsLocator &i_TextureDir);

	//--------------------------------------------------------------------
	// Get list of used textures in order to support copying them
	// to the material library when exporting.
	//--------------------------------------------------------------------
	virtual void GetTextureList(std::vector<fsLocator>& o_TextureList);

	virtual shared_ptr<gpxShaderParams> GetShaderData();

private:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	shared_ptr<effShaderParams> m_pShaderData;
	shared_ptr<gpxShaderParams> m_pShaderDataProxy;
	mdlMaterialInfo& m_Material;
	matMaterial* m_pActualMaterial;
	effParamTexture* m_pRampParam;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateTexturePrty(prtyProperty *i_pProperty, bool i_bDirty);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void NotifyRampUI();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void RampChangedFromUI(bool i_bDirty);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateRampTexture(bool i_bDirty);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ConfirmRampTexture(int i_texSize);
};
