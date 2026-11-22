/*****************************************************************************
**  g3dGasCacheOptiXTest.cpp
**
**      g3dGasCacheOptiX on GraphicsVulkan's fragments: a static mesh's GAS
**	is built in place from Vulkan's memory, with no CUDA upload, right
**	after the mesh is made (flushing Vulkan's open command buffer), once,
**	and cached; a clone (sharing the buffers) shares it; Deallocate and
**	Reallocate rebuild it; a morphable mesh is uploaded and refit when its
**	vertices move, rebuilt after c_MaxRefits refits, and built anew when
**	its index buffer is replaced; transparency sorting builds nothing;
**	16- and 32-bit indices; line lists get none; a freed fragment's entry
**	goes at Collect. Rays (optxRayQuery) check the GAS holds the current
**	geometry.
**
**	Extra Large Technology
\****************************************************************************/
#include "Ox/gtOptiX.hpp"

#ifndef GVK_BUMP_BUFFERUTIL_HPP
#include "GraphicsVulkan/bump/bumpBufferUtil.hpp"
#endif

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

using namespace gox;
using gvk::bumpTriMeshBumpFrag;

namespace
{
	class g3dGasCacheOptiXTest : public gtOptiX
	{
		protected:
			void SetUp() override
			{
				gtOptiX::SetUp();
				if (IsSkipped())
				{
					return;
				}
				std::string error;
				ASSERT_TRUE(m_Query.Create(error)) << error;
			}

			float trace_down(OptixTraversableHandle i_Handle, float x, float y)
			{
				return gtOx::TraceDown(m_Query, i_Handle, x, y);
			}

			OptixTraversableHandle ensure(const gvk::tmeshFrag& i_Fragment)
			{
				const g3dGasCacheOptiX::Gas* gas = Cache().Ensure(i_Fragment);
				return gas ? gas->m_Handle : 0;
			}

			optxRayQuery m_Query;
	};

	//	One triangle in the plane z, moved through Lock/Unlock, as skinning writes
	void move_to(bumpTriMeshBumpFrag& io_Fragment, float z)
	{
		g3dType::BumpTex1Vertex* vertices = reinterpret_cast<g3dType::BumpTex1Vertex*>(io_Fragment.Lock());
		ASSERT_NE(nullptr, vertices);
		for (int i = 0; i < io_Fragment.GetNumVertices(); ++i)
		{
			vertices[i].m_Vertex.SetZ(z);
		}
		io_Fragment.Unlock();
	}
}

TEST_F(g3dGasCacheOptiXTest, StaticBuiltInPlaceOnceAndCached)
{
	auto fragment = gtOx::Triangle(0);
	const g3dGasCacheOptiX::Gas* gas = Cache().Ensure(*fragment);
	ASSERT_NE(nullptr, gas);
	ASSERT_NE(0u, gas->m_Handle);
	EXPECT_EQ(gas, Cache().Ensure(*fragment));
	EXPECT_EQ(1, Cache().GetStats().m_Builds);
	EXPECT_EQ(0, Cache().GetStats().m_Uploads);		// read from Vulkan's memory
	EXPECT_EQ(1, Cache().GetStats().m_Flushes);		// the upload was still in Vulkan's open command buffer
	EXPECT_EQ(1, Cache().GetStats().m_UploadWaits);
	EXPECT_EQ(1u, gas->m_NumTriangles);
	ASSERT_EQ(1u, gas->m_Inputs.size());
	EXPECT_EQ(2u, gas->m_Inputs[0].m_IndexSize);
	EXPECT_EQ(sizeof(g3dType::BumpTex1Vertex), gas->m_VertexStride);
	EXPECT_FLOAT_EQ(5.0f, trace_down(gas->m_Handle, 0, 0));
	EXPECT_EQ(-1.0f, trace_down(gas->m_Handle, 3, 3));

	//	Once the upload has landed, a new GAS needs no wait
	auto second = gtOx::Triangle(-1);
	Context().Flush();
	Device().WaitIdle();
	ASSERT_NE(0u, ensure(*second));
	EXPECT_EQ(1, Cache().GetStats().m_UploadWaits);
	EXPECT_EQ(0, Cache().GetStats().m_Uploads);
}

TEST_F(g3dGasCacheOptiXTest, CloneSharesTheGas)
{
	auto fragment = gtOx::Triangle(0);
	gvk::bumpFragmentCreate create;
	std::unique_ptr<g3dFragment> clone(create.CloneFragment(fragment.get()));
	const gvk::tmeshFrag* copy = dynamic_cast<const gvk::tmeshFrag*>(clone.get());
	ASSERT_NE(nullptr, copy);
	EXPECT_EQ(2, fragment->GetVertexBuffer()->GetFragmentCount());
	EXPECT_EQ(Cache().Ensure(*fragment), Cache().Ensure(*copy));
	EXPECT_EQ(1, Cache().GetStats().m_Builds);
}

