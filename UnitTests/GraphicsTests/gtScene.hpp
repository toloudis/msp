/*****************************************************************************
**  gtScene.hpp
**
**      Synthetic scene pieces for the backend parity tests, built through
**      the Graphics API only, so the same code runs under either backend
**      (gtBackend): meshes (a UV sphere, a plane, a large grid) made into
**      fragments with g3dFragmentCreate; materials on material shaders
**      (Blinn.fx, ...) and on built-in shaders with effect data (Phong.fx,
**      Solid.fx, ...); textures; and gtSceneBuilder, a one-layer g3dScene
**      with its nodes, lights and camera.
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

#ifndef MA_POINT2D_HPP
#include "Core/Ma/maPoint2d.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/Ma/maPoint3d.hpp"
#endif
#ifndef MA_MATRIX4X4_HPP
#include "Core/Ma/maMatrix4x4.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/Cam/camCamera.hpp"
#endif

#include <cstdint>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <vector>

class effShaderData;
class g3dFragment;
class g3dLayer;
class g3dLight;
class g3dScene;
class g3dSceneNode;
class matMaterial;
class matTexture;

//	A triangle mesh: positions, normals, UVs, and a triangle list
struct gtMesh
{
	std::vector<maPoint3d>	m_Positions;
	std::vector<maPoint3d>	m_Normals;
	std::vector<maPoint2d>	m_UVs;
	std::vector<uint32_t>	m_Indices;

	int VertexCount() const		{ return (int)m_Positions.size(); }
	int IndexCount() const		{ return (int)m_Indices.size(); }

	//	A fragment through g3dFragmentCreate (the current backend's), with
	//	UVs; 16-bit indices are passed when they fit, as the importers do
	g3dFragment* CreateFragment(matMaterial* i_pMaterial = NULL, bool i_bMorphable = false) const;
};

namespace gtScene
{
	//	Unit sphere at the origin: i_Rings bands from pole to pole,
	//	i_Segments around; (i_Rings + 1) * (i_Segments + 1) vertices
	gtMesh Sphere(int i_Rings, int i_Segments);

	//	The square -1..1 in X and Y at Z = 0, facing +Z, UVs 0..1
	gtMesh Plane();

	//	The square -1..1 in X and Y at Z = 0, facing +Z, cut into i_N x i_N
	//	quads: (i_N + 1)^2 vertices (Grid(300) needs 32-bit indices)
	gtMesh Grid(int i_N);

	//	The values a material's parameters are given
	enum Values
	{
		e_NonDefault,		// every float and colour moved off its default
		e_Zero,				// floats 0 (zero specular power), colours transparent black
		e_Huge,				// floats 1e6 (very large specular power), colours white
	};

	//	A material on a material shader ("Blinn.fx"): its parameters built by
	//	the effect (BuildPrtyObject), then set to i_Values; i_Textures go to
	//	its texture parameters in order (missing ones stay empty)
	matMaterial* ShaderMaterial(const std::string& i_Shader, Values i_Values,
								const std::vector<matTexture*>& i_Textures = std::vector<matTexture*>());

	//	A material on a built-in shader with effect data ("Phong.fx",
	//	"Solid.fx", "default"); i_Setup edits the data before the material is
	//	made (so its transparency is seen)
	matMaterial* DataMaterial(const std::string& i_Shader,
							  const std::function<void(effShaderData*)>& i_Setup = std::function<void(effShaderData*)>());

	//	effPhongData set to i_Values (colours, specular power, transparency)
	void SetPhong(effShaderData* io_pData, Values i_Values);

	//	An RGBA8 i_Width x i_Height texture of i_Cells x i_Cells coloured
	//	checks (a PNG written to i_Dir, loaded through matTextureMgr, mips
	//	generated). Release with matTextureMgr::ReleaseTexture.
	matTexture* CheckerTexture(const std::filesystem::path& i_Dir, int i_Width, int i_Height, int i_Cells);

	//	A translation and uniform scale
	maMatrix4x4 Place(float i_X, float i_Y, float i_Z, float i_Scale = 1.0f);
}

//	A one-layer scene for the scene renderers, with what it owns: nodes and
//	their fragments, materials, textures and lights (destroyed with it, so
//	make it inside gtBackend::Run). The camera looks at the origin from +Z.
class gtSceneBuilder
{
	public:
		gtSceneBuilder();
		~gtSceneBuilder();

		//	Takes ownership
		matMaterial*	Keep(matMaterial* i_pMaterial);
		matTexture*		Keep(matTexture* i_pTexture);

		//	A node with i_Mesh's fragment, placed by i_World (its world box
		//	set as the scene manager would)
		g3dSceneNode*	Add(const gtMesh& i_Mesh, matMaterial* i_pMaterial, const maMatrix4x4& i_World);

		void			PointLight(const maPoint3d& i_Position, const maFloatRGBA& i_Color, float i_Range = 50.0f);
		void			DirectionalLight(const maVector3d& i_Direction, const maFloatRGBA& i_Color);

		g3dScene&		Scene()		{ return *m_pScene; }
		g3dLayer&		Layer()		{ return *m_pLayer; }
		camCamera&		Camera()	{ return m_Camera; }

	private:
		gtSceneBuilder(const gtSceneBuilder&) = delete;
		gtSceneBuilder& operator = (const gtSceneBuilder&) = delete;

		camCamera						m_Camera;
		std::unique_ptr<g3dSceneNode>	m_pRoot;			// owns the nodes Add makes
		g3dLayer*						m_pLayer;			// owned by the scene
		std::unique_ptr<g3dScene>		m_pScene;
		std::vector<std::unique_ptr<g3dFragment>>	m_Fragments;
		std::vector<std::unique_ptr<matMaterial>>	m_Materials;
		std::vector<matTexture*>		m_Textures;
		std::vector<g3dLight*>			m_Lights;
};
