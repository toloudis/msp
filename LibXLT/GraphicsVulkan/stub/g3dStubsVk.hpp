/*****************************************************************************
**  g3dStubsVk.hpp
**
**      Vulkan implementations of the 3D graphics hooks that aren't ported
**	yet: they keep MachStudio running (every call returns a usable object
**	or a harmless value) and log GVK_NOT_YET the first time a call that
**	should do real work runs. Each class moves to its real package when a
**	later plan ports it.
**
**	  hairModelFragVk, hairRendererVk
**		hair fragments (mayFragCreate::CreateHairFragment) keep their
**		vertices and bounding box on the CPU; their renderer, registered
**		as DX11's hairPackage registers hairRenderer, draws nothing
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
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_BASERENDERER_HPP
#include "Graphics/G3d/g3dBaseRenderer.hpp"
#endif
#ifndef G3D_INDEXPTR_HPP
#include "Graphics/G3d/g3dIndexPtr.hpp"
#endif
#ifndef G3D_TYPE_HPP
#include "Graphics/G3d/g3dType.hpp"
#endif

#include <string>
#include <vector>

namespace gvk
{
	class hairModelFragVk : public g3dFragment
	{
		public:
			//	Kept on the CPU as DX11's bump fragments keep them: g3dType::
			//	BumpTex1Vertex vertices (missing positions, normals or UVs are
			//	zero, +Z and (0,0); S and T are +X and +Y) and 16-bit indices
			//	unless g3dIndexPtr::Needs32Bit.
			hairModelFragVk(const maPoint3d* i_Vertices, const maPoint3d* i_Normals, const maPoint2d* i_UVs, int i_nVertices,
							const g3dIndexPtr* i_pIndices, int i_nIndices, matMaterial* i_pMaterial, bool i_bMorphable);
			hairModelFragVk(const hairModelFragVk& i_Other);
			virtual ~hairModelFragVk();

			//	The render mode hairRendererVk got (as hairModelFrag::SetRendererId)
			static void SetRendererId(int i_RenderMode);

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

			static int								sm_RendererId;
			std::vector<g3dType::BumpTex1Vertex>	m_Vertices;
			std::vector<uint16_t>					m_Indices16;	// one of these, by g3dIndexPtr::Needs32Bit
			std::vector<uint32_t>					m_Indices32;
	};

	class hairRendererVk : public g3dBaseRenderer
	{
		public:
			virtual void Deallocate()	{}
			virtual void Reallocate()	{}
			virtual int Render(const g3dSceneNode* i_pNode, const matMaterial* i_pMaterial, matShaderEffect* i_pEffect);
	};
}
