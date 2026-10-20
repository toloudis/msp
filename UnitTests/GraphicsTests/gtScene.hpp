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
class g3dProjectedLight;
class g3dScene;
class g3dSceneNode;
class g3dSceneRenderer;
class g3dTargetRenderer;
class matMaterial;
class matTexture;
class scObject;
struct g3dAmbientEnvState;
struct g3dRenderState;

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
		e_Defaults,			// the effect's own values (ShaderMaterial only)
		e_Tinted,			// floats the effect's own, colours moderate tints
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

	//	Sets a material shader material's parameter named i_pName (its HLSL
	//	name); a test failure if it has none
	void SetFloat(matMaterial* io_pMaterial, const char* i_pName, float i_Value);
	void SetColor(matMaterial* io_pMaterial, const char* i_pName, const maFloatRGBA& i_Value);

	//	An RGBA8 i_Width x i_Height texture of i_Cells x i_Cells coloured
	//	checks (a PNG written to i_Dir, loaded through matTextureMgr, mips
	//	generated). i_Blur > 0 softens the edges with a box filter of that
	//	radius in texels: anisotropic filtering of a hard edge is the
	//	driver's own and differs between D3D11 and Vulkan on the same GPU
	//	(up to ~35/255 at the edges). Release with matTextureMgr::ReleaseTexture.
	matTexture* CheckerTexture(const std::filesystem::path& i_Dir, int i_Width, int i_Height, int i_Cells, int i_Blur = 0);

	//	An RGBA8 i_Size x i_Size height map of i_Bumps x i_Bumps smooth bumps
	//	(grey, 0 to 255), and the matching tangent-space normal map (RGB the
	//	normal packed into 0..255, flat is (128, 128, 255)): a displacement
	//	map and a normal map without hard edges (PNGs written to i_Dir, loaded
	//	through matTextureMgr, mips generated). Release with
	//	matTextureMgr::ReleaseTexture.
	matTexture* HeightTexture(const std::filesystem::path& i_Dir, int i_Size, int i_Bumps);
	matTexture* NormalTexture(const std::filesystem::path& i_Dir, int i_Size, int i_Bumps);

	//	An i_Width x 1 RGBA32F ramp without mips, as MachStudio's ramps are
	//	(rmpTextureMgr), smooth from left to right (dark blue, red-orange,
	//	pale yellow): a gradient map for lookups by a computed coordinate,
	//	where a checker's hard edges would turn last-bit differences into
	//	large ones. Release with matTextureMgr::ReleaseTexture.
	matTexture* RampTexture(int i_Width);

	//	A lat-long i_Width x i_Height environment: a sky graded from blue at
	//	the top to a dark ground at the bottom, and a sun spot of value
	//	i_Peak above the horizon, in the builder camera's view (an RGBE .hdr
	//	written to i_Dir, loaded through matTextureMgr, mips generated; make
	//	the size a power of two, as DX11 then makes the mips with
	//	DirectXTex's box filter and Vulkan matches it). Release with
	//	matTextureMgr::ReleaseTexture.
	matTexture* HdrTexture(const std::filesystem::path& i_Dir, int i_Width, int i_Height, float i_Peak);

	//	A translation and uniform scale
	maMatrix4x4 Place(float i_X, float i_Y, float i_Z, float i_Scale = 1.0f);

	//	An opaque Phong data material (SetPhong's values, transparency 1)
	matMaterial* OpaquePhong(const std::string& i_Shader, Values i_Values);
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

		//	A node in a second (overlay) layer: camera space, z-buffered,
		//	as MachStudio's camera layer (api3dSceneImpl); made on first use
		g3dSceneNode*	AddLayer(const gtMesh& i_Mesh, matMaterial* i_pMaterial, const maMatrix4x4& i_Camera);

		//	A node in a viewer layer: a layer that isn't the scene's but is
		//	passed to the renderer's Render (ViewerLayers), camera space and
		//	z-buffered; made on first use
		g3dSceneNode*	AddViewerLayer(const gtMesh& i_Mesh, matMaterial* i_pMaterial, const maMatrix4x4& i_Camera);

		void			PointLight(const maPoint3d& i_Position, const maFloatRGBA& i_Color, float i_Range = 50.0f);
		void			DirectionalLight(const maVector3d& i_Direction, const maFloatRGBA& i_Color);

		//	A spot light at i_Position aimed at i_Target (a 60 degree cone,
		//	range 30), casting shadows. With i_ShadowMapSize > 0 it gets a
		//	shadow map (matTextureMgr::CreateShadowMap), a depth map renderer
		//	(g3dSceneRendererCreate::CreateDepthMapRenderer) and a target
		//	renderer drawing the scene into the map from the light's shadow
		//	camera, as prjltProjectedLightObject sets them up; RenderTargets
		//	renders the map. 0: no shadow map.
		g3dProjectedLight*	ProjectedLight(const maPoint3d& i_Position, const maPoint3d& i_Target,
									   const maFloatRGBA& i_Color, int i_ShadowMapSize);

		//	Makes i_pNode's material reflective, as mtrlScriptObject does:
		//	the material gets a reflection map (a mipped render target for a
		//	planar reflection, a cube render target otherwise; the material's
		//	resolution, 128 by default), and a reflection renderer
		//	(g3dSceneRendererCreate::CreatePlanarReflectionRenderer /
		//	CreateCubeMapRenderer, i_bPerFrame: the cube map is rendered
		//	every frame, else once) with its g3dPlanarReflectionTargetRenderer /
		//	g3dCubicReflectionTargetRenderer from the builder's camera, on a
		//	black background; RenderTargets renders it. Use a material shader
		//	with reflection maps ("BlinnReflection.fx") and give each
		//	reflector its own material.
		void			Reflective(g3dSceneNode* i_pNode, bool i_bPlanar, bool i_bPerFrame);

		//	Renders the builder's target renderers (shadow maps and
		//	reflections, in the order they were made) for this frame, as
		//	api3dTargetRendererMgr does before the view is drawn
		void			RenderTargets(float i_fSimTime = 1.25f);

		//	The usual two lights: a warm point light and a cool directional one
		void			TwoLights();

		//	Whether the lights made from now on cast shadows (as MachStudio's
		//	point and projected lights do: the HDR renderer's lit pass draws
		//	only those); off by default
		void			LightsCastShadows(bool i_bCast)		{ m_bLightsCastShadows = i_bCast; }

		//	Twelve spheres (Plan 4b's): rows Blinn, Phong, Lambert material
		//	shaders and the built-in Phong (left out if !i_bBuiltInPhong: its
		//	data materials count as transparent); columns non-default, zero
		//	and very large values; and TwoLights if i_bLights
		void			LitSpheres(bool i_bLights = true, bool i_bBuiltInPhong = true);

		//	The global environment ("swl", as evmtEnvironment::UpdateToSceneGlobal
		//	sets it): every node lit by i_pDiffuse and i_pSpecular (the root's
		//	environment, which the nodes inherit), and the scene's global
		//	ambient enabled, drawn as the background if i_bBackground. The root
		//	also gets a render state linking the lights made so far, as
		//	MachStudio's objects have (api3dObjectSingle::AddLightToRenderState):
		//	the state traverser records environments only under a render state.
		void			GlobalEnvironment(matTexture* i_pDiffuse, matTexture* i_pSpecular, bool i_bBackground);

		g3dScene&		Scene()		{ return *m_pScene; }
		g3dLayer&		Layer()		{ return *m_pLayer; }
		g3dLayer*		OverlayLayer()	{ return m_pOverlayLayer; }		// NULL until AddLayer
		const std::vector<g3dLayer*>&	ViewerLayers() const	{ return m_ViewerLayers; }
		camCamera&		Camera()	{ return m_Camera; }

	private:
		gtSceneBuilder(const gtSceneBuilder&) = delete;
		gtSceneBuilder& operator = (const gtSceneBuilder&) = delete;

		camCamera						m_Camera;
		std::unique_ptr<g3dSceneNode>	m_pRoot;			// owns the nodes Add makes
		g3dLayer*						m_pLayer;			// owned by the scene
		std::unique_ptr<g3dSceneNode>	m_pOverlayRoot;		// owns the nodes AddLayer makes
		g3dLayer*						m_pOverlayLayer = NULL;	// owned by the scene
		std::unique_ptr<g3dSceneNode>	m_pViewerRoot;		// owns the nodes AddViewerLayer makes
		std::unique_ptr<g3dScene>		m_pViewerScene;		// owns the viewer layer (and updates its world data)
		std::vector<g3dLayer*>			m_ViewerLayers;
		std::unique_ptr<g3dAmbientEnvState>	m_pEnvironment;	// the root's, from GlobalEnvironment
		std::unique_ptr<g3dRenderState>	m_pRenderState;	// the root's, from GlobalEnvironment
		std::unique_ptr<g3dScene>		m_pScene;
		std::vector<std::unique_ptr<g3dFragment>>	m_Fragments;
		std::vector<std::unique_ptr<matMaterial>>	m_Materials;
		std::vector<matTexture*>		m_Textures;
		std::vector<g3dLight*>			m_Lights;
		std::vector<std::unique_ptr<camCamera>>			m_ShadowCameras;
		std::vector<std::unique_ptr<g3dSceneRenderer>>	m_DepthRenderers;
		std::vector<std::unique_ptr<scObject>>			m_Reflectors;		// the reflection renderers' objects
		std::vector<std::unique_ptr<g3dSceneRenderer>>	m_ReflectionRenderers;
		std::vector<std::unique_ptr<g3dTargetRenderer>>	m_TargetRenderers;
		bool							m_bLightsCastShadows = false;
};
