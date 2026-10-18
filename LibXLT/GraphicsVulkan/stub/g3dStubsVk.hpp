/*****************************************************************************
**  g3dStubsVk.hpp
**
**      Vulkan implementations of the 3D graphics hooks that aren't ported
**	yet (see g2dStubsVk.hpp for the rules):
**
**	  g3dSceneRendererVk, g3dPickRendererVk, g3dSceneRendererCreateVk
**		every renderer type; they draw nothing (the target renderer has
**		already cleared the target), the pick renderer picks nothing
**	  g3dBakeVk				baking (nothing to bake)
**	  g3dFragmentVk, g3dFragmentCreateVk
**		mesh fragments keep their vertices and indices (and bounding box)
**		on the CPU; no GPU buffers yet
**	  g3dVertexBufferVk, mdlFragCreateVk
**		model-import fragment creation: empty fragments for now
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_G3DSTUBSVK_HPP
#error g3dStubsVk.hpp multiply included
#endif
#define GVK_G3DSTUBSVK_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/G3d/g3dSceneRenderer.hpp"
#endif
#ifndef G3D_SCENERENDERERCREATE_HPP
#include "Graphics/G3d/g3dSceneRendererCreate.hpp"
#endif
#ifndef G3D_BAKE_HPP
#include "Graphics/G3d/g3dBake.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_FRAGMENTCREATE_HPP
#include "Graphics/G3d/g3dFragmentCreate.hpp"
#endif
#ifndef G3D_VERTEXBUFFER_HPP
#include "Graphics/G3d/g3dVertexBuffer.hpp"
#endif
#ifndef G3D_TYPE_HPP
#include "Graphics/G3d/g3dType.hpp"
#endif
#ifndef MDL_FRAGCREATE_HPP
#include "Graphics/mdl/mdlFragCreate.hpp"
#endif

#include <string>
#include <vector>

namespace gvk
{
	class g3dSceneRendererVk : public g3dSceneRenderer
	{
		public:
			explicit g3dSceneRendererVk(const char* i_pType) : m_pType(i_pType) {}
			virtual int Render(g2dRenderTarget* i_pWindow, const camCamera& i_Camera, const g3dScene& i_Scene,
							   const std::vector<g3dLayer*>& i_ViewerLayers, float i_fSimTime);
			virtual void ReleaseResources();

		private:
			const char* m_pType;
	};

	class g3dPickRendererVk : public g3dPickRenderer
	{
		public:
			virtual int Render(g2dRenderTarget* i_pWindow, const camCamera& i_Camera, const g3dScene& i_Scene,
							   const std::vector<g3dLayer*>& i_ViewerLayers, float i_fSimTime);
			virtual void ReleaseResources();
			virtual void SetPickViewport(g2dRenderTarget* i_pTarget);
			virtual void RestoreViewport();
			virtual void GetPickInfo(g2dRenderTarget* i_pWindow, g3dPickInfo& o_PickInfo);
	};

	class g3dSceneRendererCreateVk : public g3dSceneRendererCreateImpl
	{
		public:
			virtual g3dSceneRenderer* CreateDefaultRenderer();
			virtual g3dSceneRenderer* CreateHDRRenderer();
			virtual g3dSceneRenderer* CreateSimpleRenderer();
			virtual g3dSceneRenderer* CreateTextureRenderer();
			virtual g3dSceneRenderer* CreateDepthMapRenderer();
			virtual g3dSceneRenderer* CreateRSMRenderer();
			virtual g3dSceneRenderer* CreateOpacityMapRenderer(g3dProjectedLight* pProjLight);
			virtual g3dSceneRenderer* CreateCubeMapRenderer(scObject* i_Obj, matMaterial* i_Material, bool i_RenderPerFrame);
			virtual g3dSceneRenderer* CreatePlanarReflectionRenderer(scObject* i_Obj, matMaterial* i_Material);
			virtual g3dSceneRenderer* CreateAmbientOcclusionRenderer();
			virtual g3dSceneRenderer* CreateGlobalIlluminationRenderer();
			virtual g3dSceneRenderer* CreateDepthRenderer();
			virtual g3dSceneRenderer* CreateUVRenderer();
			virtual g3dPickRenderer* CreatePickRenderer();
			virtual g3dSceneRenderer* CreateShadowMaskRenderer();
			virtual g3dSceneRenderer* CreateIlluminationRenderer();
			virtual g3dSceneRenderer* CreateNormalRenderer();
			virtual g3dSceneRenderer* CreateVelocityMapRenderer();
			virtual g3dSceneRenderer* CreateMaterialsRenderer();
			virtual g3dSceneRenderer* CreateReflectionOnlyRenderer();
			virtual g3dSceneRenderer* CreateGlowRenderer();
	};

	class g3dBakeVk : public g3dBakeImpl
	{
		public:
			virtual int Init(const g2dPFD& i_PFD, const g3dScene* i_pScene, const fsLocator& i_OutputPath, float i_fSimTime,
							 const camCamera& i_Camera, bool i_bIsSaveAndReplace, const std::string& i_OutputFormat,
							 int i_Res, int i_TextureReduce);
			virtual int CleanUp();
			virtual bool BakeNextNode(std::map<const g3dFragment*, std::string>& io_TextureNameMap);
			virtual void SetOutputDirectory(const fsLocator& i_OutputDir);
	};

	class g3dFragmentVk : public g3dFragment
	{
		public:
			//	Positions and normals (any may be NULL), as g3dType::Tex1Vertex when
			//	i_UVs is given, else g3dType::NonTexVertex; indices kept as 32-bit.
			g3dFragmentVk(const maPoint3d* i_Vertices, const maPoint3d* i_Normals, const maPoint2d* i_UVs, int i_nVertices,
						  const g3dIndexPtr* i_pIndices, int i_nIndices, matMaterial* i_pMaterial, bool i_bMorphable);
			g3dFragmentVk(const g3dFragmentVk& i_Other);
			virtual ~g3dFragmentVk();

			virtual void Deallocate();
			virtual void Reallocate();
			virtual void UpdateVertices(int i_NumVertices, const maPoint3d* i_pVertices, const maVector3d* i_pNormals = NULL);
			virtual int GetNumVertices() const;
			virtual int GetNumIndices() const;
			virtual g3dType::VertexFormat GetVertexFormat() const;
			virtual unsigned char* Lock();
			virtual void Unlock();
			virtual unsigned char* LockIndices();
			virtual void UnlockIndices();
			virtual unsigned char* ReadOnlyLock();
			virtual void ReadOnlyUnlock();
			virtual unsigned char* ReadOnlyLockIndices();
			virtual void ReadOnlyUnlockIndices();
			virtual void ComponentSort(const maMatrix4x4& i_Transorm, const maPoint3d& i_CameraPos);

		private:
			void update_bounds();

			bool								m_bTextured;
			std::vector<g3dType::NonTexVertex>	m_Plain;
			std::vector<g3dType::Tex1Vertex>	m_Textured;
			std::vector<uint32_t>				m_Indices;
	};

	class g3dFragmentCreateVk : public g3dFragmentCreateImpl
	{
		public:
			virtual g3dFragment* CreateFragment(const maPoint3d* i_Vertices, const maPoint3d* i_Normals, int i_nVertices,
												const g3dIndexPtr i_Indices, int i_nIndices, matMaterial* i_pMaterial, bool i_bMorphable);
			virtual g3dFragment* CreateFragment(const maPoint3d* i_Vertices, const maPoint3d* i_Normals, const maPoint2d* i_UVs,
												int i_nVertices, const g3dIndexPtr i_Indices, int i_nIndices,
												matMaterial* i_pMaterial, bool i_bMorphable);
			virtual g3dFragment* CreateLineList(const maPoint3d* i_Vertices, const maPoint3d* i_Normals, int i_nVertices,
												const g3dIndexPtr i_Indices, int i_nIndices, matMaterial* i_pMaterial,
												bool i_bMorphable, const maPoint2d* i_UVs, const maPoint3d* i_Ss, const maPoint3d* i_Ts);
			virtual g3dFragment* CloneFragment(const g3dFragment* i_pFragment);
			virtual void SetComputeBasisVectors(bool i_bCompute);
			virtual void StoreGeometryInVideoMemory(bool i_bVideoMem);
	};

	class g3dVertexBufferVk : public g3dVertexBuffer
	{
		public:
			virtual void UpdateVertices(int i_NumVertices, const maPoint3d* i_pVertices, const maVector3d* i_pNormals = NULL);
			virtual int GetNumVertices() const;
			virtual g3dType::VertexFormat GetVertexFormat() const;
			virtual unsigned char* Lock();
			virtual void Unlock();
			virtual void SetBoundingBox(const maAxisBox& i_BoundingBox);
	};

	class mdlFragCreateVk : public mdlFragCreateImpl
	{
		public:
			virtual bool UseBumpFrag(int i_NumTexCoords, bool i_EffectQualified);
			virtual g3dFragment* CreateFragment(const mdlSplitFragInfo& i_Info, bool i_Morphable = false);
			virtual g3dFragment* CreateHairFragment(const mdlHairInfo& i_Info, matMaterial* i_pMaterial);
			virtual void OptimizeFragment(mdlSplitFragInfo& io_Info);
			virtual g3dVertexBuffer* CreateFragmentGroup(const mdlFragInfo& i_FragInfo, std::vector<g3dFragment*>& o_Fragments,
														 bool i_Morphable = false);
			virtual void UpdateFragmentGroup(const mdlFragInfo& i_FragInfo, g3dVertexBuffer& io_SharedVertexBuffer,
											 std::vector<g3dFragment*>& io_Fragments, bool i_Morphable = false);
	};
}
