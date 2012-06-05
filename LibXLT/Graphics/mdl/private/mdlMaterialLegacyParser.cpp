/****************************************************************************\
**	mdlMaterialLegacyParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/private/mdlMaterialLegacyParser.hpp"

#include "Core/app/appSimTime.hpp"
#include "Core/ch/chBinReader.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/an/anKeyAnimation.hpp"
#include "Graphics/eff/effFurData.hpp"
#include "Graphics/eff/effFurDataParser.hpp"
#include "Graphics/eff/effGlowDataParser.hpp"
#include "Graphics/eff/effNormalsDataParser.hpp"
#include "Graphics/eff/effRendermanOverrideDataParser.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/eff/effWaterData.hpp"
#include "Graphics/mat/matMatAnim.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/mdl/mdlMatInfo.hpp"


//----------------------------------------------------------------------------
//	Any of these mdlMaterialLegacyParser functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//----------------------------------------------------------------------------
namespace mdlMaterialLegacyParser
{
	namespace
	{
		const chDefs::Name c_VCOL = chDefs::MakeName('V', 'C', 'O', 'L');
		const chDefs::Name c_MTBL = chDefs::MakeName('M', 'T', 'B', 'L');
		const chDefs::Name c_MATR = chDefs::MakeName('M', 'A', 'T', 'R');
		const chDefs::Name c_MTID = chDefs::MakeName('M', 'T', 'I', 'D');
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
		const chDefs::Name c_UVSC = chDefs::MakeName('U', 'V', 'S', 'C');
		const chDefs::Name c_SHDR = chDefs::MakeName('S', 'H', 'D', 'R');
		const chDefs::Name c_SHNM = chDefs::MakeName('S', 'H', 'N', 'M');
		const chDefs::Name c_MLIB = chDefs::MakeName('M', 'L', 'I', 'B');

		bool l_bAlwaysFullAmbient = false;

		// Mutex for synchronizing threads so that
		// only one thread is creating materials at the same time.
		//envMutex l_Mutex;

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		struct mayTexLayerInfo
		{
			enum LayerType
			{
				e_Basic = 0
			};
			enum TextureType
			{
				e_Normal = 0,
				e_AlphaBlend,
				e_BumpHeight,
				e_SpecularMap,
				e_EnvMap,
				e_GlossMap
			};

			mayTexLayerInfo();

			LayerType	m_LayerType;

			std::string m_TextureName;
			TextureType	m_TextureType;

			maVector2d m_Translation;
			maVector2d m_Scale;
			float m_Rotation;	// radians

			envType::Int8 m_UVSet;

			//------------------------------------------------------------------------
			//	SetupLayer is a helper function that will load a texture, add it
			//	to the last slot of the given material, and set the other material
			//	texture parameters to conform to the mayTexLayerInfo.
			//------------------------------------------------------------------------
			//void SetupLayer(matMaterial* o_Material,
			//				int i_LayerNumber,
			//				const fsLocator& i_TextureDir) const;
			// should this be fsResourceFinder now? who uses this function?
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		mayTexLayerInfo::mayTexLayerInfo()
		:	m_LayerType(mayTexLayerInfo::e_Basic),
			m_TextureType(mayTexLayerInfo::e_Normal),
			m_Translation(0,0),
			m_Scale(1,1),
			m_Rotation(0),
			m_UVSet(0)
		{
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		struct mayShaderParams
		{
			enum ParamType
			{
				e_float = 0,
				e_vector,
				e_matrix,
				e_texture
			};
			// on i/o these have to be ready for shader changes...
			// drop variables that are not recognized by the shader 
			// or are the wrong type
			std::string m_shaderName;
			std::map<std::string, maVector4d> m_vectors;
			std::map<std::string, float> m_floats;
			std::map<std::string, maMatrix4x4> m_matrices;
			std::map<std::string, std::string> m_textures;
			std::map<std::string, bool> m_bools;
			std::map<std::string, std::string> m_strings;

			void Dump(std::string i_Name) const;
		};

		//enum VertexShaderType
		//{
		//	e_Normal = 0,
		//	e_Anisotropic,
		//	e_Membrane,
		//	e_Rainbow,
		//	e_ReflectRefract,
		//	e_Toon,
		//	e_BumpDiffuse,
		//	e_BumpSpecularMap
		//};

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

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void read_TXLY(chReader& i_Reader, mayTexLayerInfo &o_Layer)
		{
			// texture name
			i_Reader.Read(o_Layer.m_TextureName);

			// blend type
			envType::Int8 int8;
			i_Reader.Read(int8);
			o_Layer.m_TextureType = mayTexLayerInfo::TextureType(int8);

			chChunkParserUtil::Read(i_Reader, o_Layer.m_Scale);		// scale
			chChunkParserUtil::Read(i_Reader, o_Layer.m_Translation); // translation
			i_Reader.Read(o_Layer.m_Rotation);	// rotation

			// uv source
			i_Reader.Read(int8);
			o_Layer.m_UVSet = int8;

		}

//		//------------------------------------------------------------------------
//		//------------------------------------------------------------------------
//		void read_MEFF(	chReader& i_Reader,
//						chDefs::Version i_Version,
//						chDefs::Size i_Size,
//						mdlMatInfo& o_MatInfo)
//		{
//			// shader effect file name
//			i_Reader.Read(o_MatInfo.m_EffectName);
//			//DBG_LOG("Read effect file " << o_MatInfo.m_EffectName.c_str());
//		}
//
//		//------------------------------------------------------------------------
//		//------------------------------------------------------------------------
//		void read_VSHD(	chReader& i_Reader,
//						chDefs::Version i_Version,
//						chDefs::Size i_Size,
//						mdlMatInfo& o_MatInfo)
//		{
//			envType::Int8 vshader;
//			i_Reader.Read(vshader);
//
//			switch (vshader)
//			{
//			default:
//				DBG_ASSERT(false, "Unknown vertex shader type");
//			case e_Normal:
////				o_MatInfo.m_pMaterial->SetSpecialEffect(matShaderIndex::e_Normal);
//				break;
//			case e_Anisotropic:
//			case e_Membrane:
//			case e_Rainbow:
//			case e_ReflectRefract:
//			case e_Toon:
//			case e_BumpDiffuse:
//			case e_BumpSpecularMap:
//				DBG_WARNING("Deprecated special effect shader is being set.");
////				o_MatInfo.m_pMaterial->SetSpecialEffect(matShaderIndex::e_BumpSpecularMap);
//				break;
//			}
//		}
//
//		//--------------------------------------------------------------------
//		//--------------------------------------------------------------------
//		matMatParamIndex::matMatParamIndexType get_param_index(chReader& i_Reader,
//								int i_MatComponent, int i_TextureLayer)
//		{
//			switch (i_MatComponent)
//			{
//			case 0:
//				return matMatParamIndex::e_Ambient;
//			case 1:
//				return matMatParamIndex::e_Diffuse;
//			case 2:
//				return matMatParamIndex::e_Specular;
//			case 3:
//				return matMatParamIndex::e_Emissive;
//			case 4:
//				return matMatParamIndex::e_SpecularPower;
//			case 5:
//				DBG_ASSERT(i_TextureLayer < 4, "Can't animate texture layer above fourth");
//				return matMatParamIndex::matMatParamIndexType(matMatParamIndex::e_TextureTranslation0 + i_TextureLayer);
//			case 6:
//				DBG_ASSERT(i_TextureLayer < 4, "Can't animate texture layer above fourth");
//				return matMatParamIndex::matMatParamIndexType(matMatParamIndex::e_TextureScale0 + i_TextureLayer);
//			case 7:
//				DBG_ASSERT(i_TextureLayer < 4, "Can't animate texture layer above fourth");
//				return matMatParamIndex::matMatParamIndexType(matMatParamIndex::e_TextureRotation0 + i_TextureLayer);
//			default:
//				DBG_LOG("Unknown material component in mdlImport::read_MANI");
//				throw mdlInvalidModelFileX(i_Reader.GetLocator());
//			}
//		}
//
//		//------------------------------------------------------------------------
//		//------------------------------------------------------------------------
//		void read_MANI(	chReader& i_Reader,
//						chDefs::Version i_Version,
//						chDefs::Size i_Size,
//						mdlMatInfo& o_MatInfo)
//		{
//			// material component values:
//			//  0 - Ambient
//			//  1 - Diffuse
//			//  2 - Specular
//			//  3 - Emissive
//			//  4 - Specular Power
//			//  5 - Texture Translation
//			//  6 - Texture Scale
//			//  7 - Texture Rotation
//
//			envType::Int8 mat_component, texture_layer, val, num_keys;
//			i_Reader.Read(mat_component);
//			i_Reader.Read(texture_layer);
//			i_Reader.Read(val);
//			bool bLooping = (val != 0);
//			i_Reader.Read(val);
//			bool bReversing = (val != 0);
//
//			i_Reader.Read(num_keys);
//			if (num_keys <= 0) return;
//
//			int key_data_size = 4;	// number of floats per key in data
//			if (mat_component == 4 || mat_component == 7)
//				key_data_size = 1;
//			else if (mat_component == 5 || mat_component == 6)
//				key_data_size = 2;
//
//			std::vector<float> times(num_keys);
//			std::vector<float> data(num_keys * key_data_size);
//			int index = 0;
//			for (int i=0; i<num_keys; i++)
//			{
//				for (int f=0; f<key_data_size; f++)
//					i_Reader.Read(data[index++]);
//
//				i_Reader.Read(times[i]);
//			}
//
//			// Added in version "1" - boolean for initial active state
//			bool bInitialActive = true;
//			if (i_Version >= 1)
//			{
//				envType::Int8 initial_active = 1;
//				i_Reader.Read(initial_active);
//				bInitialActive = (initial_active != 0);
//			}
//
//			// Create matMatAnim
//			//
//			if (key_data_size == 4)
//			{
//				anKeyAnimation<maFloatRGBA>* color_anim =
//					new anKeyAnimation<maFloatRGBA>(maFloatRGBA(data[0], data[1], data[2], data[3]));
//				index = key_data_size;
//				for (int k=1; k<num_keys; k++, index+=key_data_size)
//					color_anim->AddKey(times[k], maFloatRGBA(data[index], data[index+1], data[index+2], data[index+3]));
//
//				color_anim->SetLooping(bLooping);
//				color_anim->SetReversing(bReversing);
//
//				matMatAnim *mat_anim =
//					new matMatAnim( color_anim,
//									get_param_index(i_Reader, mat_component, texture_layer),
//									appSimTime::GetTime() );
//				mat_anim->SetActive(bInitialActive);
//				o_MatInfo.m_pMaterial->AddMatAnim( mat_anim );
//			}
//			else if (key_data_size == 2)
//			{
//				anKeyAnimation<maVector2d>* vec_anim =
//					new anKeyAnimation<maVector2d>(maVector2d(data[0], data[1]));
//				index = key_data_size;
//				for (int k=1; k<num_keys; k++, index+=key_data_size)
//					vec_anim->AddKey(times[k], maVector2d(data[index], data[index+1]));
//
//				vec_anim->SetLooping(bLooping);
//				vec_anim->SetReversing(bReversing);
//
//				matMatAnim *mat_anim =
//					new matMatAnim( vec_anim,
//									get_param_index(i_Reader, mat_component, texture_layer),
//									appSimTime::GetTime() );
//				mat_anim->SetActive(bInitialActive);
//				o_MatInfo.m_pMaterial->AddMatAnim( mat_anim );
//			}
//			else if (key_data_size == 1)
//			{
//				anKeyAnimation<float>* float_anim = new anKeyAnimation<float>(data[0]);
//				for (int k=1; k<num_keys; k++)
//					float_anim->AddKey(times[k], data[k]);
//
//				float_anim->SetLooping(bLooping);
//				float_anim->SetReversing(bReversing);
//
//				matMatAnim *mat_anim =
//					new matMatAnim( float_anim,
//									get_param_index(i_Reader, mat_component, texture_layer),
//									appSimTime::GetTime() );
//				mat_anim->SetActive(bInitialActive);
//				o_MatInfo.m_pMaterial->AddMatAnim( mat_anim );
//			}
//		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void read_EFCT( chReader& i_Reader, 
						chDefs::Version i_Version,
						mayShaderParams& o_params)
		{
			envType::Int8 i;
			envType::Int8 count;
			float fval[16];
			std::string sval, sval2;
			
			i_Reader.Read(o_params.m_shaderName);
			DBG_LOG("READ EFFECT name = " << o_params.m_shaderName.c_str());

			// floats
			i_Reader.Read(count);
			for (i = 0; i < count; i++)
			{
				i_Reader.Read(sval);
				i_Reader.Read(fval[0]);
				o_params.m_floats[sval] = fval[0];
				DBG_LOG("  sval.c_str()" << "=" << fval[0]);
			}

			// vectors
			i_Reader.Read(count);
			for (i = 0; i < count; i++)
			{
				i_Reader.Read(sval);
				i_Reader.Read(fval[0]);
				i_Reader.Read(fval[1]);
				i_Reader.Read(fval[2]);
				i_Reader.Read(fval[3]);
				o_params.m_vectors[sval] = maVector4d(fval[0],fval[1],fval[2],fval[3]);
				DBG_LOG("  " << sval.c_str() << "=" << fval[0] << ","  << fval[1] << "," << fval[2] << "," << fval[3] );
			}

			// matrices
			i_Reader.Read(count);
			for (i = 0; i < count; i++)
			{
				i_Reader.Read(sval);
				i_Reader.Read(fval[0]);
				i_Reader.Read(fval[1]);
				i_Reader.Read(fval[2]);
				i_Reader.Read(fval[3]);
				i_Reader.Read(fval[4]);
				i_Reader.Read(fval[5]);
				i_Reader.Read(fval[6]);
				i_Reader.Read(fval[7]);
				i_Reader.Read(fval[8]);
				i_Reader.Read(fval[9]);
				i_Reader.Read(fval[10]);
				i_Reader.Read(fval[11]);
				i_Reader.Read(fval[12]);
				i_Reader.Read(fval[13]);
				i_Reader.Read(fval[14]);
				i_Reader.Read(fval[15]);
				o_params.m_matrices[sval] = 
					maMatrix4x4(fval[0],fval[1],fval[2],fval[3],
					fval[4], fval[5], fval[6], fval[7],
					fval[8], fval[9], fval[10],fval[11],
					fval[12],fval[13],fval[14],fval[15]
					);
				DBG_LOG("  " << sval.c_str() << "=matrix" );
			}

			// hard coded texture filenames
			i_Reader.Read(count);
			for (i = 0; i < count; i++)
			{
				i_Reader.Read(sval);
				i_Reader.Read(sval2);
				o_params.m_textures[sval] = sval2;
				DBG_LOG("  " << sval.c_str() << "(tex)=" << sval2.c_str());
			}

			if (i_Version > 0)
			{
				envType::Int8 tmpBool;
				// bools
				i_Reader.Read(count);
				for (i = 0; i < count; i++)
				{
					i_Reader.Read(sval);
					i_Reader.Read(tmpBool);
					o_params.m_bools[sval]=(tmpBool==1)?true:false;
					DBG_LOG("  " << sval.c_str() << "=" << ((tmpBool==1)?"true":"false"));
				}

				// strings
				i_Reader.Read(count);
				for (i = 0; i < count; i++)
				{
					i_Reader.Read(sval);
					i_Reader.Read(sval2);
					o_params.m_strings[sval]=sval2;
					DBG_LOG("  " << sval.c_str() << "=" << sval2.c_str());
				}

			}
			DBG_LOG("END READ EFFECT name=" << o_params.m_shaderName.c_str());
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		//void read_SHDR( chReader& i_Reader, 
		//				chDefs::Version i_Version,
		//				mdlMatInfo& o_MatInfo)
		//{
		//	chDefs::Name name;
		//	chDefs::Size size;
		//	chDefs::Version version;

		//	effShaderData* shaderdata = NULL;
		//	std::string shadername;

		//	while( i_Reader.ReadChunkHeader(name, version, size) )
		//	{
		//		if (name == c_SHNM)
		//		{
		//			i_Reader.Read(shadername);
		//		}
		//		else
		//		{
		//			shaderdata = matShaderParser::ReadShader(i_Reader, name, version, size);
		//		}

		//		i_Reader.FinishChunk();
		//	}
		//	if (shaderdata == NULL)
		//	{
		//		DBG_WARNING1("mdlMaterialLegacyParser failed to parse shader %s : reverting to default phong shader.", shadername.c_str());
		//		shadername = "";
		//	}
		//	// shaderdata is copied so can be deleted.
		//	o_MatInfo.SetShader(shadername, shaderdata);
		//	delete shaderdata;
		//}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		//void read_MFUR( chReader& i_Reader, 
		//	chDefs::Version i_Version,
		//	mdlMatInfo& o_matInfo)
		//{
		//	chDefs::Name name;
		//	chDefs::Size size;
		//	chDefs::Version version;

		//	try
		//	{
		//		while( i_Reader.ReadChunkHeader(name, version, size) )
		//		{
		//			if( name == c_EFCT )
		//			{
		//				mayShaderParams furParams;
		//				read_EFCT(i_Reader, version, furParams);

		//				std::map<std::string, std::string>::const_iterator sIt;
		//				std::map<std::string, float>::const_iterator fit;
		//				std::map<std::string, maVector4d>::const_iterator vit;
		//				std::map<std::string, bool>::const_iterator bit;
		//				sIt = furParams.m_strings.find("TextureFolder");
		//				if (sIt != furParams.m_strings.end())
		//					o_matInfo.m_FurData.m_TextureFolder = sIt->second;
		//				fit	= furParams.m_floats.find("LengthScale");
		//				if (fit != furParams.m_floats.end())
		//					o_matInfo.m_FurData.m_LengthScale = fit->second;
		//				fit	= furParams.m_floats.find("NumShells");
		//				if (fit != furParams.m_floats.end())
		//					o_matInfo.m_FurData.m_NumShells = fit->second;
		//				fit	= furParams.m_floats.find("finFader");
		//				if (fit != furParams.m_floats.end())
		//					o_matInfo.m_FurData.m_FinFader = fit->second;
		//				fit	= furParams.m_floats.find("shellFader");
		//				if (fit != furParams.m_floats.end())
		//					o_matInfo.m_FurData.m_ShellFader = fit->second;
		//				vit	= furParams.m_vectors.find("SpreadScale");
		//				if (vit != furParams.m_vectors.end())
		//					o_matInfo.m_FurData.m_SpreadScale = maVector3d(vit->second.GetX(), vit->second.GetY(), vit->second.GetZ());
		//				bit	= furParams.m_bools.find("bAnisotropic");
		//				if (bit != furParams.m_bools.end())
		//					o_matInfo.m_FurData.m_bAnisotropic = bit->second;
		//				bit	= furParams.m_bools.find("bColorSourcing");
		//				if (bit != furParams.m_bools.end())
		//					o_matInfo.m_FurData.m_bColorSourcing = bit->second;
		//				bit	= furParams.m_bools.find("bFurThinning");
		//				if (bit != furParams.m_bools.end())
		//					o_matInfo.m_FurData.m_bFurThinning = bit->second;
		//				bit	= furParams.m_bools.find("bShowFins");
		//				if (bit != furParams.m_bools.end())
		//					o_matInfo.m_FurData.m_bShowFins = bit->second;
		//			
		//			}
		//			else if (name == effFurDataParser::GetChunkName())
		//			{
		//				effFurDataParser parser;
		//				parser.Read(i_Reader, version, size, o_matInfo.m_FurData);
		//			}

		//			i_Reader.FinishChunk();
		//		}
		//	}
		//	catch ( const chInvalidChunkX& i_Ex )
		//	{
		//		i_Ex;
		//		DBG_LOG("Invalid chunk in mdlImport::read_MFUR");
		//		throw mdlInvalidModelFileX(i_Reader.GetLocator());
		//	}
		//}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		//void read_MGLO( chReader& i_Reader, 
		//	chDefs::Version i_Version,
		//	mdlMatInfo& o_matInfo)
		//{
		//	chDefs::Name name;
		//	chDefs::Size size;
		//	chDefs::Version version;

		//	try
		//	{
		//		while( i_Reader.ReadChunkHeader(name, version, size) )
		//		{
		//			if( name == c_EFCT )
		//			{
		//				mayShaderParams glowParams;
		//				read_EFCT(i_Reader, version, glowParams);

		//				std::map<std::string, float>::const_iterator fit;
		//				std::map<std::string, maVector4d>::const_iterator vit;
		//				std::map<std::string, bool>::const_iterator bit;
		//				fit	= glowParams.m_floats.find("glowAmount");
		//				if (fit != glowParams.m_floats.end())
		//					o_matInfo.m_GlowData.m_GlowAmount = fit->second;
		//				fit	= glowParams.m_floats.find("glowSize");
		//				if (fit != glowParams.m_floats.end())
		//					o_matInfo.m_GlowData.m_GlowSize = fit->second;
		//				vit	= glowParams.m_vectors.find("glowScale");
		//				if (vit != glowParams.m_vectors.end())
		//					o_matInfo.m_GlowData.m_GlowScale = vit->second;
		//				bit = glowParams.m_bools.find("bConstantGlow");
		//				if (bit != glowParams.m_bools.end())
		//					o_matInfo.m_GlowData.m_bConstantGlow = bit->second;
		//			}
		//			else if (name == c_TXLY)
		//			{
		//				mayTexLayerInfo glowMask;
		//				read_TXLY(i_Reader, glowMask);
		//				o_matInfo.m_GlowData.m_NameGlowMask = glowMask.m_TextureName;
		//			}
		//			else if (name == effGlowDataParser::GetChunkName())
		//			{
		//				effGlowDataParser parser;
		//				parser.Read(i_Reader, version, size, o_matInfo.m_GlowData);
		//			}

		//			i_Reader.FinishChunk();
		//		}
		//	}
		//	catch ( const chInvalidChunkX& i_Ex )
		//	{
		//		i_Ex;
		//		DBG_LOG("Invalid chunk in mdlImport::read_MGLO");
		//		throw mdlInvalidModelFileX(i_Reader.GetLocator());
		//	}
		//}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		//void read_MATR(	chReader& i_Reader,
		//				chDefs::Version i_Version,
		//				chDefs::Size i_Size,
		//				mdlMatInfo& o_MatInfo)
		//{
		//	chDefs::Name name;
		//	chDefs::Size size;
		//	chDefs::Version version;

		//	try
		//	{
		//		while( i_Reader.ReadChunkHeader(name, version, size) )
		//		{
		//			if( name == c_MNAM )
		//			{
		//				// material name
		//				i_Reader.Read(o_MatInfo.m_Name);
		//			}
		//			else if (name == c_MANI)
		//			{
		//				// material animation
		//				read_MANI(i_Reader, version, size, o_MatInfo);
		//			}
		//			else if (name == c_SHDR)
		//			{
		//				read_SHDR(i_Reader, version, o_MatInfo);
		//				o_MatInfo.m_pMaterial->SetHasSpecular(true);
		//			}
		//			else if (name == c_MFUR)
		//			{
		//				read_MFUR(i_Reader, version, o_MatInfo);
		//				o_MatInfo.m_HasFur = true;
		//			}
		//			else if (name == c_MGLO)
		//			{
		//				read_MGLO(i_Reader, version, o_MatInfo);
		//				o_MatInfo.m_HasGlow = true;
		//			}
		//			else if (name == c_MLIB)
		//			{
		//				// path to material library filename
		//				std::string lib_filename;
		//				chChunkParserUtil::Read( i_Reader, lib_filename );
		//				fsFileUtil::ANSIFilenameToLocator( lib_filename, o_MatInfo.m_LibraryFilename );
		//			}

		//			i_Reader.FinishChunk();
		//		}
		//	}
		//	catch ( const chInvalidChunkX& i_Ex )
		//	{
		//		i_Ex;
		//		DBG_LOG("Invalid chunk in mdlImport::read_MATR");
		//		throw mdlInvalidModelFileX(i_Reader.GetLocator());
		//	}
		//}
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		//void read_MATR_legacy(	chReader& i_Reader,
		//				chDefs::Version i_Version,
		//				chDefs::Size i_Size,
		//				mdlMatInfo& o_MatInfo)
		//{
		//	chDefs::Name name;
		//	chDefs::Size size;
		//	chDefs::Version version;

		//	bool read_MBAS = false;

		//	try
		//	{
		//		while( i_Reader.ReadChunkHeader(name, version, size) )
		//		{
		//			if( name == c_MBAS )
		//			{
		//				read_MBAS = true;
		//				maFloatRGBA color;

		//				read_color(i_Reader, color);
		//				o_MatInfo.m_Diffuse = color;

		//				read_color(i_Reader, color);
		//				if (IsAlwaysFullAmbient())
		//				{
		//					o_MatInfo.m_Ambient = maFloatRGBA(1,1,1,1);
		//				}
		//				else
		//				{
		//					o_MatInfo.m_Ambient = color;
		//				}

		//				read_color(i_Reader, color);
		//				o_MatInfo.m_Specular = color;

		//				read_color(i_Reader, color);
		//				o_MatInfo.m_Emissive = color;

		//				envType::Float32 spec_power;
		//				i_Reader.Read(spec_power);

		//				if( spec_power == 0.0f )
		//				{
		//					o_MatInfo.m_pMaterial->SetHasSpecular(false);
		//					o_MatInfo.m_SpecularPower = 1.0f;
		//				}
		//				else
		//				{
		//					o_MatInfo.m_pMaterial->SetHasSpecular(true);
		//					o_MatInfo.m_SpecularPower = spec_power;
		//				}

		//				envType::Int8 num_textures;
		//				i_Reader.Read(num_textures);
		//				int i;
		//				for( i = 0 ; i < num_textures ; i++ )
		//				{
		//					mayTexLayerInfo layer;
		//					i_Reader.Read(layer.m_TextureName);
		//					layer.m_LayerType = mayTexLayerInfo::e_Basic;
		//					layer.m_TextureType = mayTexLayerInfo::e_Normal;
		//					o_MatInfo.m_TextureLayers.push_back(layer);
		//				}
		//			}
		//			else if( name == c_MNAM )
		//			{
		//				// material name
		//				i_Reader.Read(o_MatInfo.m_Name);
		//			}
		//			else if (name == c_TXLY)
		//			{
		//				// texture layer
		//				mayTexLayerInfo layer;
		//				layer.m_LayerType = mayTexLayerInfo::e_Basic;
		//				read_TXLY(i_Reader, layer);
		//				o_MatInfo.m_TextureLayers.push_back(layer);
		//			}
		//			else if (name == c_MANI)
		//			{
		//				// material animation
		//				read_MANI(i_Reader, version, size, o_MatInfo);
		//			}
		//			else if (name == c_VSHD)
		//			{
		//				// vertex shader
		//				read_VSHD(i_Reader, version, size, o_MatInfo);
		//			}
		//			else if (name == c_MBSC)
		//			{
		//				// bump scale
		//				envType::Float32 bump_scale;
		//				i_Reader.Read(bump_scale);
		//				o_MatInfo.m_BumpMapScale = bump_scale;
		//			}
		//			else if (name == c_MRFL)
		//			{
		//				// reflectivity
		//				envType::Float32 refl;
		//				i_Reader.Read(refl);
		//				o_MatInfo.m_Reflectivity = refl;
		//			}
		//			else if (name == c_UVSC)
		//			{
		//				// uv scaling factor
		//				envType::Float32 u,v;
		//				i_Reader.Read(u);
		//				o_MatInfo.m_UScale = u;
		//				i_Reader.Read(v);
		//				o_MatInfo.m_VScale = v;
		//			}
		//			else if (name == c_MEFF)
		//			{
		//				// effect filename
		//				read_MEFF(i_Reader, version, size, o_MatInfo);

		//			}
		//			else if (name == c_EFCT)
		//			{
		//				mayShaderParams shaderParams;
		//				read_EFCT(i_Reader, version, shaderParams);

		//				// this must be the legacy water shader. it's the only shader that could fall into this block of code.
		//				DBG_ASSERT(shaderParams.m_shaderName == "Water.fx", "Alternate shader other than Water being loaded!");

		//				o_MatInfo.m_EffectName = shaderParams.m_shaderName;

		//				// load legacy water shader. new I/O will take care of other shaders set in machstudio app layer, above terawatt(?)
		//				effWaterData data;

		//				std::map<std::string, float>::const_iterator fit;
		//				std::map<std::string, maVector4d>::const_iterator vit;
		//				fit = shaderParams.m_floats.find("fadeBias");
		//				data.m_FadeBias = fit->second;
		//				fit = shaderParams.m_floats.find("fadeExp");
		//				data.m_FadeExp = fit->second;
		//				fit = shaderParams.m_floats.find("noiseBumpFactor");
		//				data.m_NoiseBumpFactor = fit->second;
		//				fit = shaderParams.m_floats.find("noiseSpeed");
		//				data.m_NoiseSpeed = fit->second;
		//				fit = shaderParams.m_floats.find("ringBumpFactor");
		//				data.m_RingBumpFactor = fit->second;
		//				fit = shaderParams.m_floats.find("ringFreq");
		//				data.m_RingFreq = fit->second;
		//				fit = shaderParams.m_floats.find("ringSpeed");
		//				data.m_RingSpeed = fit->second;
		//				fit = shaderParams.m_floats.find("time_offset");
		//				data.m_TimeOffset = fit->second;
		//				fit = shaderParams.m_floats.find("waveSpeed");
		//				data.m_WaveSpeed = fit->second;
		//				vit = shaderParams.m_vectors.find("ringCenter");
		//				data.m_RingCenter = vit->second;
		//				vit = shaderParams.m_vectors.find("waterColor");
		//				data.m_WaterColor = maFloatRGBA(vit->second.GetX(), vit->second.GetY(), 
		//					vit->second.GetZ(), vit->second.GetW());

		//				o_MatInfo.SetShader(o_MatInfo.m_EffectName, &data);
		//			}
		//			else if (name == c_MFUR)
		//			{
		//				read_MFUR(i_Reader, version, o_MatInfo);
		//				o_MatInfo.m_HasFur = true;
		//			}
		//			else if (name == c_MGLO)
		//			{
		//				read_MGLO(i_Reader, version, o_MatInfo);
		//				o_MatInfo.m_HasGlow = true;
		//			}

		//			i_Reader.FinishChunk();
		//		}
		//	}
		//	catch ( const chInvalidChunkX& i_Ex )
		//	{
		//		i_Ex;
		//		DBG_LOG("Invalid chunk in mdlImport::read_MBAS");
		//		throw mdlInvalidModelFileX(i_Reader.GetLocator());
		//	}

		//	DBG_ASSERT(read_MBAS, "Didn't find required MBAS chunk in mdlImport::read_MATR");
		//}

	}

	//------------------------------------------------------------------------
	// Some apps may want to override what ambient value is in the
	// model file and force all ambient colors to full white
	//------------------------------------------------------------------------
	void SetAlwaysFullAmbient(bool i_bVal)
	{
		l_bAlwaysFullAmbient = i_bVal;
	}
	bool IsAlwaysFullAmbient()
	{
		return l_bAlwaysFullAmbient;
	}
				
	//------------------------------------------------------------------------
	// The Glow shader parameters used to be written using a EFCT chunk.
	// This function will read in the ShaderParams in the EFCT chunk
	// and set the data into the glow effect data.
	//------------------------------------------------------------------------
	void ReadGlow_EFCT(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					effGlowData& o_GlowData)	
	{
		mayShaderParams glowParams;
		read_EFCT(i_Reader, i_Version, glowParams);

		std::map<std::string, float>::const_iterator fit;
		std::map<std::string, maVector4d>::const_iterator vit;
		std::map<std::string, bool>::const_iterator bit;
		fit	= glowParams.m_floats.find("glowAmount");
		if (fit != glowParams.m_floats.end())
			o_GlowData.m_GlowAmount = fit->second;
		fit	= glowParams.m_floats.find("glowSize");
		if (fit != glowParams.m_floats.end())
			o_GlowData.m_GlowSize = fit->second;
		vit	= glowParams.m_vectors.find("glowScale");
		if (vit != glowParams.m_vectors.end())
			o_GlowData.m_GlowScale = vit->second;
		bit = glowParams.m_bools.find("bConstantGlow");
		if (bit != glowParams.m_bools.end())
			o_GlowData.m_bConstantGlow = bit->second;
	}

	//------------------------------------------------------------------------
	// The Fur shader parameters used to be written using a EFCT chunk.
	// This function will read in the ShaderParams in the EFCT chunk
	// and set the data into the fur effect data.
	//------------------------------------------------------------------------
	void ReadFur_EFCT(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					effFurData& o_FurData)	
	{
		mayShaderParams furParams;
		read_EFCT(i_Reader, i_Version, furParams);

		std::map<std::string, std::string>::const_iterator sIt;
		std::map<std::string, float>::const_iterator fit;
		std::map<std::string, maVector4d>::const_iterator vit;
		std::map<std::string, bool>::const_iterator bit;
		sIt = furParams.m_strings.find("TextureFolder");
		if (sIt != furParams.m_strings.end())
			o_FurData.m_TextureFolder = sIt->second;
		fit	= furParams.m_floats.find("LengthScale");
		if (fit != furParams.m_floats.end())
			o_FurData.m_LengthScale = fit->second;
		fit	= furParams.m_floats.find("NumShells");
		if (fit != furParams.m_floats.end())
			o_FurData.m_NumShells = fit->second;
		fit	= furParams.m_floats.find("finFader");
		if (fit != furParams.m_floats.end())
			o_FurData.m_FinFader = fit->second;
		fit	= furParams.m_floats.find("shellFader");
		if (fit != furParams.m_floats.end())
			o_FurData.m_ShellFader = fit->second;
		vit	= furParams.m_vectors.find("SpreadScale");
		if (vit != furParams.m_vectors.end())
			o_FurData.m_SpreadScale = maVector3d(vit->second.GetX(), vit->second.GetY(), vit->second.GetZ());
		bit	= furParams.m_bools.find("bAnisotropic");
		if (bit != furParams.m_bools.end())
			o_FurData.m_bAnisotropic = bit->second;
		bit	= furParams.m_bools.find("bColorSourcing");
		if (bit != furParams.m_bools.end())
			o_FurData.m_bColorSourcing = bit->second;
		bit	= furParams.m_bools.find("bFurThinning");
		if (bit != furParams.m_bools.end())
			o_FurData.m_bFurThinning = bit->second;
		bit	= furParams.m_bools.find("bShowFins");
		if (bit != furParams.m_bools.end())
			o_FurData.m_bShowFins = bit->second;
	
	}

	//------------------------------------------------------------------------
	// The shader params chunk EFCT in the general material chunk MATR 
	// was only used by a single shader type. All other shaders would have
	// been written by newer file formats. This function parses that
	// chunk into effWaterData, the only type possible.
	//------------------------------------------------------------------------
	void ReadWater_EFCT( chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					effWaterData& o_WaterData)	
	{
		mayShaderParams shaderParams;
		read_EFCT(i_Reader, i_Version, shaderParams);

		// this must be the legacy water shader. it's the only shader that could fall into this block of code.
		DBG_ASSERT(shaderParams.m_shaderName == "Water.fx", "Alternate shader other than Water being loaded!");
		if (shaderParams.m_shaderName != "Water.fx")
			return;

		std::map<std::string, float>::const_iterator fit;
		std::map<std::string, maVector4d>::const_iterator vit;
		fit = shaderParams.m_floats.find("fadeBias");
		o_WaterData.m_FadeBias = fit->second;
		fit = shaderParams.m_floats.find("fadeExp");
		o_WaterData.m_FadeExp = fit->second;
		fit = shaderParams.m_floats.find("noiseBumpFactor");
		o_WaterData.m_NoiseBumpFactor = fit->second;
		fit = shaderParams.m_floats.find("noiseSpeed");
		o_WaterData.m_NoiseSpeed = fit->second;
		fit = shaderParams.m_floats.find("ringBumpFactor");
		o_WaterData.m_RingBumpFactor = fit->second;
		fit = shaderParams.m_floats.find("ringFreq");
		o_WaterData.m_RingFreq = fit->second;
		fit = shaderParams.m_floats.find("ringSpeed");
		o_WaterData.m_RingSpeed = fit->second;
		fit = shaderParams.m_floats.find("time_offset");
		o_WaterData.m_TimeOffset = fit->second;
		fit = shaderParams.m_floats.find("waveSpeed");
		o_WaterData.m_WaveSpeed = fit->second;
		vit = shaderParams.m_vectors.find("ringCenter");
		o_WaterData.m_RingCenter = vit->second;
		vit = shaderParams.m_vectors.find("waterColor");
		o_WaterData.m_WaterColor = maFloatRGBA(vit->second.GetX(), vit->second.GetY(), 
			vit->second.GetZ(), vit->second.GetW());
	}
	void ReadWaterParams( chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					effShaderParams& o_WaterData)
	{
		effWaterData data;
		ReadWater_EFCT(i_Reader, i_Version, i_Size, data);
		data.AddToParams(o_WaterData);
	}

	
	//------------------------------------------------------------------------
	// MBAS is the chunk exported from the Maya exporter. It has just basic
	//	phong data parameters and texture layers. Read this old file format
	//	into an effPhongData object.
	//------------------------------------------------------------------------
	void ReadPhong_MBAS( chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					effPhongData& o_PhongData)	
	{
		read_color(i_Reader, o_PhongData.m_ColorDiffuse);
		read_color(i_Reader, o_PhongData.m_ColorAmbient);
		read_color(i_Reader, o_PhongData.m_ColorSpecular);
		read_color(i_Reader, o_PhongData.m_ColorEmissive);
		i_Reader.Read(o_PhongData.m_SpecularPower);

		// Should we enforce the full ambient flag anymore?
		// Probably yes, because this is the exporting from Maya.
		// Once it has been edited and saved again, it will have the
		// ambient value requested by the artist and be exported in
		// a different PhongData chunk in the new format.
		if (IsAlwaysFullAmbient())
		{
			o_PhongData.m_ColorAmbient.Set(1,1,1,1);
		}

		// Do we need to handle specular power values of 0.0 specially?
		//if( spec_power == 0.0f )
		//{
		//	o_MaterialInfo.m_pMaterial->SetHasSpecular(false);
		//	o_MaterialInfo.m_SpecularPower = 1.0f;
		//}

		envType::Int8 num_textures;
		i_Reader.Read(num_textures);
		if (num_textures > 0)
		{
			std::string texture_name;
			i_Reader.Read(texture_name);
			o_PhongData.m_NameDiffuse = texture_name;
		}
		if (num_textures > 1)
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
			DBG_WARNING("More than one texture layer type parsed, only reading first layer in file " << filename.c_str());
		}
	}

	//------------------------------------------------------------------------
	// TXLY is a chunk in the old file format that represented a texture
	//	with a certain blending state. This should be interpreted now
	//	as a texture within a effPhongData.
	//------------------------------------------------------------------------
	void ReadPhong_TXLY( chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					effPhongData& o_PhongData)	
	{
		mayTexLayerInfo layer;
		layer.m_LayerType = mayTexLayerInfo::e_Basic;
		read_TXLY(i_Reader, layer);

		switch (layer.m_TextureType)
		{
			default:
			case mayTexLayerInfo::e_AlphaBlend:
				DBG_WARNING("Unrecognized texture layer type for texture: " << layer.m_TextureName.c_str());
				break;
			case mayTexLayerInfo::e_Normal:
				o_PhongData.m_NameDiffuse = layer.m_TextureName;
				break;
			case mayTexLayerInfo::e_BumpHeight:
				o_PhongData.m_NameNormalMap = layer.m_TextureName;
				break;
			case mayTexLayerInfo::e_SpecularMap:
				o_PhongData.m_NameSpecular = layer.m_TextureName;
				break;
			case mayTexLayerInfo::e_EnvMap:
				o_PhongData.m_NameEnvironment = layer.m_TextureName;
				break;
			case mayTexLayerInfo::e_GlossMap:
				o_PhongData.m_NameGloss = layer.m_TextureName;
				break;
		}
	}

	//------------------------------------------------------------------------
	// A legacy phong material can be mapped to either Simple.fx, Phong.fx
	//	or PhongBump.fx depending on the texture layers parsed.
	//------------------------------------------------------------------------
	std::string GetShaderNameForLegacyPhong(const effPhongData& i_PhongData)	
	{
		if (!i_PhongData.m_NameNormalMap.empty())
			return "Phong_wBump.fx";
		else if (!i_PhongData.m_NameDiffuse.empty()
				|| !i_PhongData.m_NameSpecular.empty()
				|| !i_PhongData.m_NameGloss.empty()
				|| !i_PhongData.m_NameEnvironment.empty()
				|| !i_PhongData.m_NameReflectFactorMap.empty())
			return "Phong.fx";
		else
			return "Simple.fx";
	}

}

