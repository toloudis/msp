/*****************************************************************************
**  effFurData.hpp
**
**      effFurData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef EFF_FURDATA_HPP
#error effFurData.hpp multiply included
#endif
#define EFF_FURDATA_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class effFurData : public effShaderData
{
public:
	effFurData();
	virtual ~effFurData();
	effFurData(const effFurData& i_CopyFrom);
	effFurData& operator = (const effFurData& i_CopyFrom);
	bool operator == (const effFurData& i_EffFurData) const;

	virtual effShaderData* Clone() const {return new effFurData(*this);}
	virtual void GetTextures(std::vector<matTexture*>& io_Textures) const;
	virtual chDefs::Name GetChunkName() const;

	std::string m_TextureFolder;

	float m_NumShells;
	float m_LengthScale;
	maVector3d m_SpreadScale;
	float m_ShellFader;
	bool m_bShowFins;
	float m_FinFader;
	bool m_bColorSourcing;
	bool m_bFurThinning;
	bool m_bAnisotropic;

	float m_CurrentLengthPercentage;
	matTexture* m_FurColorTexture;
	matTexture* m_FurAnisoOpacityTexture;
	matTexture* m_FurOffsetThreshTexture;
	matTexture* m_FurAnisoStrandTexture;
	matTexture* m_FurDensityTexture;
	matTexture* m_FurAnisoOpacityTexture_fin;
	matTexture* m_FurOffsetThreshTexture_fin;
};
