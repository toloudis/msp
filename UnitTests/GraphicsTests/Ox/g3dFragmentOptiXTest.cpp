/*****************************************************************************
**  g3dFragmentOptiXTest.cpp
**
**      g3dFragmentOptiX's geometry BVH (GAS): built once and cached, refit
**	when a morphable fragment's vertices move, rebuilt when its indices
**	change or after c_MaxRefits refits, remade after Deallocate; none for
**	line lists, empty fragments or bad indices. Rays (optxRayQuery) check
**	the GAS holds the current geometry.
**
**	Extra Large Technology
\****************************************************************************/
#include "Ox/gtOptiX.hpp"

#include "GraphicsOptiX/G3d/g3dFragmentOptiX.hpp"
#include "GraphicsOptiX/optx/optxRayQuery.hpp"
#ifndef G3D_INDEXPTR_HPP
#include "Graphics/G3d/g3dIndexPtr.hpp"
#endif

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

using namespace gox;

namespace
{
	class g3dFragmentOptiXTest : public gtOptiX
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

			//	A ray down -z from (x, y, 5); its hit distance, or -1 for a miss
			float trace_down(OptixTraversableHandle i_Handle, float x, float y)
			{
				optxRayQueryRay ray = {};
				ray.m_Origin[0] = x;
				ray.m_Origin[1] = y;
				ray.m_Origin[2] = 5.0f;
				ray.m_Direction[2] = -1.0f;
				ray.m_TMax = 1e30f;
				ray.m_Mask = 0xFF;
				std::vector<optxRayQueryHit> hits;
				EXPECT_TRUE(m_Query.Trace(i_Handle, { ray }, hits));
				return hits.size() == 1 && hits[0].m_Hit ? hits[0].m_T : -1.0f;
			}

			optxRayQuery m_Query;
	};

	//	One triangle covering (0, 0), in the plane z
	std::vector<maPoint3d> triangle_at(float z)
	{
		return { maPoint3d(-1, -1, z), maPoint3d(1, -1, z), maPoint3d(0, 1, z) };
	}

	std::unique_ptr<g3dFragmentOptiX> make_fragment(const std::vector<maPoint3d>& i_Positions,
													const std::vector<uint32_t>& i_Indices,
													bool i_bMorphable,
													g3dFragmentOptiX::PrimitiveType i_Type = g3dFragmentOptiX::e_TriangleList)
	{
		return std::make_unique<g3dFragmentOptiX>(i_Positions.data(), nullptr, nullptr, nullptr, nullptr,
												  static_cast<int>(i_Positions.size()),
												  g3dIndexPtr(i_Indices.data()), static_cast<int>(i_Indices.size()),
												  nullptr, i_bMorphable, i_Type);
	}
}

TEST_F(g3dFragmentOptiXTest, StaticBuildsOnceAndIsCached)
{
	auto frag = make_fragment(triangle_at(0), { 0, 1, 2 }, false);
	const OptixTraversableHandle handle = frag->EnsureGAS();
	ASSERT_NE(0u, handle);
	EXPECT_EQ(handle, frag->EnsureGAS());
	EXPECT_EQ(1, frag->GetGasBuildCount());
	EXPECT_EQ(0, frag->GetGasRefitCount());
	EXPECT_EQ(1, frag->GetStore()->GetUploadCount());
	EXPECT_FLOAT_EQ(5.0f, trace_down(handle, 0, 0));
	EXPECT_EQ(-1.0f, trace_down(handle, 3, 3));
}

TEST_F(g3dFragmentOptiXTest, StaticRebuiltWhenVerticesChange)
{
	auto frag = make_fragment(triangle_at(0), { 0, 1, 2 }, false);
	ASSERT_NE(0u, frag->EnsureGAS());
	const std::vector<maPoint3d> moved = triangle_at(-1);
	frag->UpdateVertices(3, moved.data());
	const OptixTraversableHandle handle = frag->EnsureGAS();
	EXPECT_EQ(2, frag->GetGasBuildCount());
	EXPECT_EQ(0, frag->GetGasRefitCount());
	EXPECT_FLOAT_EQ(6.0f, trace_down(handle, 0, 0));
}

TEST_F(g3dFragmentOptiXTest, MorphableRefitWhenVerticesMove)
{
	auto frag = make_fragment(triangle_at(0), { 0, 1, 2 }, true);
	ASSERT_NE(0u, frag->EnsureGAS());

	//	Through Lock/Unlock, as skinning writes
	g3dType::BumpTex1Vertex* vertices = reinterpret_cast<g3dType::BumpTex1Vertex*>(frag->Lock());
	ASSERT_NE(nullptr, vertices);
	for (int i = 0; i < 3; ++i)
	{
		vertices[i].m_Vertex.SetZ(-2.0f);
	}
	frag->Unlock();

	const OptixTraversableHandle handle = frag->EnsureGAS();
	EXPECT_EQ(1, frag->GetGasBuildCount());
	EXPECT_EQ(1, frag->GetGasRefitCount());
	EXPECT_EQ(2, frag->GetStore()->GetUploadCount());
	EXPECT_FLOAT_EQ(7.0f, trace_down(handle, 0, 0));

	//	Nothing changed: nothing done
	frag->EnsureGAS();
	EXPECT_EQ(1, frag->GetGasRefitCount());
	EXPECT_EQ(2, frag->GetStore()->GetUploadCount());
}

