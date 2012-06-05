/****************************************************************************\
**	mdlMaterialLegacyParser.hpp
**
**		mdlMaterialLegacyParser.hpp supplies functions used to import 
**	material information from old file formats. The specific material
**	attricutes written in the olf file format are converted into
**	effPhongData properties.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_MATERIALLEGACYPARSER_HPP
#error mdlMaterialLegacyParser.hpp multiply included
#endif
#define MDL_MATERIALLEGACYPARSER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif

#include <map>


//============================================================================
//	Forward References
//============================================================================
class chReader;
class effFurData;
class effGlowData;
class effNormalsData;
class effRendermanOverrideData;
class effPhongData;
class effShaderParams;
class effWaterData;
struct mdlMatInfo;


//============================================================================
//	Any of these mdlMaterialLegacyParser functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
namespace mdlMaterialLegacyParser
{
	//----------------------------------------------------------------------------
	// Some apps may want to override what ambient value is in the
	// model file and force all ambient colors to full white
	//----------------------------------------------------------------------------
	void SetAlwaysFullAmbient(bool i_bVal);
	bool IsAlwaysFullAmbient();

	//------------------------------------------------------------------------
	// The Glow shader parameters used to be written using a EFCT chunk.
	// This function will read in the ShaderParams in the EFCT chunk
	// and set the data into the glow effect data.
	//------------------------------------------------------------------------
	void ReadGlow_EFCT(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					effGlowData& o_GlowData);

	//------------------------------------------------------------------------
	// The Fur shader parameters used to be written using a EFCT chunk.
	// This function will read in the ShaderParams in the EFCT chunk
	// and set the data into the fur effect data.
	//------------------------------------------------------------------------
	void ReadFur_EFCT(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					effFurData& o_FurData);

	//------------------------------------------------------------------------
	// The shader params chunk EFCT in the general material chunk MATR 
	// was only used by a single shader type. All other shaders would have
	// been written by newer file formats. This function parses that
	// chunk into effWaterData, the only type possible.
	//------------------------------------------------------------------------
	void ReadWater_EFCT( chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					effWaterData& o_WaterData);
	void ReadWaterParams( chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					effShaderParams& o_WaterData);

	//------------------------------------------------------------------------
	// MBAS is the chunk exported from the Maya exporter. It has just basic
	//	phong data parameters and texture layers. Read this old file format
	//	into an effPhongData object.
	//------------------------------------------------------------------------
	void ReadPhong_MBAS( chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					effPhongData& o_PhongData);

	//------------------------------------------------------------------------
	// TXLY is a chunk in the old file format that represented a texture
	//	with a certain blending state. This should be interpreted now
	//	as a texture within a effPhongData.
	//------------------------------------------------------------------------
	void ReadPhong_TXLY( chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					effPhongData& o_PhongData);

	//------------------------------------------------------------------------
	// A legacy phong material can be mapped to either Simple.fx, Phong.fx
	//	or PhongBump.fx depending on the texture layers parsed.
	//------------------------------------------------------------------------
	std::string GetShaderNameForLegacyPhong(const effPhongData& i_PhongData);
}

