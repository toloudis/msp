/*****************************************************************************
**  gtOptiX.hpp
**
**      gtOptiX is the test fixture for GraphicsOptiX, which runs on top of
**	GraphicsVulkan: it loads CUDA and OptiX and creates the OptiX context
**	(optxContext, validation on) once per test suite; for each test it
**	makes a Vulkan device on the same GPU (the CUDA device's UUID) with
**	external memory, through gtVulkanContext (which fails the test on any
**	validation message), turns on exportable static geometry and makes a
**	g3dGasCacheOptiX. Each test also fails on any OptiX message at error
**	or warning level it caused. Every test is skipped when there is no
**	NVIDIA driver or no usable device.
**
**	Also: the helpers the Ox tests share (fragments made by GraphicsVulkan's
**	creators, a one-layer scene, rays down -z).
**
**	Compiled only when the OptiX SDK and the CUDA toolkit were both found
**	(GraphicsTests.vcxproj), as GraphicsOptiX's own sources are.
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

#include "Vk/gtVulkanContext.hpp"

#ifndef GOX_CULOADER_HPP
#include "GraphicsOptiX/cu/cuLoader.hpp"
#endif
#ifndef GOX_OPTXCONTEXT_HPP
#include "GraphicsOptiX/optx/optxContext.hpp"
#endif
#ifndef GOX_OPTXLOADER_HPP
#include "GraphicsOptiX/optx/optxLoader.hpp"
#endif
#ifndef GOX_OPTXRAYQUERY_HPP
#include "GraphicsOptiX/optx/optxRayQuery.hpp"
#endif
#ifndef GOX_G3DGASCACHEOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dGasCacheOptiX.hpp"
#endif

#ifndef GVK_BUMP_FRAGMENTCREATE_HPP
#include "GraphicsVulkan/bump/bumpFragmentCreate.hpp"
#endif
#ifndef GVK_BUMP_TRIMESHBUMPFRAG_HPP
#include "GraphicsVulkan/bump/bumpTriMeshBumpFrag.hpp"
#endif
#ifndef GVK_G3DVKBUFFERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkBufferUtil.hpp"
#endif
#ifndef G3D_INDEXPTR_HPP
#include "Graphics/G3d/g3dIndexPtr.hpp"
#endif
#ifndef G3D_LAYER_HPP
#include "Graphics/G3d/g3dLayer.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/G3d/g3dSceneNode.hpp"
#endif

#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class gtOptiX : public gtVulkanContext
{
	public:
		static void SetUpTestSuite()
		{
			std::string& error = Error();
			error.clear();
			if (!gox::cuLoader::Load(error))
			{
				return;
			}
			if (!gox::optxLoader::Load(error))
			{
				gox::cuLoader::Unload();
				return;
			}
			if (!gox::optxContext::Create(true, error))
			{
				gox::optxLoader::Unload();
				gox::cuLoader::Unload();
			}
		}

		static void TearDownTestSuite()
		{
			if (gox::optxContext::Exists())
			{
				gox::optxContext::Destroy();
				gox::optxLoader::Unload();
				gox::cuLoader::Unload();
			}
		}

	protected:
		void SetUp() override
		{
			if (!gox::optxContext::Exists())
			{
				GTEST_SKIP() << "No OptiX device: " << Error();
			}
			gtVulkanContext::SetUp();
			if (IsSkipped() || !m_pDevice)
			{
				return;
			}
			gox::optxContext::MakeCurrent();
			m_nErrors = gox::optxContext::GetErrorCount();
			m_nWarnings = gox::optxContext::GetWarningCount();
			gvk::g3dVkBufferUtil::ExportStaticGeometry(true);
			m_pCache.reset(new gox::g3dGasCacheOptiX(Device(), Context()));
		}

		//	Subclasses release their fragments (and anything else made from the
		//	device) before this runs
		void TearDown() override
		{
			if (m_pCache)
			{
				m_pCache.reset();
				gvk::g3dVkBufferUtil::ExportStaticGeometry(false);
			}
			if (gox::optxContext::Exists() && m_pDevice)
			{
				EXPECT_TRUE(gox::optxContext::Synchronize());
				EXPECT_EQ(m_nErrors, gox::optxContext::GetErrorCount()) << "OptiX errors (see the log)";
				EXPECT_EQ(m_nWarnings, gox::optxContext::GetWarningCount()) << "OptiX warnings (see the log)";
			}
			gtVulkanContext::TearDown();
		}

		void ConfigureDevice(gvk::vkdDeviceOptions& io_Options) override
		{
			io_Options.m_RequiredDeviceUUID = gox::optxContext::GetDeviceUUID();
			io_Options.m_RequiredDeviceName = "the CUDA device \"" + gox::optxContext::GetDeviceName() + "\"";
			io_Options.m_bExternalMemory = true;
		}

		gox::g3dGasCacheOptiX& Cache() { return *m_pCache; }

		std::unique_ptr<gox::g3dGasCacheOptiX> m_pCache;

	private:
		static std::string& Error()
		{
			static std::string s_Error;
			return s_Error;
		}

		int m_nErrors = 0;
		int m_nWarnings = 0;
};

//	What the Ox tests share
namespace gtOx
{
	//	One triangle covering (0, 0) in the plane z: (-1,-1) (1,-1) (0,1),
	//	made by GraphicsVulkan's fragment creator (a bumpTriMeshBumpFrag),
	//	casting shadows
	inline std::unique_ptr<gvk::bumpTriMeshBumpFrag> Triangle(float z, bool i_bMorphable = false)
	{
		const maPoint3d positions[3] = { maPoint3d(-1, -1, z), maPoint3d(1, -1, z), maPoint3d(0, 1, z) };
		const maPoint3d normals[3] = { maPoint3d(0, 0, 1), maPoint3d(0, 0, 1), maPoint3d(0, 0, 1) };
		const uint32_t indices[3] = { 0, 1, 2 };
		gvk::bumpFragmentCreate create;
		std::unique_ptr<gvk::bumpTriMeshBumpFrag> fragment(dynamic_cast<gvk::bumpTriMeshBumpFrag*>(
			create.CreateFragment(positions, normals, 3, g3dIndexPtr(indices), 3, nullptr, i_bMorphable)));
		fragment->SetCastsShadow(true);
		return fragment;
	}

	//	Any mesh, the same way
	inline std::unique_ptr<gvk::bumpTriMeshBumpFrag> Mesh(const std::vector<maPoint3d>& i_Positions,
														   const std::vector<uint32_t>& i_Indices, bool i_bMorphable)
	{
		const std::vector<maPoint3d> normals(i_Positions.size(), maPoint3d(0, 0, 1));
		gvk::bumpFragmentCreate create;
		std::unique_ptr<gvk::bumpTriMeshBumpFrag> fragment(dynamic_cast<gvk::bumpTriMeshBumpFrag*>(
			create.CreateFragment(i_Positions.data(), normals.data(), int(i_Positions.size()), g3dIndexPtr(i_Indices.data()),
								  int(i_Indices.size()), nullptr, i_bMorphable)));
		fragment->SetCastsShadow(true);
		return fragment;
	}

	//	Row-vector matrix: scale, then translate
	inline maMatrix4x4 ScaleTranslate(float sx, float sy, float sz, float tx, float ty, float tz)
	{
		return maMatrix4x4(sx, 0, 0, 0,
						   0, sy, 0, 0,
						   0, 0, sz, 0,
						   tx, ty, tz, 1);
	}

	//	A scene of one world-space layer; the test owns the root (layers
	//	don't delete their nodes), the root owns its children
	struct Scene
	{
		std::unique_ptr<g3dSceneNode>	m_pRoot;
		std::unique_ptr<g3dScene>		m_pScene;

		Scene()
			: m_pRoot(new g3dSceneNode())
			, m_pScene(new g3dScene(new g3dLayer(m_pRoot.get())))
		{
		}

		~Scene()
		{
			m_pScene.reset();
			m_pRoot.reset();
		}

		g3dSceneNode* Add(g3dFragment* i_pFragment, const maMatrix4x4& i_Transform = maMatrix4x4())
		{
			g3dSceneNode* node = new g3dSceneNode(i_pFragment);
			node->SetTransform(i_Transform);
			m_pRoot->AddChild(node);
			node->UpdateTotalTransform();
			return node;
		}
	};

	//	A ray down -z from (x, y, i_Z)
	inline gox::optxRayQueryRay Down(float x, float y, float i_Z = 5.0f, unsigned i_Mask = 0xFF)
	{
		gox::optxRayQueryRay ray = {};
		ray.m_Origin[0] = x;
		ray.m_Origin[1] = y;
		ray.m_Origin[2] = i_Z;
		ray.m_Direction[2] = -1.0f;
		ray.m_TMax = 1e30f;
		ray.m_Mask = i_Mask;
		return ray;
	}

	//	The hit distance of one ray down from (x, y, i_Z), or -1 for a miss
	inline float TraceDown(gox::optxRayQuery& io_Query, OptixTraversableHandle i_Handle, float x, float y,
						   float i_Z = 5.0f, unsigned i_nHitRecords = 1)
	{
		std::vector<gox::optxRayQueryHit> hits;
		EXPECT_TRUE(io_Query.Trace(i_Handle, { Down(x, y, i_Z) }, hits, i_nHitRecords));
		return hits.size() == 1 && hits[0].m_Hit ? hits[0].m_T : -1.0f;
	}
}
