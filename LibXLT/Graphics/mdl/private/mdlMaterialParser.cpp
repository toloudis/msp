/****************************************************************************\
**	mdlMaterialParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/private/mdlMaterialParser.hpp"

#include "Core/app/appSimTime.hpp"
#include "Core/ch/chBinReader.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/Fs/fsAbsolutePathMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Graphics/an/anKeyAnimation.hpp"
#include "Graphics/eff/effDisplacementDataParser.hpp"
#include "Graphics/eff/effFurData.hpp"
#include "Graphics/eff/effFurDataParser.hpp"
#include "Graphics/eff/effGlowDataParser.hpp"
#include "Graphics/eff/effNormalsDataParser.hpp"
#include "Graphics/eff/effRendermanOverrideDataParser.hpp"
#include "Graphics/eff/effOutlineDataParser.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/eff/effWaterData.hpp"
#include "Graphics/mat/matMatAnim.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTexturePathUtil.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/mdl/mdlMatInfo.hpp"
#include "Graphics/mdl/private/mdlMaterialLegacyParser.hpp"


//============================================================================
//	Any of these mdlMaterialParser functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
namespace mdlMaterialParser
{
	namespace
	{
		const chDefs::Name c_MATR = chDefs::MakeName('M', 'A', 'T', 'R');
		const chDefs::Name c_MBAS = chDefs::MakeName('M', 'B', 'A', 'S');
		const chDefs::Name c_MNAM = chDefs::MakeName('M', 'N', 'A', 'M');
		const chDefs::Name c_TXLY = chDefs::MakeName('T', 'X', 'L', 'Y');
		const chDefs::Name c_VSHD = chDefs::MakeName('V', 'S', 'H', 'D');
		const chDefs::Name c_MANI = chDefs::MakeName('M', 'A', 'N', 'I');
		const chDefs::Name c_MBSC = chDefs::MakeName('M', 'B', 'S', 'C');
		const chDefs::Name c_MRFL = chDefs::MakeName('M', 'R', 'F', 'L');
		const chDefs::Name c_MEFF = chDefs::MakeName('M', 'E', 'F', 'F');
		const chDefs::Name c_EFCT = chDefs::MakeName('E', 'F', 'C', 'T');
		const chDefs::Name c_MFUR = chDefs::MakeName('M', 'F', 'U', 'R');
		const chDefs::Name c_MGLO = chDefs::MakeName('M', 'G', 'L', 'O');
		const chDefs::Name c_NMAP = chDefs::MakeName('N', 'M', 'A', 'P');
		const chDefs::Name c_UVSC = chDefs::MakeName('U', 'V', 'S', 'C');
		const chDefs::Name c_SHDR = chDefs::MakeName('S', 'H', 'D', 'R');
		const chDefs::Name c_SHNM = chDefs::MakeName('S', 'H', 'N', 'M');
		const chDefs::Name c_MLIB = chDefs::MakeName('M', 'L', 'I', 'B');
		const chDefs::Name c_MOLN = chDefs::MakeName('M', 'O', 'L', 'N');	//outline
		const chDefs::Name c_UVXF = chDefs::MakeName('U', 'V', 'X', 'F');
		const chDefs::Name c_REFL = chDefs::MakeName('R', 'E', 'F', 'L');
		const chDefs::Name c_SHDT = chDefs::MakeName('S', 'H', 'D', 'T');
		const chDefs::Name c_SVFN = chDefs::MakeName('S', 'V', 'F', 'N');	
		const chDefs::Name c_THMB = chDefs::MakeName('T', 'H', 'M', 'B');	
		const chDefs::Name c_MDSP = chDefs::MakeName('M', 'D', 'S', 'P');	//displacement
		const chDefs::Name c_RMOV = chDefs::MakeName('R', 'M', 'O', 'V');	//renderman override

		//------------------------------------------------------------------------
		// Note: this parses differently than the chChunkParserUtil
		//------------------------------------------------------------------------
		void read_color(chReader& i_Reader, maFloatRGBA& o_Color)
		{
			envType::Float32 r,g,b,a;
			i_Reader.Read(r);
			i_Reader.Read(g);
			i_Reader.Read(b);
			i_Reader.Read(a);
			o_Color.Set(r,g,b,a);
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void resolve_fullpath(fsLocator& io_Locator, const fsLocator& i_ContainingFile)
		{
			fsLocator tex_loc = io_Locator;
			if (matTexturePathUtil::ResolveFullPath(tex_loc, i_ContainingFile,
				matShaderParser::GetAllowSingleFilenameTextures(), matShaderParser::GetResolveAbsolutePaths()))
			{
				io_Locator = tex_loc;
			}
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void read_SHDR( chReader& i_Reader, 
						chDefs::Version i_Version,
						mdlMaterialInfo& o_MaterialInfo)
		{
			chDefs::Name name;
			chDefs::Size size;
			chDefs::Version version;

			shared_ptr<effShaderParams> shaderparams;
			shared_ptr<effShaderData> shaderdata;

			fsLocator fullShaderPath;
			std::string shadername;	
			std::string shaderFileName;
			itString finalShaderName;

			while ( i_Reader.ReadChunkHeader(name, version, size) )
			{
				if (name == c_SHNM)
				{
					i_Reader.Read(shadername);
					finalShaderName = itString(shadername.c_str());

					if ( version < 2 )
					{
						fsFileUtil::UnicodeStringToLocator( itString(shadername.c_str()), fullShaderPath );
						shaderFileName = itStringUtil::GetStdString( fullShaderPath.GetLastName() );

						if ( fullShaderPath.GetNumNames() == 1 )
						{
							// Support old shader names
							if ( shaderFileName == "BlinnRefl.fx" ) 
							{
								fullShaderPath.Pop();
								fullShaderPath.Push("BlinnReflection.fx");
							} 
							else if ( shaderFileName == "BlinnSkin.fx" )
							{
								fullShaderPath.Pop();
								fullShaderPath.Push("SubSurfaceScatter_wBlinnSpecular.fx");
							}
							else if ( shaderFileName == "Hair.fx" ) 
							{
								fullShaderPath.Pop();
								fullShaderPath.Push("SpecularFresnel.fx");
							}
							else if ( shaderFileName == "PhongBump.fx" )
							{
								fullShaderPath.Pop();
								fullShaderPath.Push("Phong_wBump.fx");
							}
							else if ( shaderFileName == "ReflRefr.fx" )
							{
								fullShaderPath.Pop();
								fullShaderPath.Push("PhongReflection.fx");
							}
							else if ( shaderFileName == "Skin.fx" )
							{
								fullShaderPath.Pop();
								fullShaderPath.Push("SubSurfaceScatter.fx");
							}
						}
						
						fsFileUtil::LocatorToUnicodeString( fullShaderPath, finalShaderName );
					}

				}
				else if (name == c_SHDT)
				{
					// new name/value pairs
					shaderparams.reset(matShaderParser::ReadShaderParams(i_Reader, name, version, size, o_MaterialInfo ));
				}
				else
				{
					// old hard coded shaders with UV transform and refl map info
					shaderparams.reset(matShaderParser::ReadShaderData(i_Reader, name, version, size, o_MaterialInfo ));
				}

				i_Reader.FinishChunk();
			}

			if (!shaderparams)
			{
				DBG_WARNING("mdlMaterialParser failed to parse shader " << shadername.c_str() << ": reverting to default phong shader." );
				shadername = "";
			}
			else
			{
				if (!shadername.empty())
				{
					shaderparams->SetShaderName(finalShaderName);
				}
			}

			// shaderdata is copied
//			o_MaterialInfo.SetShader(shadername, shaderdata.get());
			o_MaterialInfo.SetShaderParams(shaderparams);
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void read_MFUR( chReader& i_Reader, 
			chDefs::Version i_Version,
			mdlMaterialInfo& o_MaterialInfo)
		{
			chDefs::Name name;
			chDefs::Size size;
			chDefs::Version version;

			effFurData furParams;

			try
			{
				while ( i_Reader.ReadChunkHeader(name, version, size) )
				{
					if( name == c_EFCT )
					{
						mdlMaterialLegacyParser::ReadFur_EFCT(i_Reader, version, size, furParams);
					}
					else if (name == effFurDataParser::GetChunkName())
					{
						effFurDataParser parser;
						parser.Read(i_Reader, version, size, furParams);
					}

					i_Reader.FinishChunk();
				}
			}
			catch ( const chInvalidChunkX& i_Ex )
			{
				i_Ex;
				DBG_LOG("Invalid chunk in mdlImport::read_MFUR");
				throw mdlInvalidModelFileX(i_Reader.GetLocator());
			}
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void read_MGLO( chReader& i_Reader, 
			chDefs::Version i_Version,
			mdlMaterialInfo& o_MaterialInfo)
		{
			chDefs::Name name;
			chDefs::Size size;
			chDefs::Version version;

			// enable glow and create glow parameters for editing
			o_MaterialInfo.SetHasGlow(true);

			try
			{
				while ( i_Reader.ReadChunkHeader(name, version, size) )
				{
					if( name == c_EFCT )
					{
						mdlMaterialLegacyParser::ReadGlow_EFCT(i_Reader, version, size, o_MaterialInfo.GlowParams());
					}
					else if (name == c_TXLY)
					{
						// Old file format. Don't really have to read TXLY chunk here,
						// we are only going to use the texture name, which is first.
						std::string glowMask;
						i_Reader.Read(glowMask);

						fsLocator locGlowMask(itString(glowMask.c_str()));
						resolve_fullpath(locGlowMask, i_Reader.GetLocator());
						o_MaterialInfo.GlowParams().m_NameGlowMask = locGlowMask;
					}
					else if (name == effGlowDataParser::GetChunkName())
					{
						effGlowDataParser parser;
						parser.Read(i_Reader, version, size, o_MaterialInfo.GlowParams());
					}

					i_Reader.FinishChunk();
				}
			}
			catch ( const chInvalidChunkX& i_Ex )
			{
				i_Ex;
				DBG_LOG("Invalid chunk in mdlImport::read_MGLO");
				throw mdlInvalidModelFileX(i_Reader.GetLocator());
			}
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void read_RMOV( chReader& i_Reader, 
			chDefs::Version i_Version,
			mdlMaterialInfo& o_MaterialInfo)
		{
			chDefs::Name name;
			chDefs::Size size;
			chDefs::Version version;

			o_MaterialInfo.SetHasRendermanOverride(true);

			try
			{
				while ( i_Reader.ReadChunkHeader(name, version, size) )
				{
					if (name == effRendermanOverrideDataParser::GetChunkName())
					{
						effRendermanOverrideDataParser parser;
						parser.Read(i_Reader, version, size, o_MaterialInfo.RendermanOverrideParams());
					}

					i_Reader.FinishChunk();
				}
			}
			catch ( const chInvalidChunkX& i_Ex )
			{
				i_Ex;
				DBG_LOG("Invalid chunk in mdlImport::read_RMOV");
				throw mdlInvalidModelFileX(i_Reader.GetLocator());
			}
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void read_NMAP( chReader& i_Reader, 
			chDefs::Version i_Version,
			mdlMaterialInfo& o_MaterialInfo)
		{
			chDefs::Name name;
			chDefs::Size size;
			chDefs::Version version;

			try
			{
				while ( i_Reader.ReadChunkHeader(name, version, size) )
				{
					if (name == effNormalsDataParser::GetChunkName())
					{
						effNormalsDataParser parser;
						parser.Read(i_Reader, version, size, o_MaterialInfo.NormalsParams());
					}

					i_Reader.FinishChunk();
				}
			}
			catch ( const chInvalidChunkX& i_Ex )
			{
				i_Ex;
				DBG_LOG("Invalid chunk in mdlImport::read_MNMP");
				throw mdlInvalidModelFileX(i_Reader.GetLocator());
			}
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void read_UVXF( chReader& i_Reader, 
						chDefs::Version i_Version,
						mdlMaterialInfo& o_MaterialInfo)
		{
			i_Reader.Read(o_MaterialInfo.UVTransform().m_UScale);
			i_Reader.Read(o_MaterialInfo.UVTransform().m_VScale);
			i_Reader.Read(o_MaterialInfo.UVTransform().m_UTrans);
			i_Reader.Read(o_MaterialInfo.UVTransform().m_VTrans);
			i_Reader.Read(o_MaterialInfo.UVTransform().m_UVAngle);
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void read_REFL( chReader& i_Reader, 
						chDefs::Version i_Version,
						mdlMaterialInfo& o_MaterialInfo)
		{
			o_MaterialInfo.SetHasReflection(true);

			i_Reader.Read(o_MaterialInfo.ReflectionParams().m_bAutoGenEnvMap);
			i_Reader.Read(o_MaterialInfo.ReflectionParams().m_ReflMapResolution);
			i_Reader.Read(o_MaterialInfo.ReflectionParams().m_bIsPlanar);
			i_Reader.Read(o_MaterialInfo.ReflectionParams().m_NearPlane);
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void read_MOLN( chReader& i_Reader, 
						chDefs::Version i_Version,
						mdlMaterialInfo& o_MaterialInfo)
		{
			chDefs::Name name;
			chDefs::Size size;
			chDefs::Version version;

			// enable outline and create outline parameters for editing
			o_MaterialInfo.SetHasOutline(true);

			try
			{
				while ( i_Reader.ReadChunkHeader(name, version, size) )
				{
					if (name == effOutlineDataParser::GetChunkName())
					{
						effOutlineDataParser parser;
						parser.Read(i_Reader, version, size, o_MaterialInfo.OutlineParams());
					}

					i_Reader.FinishChunk();
				}
			}
			catch ( const chInvalidChunkX& i_Ex )
			{
				i_Ex;
				DBG_LOG("Invalid chunk in mdlImport::read_MOLN");
				throw mdlInvalidModelFileX(i_Reader.GetLocator());
			}
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void read_MDSP( chReader& i_Reader, 
			chDefs::Version i_Version,
			mdlMaterialInfo& o_MaterialInfo)
		{
			chDefs::Name name;
			chDefs::Size size;
			chDefs::Version version;

			o_MaterialInfo.SetHasDisplacement(true);

			try
			{
				while ( i_Reader.ReadChunkHeader(name, version, size) )
				{
					if (name == effDisplacementDataParser::GetChunkName())
					{
						effDisplacementDataParser parser;
						parser.Read(i_Reader, version, size, o_MaterialInfo.DisplacementParams());
					}

					i_Reader.FinishChunk();
				}
			}
			catch ( const chInvalidChunkX& i_Ex )
			{
				i_Ex;
				DBG_LOG("Invalid chunk in mdlImport::read_MDSP");
				throw mdlInvalidModelFileX(i_Reader.GetLocator());
			}


//			i_Reader.Read(o_MaterialInfo.DisplacementParams().m_Scale );
//			i_Reader.Read(o_MaterialInfo.DisplacementParams().m_Bias );
//			i_Reader.Read(o_MaterialInfo.DisplacementParams().m_Blur );
//			i_Reader.Read(o_MaterialInfo.DisplacementParams().m_TessellationValue );
//			i_Reader.Read(o_MaterialInfo.DisplacementParams().m_NameDisplacementMap );

		}


		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void read_THMB( chReader& i_Reader, 
						chDefs::Version i_Version,
						mdlMaterialInfo& o_MaterialInfo)
		{
			// enable outline and create outline parameters for editing
			//o_MaterialInfo.SetHasThumb(true);

			try
			{		
				i_Reader.FinishChunk();	
			}
			catch ( const chInvalidChunkX& i_Ex )
			{
				i_Ex;
				DBG_LOG("Invalid chunk in mdlImport::read_THMB");
				throw mdlInvalidModelFileX(i_Reader.GetLocator());
			}
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void read_MLIB( chReader& i_Reader, 
						chDefs::Version i_Version,
						mdlMaterialInfo& o_MaterialInfo)
		{
			// path to material library filename
			std::string lib_filename;
			chChunkParserUtil::Read( i_Reader, lib_filename );
			fsLocator matlib_loc;
			fsFileUtil::ANSIFilenameToLocator( lib_filename, matlib_loc);
			o_MaterialInfo.SetLibraryFilename( matlib_loc );
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void read_MATR(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						mdlMaterialInfo& o_MaterialInfo)
		{
			chDefs::Name name;
			chDefs::Size size;
			chDefs::Version version;

			try
			{
				while ( i_Reader.ReadChunkHeader(name, version, size) )
				{
					if( name == c_MNAM )
					{
						// material name
						std::string material_name;
						i_Reader.Read(material_name);
						o_MaterialInfo.SetMaterialName(material_name);
					}
					else if (name == c_MANI)
					{
						// material animation
						//read_MANI(i_Reader, version, size, o_MaterialInfo);
						std::string filename;
						fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
						DBG_WARNING("Skipping material animation: " << o_MaterialInfo.GetMaterialName().c_str() << " in file " << filename );
					}
					else if (name == c_SHDR)
					{
						read_SHDR(i_Reader, version, o_MaterialInfo);
					}
					else if (name == c_MFUR)
					{
						read_MFUR(i_Reader, version, o_MaterialInfo);
					}
					else if (name == c_MGLO)
					{
						read_MGLO(i_Reader, version, o_MaterialInfo);
					}
					else if (name == c_NMAP)
					{
						read_NMAP(i_Reader, version, o_MaterialInfo);
					}
					else if (name == c_MLIB)
					{
						read_MLIB(i_Reader, version, o_MaterialInfo);
					}
					else if (name == c_MOLN)
					{
						read_MOLN(i_Reader, version, o_MaterialInfo);
					}
					else if (name == c_THMB)
					{
						read_THMB(i_Reader, version, o_MaterialInfo);
					}
					else if (name == c_UVXF)
					{
						read_UVXF(i_Reader, version, o_MaterialInfo);
					}
					else if (name == c_REFL)
					{
						read_REFL(i_Reader, version, o_MaterialInfo);
					}
					else if (name == c_MDSP)
					{
						read_MDSP(i_Reader, version, o_MaterialInfo);
					}
					else if (name == c_RMOV)
					{
						read_RMOV(i_Reader, version, o_MaterialInfo);
					}

					i_Reader.FinishChunk();
				}
			}
			catch ( const chInvalidChunkX& i_Ex )
			{
				i_Ex;
				DBG_LOG("Invalid chunk in mdlImport::read_MATR");
				throw mdlInvalidModelFileX(i_Reader.GetLocator());
			}
		}
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void read_MATR_legacy(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						mdlMaterialInfo& o_MaterialInfo)
		{
			chDefs::Name name;
			chDefs::Size size;
			chDefs::Version version;

			bool read_MBAS = false, read_water = false;
			shared_ptr<effPhongData> phong_data(new effPhongData());

			try
			{
				while ( i_Reader.ReadChunkHeader(name, version, size) )
				{
					if( name == c_MBAS )
					{
						read_MBAS = true;
						mdlMaterialLegacyParser::ReadPhong_MBAS(i_Reader, version, size, *phong_data);
					}
					else if( name == c_MNAM )
					{
						// material name
						std::string str;
						i_Reader.Read(str);
						o_MaterialInfo.SetMaterialName(str);
					}
					else if (name == c_TXLY)
					{
						// texture layer
						mdlMaterialLegacyParser::ReadPhong_TXLY(i_Reader, version, size, *phong_data);
					}
					else if (name == c_MANI)
					{
						// material animation
						//read_MANI(i_Reader, version, size, o_MaterialInfo);
						std::string filename;
						fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
						DBG_WARNING("Skipping material animation: " << o_MaterialInfo.GetMaterialName().c_str() << " in file " << filename);
					}
					else if (name == c_VSHD)
					{
						// vertex shader
						//read_VSHD(i_Reader, version, size, o_MaterialInfo);
						std::string filename;
						fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
						DBG_WARNING("Skipping vertex shader: " << o_MaterialInfo.GetMaterialName().c_str() << " in file " << filename);
					}
					else if (name == c_MBSC)
					{
						// bump scale
						envType::Float32 bump_scale;
						i_Reader.Read(bump_scale);
						phong_data->m_BumpMapScale = bump_scale;
					}
					else if (name == c_MRFL)
					{
						// reflectivity
						envType::Float32 refl;
						i_Reader.Read(refl);
						phong_data->m_Reflectivity = refl;
					}
					else if (name == c_UVSC)
					{
						// uv scaling factor
						envType::Float32 u,v;
						i_Reader.Read(u);
						i_Reader.Read(v);
						phong_data->m_UV.m_UScale = u;
						phong_data->m_UV.m_VScale = v;
					}
					else if (name == c_MEFF)
					{
						// effect filename
						//read_MEFF(i_Reader, version, size, o_MaterialInfo);
						std::string filename;
						fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
						DBG_WARNING("Skipping old effect filename chunk: " << o_MaterialInfo.GetMaterialName().c_str() << " in file " << filename);
					}
					else if (name == c_EFCT)
					{
						// load legacy water shader. new I/O will take care of other shaders set in machstudio app layer, above terawatt(?)
						read_water = true;
//						shared_ptr<effWaterData> water_data(new effWaterData());
//						mdlMaterialLegacyParser::ReadWater_EFCT(i_Reader, version, size, *water_data);
//						o_MaterialInfo.SetShader("Water.fx", water_data.get());

						shared_ptr<effShaderParams> water_params(new effShaderParams());
						water_params->SetShaderName(itString("Water.fx"));
						mdlMaterialLegacyParser::ReadWaterParams(i_Reader, version, size, *water_params);
						o_MaterialInfo.SetShaderParams(water_params);
					}
					else if (name == c_MFUR)
					{
						read_MFUR(i_Reader, version, o_MaterialInfo);
					}
					else if (name == c_MGLO)
					{
						read_MGLO(i_Reader, version, o_MaterialInfo);
					}

					i_Reader.FinishChunk();
				}
			}
			catch ( const chInvalidChunkX& i_Ex )
			{
				i_Ex;
				DBG_LOG("Invalid chunk in mdlImport::read_MBAS");
				throw mdlInvalidModelFileX(i_Reader.GetLocator());
			}

			if (!read_water)
			{
				DBG_ASSERT(read_MBAS, "Didn't find required MBAS chunk in mdlImport::read_MATR");
				std::string shader_name = mdlMaterialLegacyParser::GetShaderNameForLegacyPhong(*phong_data);

				shared_ptr<effShaderParams> phong_params(new effShaderParams());
				phong_params->SetShaderName(itString(shader_name.c_str()));
				phong_data->AddToParams(*phong_params);

				//o_MaterialInfo.SetShader(shader_name, phong_data.get());
				o_MaterialInfo.SetShaderParams(phong_params);
			}

		}

	}	// end of local namespace

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReadMATR(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					mdlMaterialInfo& o_MaterialInfo)
	{
		if (i_Version < 1)
		{
			read_MATR_legacy(i_Reader, i_Version, i_Size, o_MaterialInfo);
		}
		else
		{
			read_MATR(i_Reader, i_Version, i_Size, o_MaterialInfo);
		}
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void ReadMaterialTable(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					mdlMatInfoTable &o_MaterialTable)
	{
		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		try
		{
			while ( i_Reader.ReadChunkHeader(name, version, size) )
			{
				if( name == c_MATR )
				{
					// Only read one material at a time so that
					// we don't confuse the effect and shader code.
					//envScopedLock material_lock(l_Mutex);

					shared_ptr<mdlMatInfo> new_mat_info(new mdlMatInfo());
					ReadMATR(i_Reader, version, size, new_mat_info->m_Info);
					if (new_mat_info->m_Info.GetMaterialName().empty())
					{
						DBG_LOG("Material in table does not have name in mdlImport::ReadMaterialTable");
						throw mdlInvalidModelFileX(i_Reader.GetLocator());
					}
					o_MaterialTable[new_mat_info->m_Info.GetMaterialName()] = new_mat_info;
				}
				else if (name == c_SVFN)
				{
					// Read in name of fullpath of filename that was saved,
					// we can compare this to the filename we are currently loading
					// in order to resolve absolute paths to textures
					fsLocator org_locator;
					itString savefilename;
					chChunkParserUtil::Read( i_Reader, savefilename );
					fsFileUtil::UnicodeStringToLocator( savefilename, org_locator );
					fsAbsolutePathMgr::AddDirMappingFromFiles(org_locator, i_Reader.GetLocator());
				}
				i_Reader.FinishChunk();
			}
		}
		catch ( const chInvalidChunkX& i_Ex )
		{
			i_Ex;
			DBG_LOG("Invalid chunk in mdlImport::ReadMaterialTable");
			throw mdlInvalidModelFileX(i_Reader.GetLocator());
		}
	}

}

