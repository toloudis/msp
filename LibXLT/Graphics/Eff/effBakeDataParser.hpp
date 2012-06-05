/*****************************************************************************
**	effBakeDataParser.hpp
**
**		effBakeDataParser handles the creation of different types shader 
**		data objects.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_BAKEDATAPARSER_HPP
#error effBakeDataParser.hpp multiply included
#endif
#define EFF_BAKEDATAPARSER_HPP

#ifndef MAT_SHADERPARSER_HPP
#include "Graphics/mat/matShaderParser.hpp"
#endif

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif

#include <vector>


//============================================================================
//	forward references
//============================================================================
class effShaderData;
class chReader;
class chWriter;


//============================================================================
//============================================================================
class effBakeDataParser : public effDataParser
{
public:
	//--------------------------------------------------------------------
	// Destructor
	//--------------------------------------------------------------------
	virtual ~effBakeDataParser() {}

	//--------------------------------------------------------------------
	//	Read is called by a parser utility when the chunk name has been read
	//  from the data file.  It will parse from the chunk into the given
	//  parsable object.
	//--------------------------------------------------------------------
	virtual void Read(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						effShaderData& o_Shader ) const;
	//--------------------------------------------------------------------
	//	Read is called by a parser utility when the chunk name has been read
	//  from the data file.  It will parse from the chunk into the given
	//  parsable object.
	//--------------------------------------------------------------------
	virtual void Read(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						mdlMaterialInfo& o_MaterialInfo,
						effShaderParams& o_Shader) const;

	//--------------------------------------------------------------------
	//	Write is called to parse data from the given object to the given
	//	chWriter.  i_Shader is the object being written.
	//--------------------------------------------------------------------
	virtual void Write( chWriter& i_Writer,
						const effShaderData& i_Shader ) const;

	//--------------------------------------------------------------------
	// Create should be overridden to create the user specific parsable object.
	// Users of parsers should call create to create the object that is then
	// passed to Read.
	//--------------------------------------------------------------------
	virtual effShaderData* Create() const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static chDefs::Name GetChunkName();
};
