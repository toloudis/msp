/*****************************************************************************
**	eonConverter.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "eonConverter.hpp"

#include "Core/ch/chBinReader.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/mdl/mdlReader.hpp"
#include "Graphics/mdl/mdlSplitFragInfo.hpp"

//============================================================================
// EON includes - their SDK should be installed into "3rdParty\EON SDK"
// such that the first includes resolves to:
// C:\Projects\SourceCode\3rdParty\EON SDK\include\Dali\Libraries\SceneBuilder\ISceneBuilder.h
//============================================================================
#include <Dali/Libraries/SceneBuilder/ISceneBuilder.h>
#include <Dali/Libraries/SceneBuilder/win32/EONCOMSceneBuilderFactory.h>
#include <Dali/Libraries/Meshes/TriMesh.h>
#include <Dali/Libraries/SceneBuilder/SceneLight.h>
#include <Dali/Libraries/SceneBuilder/SceneTexture2D.h>
#include <Dali/Libraries/SceneBuilder/SceneMaterialAdvanced.h>
#include <Dali/Libraries/SceneBuilder/SceneMultiMaterial.h>

#include "Graphics/mdl/mdlFragInfo.hpp"


using namespace std;
using namespace EON;

//==============================================================================
//	library pragmas
//==============================================================================
//#pragma comment(lib,"Version.lib")
//#pragma comment(lib,"basetypesd.lib")
//#pragma comment(lib,"systemd.lib")
//#pragma comment(lib,"ExceptionBased.lib")
//#pragma comment(lib,"graphicsd.lib")
//#pragma comment(lib,"libjpegd.lib")
//#pragma comment(lib,"libpngd.lib")
//#pragma comment(lib,"XMLUtilitiesd.lib")
//#pragma comment(lib,"NvTriStripLongd.lib")
//#pragma comment(lib,"Meshesd.lib")
//#pragma comment(lib,"SceneBuilderd.lib")

//============================================================================
//============================================================================
namespace eonConverter
{
	namespace
	{
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		const chDefs::Name c_ECVF = chDefs::MakeName('E', 'C', 'V', 'F');	// Convert a file
		const chDefs::Name c_FNAM = chDefs::MakeName('F', 'N', 'A', 'M');	// filename
		const chDefs::Name c_FRAG = chDefs::MakeName('F', 'R', 'A', 'G');	// fragment chunk
		//const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');	// fragment name
		const chDefs::Name c_MATD = chDefs::MakeName('M', 'A', 'T', 'D');	// material chunk
		const chDefs::Name c_UVST = chDefs::MakeName('U', 'V', 'S', 'T');	// uv transform
		const chDefs::Name c_SCAL = chDefs::MakeName('S', 'C', 'A', 'L');	// scale geometry

		
		//------------------------------------------------------------------------
		// Keep track of which textures we have already created.
		//------------------------------------------------------------------------
		std::set<std::string> l_SharedTextures;

		//------------------------------------------------------------------------
		// Scaling to bring our geometry into scale of EON
		//------------------------------------------------------------------------
		float l_GeomScaling = 0.001f;

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		struct SceneBuilderCleanUp
		{
			ISceneBuilder* m_SceneBuilder;
			SceneBuilderCleanUp(ISceneBuilder* i_SceneBuilder)
				: m_SceneBuilder(i_SceneBuilder) {}
			~SceneBuilderCleanUp()
			{
				// Dispose the SceneBuilder
				EON::COMSceneBuilderFactory::DisposeInstance(m_SceneBuilder);
			}
		};

		//--------------------------------------------------------------------
		// Convert vector types
		//--------------------------------------------------------------------
		Vector3 gver_cvrt(const maVector3d& i_Vec)
		{
			// apply transformation to coordinates in order to 
			// match the Z-up world in EON
			return Vector3(i_Vec.m_X*l_GeomScaling, -i_Vec.m_Z*l_GeomScaling, i_Vec.m_Y*l_GeomScaling);
		}
		Vector3 vec3_cvrt(const maVector3d& i_Vec)
		{
			return Vector3(i_Vec.m_X, i_Vec.m_Y, i_Vec.m_Z);
		}
		Vector2 uv_cvrt(const maVector2d& i_Vec)
		{
			// Have to switch the texture coords from DX to OGL
			return Vector2(i_Vec.m_X, 1 - i_Vec.m_Y);
		}

		//--------------------------------------------------------------------
		// Convert from our fragment data into EON TriMesh data
		//--------------------------------------------------------------------
		void  convert_fragment(const mdlSplitFragInfo& i_Info,
								TriMesh &o_OutputMesh,
								const maPoint2d& i_UVScale,
								const maPoint2d& i_UVTranslate)
		{
			std::vector<Vector3> coords(i_Info.m_Vertices.size());
			std::transform(i_Info.m_Vertices.begin(),
						   i_Info.m_Vertices.end(),
						   coords.begin(),
						   //vec3_cvrt); 
						   gver_cvrt);
			o_OutputMesh.vertices().setCoordinates( coords );

			//for (int i=0; i<coords.size(); ++i)
			//{
			//	const Vector3 &vert = coords[i];
			//	DBG_LOG4("Vert (%d): %f %f %f", i, vert.x, vert.y, vert.z);
			//}

			std::vector<Vector3> norms(i_Info.m_Normals.size());
			std::transform(i_Info.m_Normals.begin(),
						   i_Info.m_Normals.end(),
						   norms.begin(),
						   vec3_cvrt);
			o_OutputMesh.vertices().setNormals( norms );

			std::vector<Vector2> uvs(i_Info.m_UVs.size());
			std::transform(i_Info.m_UVs.begin(),
						   i_Info.m_UVs.end(),
						   uvs.begin(),
						   uv_cvrt);
			o_OutputMesh.vertices().setTextureCoordinates( 0, uvs );

			// Generate transformed UVs for light map layer
			const int num_uvs = i_Info.m_UVs.size();
			std::vector<maVector2d> scaled_uvs(num_uvs);
			for (int i=0; i<num_uvs; i++)
			{
				scaled_uvs[i].Set( i_Info.m_UVs[i].m_X * i_UVScale.m_X + i_UVTranslate.m_X,
								i_Info.m_UVs[i].m_Y * i_UVScale.m_Y + i_UVTranslate.m_Y );

				std::vector<Vector2> ltmap_uvs(i_Info.m_UVs.size());
				std::transform(scaled_uvs.begin(),
							   scaled_uvs.end(),
							   ltmap_uvs.begin(),
							   uv_cvrt);
				o_OutputMesh.vertices().setTextureCoordinates( 1, ltmap_uvs );	
			}


			Faces::FaceData fd(Faces::FaceData::POLYGONS);
			std::vector<Int32> faces(i_Info.m_Indices.size()+1);
			std::vector<Int32>::iterator face_it = faces.begin();
			(*face_it++) = 3;
			std::copy(i_Info.m_Indices.begin(),
					  i_Info.m_Indices.end(),
					  face_it);
			fd.setCoordinateIndices(faces);
			o_OutputMesh.faces().addFaceData(fd);
			
			//for (int i=0; i<faces.size(); ++i)
			//{
			//	DBG_LOG2("Ind (%d): %d", i, faces[i]);
			//}
		}

		//----------------------------------------------------------------------------
		// Create name for texture from filename which may contain some
		//	path information.
		//----------------------------------------------------------------------------
		std::string create_texture_name(const std::string& i_TextureName)
		{
			int pos = i_TextureName.rfind(char('\\'));
			if (pos > 0)
				return std::string(i_TextureName.begin()+pos+1, i_TextureName.end());
			return i_TextureName;
		}

		//----------------------------------------------------------------------------
		// Use SceneBuilder to create texture, if needed.
		//----------------------------------------------------------------------------
		std::string create_texture(ISceneBuilder* i_pSceneBuilder,
									const std::string& i_TextureName)
		{
			//std::string tex_name = i_TextureName; // need to manipulate this name?
			std::string tex_name = create_texture_name(i_TextureName);
			if (!tex_name.empty())
			{
				if (l_SharedTextures.find(i_TextureName) == l_SharedTextures.end())
				{
					SceneTexture2D texture(tex_name);
					{
						//std::string rel_path("..\\Textures\\");
						//texture.setImageSourceFilePath(rel_path + i_TextureName);
						//DBG_LOG1("Test rel_path: %s", create_texture_name(rel_path + i_TextureName).c_str());
						texture.setImageSourceFilePath(i_TextureName);
						//texture.setMipMap(false);
						i_pSceneBuilder->createTexture( &texture );
					}
					l_SharedTextures.insert(i_TextureName);
				}
			}
			return tex_name;
		}
		
		//----------------------------------------------------------------------------
		// Create a fragment with given attributes
		//----------------------------------------------------------------------------
		void create_fragment(ISceneBuilder* i_pSceneBuilder,
						SceneHierarchy& io_ScenePath,
						shared_ptr<mdlSplitFragInfo>& i_SplitFragInfo,
						const std::string& i_BaseTextureName,
						const std::string& i_LightMapName,
						const maPoint2d& i_UVScale,
						const maPoint2d& i_UVTranslate,
						const maFloatRGBA& i_Diffuse,
						const maFloatRGBA& i_Ambient,
						const maFloatRGBA& i_Specular,
						const maFloatRGBA& i_Emissive)
		{
			TriMesh eon_mesh;
			convert_fragment(*i_SplitFragInfo, eon_mesh, i_UVScale, i_UVTranslate);
			{
				eon_mesh.setName(i_SplitFragInfo->m_Name);
				i_pSceneBuilder->createGeometry(&eon_mesh);
			}
			
			// Create a texture, if needed
			std::string tex_name = create_texture(i_pSceneBuilder, i_BaseTextureName);
			std::string lmap_name = create_texture_name(i_LightMapName);
			//std::string lmap_name = create_texture(i_pSceneBuilder, i_LightMapName);
			SceneTexture2D lmap_texture(lmap_name);
			{
				lmap_texture.setImageSourceFilePath(i_LightMapName);
				//lmap_texture.setMipMap(false);
				i_pSceneBuilder->createTexture( &lmap_texture );
			}

			// Create a new material for every fragment
			std::string mat_name = i_SplitFragInfo->m_MaterialName;
			mat_name += "_";
			mat_name += i_SplitFragInfo->m_Name;
			mat_name += "_Mat";
			SceneMaterialAdvanced material(mat_name);
			{
				if (!tex_name.empty())
					material.setDiffuseTexture(tex_name);

				if (!i_LightMapName.empty())
				{
					material.setDarkmapTexture(lmap_texture.getName());
					material.setDarkmapUVSet(1);
				}

				material.setDiffuse(ColorRGB(i_Diffuse.GetRed(), i_Diffuse.GetGreen(), i_Diffuse.GetBlue()));
				material.setAmbient(ColorRGB(i_Ambient.GetRed(), i_Ambient.GetGreen(), i_Ambient.GetBlue()));
				material.setSpecular(ColorRGB(i_Specular.GetRed(), i_Specular.GetGreen(), i_Specular.GetBlue()));
				material.setEmissive(ColorRGB(i_Emissive.GetRed(), i_Emissive.GetGreen(), i_Emissive.GetBlue()));
				material.setOpacity(i_Diffuse.GetAlpha());
				
				//material.setDiffuse(ColorRGB(1.0, 1.0, 1.0));
				//material.setAmbient(ColorRGB(1.0, 1.0, 1.0));
				//material.setSpecular(ColorRGB(0.0, 0.0, 0.0));
				//material.setEmissive(ColorRGB(0.0, 0.0, 0.0));
				//material.setOpacity(1.0);

				i_pSceneBuilder->createMaterial(&material);
			}

			// Push a level for this fragment
			io_ScenePath.push(i_SplitFragInfo->m_Name + "_Node");

			// Create the mesh object
			i_pSceneBuilder->createObject(i_SplitFragInfo->m_Name, 
										eon_mesh.getName(), 
										material.getName(), 
										io_ScenePath);

			// Change current path path to root again
			io_ScenePath.pop();
		}

		//----------------------------------------------------------------------------
		// Handle a chunk representing a single fragment within file to convert
		//----------------------------------------------------------------------------
		void parse_FRAG(chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						ISceneBuilder* i_pSceneBuilder,
						SceneHierarchy& io_ScenePath,
						shared_ptr<mdlSplitFragInfo>& i_SplitFragInfo)
		{
			chDefs::Name name;
			chDefs::Size size;
			chDefs::Version version;

			std::string materialName;
			std::string baseTextureName;
			std::string lightMapName;
			maPoint2d uv_scale(1,1), uv_translate(0,0);
			
			maFloatRGBA diffuse(1.0, 1.0, 1.0, 1.0);
			maFloatRGBA ambient(0.5, 0.5, 0.5, 1.0);
			maFloatRGBA specular(0.5, 0.5, 0.5, 1.0);
			maFloatRGBA emissive(0.5, 0.5, 0.5, 1.0);

			while( i_Reader.ReadChunkHeader(name, version, size) )
			{
				if (name == c_MATD)
				{
					chChunkParserUtil::Read( i_Reader, materialName); 
					chChunkParserUtil::Read( i_Reader, baseTextureName);
					chChunkParserUtil::Read( i_Reader, lightMapName);
					DBG_WARNING1("materialName: %s", materialName.c_str());

					// Added material colors in version 1
					if (version >= 1)
					{
						chChunkParserUtil::Read( i_Reader, diffuse); 
						chChunkParserUtil::Read( i_Reader, ambient); 
						chChunkParserUtil::Read( i_Reader, specular); 
						chChunkParserUtil::Read( i_Reader, emissive); 
					}
				}
				else if (name == c_UVST)
				{
					chChunkParserUtil::Read( i_Reader, uv_scale);
					chChunkParserUtil::Read( i_Reader, uv_translate);
					DBG_WARNING2("UV Scale: %f %f", uv_scale.m_X, uv_scale.m_Y);
					DBG_WARNING2("UV Translate: %f %f", uv_translate.m_X, uv_translate.m_Y);
				}

				i_Reader.FinishChunk();
			}

			// After reading in the details, create the fragment
			create_fragment(i_pSceneBuilder, io_ScenePath, i_SplitFragInfo, 
								baseTextureName, lightMapName,
								uv_scale, uv_translate,
								diffuse, ambient, specular, emissive);
		}

		//----------------------------------------------------------------------------
		// Handle a chunk representing a single MachStudio file to convert
		//----------------------------------------------------------------------------
		void parse_ECVF(chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						ISceneBuilder* i_pSceneBuilder,
						SceneHierarchy& io_ScenePath)
		{
			//TODO: Should we push and pop a scene node for each filename?

			chDefs::Name name;
			chDefs::Size size;
			chDefs::Version version;

			bool bDidPushSceneNode = false;

			std::vector< shared_ptr<mdlSplitFragInfo> > split_frags;
			int frag_ind = 0;
			while( i_Reader.ReadChunkHeader(name, version, size) )
			{
				if (name == c_FNAM)
				{
					// Get filename
					fsLocator geo_loc;
					std::string filename;
					chChunkParserUtil::Read( i_Reader, filename );
					fsFileUtil::ANSIFilenameToLocator(filename, geo_loc);

					std::cout << "Processing file: " << filename << endl;

					// Read in the geometry
					std::vector< shared_ptr<mdlFragInfo> > geoData;
					mdlReader::ReadWorldFragments(geo_loc, geoData); 

					// Split frags are appended to list
					for (int i=0; i<geoData.size(); ++i)
					{
						mdlFragUtil::SplitFragments(*geoData[i], split_frags);
					}

					itString file_node = geo_loc.GetLastName();
					file_node.StripExtension();
					file_node += itString::CharType(0); // NULL-terminate the string
					bDidPushSceneNode = true;
					io_ScenePath.push(String16(file_node.GetString()));
				}
				else if (name == c_FRAG)
				{
					if (frag_ind < split_frags.size())
					{
						parse_FRAG(i_Reader, version, size, i_pSceneBuilder, io_ScenePath, split_frags[frag_ind]);
						frag_ind++;
					}
					else
					{
						DBG_ERROR0("Do not have enough fragments.");
						throw mdlInvalidModelFileX(i_Reader.GetLocator());
					}
				}

				i_Reader.FinishChunk();
			}

			if (bDidPushSceneNode)
				io_ScenePath.pop();
		}
	}

	//------------------------------------------------------------------------
	//  DoConversion() - read instructions from conversion file and
	//		do conversion of file formats.
	//------------------------------------------------------------------------
	void  DoConversion(const fsLocator& i_ConvFile)
	{
		gfFileBin file(i_ConvFile, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);

		//	If this isn't a real Terawatt/XLT binary file this will throw
		file.ReadHeader();

		std::string template_file("SimpleTemplate.eoz");

		// Get directory that the exe is running in, in order to find the template
		// we need.
		char exePath[MAX_PATH];
		int PathLen = GetModuleFileNameA(NULL, exePath, MAX_PATH);
		if (PathLen > 0)
		{
			fsLocator exe_loc;
			fsFileUtil::ANSIFilenameToLocator(exePath, exe_loc);
			exe_loc.Pop();
			exe_loc.Push(template_file.c_str());
			fsFileUtil::LocatorToANSIFilename(exe_loc, template_file);
		}

		// Start access to EON SDK
		ISceneBuilder* sceneBuilder = COMSceneBuilderFactory::CreateInstance(template_file.c_str());

		// Wrap the pointer with an object to dispose of the factory
		// in case of an exception.
		SceneBuilderCleanUp sceneBuilder_CleanUp(sceneBuilder);

		SceneHierarchy scenePath;

		// Create a root frame 
		scenePath.push("Root Node");

		// Begin reading chunks from our conversion instruction file.
		chBinReader reader(file);

		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		while( reader.ReadChunkHeader(name, version, size) )
		{
			if (name == c_ECVF)
			{
				parse_ECVF(reader, version, size, sceneBuilder, scenePath);
			}
			else if (name == c_SCAL)
			{
				chChunkParserUtil::Read(reader, l_GeomScaling);
			}

			reader.FinishChunk();
		}

		// Return to Root
		scenePath.pop();

		// Add a spot-light in the root - not sure if this is useful
		//{
		//	SpotLightParam param(1.0, 2.0, 3.0);
		//	SceneLight light(String16(L"SpotLight"), SceneLight::LIGHTTYPE_SPOT, &param, 
		//		ColorRGB(0.1, 0.2, 0.3), false);
		//	sceneBuilder->createLight( &light, scenePath, 
		//		SceneTransform( Vector3(1,2,3),Rotation(4,Vector3(5,6,7)), Vector3(8,9,10), 
		//		Rotation(11, Vector3(12,13,14)), true ),
		//		SceneTransform() );
		//}
		
		// Save it to a eoz-file
		itString filename;
		fsFileUtil::LocatorToUnicodeString(i_ConvFile, filename);
		filename.StripExtension();
		filename += itString(".eoz");
		filename += itString::CharType(0); // have to null-terminate it
		String16 output_filename(filename.GetString());
		sceneBuilder->persistToFile(output_filename);
	}
}