TEST_F(g3dGasCacheOptiXTest, DeallocateReallocateRebuilds)
{
	//	Morphable: Reallocate refills a static vertex buffer through Unlock,
	//	which a default-usage buffer refuses (as on DX11)
	auto fragment = gtOx::Triangle(0, true);
	ASSERT_NE(0u, ensure(*fragment));
	const uint64_t generation = fragment->GetVertexBuffer()->GetGeneration();

	fragment->Deallocate();
	EXPECT_NE(generation, fragment->GetVertexBuffer()->GetGeneration());
	EXPECT_EQ(nullptr, Cache().Ensure(*fragment));	// no buffers: no GAS

	fragment->Reallocate();
	const OptixTraversableHandle handle = ensure(*fragment);
	ASSERT_NE(0u, handle);
	EXPECT_EQ(2, Cache().GetStats().m_Builds);
	EXPECT_EQ(2, Cache().GetStats().m_Uploads);		// the dynamic vertices, each time
	EXPECT_FLOAT_EQ(5.0f, trace_down(handle, 0, 0));
}

TEST_F(g3dGasCacheOptiXTest, MorphableRefitWhenVerticesMove)
{
	auto fragment = gtOx::Triangle(0, true);
	ASSERT_NE(0u, ensure(*fragment));
	EXPECT_EQ(1, Cache().GetStats().m_Uploads);		// the vertices; the indices are static

	move_to(*fragment, -2.0f);
	const OptixTraversableHandle handle = ensure(*fragment);
	EXPECT_EQ(1, Cache().GetStats().m_Builds);
	EXPECT_EQ(1, Cache().GetStats().m_Refits);
	EXPECT_EQ(2, Cache().GetStats().m_Uploads);
	EXPECT_FLOAT_EQ(7.0f, trace_down(handle, 0, 0));

	//	Nothing changed: nothing done
	ensure(*fragment);
	EXPECT_EQ(1, Cache().GetStats().m_Refits);
	EXPECT_EQ(2, Cache().GetStats().m_Uploads);
}

TEST_F(g3dGasCacheOptiXTest, MorphableRebuiltAfterMaxRefits)
{
	auto fragment = gtOx::Triangle(0, true);
	ASSERT_NE(0u, ensure(*fragment));
	for (int i = 1; i <= g3dGasCacheOptiX::c_MaxRefits + 1; ++i)
	{
		move_to(*fragment, -0.01f * i);
		ASSERT_NE(0u, ensure(*fragment));
	}
	EXPECT_EQ(2, Cache().GetStats().m_Builds);
	EXPECT_EQ(g3dGasCacheOptiX::c_MaxRefits, Cache().GetStats().m_Refits);
	EXPECT_NEAR(5.0f + 0.01f * (g3dGasCacheOptiX::c_MaxRefits + 1), trace_down(ensure(*fragment), 0, 0), 1e-4f);
}

TEST_F(g3dGasCacheOptiXTest, ReplacedIndexBufferBuildsAnew)
{
	//	Two triangles; the indices switch from the first to the second
	std::vector<maPoint3d> positions = { maPoint3d(-1, -1, 0), maPoint3d(1, -1, 0), maPoint3d(0, 1, 0),
										 maPoint3d(-1, -1, -3), maPoint3d(1, -1, -3), maPoint3d(0, 1, -3) };
	auto fragment = gtOx::Mesh(positions, { 0, 1, 2 }, true);
	ASSERT_NE(0u, ensure(*fragment));

	const uint32_t lower[3] = { 3, 4, 5 };
	shared_ptr<gvk::tmeshIndexBuffer> indices = gvk::bumpBufferUtil::CreateIndexBuffer(g3dIndexPtr(lower), 3, 3, 6, false);
	fragment->UpdateIndexBuffer(indices);
	const OptixTraversableHandle handle = ensure(*fragment);
	EXPECT_EQ(2, Cache().GetStats().m_Builds);
	EXPECT_EQ(0, Cache().GetStats().m_Refits);
	EXPECT_FLOAT_EQ(8.0f, trace_down(handle, 0, 0));

	//	The first entry's index buffer is gone
	EXPECT_EQ(2u, Cache().GetEntryCount());
	Cache().Collect();
	EXPECT_EQ(1u, Cache().GetEntryCount());
}

