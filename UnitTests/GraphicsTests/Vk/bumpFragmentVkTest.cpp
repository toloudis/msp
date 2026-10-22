/*****************************************************************************
**  bumpFragmentVkTest.cpp
**
**      GraphicsVulkan's tmesh and bump fragments against GraphicsDX11's:
**      the same meshes made through g3dFragmentCreate under each layer
**      (gtBackend) give the same vertex format, vertex bytes (basis
**      vectors computed), index width and bytes, bounding box, sub-
**      fragments and bake factors. Drawn with a Solid material through
**      bumpBumpRenderer they give the same image (also split into sub-
**      fragments and draw-call-limited, with more than 65,535 vertices),
**      and a fragment destroyed right after its draw stays valid until the
**      GPU is done with it (validation; Review Focus 1).
**
**      Skipped without a Vulkan device (the DX11 half still runs).
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtImage.hpp"
#include "gtScene.hpp"
#include "gtTarget.hpp"

#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_TYPE_HPP
#include "Graphics/G3d/g3dType.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/Ma/maAxisBox.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/G3d/g3dSceneNode.hpp"
#endif
#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/G3d/g3dRendererMgr.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/Mat/matShaderMgr.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/Mat/matShaderEffect.hpp"
#endif
#ifndef EFF_SOLIDDATA_HPP
#include "Graphics/Eff/effSolidData.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/Cam/camCamera.hpp"
#endif

//	DX11
#ifndef TMESH_FRAG_HPP
#include "GraphicsDX11/tmesh/tmeshFrag.hpp"
#endif
#ifndef BUMP_BUMPRENDERER_HPP
#include "GraphicsDX11/bump/bumpBumpRenderer.hpp"
#endif
#ifndef G3D_RASTERIZERSTATEMGR_HPP
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#endif

//	Vulkan
#ifndef GVK_TMESH_FRAG_HPP
#include "GraphicsVulkan/tmesh/tmeshFrag.hpp"
#endif
#ifndef GVK_BUMP_BUMPRENDERER_HPP
#include "GraphicsVulkan/bump/bumpBumpRenderer.hpp"
#endif
#ifndef GVK_G3DRASTERIZERSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dRasterizerStateMgr.hpp"
#endif

#include <gtest/gtest.h>

#include <cstring>
#include <memory>
#include <vector>

namespace
{
	//	What a fragment holds, read through g3dFragment (and the backend's
	//	tmeshFrag for the index width)
	struct FragmentData
	{
		int							m_Format = -1;
		int							m_Vertices = 0;
		std::vector<uint8_t>		m_VertexBytes;
		int							m_IndexSize = 0;
		int							m_Indices = 0;
		std::vector<uint8_t>		m_IndexBytes;
		float						m_Box[6] = {};
		std::vector<int>			m_SubFragments;		// start index, triangle count, ...
		std::vector<float>			m_SubBoxes;
		bool						m_bVelocity = false;
		float						m_Bake[4] = {};
		float						m_Overlap = 0;
	};

	void read_box(const maAxisBox& i_Box, float* o_Six)
	{
		o_Six[0] = i_Box.GetMinX(); o_Six[1] = i_Box.GetMinY(); o_Six[2] = i_Box.GetMinZ();
		o_Six[3] = i_Box.GetMaxX(); o_Six[4] = i_Box.GetMaxY(); o_Six[5] = i_Box.GetMaxZ();
	}

	FragmentData read(gtBackend::Kind i_Kind, const gtMesh& i_Mesh, bool i_bMorphable)
	{
		FragmentData data;
		std::unique_ptr<g3dFragment> fragment(i_Mesh.CreateFragment(NULL, i_bMorphable));
		EXPECT_TRUE(fragment);
		if (!fragment)
			return data;

		data.m_Format = fragment->GetVertexFormat();
		data.m_Vertices = fragment->GetNumVertices();
		const uint8_t* vertices = fragment->ReadOnlyLock();
		data.m_VertexBytes.assign(vertices, vertices + data.m_Vertices * sizeof(g3dType::BumpTex1Vertex));
		fragment->ReadOnlyUnlock();

		if (i_Kind == gtBackend::e_DX11)
		{
			const tmeshFrag* frag = dynamic_cast<const tmeshFrag*>(fragment.get());
			EXPECT_TRUE(frag) << "DX11: not a tmeshFrag";
			data.m_IndexSize = frag ? frag->GetSizeOfIndex() : 0;
		}
		else
		{
			const gvk::tmeshFrag* frag = dynamic_cast<const gvk::tmeshFrag*>(fragment.get());
			EXPECT_TRUE(frag) << "Vulkan: not a gvk::tmeshFrag";
			data.m_IndexSize = frag ? frag->GetSizeOfIndex() : 0;
		}
		data.m_Indices = fragment->GetNumIndices();
		const uint8_t* indices = fragment->ReadOnlyLockIndices();
		data.m_IndexBytes.assign(indices, indices + data.m_Indices * data.m_IndexSize);
		fragment->ReadOnlyUnlockIndices();

		read_box(fragment->GetBoundingBox(), data.m_Box);
		data.m_bVelocity = fragment->GetHasVelocityBuffer();
		maPoint2d scale, translate;
		fragment->GetUVBakeFactors(scale, translate);
		data.m_Bake[0] = scale.m_X; data.m_Bake[1] = scale.m_Y;
		data.m_Bake[2] = translate.m_X; data.m_Bake[3] = translate.m_Y;
		data.m_Overlap = fragment->GetUVOverlapFactor();

		fragment->Split(fragment->GetBoundingBox());
		for (const g3dFragment::sSubFrag& sub : fragment->GetSubFragments())
		{
			data.m_SubFragments.push_back(sub.m_startIndex);
			data.m_SubFragments.push_back(sub.m_numTris);
			float box[6];
			read_box(sub.m_bounds, box);
			data.m_SubBoxes.insert(data.m_SubBoxes.end(), box, box + 6);
		}
		return data;
	}

	//	One mesh to read
	struct ReadCase
	{
		const char*	m_pName;
		gtMesh		m_Mesh;
		bool		m_bMorphable;
	};

	void expect_same(const FragmentData& dx11, const FragmentData& vulkan);

	//	Reads every case on each backend (one layer start-up each) and compares
	void expect_same(const std::vector<ReadCase>& i_Cases)
	{
		std::vector<FragmentData> dx11s, vulkans;
		gtBackend::Run(gtBackend::e_DX11, [&]()
		{
			for (const ReadCase& c : i_Cases)
				dx11s.push_back(read(gtBackend::e_DX11, c.m_Mesh, c.m_bMorphable));
		});
		if (!gtBackend::Run(gtBackend::e_Vulkan, [&]()
		{
			for (const ReadCase& c : i_Cases)
				vulkans.push_back(read(gtBackend::e_Vulkan, c.m_Mesh, c.m_bMorphable));
		}))
			return;
		ASSERT_EQ(i_Cases.size(), dx11s.size());
		ASSERT_EQ(i_Cases.size(), vulkans.size());
		for (size_t i = 0; i < i_Cases.size(); i++)
		{
			SCOPED_TRACE(i_Cases[i].m_pName);
			expect_same(dx11s[i], vulkans[i]);
		}
	}

	void expect_same(const FragmentData& dx11, const FragmentData& vulkan)
	{
		EXPECT_EQ(g3dType::e_BumpTex1Vertex, dx11.m_Format);
		EXPECT_EQ(dx11.m_Format, vulkan.m_Format);
		EXPECT_EQ(dx11.m_Vertices, vulkan.m_Vertices);
		EXPECT_TRUE(dx11.m_VertexBytes == vulkan.m_VertexBytes) << "vertex bytes differ";
		EXPECT_EQ(dx11.m_IndexSize, vulkan.m_IndexSize);
		EXPECT_EQ(dx11.m_Indices, vulkan.m_Indices);
		EXPECT_TRUE(dx11.m_IndexBytes == vulkan.m_IndexBytes) << "index bytes differ";
		EXPECT_EQ(0, memcmp(dx11.m_Box, vulkan.m_Box, sizeof(dx11.m_Box))) << "bounding boxes differ";
		EXPECT_EQ(dx11.m_SubFragments, vulkan.m_SubFragments);
		EXPECT_EQ(dx11.m_SubBoxes, vulkan.m_SubBoxes);
		EXPECT_EQ(dx11.m_bVelocity, vulkan.m_bVelocity);
		EXPECT_EQ(0, memcmp(dx11.m_Bake, vulkan.m_Bake, sizeof(dx11.m_Bake))) << "bake factors differ";
		EXPECT_EQ(dx11.m_Overlap, vulkan.m_Overlap);
	}
}

TEST(bumpFragmentVkTest, SameVertexAndIndexData)
{
	const gtMesh grid = gtScene::Grid(300);
	ASSERT_GT(grid.VertexCount(), 65535);
	expect_same({
		{ "sphere", gtScene::Sphere(12, 24), false },
		{ "sphere, morphable", gtScene::Sphere(12, 24), true },
		{ "plane", gtScene::Plane(), false },
		{ "grid 300 (32-bit indices)", grid, false },
	});
}

namespace
{
	//	How a mesh is drawn
	struct DrawOptions
	{
		bool	m_bMorphable = false;
		bool	m_bSplit = false;				// into sub-fragments (bumpTriMeshBumpFrag::Split), each drawn
		int		m_DrawLimit = -1;				// bumpBumpRenderer::SetDrawLimit (-1: D3D's)
		bool	m_bDestroyBeforeRead = false;	// the fragment goes before the GPU has drawn it
		bool	m_bMorphAfterDraw = false;		// UpdateVertices after the first draw, then draw again
	};

	//	The mesh drawn with a Solid material through bumpBumpRenderer (as a
	//	renderer's DrawNode does), seen from +Z, into a 128x128 target
	gtPicture draw(gtBackend::Kind i_Kind, const gtMesh& i_Mesh, const DrawOptions& i_Options)
	{
		const float clear[4] = { 0, 0, 0, 1 };
		gtTarget target(i_Kind, 128, 128, clear);

		camCamera camera;
		camera.SetFOV(45.0f);
		camera.SetAspect(1.0f);
		camera.SetClip(0.1f, 100.0f);
		camera.LookAt(maPoint3d(0.3f, 0.4f, 3.5f), maPoint3d(0, 0, 0), maVector3d(0, 1, 0));
		maMatrix4x4 view, projection, world;
		camera.GetCameraMatrix(view);
		camera.GetProjectionMatrix(projection);
		projection.ScaleBy(-1.0f, 1.0f, 1.0f);		// as g3dSceneRenderUtil::SetViewingTransforms
		world.Identity();

		matMaterial material("Solid.fx", matShaderMgr::CreateData("Solid.fx"));
		effSolidData* solid = dynamic_cast<effSolidData*>(material.GetEffectData());
		EXPECT_TRUE(solid != NULL);
		if (solid)
			solid->m_Color = maFloatRGBA(0.25f, 0.5f, 0.75f, 1.0f);
		matShaderEffect* effect = matShaderMgr::GetEffect(material);
		EXPECT_TRUE(effect != NULL);
		if (!effect)
			return gtPicture();

		g3dFragment* fragment = i_Mesh.CreateFragment(&material, i_Options.m_bMorphable);
		if (i_Options.m_bSplit)
			fragment->Split(fragment->GetBoundingBox());
		const int parts = fragment->GetSubFragments().empty() ? 1 : (int)fragment->GetSubFragments().size();

		if (i_Kind == gtBackend::e_DX11)
		{
			bumpBumpRenderer::SetDrawLimit(i_Options.m_DrawLimit);
			g3dRasterizerStateMgr::SetRasterizerState(D3D11_CULL_NONE, D3D11_FILL_SOLID);
		}
		else
		{
			gvk::bumpBumpRenderer::SetDrawLimit(i_Options.m_DrawLimit);
			gvk::g3dRasterizerStateMgr::SetRasterizerState(VK_CULL_MODE_NONE, VK_POLYGON_MODE_FILL);
		}

		auto render = [&]()
		{
			effect->SetTechnique(matShaderEffect::e_Default);
			effect->SetupMatrices(world, view, projection, camera.GetPosition());
			effect->SetupMaterial(&material);
			for (int part = 0; part < parts; part++)
			{
				g3dSceneNode node(fragment, world, part);
				EXPECT_GT(g3dRendererMgr::Render(&node, &material, effect), 0) << "part " << part;
			}
		};
		render();
		if (i_Options.m_bMorphAfterDraw)
		{
			//	move the mesh aside and draw it again: the image has both
			std::vector<maPoint3d> moved(i_Mesh.m_Positions);
			for (maPoint3d& p : moved)
				p = maPoint3d(p.m_X * 0.5f - 0.4f, p.m_Y * 0.5f, p.m_Z - 1.0f);
			fragment->UpdateVertices((int)moved.size(), moved.data(), NULL);
			render();
		}

		if (i_Options.m_bDestroyBeforeRead)
		{
			delete fragment;
			fragment = NULL;
		}
		gtPicture picture = target.Read();
		delete fragment;

		if (i_Kind == gtBackend::e_DX11)
			bumpBumpRenderer::SetDrawLimit(-1);
		else
			gvk::bumpBumpRenderer::SetDrawLimit(-1);
		return picture;
	}

	//	One mesh to draw; m_pName names the saved images if they differ
	struct DrawCase
	{
		const char*	m_pName;
		gtMesh		m_Mesh;
		DrawOptions	m_Options;
	};

	void expect_same_image(const char* i_pTest, const gtPicture& dx11, const gtPicture& vulkan);

	//	Draws every case on each backend (one layer start-up each) and compares
	void expect_same_images(const std::vector<DrawCase>& i_Cases)
	{
		std::vector<gtPicture> dx11s, vulkans;
		gtBackend::Run(gtBackend::e_DX11, [&]()
		{
			for (const DrawCase& c : i_Cases)
				dx11s.push_back(draw(gtBackend::e_DX11, c.m_Mesh, c.m_Options));
		});
		if (!gtBackend::Run(gtBackend::e_Vulkan, [&]()
		{
			for (const DrawCase& c : i_Cases)
				vulkans.push_back(draw(gtBackend::e_Vulkan, c.m_Mesh, c.m_Options));
		}))
			return;
		ASSERT_EQ(i_Cases.size(), dx11s.size());
		ASSERT_EQ(i_Cases.size(), vulkans.size());
		for (size_t i = 0; i < i_Cases.size(); i++)
		{
			SCOPED_TRACE(i_Cases[i].m_pName);
			expect_same_image(i_Cases[i].m_pName, dx11s[i], vulkans[i]);
		}
	}

	void expect_same_image(const char* i_pTest, const gtPicture& dx11, const gtPicture& vulkan)
	{
		EXPECT_TRUE(dx11.Valid());
		EXPECT_TRUE(vulkan.Valid());
		if (!dx11.Valid() || !vulkan.Valid())
			return;

		//	the mesh covers a good part of the image
		int covered = 0;
		for (size_t i = 0; i < dx11.m_Rgba.size(); i += 4)
			covered += (dx11.m_Rgba[i] | dx11.m_Rgba[i + 1] | dx11.m_Rgba[i + 2]) != 0;
		EXPECT_GT(covered, 128 * 128 / 20) << "DX11 drew almost nothing";

		const gtCompareResult result = gtCompareImages(dx11.m_Rgba.data(), vulkan.m_Rgba.data(), 128, 128);
		if (!result.m_bPass)
		{
			ADD_FAILURE() << result.Describe() << " (images in "
				<< gtSaveComparison(i_pTest, dx11.m_Rgba.data(), vulkan.m_Rgba.data(), 128, 128) << ")";
		}
	}
}

TEST(bumpFragmentVkTest, FragmentDrawsMatchDX11)
{
	//	static; morphable, updated after the first draw
	DrawOptions morph;
	morph.m_bMorphable = true;
	morph.m_bMorphAfterDraw = true;
	expect_same_images({
		{ "FragmentDrawsMatchDX11_static", gtScene::Sphere(16, 32), DrawOptions() },
		{ "FragmentDrawsMatchDX11_morph", gtScene::Sphere(16, 32), morph },
	});
}

TEST(bumpFragmentVkTest, LargeMeshMatchesDX11)
{
	const gtMesh grid = gtScene::Grid(300);
	ASSERT_GT(grid.VertexCount(), 65535);
	//	whole; split and draw-call-limited
	DrawOptions split;
	split.m_bSplit = true;
	split.m_DrawLimit = 3000;
	expect_same_images({
		{ "LargeMeshMatchesDX11_whole", grid, DrawOptions() },
		{ "LargeMeshMatchesDX11_split", grid, split },
	});
}

TEST(bumpFragmentVkTest, DestroyAfterDrawIsSafe)
{
	//	The images still match, and gtBackend fails the test on any
	//	validation message (a freed buffer the GPU still reads)
	DrawOptions still, morph;
	still.m_bDestroyBeforeRead = true;
	morph.m_bMorphable = true;
	morph.m_bDestroyBeforeRead = true;
	expect_same_images({
		{ "DestroyAfterDrawIsSafe_static", gtScene::Sphere(8, 16), still },
		{ "DestroyAfterDrawIsSafe_morph", gtScene::Sphere(8, 16), morph },
	});
}
