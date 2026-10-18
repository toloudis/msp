/*****************************************************************************
**  g3dStubsVk.cpp
**
**      See g3dStubsVk.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_G3DSTUBSVK_HPP
#include "GraphicsVulkan/stub/g3dStubsVk.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif
#ifndef G3D_INDEXPTR_HPP
#include "Graphics/G3d/g3dIndexPtr.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif

namespace gvk
{
	//------------------------------------------------------------------------
	//	Renderers
	//------------------------------------------------------------------------
	int g3dSceneRendererVk::Render(g2dRenderTarget*, const camCamera&, const g3dScene&, const std::vector<g3dLayer*>&, float)
	{
		GVK_NOT_YET(m_pType);
		return 0;
	}

	void g3dSceneRendererVk::ReleaseResources()
	{
	}

	int g3dPickRendererVk::Render(g2dRenderTarget*, const camCamera&, const g3dScene&, const std::vector<g3dLayer*>&, float)
	{
		GVK_NOT_YET("g3dPickRendererVk::Render");
		return 0;
	}

	void g3dPickRendererVk::ReleaseResources()					{}
	void g3dPickRendererVk::SetPickViewport(g2dRenderTarget*)	{ GVK_NOT_YET("g3dPickRendererVk::SetPickViewport"); }
	void g3dPickRendererVk::RestoreViewport()					{}
	void g3dPickRendererVk::GetPickInfo(g2dRenderTarget*, g3dPickInfo&)	{ GVK_NOT_YET("g3dPickRendererVk::GetPickInfo"); }

	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateDefaultRenderer()		{ return new g3dSceneRendererVk("default renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateHDRRenderer()			{ return new g3dSceneRendererVk("HDR renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateSimpleRenderer()		{ return new g3dSceneRendererVk("simple renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateTextureRenderer()		{ return new g3dSceneRendererVk("texture renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateDepthMapRenderer()	{ return new g3dSceneRendererVk("depth map renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateRSMRenderer()			{ return new g3dSceneRendererVk("RSM renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateOpacityMapRenderer(g3dProjectedLight*)	{ return new g3dSceneRendererVk("opacity map renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateCubeMapRenderer(scObject*, matMaterial*, bool)	{ return new g3dSceneRendererVk("cube map renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreatePlanarReflectionRenderer(scObject*, matMaterial*)	{ return new g3dSceneRendererVk("planar reflection renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateAmbientOcclusionRenderer()	{ return new g3dSceneRendererVk("ambient occlusion renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateGlobalIlluminationRenderer()	{ return new g3dSceneRendererVk("global illumination renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateDepthRenderer()		{ return new g3dSceneRendererVk("depth renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateUVRenderer()			{ return new g3dSceneRendererVk("UV renderer"); }
	g3dPickRenderer* g3dSceneRendererCreateVk::CreatePickRenderer()			{ return new g3dPickRendererVk; }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateShadowMaskRenderer()	{ return new g3dSceneRendererVk("shadow mask renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateIlluminationRenderer()	{ return new g3dSceneRendererVk("illumination renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateNormalRenderer()		{ return new g3dSceneRendererVk("normal renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateVelocityMapRenderer()	{ return new g3dSceneRendererVk("velocity map renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateMaterialsRenderer()	{ return new g3dSceneRendererVk("materials renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateReflectionOnlyRenderer()	{ return new g3dSceneRendererVk("reflection-only renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateGlowRenderer()		{ return new g3dSceneRendererVk("glow renderer"); }

	//------------------------------------------------------------------------
	//	Baking
	//------------------------------------------------------------------------
	int g3dBakeVk::Init(const g2dPFD&, const g3dScene*, const fsLocator&, float, const camCamera&, bool,
						const std::string&, int, int)
	{
		GVK_NOT_YET("g3dBakeVk::Init");
		return 0;
	}

	int g3dBakeVk::CleanUp()													{ return 0; }
	bool g3dBakeVk::BakeNextNode(std::map<const g3dFragment*, std::string>&)	{ return false; }		// nothing (more) to bake
	void g3dBakeVk::SetOutputDirectory(const fsLocator&)						{}

	//------------------------------------------------------------------------
	//	g3dFragmentVk
	//------------------------------------------------------------------------
	g3dFragmentVk::g3dFragmentVk(const maPoint3d* i_Vertices, const maPoint3d* i_Normals, const maPoint2d* i_UVs,
								 int i_nVertices, const g3dIndexPtr* i_pIndices, int i_nIndices,
								 matMaterial* i_pMaterial, bool i_bMorphable)
		: g3dFragment(i_pMaterial, 0, i_bMorphable)
		, m_bTextured(i_UVs != NULL)
	{
		for (int i = 0; i < i_nVertices; ++i)
		{
			const maPoint3d position = i_Vertices ? i_Vertices[i] : maPoint3d(0, 0, 0);
			const maPoint3d normal = i_Normals ? i_Normals[i] : maPoint3d(0, 0, 1);
			if (m_bTextured)
			{
				g3dType::Tex1Vertex vertex;
				vertex.m_Vertex = position;
				vertex.m_Normal = normal;
				vertex.m_TexCoord = i_UVs[i];
				m_Textured.push_back(vertex);
			}
			else
			{
				g3dType::NonTexVertex vertex;
				vertex.m_Vertex = position;
				vertex.m_Normal = normal;
				m_Plain.push_back(vertex);
			}
		}
		for (int i = 0; i_pIndices && i < i_nIndices; ++i)
		{
			m_Indices.push_back(i_pIndices->GetIndex(i));
		}
		update_bounds();
	}

	g3dFragmentVk::g3dFragmentVk(const g3dFragmentVk& i_Other)
		: g3dFragment(i_Other)
		, m_bTextured(i_Other.m_bTextured)
		, m_Plain(i_Other.m_Plain)
		, m_Textured(i_Other.m_Textured)
		, m_Indices(i_Other.m_Indices)
	{
	}

	g3dFragmentVk::~g3dFragmentVk()
	{
	}

	void g3dFragmentVk::update_bounds()
	{
		maAxisBox box;
		bool first = true;
		const int count = GetNumVertices();
		for (int i = 0; i < count; ++i)
		{
			const maPoint3d& p = m_bTextured ? m_Textured[i].m_Vertex : m_Plain[i].m_Vertex;
			if (first)
			{
				box = maAxisBox(p, p);
				first = false;
			}
			else
			{
				box.Union(p);
			}
		}
		SetBoundingBox(box);
	}

	void g3dFragmentVk::Deallocate()	{}
	void g3dFragmentVk::Reallocate()	{}

	void g3dFragmentVk::UpdateVertices(int i_NumVertices, const maPoint3d* i_pVertices, const maVector3d* i_pNormals)
	{
		const int count = std::min(i_NumVertices, GetNumVertices());
		for (int i = 0; i < count; ++i)
		{
			maPoint3d& position = m_bTextured ? m_Textured[i].m_Vertex : m_Plain[i].m_Vertex;
			maPoint3d& normal = m_bTextured ? m_Textured[i].m_Normal : m_Plain[i].m_Normal;
			if (i_pVertices)
			{
				position = i_pVertices[i];
			}
			if (i_pNormals)
			{
				normal = maPoint3d(i_pNormals[i].GetX(), i_pNormals[i].GetY(), i_pNormals[i].GetZ());
			}
		}
		update_bounds();
	}

	int g3dFragmentVk::GetNumVertices() const
	{
		return static_cast<int>(m_bTextured ? m_Textured.size() : m_Plain.size());
	}

	int g3dFragmentVk::GetNumIndices() const
	{
		return static_cast<int>(m_Indices.size());
	}

	g3dType::VertexFormat g3dFragmentVk::GetVertexFormat() const
	{
		return m_bTextured ? g3dType::e_Tex1Vertex : g3dType::e_NonTexVertex;
	}

	unsigned char* g3dFragmentVk::Lock()
	{
		return m_bTextured ? reinterpret_cast<unsigned char*>(m_Textured.data())
						   : reinterpret_cast<unsigned char*>(m_Plain.data());
	}

	void g3dFragmentVk::Unlock()								{ update_bounds(); }
	unsigned char* g3dFragmentVk::LockIndices()					{ return reinterpret_cast<unsigned char*>(m_Indices.data()); }
	void g3dFragmentVk::UnlockIndices()							{}
	unsigned char* g3dFragmentVk::ReadOnlyLock()				{ return Lock(); }
	void g3dFragmentVk::ReadOnlyUnlock()						{}
	unsigned char* g3dFragmentVk::ReadOnlyLockIndices()			{ return LockIndices(); }
	void g3dFragmentVk::ReadOnlyUnlockIndices()					{}
	void g3dFragmentVk::ComponentSort(const maMatrix4x4&, const maPoint3d&)	{ GVK_NOT_YET("g3dFragmentVk::ComponentSort"); }

	//------------------------------------------------------------------------
	//	g3dFragmentCreateVk
	//------------------------------------------------------------------------
	g3dFragment* g3dFragmentCreateVk::CreateFragment(const maPoint3d* i_Vertices, const maPoint3d* i_Normals, int i_nVertices,
													 const g3dIndexPtr i_Indices, int i_nIndices, matMaterial* i_pMaterial,
													 bool i_bMorphable)
	{
		return new g3dFragmentVk(i_Vertices, i_Normals, NULL, i_nVertices, &i_Indices, i_nIndices, i_pMaterial, i_bMorphable);
	}

	g3dFragment* g3dFragmentCreateVk::CreateFragment(const maPoint3d* i_Vertices, const maPoint3d* i_Normals, const maPoint2d* i_UVs,
													 int i_nVertices, const g3dIndexPtr i_Indices, int i_nIndices,
													 matMaterial* i_pMaterial, bool i_bMorphable)
	{
		return new g3dFragmentVk(i_Vertices, i_Normals, i_UVs, i_nVertices, &i_Indices, i_nIndices, i_pMaterial, i_bMorphable);
	}

	g3dFragment* g3dFragmentCreateVk::CreateLineList(const maPoint3d* i_Vertices, const maPoint3d* i_Normals, int i_nVertices,
													 const g3dIndexPtr i_Indices, int i_nIndices, matMaterial* i_pMaterial,
													 bool i_bMorphable, const maPoint2d*, const maPoint3d*, const maPoint3d*)
	{
		return new g3dFragmentVk(i_Vertices, i_Normals, NULL, i_nVertices, &i_Indices, i_nIndices, i_pMaterial, i_bMorphable);
	}

	g3dFragment* g3dFragmentCreateVk::CloneFragment(const g3dFragment* i_pFragment)
	{
		const g3dFragmentVk* fragment = dynamic_cast<const g3dFragmentVk*>(i_pFragment);
		return fragment ? new g3dFragmentVk(*fragment) : NULL;
	}

	void g3dFragmentCreateVk::SetComputeBasisVectors(bool)		{}
	void g3dFragmentCreateVk::StoreGeometryInVideoMemory(bool)	{}

	//------------------------------------------------------------------------
	//	g3dVertexBufferVk, mdlFragCreateVk - model import isn't ported yet
	//------------------------------------------------------------------------
	void g3dVertexBufferVk::UpdateVertices(int, const maPoint3d*, const maVector3d*)	{ GVK_NOT_YET("g3dVertexBufferVk::UpdateVertices"); }
	int g3dVertexBufferVk::GetNumVertices() const					{ return 0; }
	g3dType::VertexFormat g3dVertexBufferVk::GetVertexFormat() const	{ return g3dType::e_NonTexVertex; }
	unsigned char* g3dVertexBufferVk::Lock()						{ return NULL; }
	void g3dVertexBufferVk::Unlock()								{}
	void g3dVertexBufferVk::SetBoundingBox(const maAxisBox&)		{}

	bool mdlFragCreateVk::UseBumpFrag(int, bool)	{ return true; }

	g3dFragment* mdlFragCreateVk::CreateFragment(const mdlSplitFragInfo&, bool i_Morphable)
	{
		GVK_NOT_YET("mdlFragCreateVk::CreateFragment");
		return new g3dFragmentVk(NULL, NULL, NULL, 0, NULL, 0, NULL, i_Morphable);
	}

	g3dFragment* mdlFragCreateVk::CreateHairFragment(const mdlHairInfo&, matMaterial* i_pMaterial)
	{
		GVK_NOT_YET("mdlFragCreateVk::CreateHairFragment");
		return new g3dFragmentVk(NULL, NULL, NULL, 0, NULL, 0, i_pMaterial, false);
	}

	void mdlFragCreateVk::OptimizeFragment(mdlSplitFragInfo&)	{}

	g3dVertexBuffer* mdlFragCreateVk::CreateFragmentGroup(const mdlFragInfo&, std::vector<g3dFragment*>&, bool)
	{
		GVK_NOT_YET("mdlFragCreateVk::CreateFragmentGroup");
		return new g3dVertexBufferVk;
	}

	void mdlFragCreateVk::UpdateFragmentGroup(const mdlFragInfo&, g3dVertexBuffer&, std::vector<g3dFragment*>&, bool)
	{
		GVK_NOT_YET("mdlFragCreateVk::UpdateFragmentGroup");
	}
}
