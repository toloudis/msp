/****************************************************************************\
**  shdrVS.hpp
**
**
** Area17
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef SHDR_SHADERS_HPP
#error shdrPipeline.hpp multiply included
#endif
#define SHDR_SHADERS_HPP

#ifndef ENV_BOOST_HPP
#include "Core/env/envBoost.hpp"
#endif

class oglContext;
class shdrDefaultVS;
class shdrColorHeadlightPS;
class shdrColorPS;
class shdrConvolution2dCS;
class shdrCS;
class shdrLambertPS;
class shdrMaterialPS;
class shdrPS;
class shdrVolumeVizPS;
class shdrVolumeVizIsoPS;
class shdrVolumeVizMipPS;
class shdrVolumeVizVS;

class shdrShaders
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	shdrShaders(oglContext* i_pDevice);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~shdrShaders();

	boost::shared_ptr<shdrDefaultVS> m_DefaultVS;
	boost::shared_ptr<shdrColorPS> m_ColorPS;
	boost::shared_ptr<shdrColorHeadlightPS> m_ColorHeadlightPS;
	boost::shared_ptr<shdrLambertPS> m_LambertPS;
	boost::shared_ptr<shdrPS> m_pPSTextureToTexture;
	boost::shared_ptr<shdrVolumeVizPS> m_pPSVolumeViz;
	boost::shared_ptr<shdrVolumeVizIsoPS> m_pPSVolumeVizIso;
	boost::shared_ptr<shdrVolumeVizMipPS> m_pPSVolumeVizMip;
	boost::shared_ptr<shdrVolumeVizVS> m_pVSVolumeViz;

	boost::shared_ptr<shdrMaterialPS> m_pMatPhong;

};