TEST_F(g3dGasCacheOptiXTest, TransparencySortBuildsNothing)
{
	//	Two triangles at different depths, sorted (index order rewritten in place)
	const maPoint3d positions[6] = { maPoint3d(-1, -1, 0), maPoint3d(1, -1, 0), maPoint3d(0, 1, 0),
									 maPoint3d(-1, -1, -3), maPoint3d(1, -1, -3), maPoint3d(0, 1, -3) };
	const maPoint3d normals[6] = { maPoint3d(0, 0, 1), maPoint3d(0, 0, 1), maPoint3d(0, 0, 1),
								   maPoint3d(0, 0, 1), maPoint3d(0, 0, 1), maPoint3d(0, 0, 1) };
	const uint32_t indices[6] = { 0, 1, 2, 3, 4, 5 };
	std::unique_ptr<bumpTriMeshBumpFrag> fragment(new bumpTriMeshBumpFrag(positions, normals, nullptr, nullptr, nullptr, 6,
																		  g3dIndexPtr(indices), 6, 6, nullptr, false, true));
	ASSERT_TRUE(fragment->GetIndexBuffer()->GetLockable());
	ASSERT_NE(0u, ensure(*fragment));
	const uint64_t generation = fragment->GetIndexBuffer()->GetGeneration();

	fragment->ComponentSort(maMatrix4x4(), maPoint3d(0, 0, -10));	// from below: the lower triangle first
	EXPECT_EQ(generation, fragment->GetIndexBuffer()->GetGeneration());
	const OptixTraversableHandle handle = ensure(*fragment);
	EXPECT_EQ(1, Cache().GetStats().m_Builds);
	EXPECT_EQ(0, Cache().GetStats().m_Refits);
	EXPECT_FLOAT_EQ(5.0f, trace_down(handle, 0, 0));
}

TEST_F(g3dGasCacheOptiXTest, SixteenAndThirtyTwoBitIndices)
{
	//	A grid with more than 65,535 vertices gets 32-bit indices; one
	//	triangle covering (0, 0) at z = -1 among degenerate ones
	const int count = 70000;
	std::vector<maPoint3d> positions(count, maPoint3d(50, 50, 0));
	positions[count - 3] = maPoint3d(-1, -1, -1);
	positions[count - 2] = maPoint3d(1, -1, -1);
	positions[count - 1] = maPoint3d(0, 1, -1);
	auto big = gtOx::Mesh(positions, { 0, 1, 2, uint32_t(count - 3), uint32_t(count - 2), uint32_t(count - 1) }, false);
	EXPECT_EQ(4, big->GetSizeOfIndex());
	const g3dGasCacheOptiX::Gas* gas = Cache().Ensure(*big);
	ASSERT_NE(nullptr, gas);
	EXPECT_EQ(4u, gas->m_Inputs[0].m_IndexSize);
	EXPECT_EQ(unsigned(count), gas->m_NumVertices);
	EXPECT_FLOAT_EQ(6.0f, trace_down(gas->m_Handle, 0, 0));

	auto small = gtOx::Triangle(0);
	EXPECT_EQ(2, small->GetSizeOfIndex());
	gas = Cache().Ensure(*small);
	ASSERT_NE(nullptr, gas);
	EXPECT_EQ(2u, gas->m_Inputs[0].m_IndexSize);
	EXPECT_FLOAT_EQ(5.0f, trace_down(gas->m_Handle, 0, 0));
}

TEST_F(g3dGasCacheOptiXTest, NoGasForLines)
{
	const maPoint3d positions[3] = { maPoint3d(-1, -1, 0), maPoint3d(1, -1, 0), maPoint3d(0, 1, 0) };
	const maPoint3d normals[3] = { maPoint3d(0, 0, 1), maPoint3d(0, 0, 1), maPoint3d(0, 0, 1) };
	const uint32_t indices[4] = { 0, 1, 1, 2 };
	gvk::bumpFragmentCreate create;
	std::unique_ptr<g3dFragment> lines(create.CreateLineList(positions, normals, 3, g3dIndexPtr(indices), 4, nullptr, false,
															 nullptr, nullptr, nullptr));
	const gvk::tmeshFrag* fragment = dynamic_cast<const gvk::tmeshFrag*>(lines.get());
	ASSERT_NE(nullptr, fragment);
	EXPECT_FALSE(g3dGasCacheOptiX::IsTraceable(*fragment));
	EXPECT_EQ(nullptr, Cache().Ensure(*fragment));
	EXPECT_EQ(0, Cache().GetStats().m_Builds);
}

TEST_F(g3dGasCacheOptiXTest, FreedFragmentDropsItsEntry)
{
	auto fragment = gtOx::Triangle(0);
	ASSERT_NE(0u, ensure(*fragment));
	EXPECT_EQ(1u, Cache().GetEntryCount());
	Cache().Collect();
	EXPECT_EQ(1u, Cache().GetEntryCount());

	fragment.reset();
	Cache().Collect();
	EXPECT_EQ(0u, Cache().GetEntryCount());
}
