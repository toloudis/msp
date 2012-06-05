/*****************************************************************************
**	matShaderParser.hpp
**
**		matShaderParser handles the creation of different types shader 
**		data objects.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_SHADERPARSER_HPP
#error matShaderParser.hpp multiply included
#endif
#define MAT_SHADERPARSER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/env/envBoost.hpp"
#endif

#include <vector>

//============================================================================
//	forward references
//============================================================================
class chReader;
class chWriter;
class effShaderData;
class effShaderParams;
class effUVTransform;
class maFloatRGBA;
class maVector3d;
class maVector4d;
class mdlMaterialInfo;
class prtyProperty;

//============================================================================
//============================================================================
class effDataParser
{
public:
	//--------------------------------------------------------------------
	// Destructor
	//--------------------------------------------------------------------
	virtual ~effDataParser() {}

	//--------------------------------------------------------------------
	//	Read is called by a parser utility when the chunk name has been read
	//  from the data file.  It will parse from the chunk into the given
	//  parsable object.
	//--------------------------------------------------------------------
	virtual void Read(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						mdlMaterialInfo& o_MaterialInfo,
						effShaderParams& o_Shader) const = 0;

	//--------------------------------------------------------------------
	//	Write is called to parse data from the given object to the given
	//	chWriter.  i_Shader is the object being written.
	//--------------------------------------------------------------------
	virtual void Write( chWriter& i_Writer,
						const effShaderData& i_Shader ) const = 0;

	//--------------------------------------------------------------------
	// Create should be overridden to create the user specific parsable object.
	// Users of parsers should call create to create the object that is then
	// passed to Read.
	//--------------------------------------------------------------------
	//virtual effShaderData* Create(tmlnScriptObject& io_Object) const = 0;
	virtual effShaderData* Create() const=0;
};

//============================================================================
//============================================================================
namespace matShaderParser
{
	//--------------------------------------------------------------------
	//	Deinitialize/Initialize
	//--------------------------------------------------------------------
	void Initialize();
	void DeInitialize();

	//--------------------------------------------------------------------
	// In older code, when there were project settings, single
	// filenames were used for all textures and the textures
	// were searched for using directory structure rules.
	// Newer code has to use the locate dialog to resolve the
	// single filenames and promote them to full paths.
	// This flag is used to switch between the two versions.
	// Default is false, it should be set to true when
	// an older file is loaded with project settings.
	//--------------------------------------------------------------------
	bool GetAllowSingleFilenameTextures();
	void SetAllowSingleFilenameTextures(bool i_bVal);

	//--------------------------------------------------------------------
	// Confirm that the fullpath exists, or map it to a local directory.
	//--------------------------------------------------------------------
	bool GetResolveAbsolutePaths();
	void SetResolveAbsolutePaths(bool i_bVal);

	//--------------------------------------------------------------------
	// Adds a method for creating parsers for a given shader type.
	//--------------------------------------------------------------------
	void AddShaderParser(chDefs::Name i_Name, 
						 effDataParser* i_pParser);	

	//--------------------------------------------------------------------
	//   ReadShaders - read shaders into vector
	//--------------------------------------------------------------------
	void ReadShaders(	chReader& i_Reader,
						 std::vector<effShaderData*> &o_Shaders );

	//--------------------------------------------------------------------
	//   WriteShaders - write shaders from vector into file
	//--------------------------------------------------------------------
	void WriteShaders( chWriter& o_Writer,
					   const std::vector<effShaderData*> &i_Shaders );


	//--------------------------------------------------------------------
	// Reads shader info from given chunk name.  This may return NULL,
	// if no parser can handle the chunk.
	//--------------------------------------------------------------------
	effShaderParams* ReadShaderData(chReader& i_Reader,
							chDefs::Name i_Name,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							mdlMaterialInfo& o_MaterialInfo);

	//--------------------------------------------------------------------
	// Reads abstract name/value pair shader params.
	//--------------------------------------------------------------------
	effShaderParams* ReadShaderParams(chReader& i_Reader,
							chDefs::Name i_Name,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							mdlMaterialInfo& o_MaterialInfo);

	//--------------------------------------------------------------------
	// Writes shader to file, returning false if no parser can handle
	//	the shader's format.
	//--------------------------------------------------------------------
	bool WriteShader( chWriter& o_Writer, const effShaderData& i_Shader );
	bool WriteShader( chWriter& o_Writer, shared_ptr<effShaderParams> i_Shader );
};
