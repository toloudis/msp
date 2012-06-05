/*****************************************************************************
**	matShaderParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mat/matShaderParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/Fs/fsAbsolutePathMgr.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Graphics/eff/effBlinnDataParser.hpp"
#include "Graphics/eff/effDisplacementDataParser.hpp"
#include "Graphics/eff/effFurDataParser.hpp"
#include "Graphics/eff/effGlowDataParser.hpp"
#include "Graphics/eff/effHairDataParser.hpp"
#include "Graphics/eff/effLambertDataParser.hpp"
#include "Graphics/eff/effNormalsData.hpp"
#include "Graphics/eff/effNormalsDataParser.hpp"
#include "Graphics/eff/effRendermanOverrideData.hpp"
#include "Graphics/eff/effRendermanOverrideDataParser.hpp"
#include "Graphics/eff/effOutlineDataParser.hpp"
#include "Graphics/eff/effPhongDataParser.hpp"
#include "Graphics/eff/effReflDataParser.hpp"
#include "Graphics/eff/effShaderParams.hpp"
#include "Graphics/eff/effSkinDataParser.hpp"
#include "Graphics/eff/effToonDataParser.hpp"
#include "Graphics/eff/effWaterDataParser.hpp"
#include "Graphics/Mat/matTexturePathUtil.hpp"
#include "Graphics/mdl/mdlMatInfo.hpp"
#include "Graphics/mdl/private/mdlMaterialParser.hpp"

#include <map>


//============================================================================
//============================================================================
namespace matShaderParser
{
	namespace
	{
		//--------------------------------------------------------------------
		// Can single filename textures be used, or do they have to be
		// resolved into full paths.
		//--------------------------------------------------------------------
		bool l_bAllowSingleFilenameTextures = false;

		//--------------------------------------------------------------------
		// Confirm that the fullpath exists, or map it to a local directory.
		//--------------------------------------------------------------------
		bool l_bResolveAbsolutePaths = true;

		//--------------------------------------------------------------------
		// SHader DaTa
		//--------------------------------------------------------------------
		const chDefs::Name c_SHDT = chDefs::MakeName('S', 'H', 'D', 'T');

		//--------------------------------------------------------------------
		// SHader VersioN
		//--------------------------------------------------------------------
		const chDefs::Name c_SHVN = chDefs::MakeName('S', 'H', 'V', 'N');
		//--------------------------------------------------------------------
		// Parameter FLoaT
		//--------------------------------------------------------------------
		const chDefs::Name c_PFLT = chDefs::MakeName('P', 'F', 'L', 'T');
		//--------------------------------------------------------------------
		// Parameter CoLoR
		//--------------------------------------------------------------------
		const chDefs::Name c_PCLR = chDefs::MakeName('P', 'C', 'L', 'R');
		//--------------------------------------------------------------------
		// Parameter TEXture
		//--------------------------------------------------------------------
		const chDefs::Name c_PTEX = chDefs::MakeName('P', 'T', 'E', 'X');
		//--------------------------------------------------------------------
		// Parameter Compound TeXture
		//--------------------------------------------------------------------
		const chDefs::Name c_PCTX = chDefs::MakeName('P', 'C', 'T', 'X');
		//--------------------------------------------------------------------
		// Parameter Compound Texture Data
		//--------------------------------------------------------------------
		const chDefs::Name c_PCTD = chDefs::MakeName('P', 'C', 'T', 'D');
		//--------------------------------------------------------------------
		// Parameter BOOlean
		//--------------------------------------------------------------------
		const chDefs::Name c_PBOO = chDefs::MakeName('P', 'B', 'O', 'O');
		//--------------------------------------------------------------------
		// Parameter INTeger
		//--------------------------------------------------------------------
		const chDefs::Name c_PINT = chDefs::MakeName('P', 'I', 'N', 'T');
		
		//--------------------------------------------------------------------
		// Check absolute path, finding local directories that match
		//--------------------------------------------------------------------
		void resolve_fullpath(prtyTextureFileName &io_FilePath,
							  const fsLocator& i_ContainingFile)
		{
			fsLocator tex_loc = io_FilePath.GetValue();
			if (matTexturePathUtil::ResolveFullPath(tex_loc, i_ContainingFile,
						  l_bAllowSingleFilenameTextures, l_bResolveAbsolutePaths))
			{
				//preserve the callback state
				prtyTextureFileData tex;
				tex.m_TextureLocator = tex_loc;
				tex.m_CurrentCallback = io_FilePath.GetFullValue().m_CurrentCallback;
				io_FilePath.SetValue(tex);
			}
		}

		//
		class ShaderParserMap
		{
			public:
				typedef std::map<chDefs::Name, effDataParser*> ParserMap;

				~ShaderParserMap()
				{
					ParserMap::iterator it = m_Parsers.begin();
					ParserMap::iterator end = m_Parsers.end();
					for( ; it!=end; ++it )
					{
						delete it->second;
					}
					m_Parsers.clear();
				}
				void AddParser( chDefs::Name i_Name, effDataParser* i_pParser )
				{
					DBG_ASSERT(m_Parsers.find(i_Name) == m_Parsers.end(), "Overlap in parser codes: " << (const char*)(&i_Name));
					if (m_Parsers.find(i_Name) != m_Parsers.end())
						return;
					m_Parsers[i_Name] = i_pParser;
				}
				const effDataParser* GetParser( chDefs::Name i_Name ) const
				{
					ParserMap::const_iterator it = m_Parsers.find(i_Name);
					if (it == m_Parsers.end())
						return NULL;
					return it->second;
				}
			private:
				ParserMap m_Parsers;
		};

		ShaderParserMap *l_pParserMgr = NULL;
	}


	//--------------------------------------------------------------------
	//	Deinitialize/Initialize
	//--------------------------------------------------------------------
	void Initialize()
	{
		l_pParserMgr = new ShaderParserMap;

		AddShaderParser(effPhongDataParser::GetChunkName(), new effPhongDataParser());
		AddShaderParser(effWaterDataParser::GetChunkName(), new effWaterDataParser());
		AddShaderParser(effFurDataParser::GetChunkName(), new effFurDataParser());
		AddShaderParser(effGlowDataParser::GetChunkName(), new effGlowDataParser());
		AddShaderParser(effNormalsDataParser::GetChunkName(), new effNormalsDataParser());
		AddShaderParser(effReflDataParser::GetChunkName(), new effReflDataParser());
		AddShaderParser(effHairDataParser::GetChunkName(), new effHairDataParser());
		AddShaderParser(effSkinDataParser::GetChunkName(), new effSkinDataParser());
		AddShaderParser(effLambertDataParser::GetChunkName(), new effLambertDataParser());
		AddShaderParser(effBlinnDataParser::GetChunkName(), new effBlinnDataParser());
		AddShaderParser(effToonDataParser::GetChunkName(), new effToonDataParser());
		AddShaderParser(effOutlineDataParser::GetChunkName(), new effOutlineDataParser());
		AddShaderParser(effDisplacementDataParser::GetChunkName(), new effDisplacementDataParser());
		AddShaderParser(effRendermanOverrideDataParser::GetChunkName(), new effRendermanOverrideDataParser());
	}
	void DeInitialize()
	{
		delete l_pParserMgr;
	}

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
	bool GetAllowSingleFilenameTextures()
	{
		return l_bAllowSingleFilenameTextures;
	}
	void SetAllowSingleFilenameTextures(bool i_bVal)
	{
		l_bAllowSingleFilenameTextures = i_bVal;
	}

	//--------------------------------------------------------------------
	// Confirm that the fullpath exists, or map it to a local directory.
	//--------------------------------------------------------------------
	bool GetResolveAbsolutePaths()
	{
		return l_bResolveAbsolutePaths;
	}
	void SetResolveAbsolutePaths(bool i_bVal)
	{
		l_bResolveAbsolutePaths = i_bVal;
	}

	//--------------------------------------------------------------------
	// Adds a method for creating parsers for a given shader type.
	//--------------------------------------------------------------------
	void AddShaderParser(chDefs::Name i_Name, 
						 effDataParser* i_pParser)
	{
		l_pParserMgr->AddParser(i_Name, i_pParser);
	}

	//--------------------------------------------------------------------
	// Reads shader info from given chunk name.  This may return NULL,
	// if no parser can handle the chunk.
	//--------------------------------------------------------------------
	effShaderParams* ReadShaderData(chReader& i_Reader,
							chDefs::Name i_Name,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							mdlMaterialInfo& o_MaterialInfo)
	{
		//DBG_LOG("Attempting read shader");
		const effDataParser* pParser = l_pParserMgr->GetParser( i_Name );
		if (!pParser) 
			return NULL;

		effShaderParams* pShader = new effShaderParams;
		//DBG_LOG("Reading shader");
		pParser->Read(i_Reader, i_Version, i_Size, o_MaterialInfo, *pShader);
		return pShader;
	}

	void ReadEffectParamFloat(chReader& i_Reader,
							chDefs::Name i_Name,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							effShaderParams* o_Params,
							mdlMaterialInfo& o_MaterialInfo)
	{
		std::string paramName;
		std::string pname, ptype;

		i_Reader.Read(paramName);
		i_Reader.Read(pname);
		prtyFloat prop(pname);
		prop.Read(i_Reader);

		if ( paramName == "g_bumpMapScale" )
		{
			effNormalsData& data = dynamic_cast<effNormalsData &>(o_MaterialInfo.NormalsParams());			
			float bumpScale = prop.GetValue();
			data.m_BumpScale = bumpScale;
		} 
		else
		{
			effParamFloat* pParam = new effParamFloat(paramName, pname, prop.GetValue());
			o_Params->AddParam(pParam);
		}
	}

	void ReadEffectParamColor(chReader& i_Reader,
							chDefs::Name i_Name,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							effShaderParams* o_Params)
	{
		std::string paramName;
		std::string pname, ptype;

		i_Reader.Read(paramName);
		i_Reader.Read(pname);
		prtyColor prop(pname);
		prop.Read(i_Reader);

		effParamColor* pParam = new effParamColor(paramName, pname, prop.GetValue());
		o_Params->AddParam(pParam);
	}

	//------------------------------------------------------------------------
	// Read the param texture the old way with a simple texture filename
	//------------------------------------------------------------------------
	void ReadEffectParamTexture(chReader& i_Reader,
							chDefs::Name i_Name,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							effShaderParams* o_Params,
							mdlMaterialInfo& o_MaterialInfo)
	{
		std::string paramName;
		std::string pname, ptype;

		i_Reader.Read(paramName);
		i_Reader.Read(pname);

		prtyTextureFileName prop(pname);
		prop.ReadTexture(i_Reader);
		resolve_fullpath(prop, i_Reader.GetLocator());

		if ( paramName == "normalMap" || paramName == "normalTex" )
		{
			effNormalsData& data = dynamic_cast<effNormalsData &>(o_MaterialInfo.NormalsParams());			
			fsLocator locNormalMap = prop.GetValue();
			data.m_NameNormalMap = locNormalMap;
		} 
		else
		{
			effParamTexture* pParam;
			if( o_Params->FindTextureParam(paramName) == NULL )
			{
				pParam = new effParamTexture(paramName, pname, prop.GetValue());
				pParam->Property() = prop;  // preserve callback string
				o_Params->AddParam(pParam);
			}
			else
			{
				pParam = o_Params->FindTextureParam(paramName);
				if( pParam->Property().GetValue().GetNumNames() > 0 )
				{
					prtyTextureFileData loc = prop.GetFullValue();
					loc.m_TextureLocator = pParam->Property().GetValue();
					prop.SetValueWithoutNotify(loc);
				}
				pParam->Property() = prop;  // preserve callback string
			}
		}
	}
	//------------------------------------------------------------------------
	// Read the param properties for a texture using the new chunk
	//------------------------------------------------------------------------
	void ReadTextureData(chReader& i_Reader,
						std::string& o_ParamName,
						std::string& o_PropertyName)
	{
		i_Reader.Read(o_ParamName);
		i_Reader.Read(o_PropertyName);
	}
	void ReadTextureProperties(chReader& i_Reader,
								chDefs::Name i_Name,
								chDefs::Version i_Version,
								chDefs::Size i_Size,							
								std::string& o_ParamName,
								std::string& o_PropertyName)
	{
		chDefs::Name name;
		chDefs::Version version;
		chDefs::Size size;

		while( i_Reader.ReadChunkHeader(name, version, size) )
		{
			if (name == c_PCTD)
			{
				ReadTextureData(i_Reader, o_ParamName, o_PropertyName);
				i_Reader.FinishChunk();
				break;  //we need to break out so we can read the texture field property
			}
		}
	}

	//------------------------------------------------------------------------
	// Read the new texture param and the child chunks
	//------------------------------------------------------------------------
	void ReadEffectParamCompoundTexture(chReader& i_Reader,
							chDefs::Name i_Name,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							effShaderParams* o_Params,
							mdlMaterialInfo& o_MaterialInfo)
	{
		std::string paramName;
		std::string pname, ptype;

		//first read the Texture data
		ReadTextureProperties(i_Reader, i_Name, i_Version, i_Size, paramName, pname);

		//now read the acture compound texture property
		prtyTextureFileName prop(pname);
		prop.Read(i_Reader);
		resolve_fullpath(prop, i_Reader.GetLocator());

		if ( paramName == "normalMap" || paramName == "normalTex" )
		{
			effNormalsData& data = dynamic_cast<effNormalsData &>(o_MaterialInfo.NormalsParams());			
			fsLocator locNormalMap = prop.GetValue();
			data.m_NameNormalMap = locNormalMap;
		} 
		else
		{
			effParamTexture* pParam;
			if( o_Params->FindTextureParam(paramName) == NULL )
			{
				pParam = new effParamTexture(paramName, pname, prop.GetValue());
				pParam->Property() = prop;  // preserve callback string
				o_Params->AddParam(pParam);
			}
			else
			{
				pParam = o_Params->FindTextureParam(paramName);
				if( pParam->Property().GetValue().GetNumNames() > 0 )
				{
					prtyTextureFileData loc = prop.GetFullValue();
					loc.m_TextureLocator = pParam->Property().GetValue();
					prop.SetValueWithoutNotify(loc);
				}
				pParam->Property() = prop;  // preserve callback string
			}

			//If the property read in any ramp data, set it to the param
			if( prop.GetFullValue().m_RampObject.get() != NULL )
			{
				pParam->SetRampProperty(prop.GetFullValue().m_RampObject);
			}
		}
	}

	void ReadEffectParamBool(chReader& i_Reader,
							chDefs::Name i_Name,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							effShaderParams* o_Params)
	{
		std::string paramName;
		std::string pname, ptype;

		i_Reader.Read(paramName);
		i_Reader.Read(pname);
		prtyBoolean prop(pname);
		prop.Read(i_Reader);

		effParamBool* pParam = new effParamBool(paramName, pname, prop.GetValue());
		o_Params->AddParam(pParam);
	}

	void ReadEffectParamInt(chReader& i_Reader,
							chDefs::Name i_Name,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							effShaderParams* o_Params)
	{
		std::string paramName;
		std::string pname, ptype;

		i_Reader.Read(paramName);
		i_Reader.Read(pname);
		prtyInt32 prop(pname);
		prop.Read(i_Reader);

		effParamInt* pParam = new effParamInt(paramName, pname, prop.GetValue());
		o_Params->AddParam(pParam);
	}

	void ReadShaderVersion(chReader& i_Reader,
							chDefs::Name i_Name,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							effShaderParams* o_Params)
	{
		int v;
		
		i_Reader.Read(v);

		o_Params->SetVersion(v);
	}

	effShaderParams* ReadShaderParams(chReader& i_Reader,
							chDefs::Name i_Name,
							chDefs::Version i_Version,
							chDefs::Size i_Size,							
							mdlMaterialInfo& o_MaterialInfo)
	{
		chDefs::Name name;
		chDefs::Version version;
		chDefs::Size size;

		effShaderParams* params = new effShaderParams();
		while( i_Reader.ReadChunkHeader(name, version, size) )
		{
			if (name == c_PFLT)
			{
				ReadEffectParamFloat(i_Reader, name, version, size, params, o_MaterialInfo);
			}
			else if (name == c_PCLR)
			{
				ReadEffectParamColor(i_Reader, name, version, size, params);			
			}
			else if (name == c_PTEX)
			{
				ReadEffectParamTexture(i_Reader, name, version, size, params, o_MaterialInfo);
			}
			else if (name == c_PCTX)
			{
				ReadEffectParamCompoundTexture(i_Reader, name, version, size, params, o_MaterialInfo);
			}
			else if (name == c_PBOO)
			{
				ReadEffectParamBool(i_Reader, name, version, size, params);
			}
			else if (name == c_PINT)
			{
				ReadEffectParamInt(i_Reader, name, version, size, params);
			}
			else if (name == c_SHVN)
			{
				ReadShaderVersion(i_Reader, name, version, size, params);
			}
			i_Reader.FinishChunk();
		}
		return params;
	}

	//--------------------------------------------------------------------
	// Writes Compound Texture property name and param name to the file
	//--------------------------------------------------------------------
	void WriteTextureData( chWriter& o_Writer, effParamTexture* i_pParam )
	{
		// We need to write the param properties in their own chunk
		static const chDefs::Version s_TextureDataChunkVersion = 1;
		o_Writer.WriteChunkHeader( c_PCTD, s_TextureDataChunkVersion, false);

		o_Writer.Write(i_pParam->GetName());
		o_Writer.Write(i_pParam->GetProperty().GetPropertyName());

		o_Writer.FinishChunk();
	}

	//--------------------------------------------------------------------
	// Writes Compound Texture data to file
	//--------------------------------------------------------------------
	void WriteTextureProperty( chWriter& o_Writer, effParamTexture* i_pParam)
	{
		if( i_pParam->GetRampProperty() != NULL )
		{
			prtyTextureFileData val = i_pParam->GetProperty().GetFullValue();
			val.m_RampObject = i_pParam->GetSmartRampProperty();
			i_pParam->Property().SetValueWithoutNotify(val);
		}
		//The property will write its own chunk header
		i_pParam->GetProperty().Write(o_Writer);
	}

	//--------------------------------------------------------------------
	// Writes shader to file, returning false if no parser can handle
	//	the shader's format.
	//--------------------------------------------------------------------
	bool WriteShader( chWriter& o_Writer,
					  const effShaderData& i_Shader )
	{
		//DBG_LOG("Attempting write shader");
		const effDataParser* pParser = l_pParserMgr->GetParser( i_Shader.GetChunkName() );
		if (!pParser)
			return false;

		//DBG_LOG("Writing shader");
		pParser->Write(o_Writer, i_Shader);
		return true;
	}

	//--------------------------------------------------------------------
	// Writes shader to file, returning false if no parser can handle
	//	the shader's format.
	//--------------------------------------------------------------------
	bool WriteShader( chWriter& o_Writer, shared_ptr<effShaderParams> i_Shader )
	{
		// chunk of shader data
		static const chDefs::Version s_Version = 1;
		o_Writer.WriteChunkHeader( c_SHDT, s_Version, true );

		// version 1 of c_SHDT added the version chunk 
		// now on read, we can test against this version number to remap shader params by name.
		static const chDefs::Version s_ShaderParamsVersionChunkVersion = 0;
		o_Writer.WriteChunkHeader( c_SHVN, s_ShaderParamsVersionChunkVersion, false);
		o_Writer.Write(i_Shader->GetVersion());
		o_Writer.FinishChunk();
		
		std::vector<effParamFloat*> vFloats;
		i_Shader->GetAllFloatParams(vFloats);
		int n = vFloats.size();
		for (int i = 0; i < n; i++)
		{
			// one chunk per param
			static const chDefs::Version s_EffParamFloatChunkVersion = 0;
			o_Writer.WriteChunkHeader( c_PFLT, s_EffParamFloatChunkVersion, false);

			effParamFloat* param = vFloats[i];
			o_Writer.Write(param->GetName());
			o_Writer.Write(param->GetProperty().GetPropertyName());
			param->GetProperty().Write(o_Writer);

			o_Writer.FinishChunk();
		}
		std::vector<effParamColor*> vColors;
		i_Shader->GetAllColorParams(vColors);
		n = vColors.size();
		for (int i = 0; i < n; i++)
		{
			// one chunk per param
			static const chDefs::Version s_EffParamColorChunkVersion = 0;
			o_Writer.WriteChunkHeader( c_PCLR, s_EffParamColorChunkVersion, false);

			effParamColor* param = vColors[i];
			o_Writer.Write(param->GetName());
			o_Writer.Write(param->GetProperty().GetPropertyName());
			param->GetProperty().Write(o_Writer);

			o_Writer.FinishChunk();
		}
		std::vector<effParamTexture*> vTextures;
		i_Shader->GetAllTextureParams(vTextures);
		n = vTextures.size();
		for (int i = 0; i < n; i++)
		{
			effParamTexture* param = vTextures[i];

			// one chunk per param
			// Version 1 promotes texture filename to fullpath
			//*** For backwards compatability we need to always write out the 
			// old chunk data of PTEX which contained only the data for a filename
			// and the property/parameter names
			static const chDefs::Version s_EffParamTextureChunkVersion = 1;
			o_Writer.WriteChunkHeader( c_PTEX, s_EffParamTextureChunkVersion, false);

			o_Writer.Write(param->GetName());
			o_Writer.Write(param->GetProperty().GetPropertyName());
			param->GetProperty().WriteTexture(o_Writer);

			o_Writer.FinishChunk();

			//*** New texture property prtyTextureFileName will write a new chunk
			//that will encapsulate the parameter data into child chunks
			//To do this we need to create a new chunk header for any class that
			//wants to write/read this property
			static const chDefs::Version s_EffParamCompoundTextureChunkVersion = 1;
			o_Writer.WriteChunkHeader( c_PCTX, s_EffParamCompoundTextureChunkVersion, false);
			
			WriteTextureData(o_Writer, param);
			WriteTextureProperty(o_Writer, param);

			o_Writer.FinishChunk();
		}

		std::vector<effParamBool*> vBools;
		i_Shader->GetAllBoolParams(vBools);
		n = vBools.size();
		for (int i = 0; i < n; i++)
		{
			// one chunk per param
			static const chDefs::Version s_EffParamBoolChunkVersion = 0;
			o_Writer.WriteChunkHeader( c_PBOO, s_EffParamBoolChunkVersion, false);

			effParamBool* param = vBools[i];
			o_Writer.Write(param->GetName());
			o_Writer.Write(param->GetProperty().GetPropertyName());
			param->GetProperty().Write(o_Writer);

			o_Writer.FinishChunk();
		}

		std::vector<effParamInt*> vInts;
		i_Shader->GetAllIntParams(vInts);
		n = vInts.size();
		for (int i = 0; i < n; i++)
		{
			// one chunk per param
			static const chDefs::Version s_EffParamIntChunkVersion = 0;
			o_Writer.WriteChunkHeader( c_PINT, s_EffParamIntChunkVersion, false);

			effParamInt* param = vInts[i];
			o_Writer.Write(param->GetName());
			o_Writer.Write(param->GetProperty().GetPropertyName());
			param->GetProperty().Write(o_Writer);

			o_Writer.FinishChunk();
		}

		o_Writer.FinishChunk();
		return true;
	}
} // end of namespace