TEST_F(g3dFragmentOptiXTest, MorphableRebuiltWhenIndicesChange)
{
	//	Two triangles; the indices switch from the first to the second
	std::vector<maPoint3d> positions = triangle_at(0);
	const std::vector<maPoint3d> lower = triangle_at(-3);
	positions.insert(positions.end(), lower.begin(), lower.end());
	auto frag = make_fragment(positions, { 0, 1, 2 }, true);
	ASSERT_NE(0u, frag->EnsureGAS());

	uint32_t* indices = reinterpret_cast<uint32_t*>(frag->LockIndices());
	indices[0] = 3;
	indices[1] = 4;
	indices[2] = 5;
	frag->UnlockIndices();

	const OptixTraversableHandle handle = frag->EnsureGAS();
	EXPECT_EQ(2, frag->GetGasBuildCount());
	EXPECT_EQ(0, frag->GetGasRefitCount());
	EXPECT_EQ(1, frag->GetStore()->GetUploadCount());
	EXPECT_FLOAT_EQ(8.0f, trace_down(handle, 0, 0));
}

TEST_F(g3dFragmentOptiXTest, MorphableRebuiltAfterMaxRefits)
{
	auto frag = make_fragment(triangle_at(0), { 0, 1, 2 }, true);
	ASSERT_NE(0u, frag->EnsureGAS());
	for (int i = 1; i <= g3dFragmentOptiX::c_MaxRefits + 1; ++i)
	{
		const std::vector<maPoint3d> moved = triangle_at(-0.01f * i);
		frag->UpdateVertices(3, moved.data());
		ASSERT_NE(0u, frag->EnsureGAS());
	}
	EXPECT_EQ(2, frag->GetGasBuildCount());
	EXPECT_EQ(g3dFragmentOptiX::c_MaxRefits, frag->GetGasRefitCount());
	EXPECT_NEAR(5.0f + 0.01f * (g3dFragmentOptiX::c_MaxRefits + 1), trace_down(frag->EnsureGAS(), 0, 0), 1e-4f);
}

TEST_F(g3dFragmentOptiXTest, DeallocateRemakesEverything)
{
	auto frag = make_fragment(triangle_at(0), { 0, 1, 2 }, false);
	ASSERT_NE(0u, frag->EnsureGAS());
	const uint64_t vertex_generation = frag->GetVertexGeneration();
	const uint64_t index_generation = frag->GetIndexGeneration();

	frag->Deallocate();
	EXPECT_NE(vertex_generation, frag->GetVertexGeneration());
	EXPECT_NE(index_generation, frag->GetIndexGeneration());
	EXPECT_EQ(0u, frag->GetDeviceIndices());

	const OptixTraversableHandle handle = frag->EnsureGAS();
	ASSERT_NE(0u, handle);
	EXPECT_EQ(2, frag->GetGasBuildCount());
	EXPECT_EQ(2, frag->GetStore()->GetUploadCount());
	EXPECT_FLOAT_EQ(5.0f, trace_down(handle, 0, 0));
}

TEST_F(g3dFragmentOptiXTest, NoGasWithoutTriangles)
{
	EXPECT_EQ(0u, make_fragment(triangle_at(0), { 0, 1, 1, 2 }, false, g3dFragmentOptiX::e_LineList)->EnsureGAS());
	EXPECT_EQ(0u, make_fragment(triangle_at(0), {}, false)->EnsureGAS());
	EXPECT_EQ(0u, make_fragment({}, {}, false)->EnsureGAS());

	//	An index past the last vertex
	auto bad = make_fragment(triangle_at(0), { 0, 1, 3 }, false);
	EXPECT_EQ(0u, bad->EnsureGAS());
	EXPECT_EQ(0, bad->GetGasBuildCount());
}

TEST_F(g3dFragmentOptiXTest, CopySharesNothing)
{
	auto frag = make_fragment(triangle_at(0), { 0, 1, 2 }, true);
	ASSERT_NE(0u, frag->EnsureGAS());
	g3dFragmentOptiX copy(*frag);
	EXPECT_NE(frag->GetStore(), copy.GetStore());
	EXPECT_NE(frag->GetVertexGeneration(), copy.GetVertexGeneration());
	EXPECT_NE(frag->GetIndexGeneration(), copy.GetIndexGeneration());
	EXPECT_EQ(3, copy.GetNumVertices());
	EXPECT_EQ(3, copy.GetNumIndices());

	const OptixTraversableHandle handle = copy.EnsureGAS();
	ASSERT_NE(0u, handle);
	EXPECT_EQ(1, copy.GetGasBuildCount());
	EXPECT_FLOAT_EQ(5.0f, trace_down(handle, 0, 0));
}
