/*****************************************************************************
**	eonTestScene.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "eonTestScene.hpp"

#include "Core/Dbg/dbgLog.hpp"


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
namespace eonTestScene
{
	namespace
	{
		//--------------------------------------------------------------------
		// Convert vector types
		//--------------------------------------------------------------------
		Vector3 gver_cvrt(const maVector3d& i_Vec)
		{
			// apply transformation to coordinates in order to 
			// match the Z-up world in EON
			const float c_Scale = 0.01f;
			return Vector3(i_Vec.m_X*c_Scale, -i_Vec.m_Z*c_Scale, i_Vec.m_Y*c_Scale);
		}
		Vector3 vec3_cvrt(const maVector3d& i_Vec)
		{
			return Vector3(i_Vec.m_X, i_Vec.m_Y, i_Vec.m_Z);
		}
		Vector2 vec2_cvrt(const maVector2d& i_Vec)
		{
			return Vector2(i_Vec.m_X, i_Vec.m_Y);
		}

		//--------------------------------------------------------------------
		// Convert from our fragment data into EON TriMesh data
		//--------------------------------------------------------------------
		void  convert_fragment(const mdlFragInfo& i_Info,
								TriMesh &o_OutputMesh)
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
						   vec2_cvrt);
			o_OutputMesh.vertices().setTextureCoordinates( 0, uvs );
			o_OutputMesh.vertices().setTextureCoordinates( 1, uvs );	// Use same set for light map

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

		void create_cube_frag(mdlFragInfo& o_Info)
		{
			o_Info.m_Vertices.resize(8);
			std::vector<maPoint3d> &coords = o_Info.m_Vertices;
			coords[0].m_X = -1.0f;	coords[0].m_Y = 1.0f;		coords[0].m_Z = 2.0f;
			coords[1].m_X = 1.0f;		coords[1].m_Y = 1.0f;		coords[1].m_Z = 2.0f;
			coords[2].m_X = 1.0f;		coords[2].m_Y = -1.0f;	coords[2].m_Z = 2.0f;
			coords[3].m_X = -1.0f;	coords[3].m_Y = -1.0f;	coords[3].m_Z = 2.0f;
			coords[4].m_X = -1.0f;	coords[4].m_Y = 1.0f;		coords[4].m_Z = -1.0f;
			coords[5].m_X = 1.0f;		coords[5].m_Y = 1.0f;		coords[5].m_Z = -1.0f;
			coords[6].m_X = 1.0f;		coords[6].m_Y = -1.0f;	coords[6].m_Z = -1.0f;
			coords[7].m_X = -1.0f;	coords[7].m_Y = -1.0f;	coords[7].m_Z = -1.0f;

			o_Info.m_Normals.resize(8);
			std::vector<maVector3d> &normals = o_Info.m_Normals;
			float l = float(1.0/sqrt(3.0));
			normals[0].m_X = -l;	normals[0].m_Y = l;	normals[0].m_Z = l;
			normals[1].m_X = l;	normals[1].m_Y = l;	normals[1].m_Z = l;
			normals[2].m_X = l;	normals[2].m_Y = -l;	normals[2].m_Z = l;
			normals[3].m_X = -l;	normals[3].m_Y = -l;	normals[3].m_Z = l;
			normals[4].m_X = -l;	normals[4].m_Y = l;	normals[4].m_Z = -l;
			normals[5].m_X = l;	normals[5].m_Y = l;	normals[5].m_Z = -l;
			normals[6].m_X = l;	normals[6].m_Y = -l;	normals[6].m_Z = -l;
			normals[7].m_X = -l;	normals[7].m_Y = -l;	normals[7].m_Z = -l;

			o_Info.m_UVs.resize(8);
			std::vector<maVector2d> &texcoords = o_Info.m_UVs;
			Float32 tc0 = 0.0f;
			Float32 tc1 = 1.0f;
			texcoords[0].m_X = tc0;	texcoords[0].m_Y = tc1;
			texcoords[1].m_X = tc1;	texcoords[1].m_Y = tc1;
			texcoords[2].m_X = tc1;	texcoords[2].m_Y = tc0;
			texcoords[3].m_X = tc0;	texcoords[3].m_Y = tc0;
			texcoords[4].m_X = tc1;	texcoords[4].m_Y = tc0;
			texcoords[5].m_X = tc0;	texcoords[5].m_Y = tc0;
			texcoords[6].m_X = tc0;	texcoords[6].m_Y = tc1;
			texcoords[7].m_X = tc1;	texcoords[7].m_Y = tc1;


			o_Info.m_Indices.resize(36);
			std::vector<envType::UInt32> &faces = o_Info.m_Indices;
			EON::UInt32 fi=0;
			faces[fi++] = 0;	faces[fi++] = 2;	faces[fi++] = 1;
			faces[fi++] = 0;	faces[fi++] = 3;	faces[fi++] = 2;
			
			// right
			faces[fi++] = 1;	faces[fi++] = 6;	faces[fi++] = 5;
			faces[fi++] = 1;	faces[fi++] = 2;	faces[fi++] = 6;
			
			// back
			faces[fi++] = 3;	faces[fi++] = 6;	faces[fi++] = 2;
			faces[fi++] = 3;	faces[fi++] = 7;	faces[fi++] = 6;
			
			// front (neg y)
			faces[fi++] = 1;	faces[fi++] = 4;	faces[fi++] = 0;
			faces[fi++] = 1;	faces[fi++] = 5;	faces[fi++] = 4;

			// left
			faces[fi++] = 0;	faces[fi++] = 4;	faces[fi++] = 7;
			faces[fi++] = 0;	faces[fi++] = 7;	faces[fi++] = 3;

			// bottom
			faces[fi++] = 4;	faces[fi++] = 5;	faces[fi++] = 6;
			faces[fi++] = 4;	faces[fi++] = 6;	faces[fi++] = 7;

		}

		//--------------------------------------------------------------------
		// Create a cube. Normals is average of three sides, i.e. the cube is an 
		// approximation of a sphere.
		//--------------------------------------------------------------------
		void createCube(TriMesh &cube)
		{
			// Cube coordinates
			const EON::UInt32 nc = 8;
			std::vector<Vector3> coords(nc);
			coords[0].x = -1.0f;	coords[0].y = 1.0f;		coords[0].z = 2.0f;
			coords[1].x = 1.0f;		coords[1].y = 1.0f;		coords[1].z = 2.0f;
			coords[2].x = 1.0f;		coords[2].y = -1.0f;	coords[2].z = 2.0f;
			coords[3].x = -1.0f;	coords[3].y = -1.0f;	coords[3].z = 2.0f;
			coords[4].x = -1.0f;	coords[4].y = 1.0f;		coords[4].z = -1.0f;
			coords[5].x = 1.0f;		coords[5].y = 1.0f;		coords[5].z = -1.0f;
			coords[6].x = 1.0f;		coords[6].y = -1.0f;	coords[6].z = -1.0f;
			coords[7].x = -1.0f;	coords[7].y = -1.0f;	coords[7].z = -1.0f;
			cube.vertices().setCoordinates(coords);

			std::vector<Vector3> normals(nc);
			float l = float(1.0/sqrt(3.0));
			normals[0].x = -l;	normals[0].y = l;	normals[0].z = l;
			normals[1].x = l;	normals[1].y = l;	normals[1].z = l;
			normals[2].x = l;	normals[2].y = -l;	normals[2].z = l;
			normals[3].x = -l;	normals[3].y = -l;	normals[3].z = l;
			normals[4].x = -l;	normals[4].y = l;	normals[4].z = -l;
			normals[5].x = l;	normals[5].y = l;	normals[5].z = -l;
			normals[6].x = l;	normals[6].y = -l;	normals[6].z = -l;
			normals[7].x = -l;	normals[7].y = -l;	normals[7].z = -l;
			cube.vertices().setNormals(normals);


			std::vector<Vector2> texcoords(nc);
			Float32 tc0 = 0.0f;
			Float32 tc1 = 1.0f;
			texcoords[0].x = tc0;	texcoords[0].y = tc1;
			texcoords[1].x = tc1;	texcoords[1].y = tc1;
			texcoords[2].x = tc1;	texcoords[2].y = tc0;
			texcoords[3].x = tc0;	texcoords[3].y = tc0;
			texcoords[4].x = tc1;	texcoords[4].y = tc0;
			texcoords[5].x = tc0;	texcoords[5].y = tc0;
			texcoords[6].x = tc0;	texcoords[6].y = tc1;
			texcoords[7].x = tc1;	texcoords[7].y = tc1;
			cube.vertices().setTextureCoordinates(0, texcoords);
			
			const EON::UInt32 ntri = 4*2;
			Faces::FaceData fd(Faces::FaceData::POLYGONS);
			std::vector<Int32> faces(37);
			EON::UInt32 fi=0;
			faces[fi++] = 3;	

			// right handed system
			// Polygons is CW.
			//
			//           z
			//           |       y
			//                  /
			//         3----2  /
			//        /    /|  
			//      /     / |  
			//    0|-----1  |
			//     |     |  |6
			//     |       /  ---> x
			//     |     |/
			//     4-----5
			//
			
			// top
			faces[fi++] = 0;	faces[fi++] = 2;	faces[fi++] = 1;
			faces[fi++] = 0;	faces[fi++] = 3;	faces[fi++] = 2;
			
			// right
			faces[fi++] = 1;	faces[fi++] = 6;	faces[fi++] = 5;
			faces[fi++] = 1;	faces[fi++] = 2;	faces[fi++] = 6;
			
			// back
			faces[fi++] = 3;	faces[fi++] = 6;	faces[fi++] = 2;
			faces[fi++] = 3;	faces[fi++] = 7;	faces[fi++] = 6;
			
			// front (neg y)
			faces[fi++] = 1;	faces[fi++] = 4;	faces[fi++] = 0;
			faces[fi++] = 1;	faces[fi++] = 5;	faces[fi++] = 4;

			// left
			faces[fi++] = 0;	faces[fi++] = 4;	faces[fi++] = 7;
			faces[fi++] = 0;	faces[fi++] = 7;	faces[fi++] = 3;

			// bottom
			faces[fi++] = 4;	faces[fi++] = 5;	faces[fi++] = 6;
			faces[fi++] = 4;	faces[fi++] = 6;	faces[fi++] = 7;

			fd.setCoordinateIndices(faces);
			
			
			cube.faces().addFaceData(fd);
			// 
			//return cube;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void buildScene(ISceneBuilder* sceneBuilder,
						const TriMesh& mesh)
		{
			// Create a mesh
			//TriMesh mesh = createCube();
			//{
			//	mesh.setName("CubeMesh");
			//	sceneBuilder->createGeometry(&mesh);
			//}

			SceneHierarchy scenePath;

			// Create a frame with the name "DummyFrame" in the root of the simulation, just for testing.
			sceneBuilder->createDummyObject("DummyFrame", scenePath, SceneTransform(), SceneTransform());

			// Create a texture
			SceneTexture2D texture("WoodTexture");
			{
				texture.setImageSourceFilePath("Wood.jpg");
				//texture.setImageSourceFilePath("Floor_Color.dds");
				//texture.setMipMap(false);
				sceneBuilder->createTexture( &texture );
			}
			SceneTexture2D ltmap("WoodLights");
			{
				//ltmap.setImageSourceFilePath("Floor_Color_baked.dds");
				//ltmap.setImageSourceFilePath("Floor_Color_opaque_baked.dds");
				//ltmap.setImageSourceFilePath("Lightmap.jpg");
				ltmap.setImageSourceFilePath("Lightmap-Floor.jpg");
				ltmap.setMipMap(false);
				sceneBuilder->createTexture( &ltmap );
			}

			// Create a material
			//SceneMaterial material("SimpleMaterial");
			//{
			//	material.setDiffuseTexture(texture.getName());
			//	material.setLightmapTexture(ltmap.getName());
			//	material.setDiffuse(ColorRGB(1.0, 0, 0));
			//	material.setAmbient(ColorRGB(0.5, 0.5, 0.5));
			//	material.setSpecular(ColorRGB(0.5, 0.5, 0.5));
			//	material.setEmissive(ColorRGB(0.5, 0.5, 0.5));
			//	material.setOpacity(1.0f);
			//	sceneBuilder->createMaterial(&material);
			//}			
			SceneMaterialAdvanced material("SimpleMaterial");
			{
				material.setDiffuseTexture(texture.getName());
				material.setDarkmapTexture(ltmap.getName());
				material.setDarkmapUVSet(1);
				material.setDiffuse(ColorRGB(1.0, 0, 0));
				material.setAmbient(ColorRGB(0.5, 0.5, 0.5));
				material.setSpecular(ColorRGB(0.5, 0.5, 0.5));
				material.setEmissive(ColorRGB(0.5, 0.5, 0.5));
				material.setOpacity(1.0f);
				sceneBuilder->createMaterial(&material);
			}

			// Change current path path to "Objects"
			scenePath.push("Objects");

			// Create the cube obejct
			sceneBuilder->createObject("Cube", mesh.getName(), material.getName(), scenePath);

			// Change current path path to root again
			scenePath.pop();

			// Add a spot-light in the root
			{
				SpotLightParam param(1.0, 2.0, 3.0);
				SceneLight light(String16(L"SpotLight"), SceneLight::LIGHTTYPE_SPOT, &param, 
					ColorRGB(0.1, 0.2, 0.3), false);
				sceneBuilder->createLight( &light, scenePath, 
					SceneTransform( Vector3(1,2,3),Rotation(4,Vector3(5,6,7)), Vector3(8,9,10), 
					Rotation(11, Vector3(12,13,14)), true ),
					SceneTransform() );
			}
		}

	}

	//------------------------------------------------------------------------
	//  DoEONTest() - write sample EON file.
	//------------------------------------------------------------------------
	void  DoEONTest()
	{
		// Create the SceneBuilder
		ISceneBuilder* sceneBuilder = COMSceneBuilderFactory::CreateInstance("SimpleTemplate.eoz");
		
		TriMesh mesh;
		createCube(mesh);
		{
			mesh.setName("CubeMesh");
			sceneBuilder->createGeometry(&mesh);
		}

		// Build a simple scene with some stuff
		buildScene(sceneBuilder, mesh);

		// Save it to a eoz-file
		sceneBuilder->persistToFile("./data/SceneBuilderGeneratedSample.eoz");

		// Dispose the SceneBuilder
		EON::COMSceneBuilderFactory::DisposeInstance(sceneBuilder);
	
	}

	//------------------------------------------------------------------------
	//  DoFragTest() - write sample EON file using one of our fragments
	//------------------------------------------------------------------------
	void  DoFragTest(const mdlFragInfo& i_Info)
	{
		// Create the SceneBuilder
		ISceneBuilder* sceneBuilder = COMSceneBuilderFactory::CreateInstance("SimpleTemplate.eoz");

		//mdlFragInfo cube_info;
		//create_cube_frag(cube_info);

		TriMesh eon_mesh;
		convert_fragment(i_Info, eon_mesh);
		//convert_fragment(cube_info, eon_mesh);
		{
			eon_mesh.setName("CubeMesh");
			sceneBuilder->createGeometry(&eon_mesh);
		}

		// Build a simple scene with some stuff
		buildScene(sceneBuilder, eon_mesh);

		// Save it to a eoz-file
		sceneBuilder->persistToFile("./data/frag_test.eoz");

		// Dispose the SceneBuilder
		EON::COMSceneBuilderFactory::DisposeInstance(sceneBuilder);
	
	}

	//------------------------------------------------------------------------
	//  DoFragTest() - write sample EON file using a set of fragments
	//------------------------------------------------------------------------
	void  DoFragTest(const std::vector< shared_ptr<mdlFragInfo> >& i_Infos)
	{
		// Create the SceneBuilder
		ISceneBuilder* sceneBuilder = COMSceneBuilderFactory::CreateInstance("SimpleTemplate.eoz");

		SceneHierarchy scenePath;

		// Create a frame with the name "DummyFrame" in the root of the simulation, just for testing.
		sceneBuilder->createDummyObject("DummyFrame", scenePath, SceneTransform(), SceneTransform());

		// Create a texture
		SceneTexture2D texture("WoodTexture");
		{
			texture.setImageSourceFilePath("Wood.jpg");
			texture.setMipMap(false);
			sceneBuilder->createTexture( &texture );
		}

		// Create a material
		SceneMaterial material("SimpleMaterial");
		{
			material.setDiffuseTexture(texture.getName());
			material.setDiffuse(ColorRGB(1.0, 0, 0));
			material.setAmbient(ColorRGB(0.5, 0.5, 0.5));
			material.setSpecular(ColorRGB(0.5, 0.5, 0.5));
			material.setEmissive(ColorRGB(0.5, 0.5, 0.5));
			material.setOpacity(1.0f);
			sceneBuilder->createMaterial(&material);
		}

		// Change current path path to "Objects"
		scenePath.push("Objects");

		for (int i=0; i<i_Infos.size(); ++i)
		{
			scenePath.push(i_Infos[i]->m_Name);

			TriMesh eon_mesh;
			convert_fragment(*i_Infos[i], eon_mesh);
			{
				eon_mesh.setName(i_Infos[i]->m_Name);
				sceneBuilder->createGeometry(&eon_mesh);
			}

			// Create the cube object
			sceneBuilder->createObject(i_Infos[i]->m_Name, eon_mesh.getName(), material.getName(), scenePath);
		
			scenePath.pop();
		}

		// Change current path path to root again
		scenePath.pop();

		// Add a spot-light in the root
		{
			SpotLightParam param(1.0, 2.0, 3.0);
			SceneLight light(String16(L"SpotLight"), SceneLight::LIGHTTYPE_SPOT, &param, 
				ColorRGB(0.1, 0.2, 0.3), false);
			sceneBuilder->createLight( &light, scenePath, 
				SceneTransform( Vector3(1,2,3),Rotation(4,Vector3(5,6,7)), Vector3(8,9,10), 
				Rotation(11, Vector3(12,13,14)), true ),
				SceneTransform() );
		}

		// Save it to a eoz-file
		sceneBuilder->persistToFile("./data/frags_test.eoz");

		// Dispose the SceneBuilder
		EON::COMSceneBuilderFactory::DisposeInstance(sceneBuilder);
	}
}
