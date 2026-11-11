/*****************************************************************************
**  optxContextTest.cpp
**
**      GraphicsOptiX's foundation: the CUDA and OptiX contexts, device
**	buffers, and the embedded ray query program traced against a
**	one-triangle GAS (so nvcc's OptiX IR loads and runs on the driver).
**
**	Extra Large Technology
\****************************************************************************/
#include "Ox/gtOptiX.hpp"

#include "GraphicsOptiX/cu/cuBuffer.hpp"
#include "GraphicsOptiX/optx/optxCheck.hpp"
#include "GraphicsOptiX/optx/optxRayQuery.hpp"

#include <cstdint>
#include <string>
#include <vector>

using namespace gox;

namespace
{
	class optxContextTest : public gtOptiX
	{
	};

	//	A GAS over triangles (float3 positions, uint3 indices)
	struct TestGas
	{
		cuBuffer				m_Vertices;
		cuBuffer				m_Indices;
		cuBuffer				m_Output;
		OptixTraversableHandle	m_Handle = 0;

		bool Build(const std::vector<float>& i_Positions, const std::vector<uint32_t>& i_Indices)
		{
			if (!m_Vertices.Upload(i_Positions.data(), i_Positions.size() * sizeof(float)) ||
				!m_Indices.Upload(i_Indices.data(), i_Indices.size() * sizeof(uint32_t)))
			{
				return false;
			}
			CUdeviceptr vertices = m_Vertices.Get();
			const unsigned int flags = OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT;
			OptixBuildInput input = {};
			input.type = OPTIX_BUILD_INPUT_TYPE_TRIANGLES;
			input.triangleArray.vertexFormat = OPTIX_VERTEX_FORMAT_FLOAT3;
			input.triangleArray.vertexStrideInBytes = 3 * sizeof(float);
			input.triangleArray.numVertices = static_cast<unsigned int>(i_Positions.size() / 3);
			input.triangleArray.vertexBuffers = &vertices;
			input.triangleArray.indexFormat = OPTIX_INDICES_FORMAT_UNSIGNED_INT3;
			input.triangleArray.indexStrideInBytes = 3 * sizeof(uint32_t);
			input.triangleArray.numIndexTriplets = static_cast<unsigned int>(i_Indices.size() / 3);
			input.triangleArray.indexBuffer = m_Indices.Get();
			input.triangleArray.flags = &flags;
			input.triangleArray.numSbtRecords = 1;

			OptixAccelBuildOptions options = {};
			options.buildFlags = OPTIX_BUILD_FLAG_PREFER_FAST_TRACE;
			options.operation = OPTIX_BUILD_OPERATION_BUILD;
			OptixAccelBufferSizes sizes = {};
			if (OPTIX_CHECK(optixAccelComputeMemoryUsage(optxContext::GetOptix(), &options, &input, 1, &sizes)))
			{
				return false;
			}
			cuBuffer temp;
			if (!temp.Reserve(sizes.tempSizeInBytes) || !m_Output.Reserve(sizes.outputSizeInBytes))
			{
				return false;
			}
			return !OPTIX_CHECK(optixAccelBuild(optxContext::GetOptix(), optxContext::GetStream(), &options, &input, 1,
												temp.Get(), temp.GetSize(), m_Output.Get(), m_Output.GetSize(),
												&m_Handle, NULL, 0)) &&
				   optxContext::Synchronize();
		}
	};

	optxRayQueryRay make_ray(float x, float y, float z, float dx, float dy, float dz, float tmax = 1e30f)
	{
		optxRayQueryRay ray = {};
		ray.m_Origin[0] = x;
		ray.m_Origin[1] = y;
		ray.m_Origin[2] = z;
		ray.m_Direction[0] = dx;
		ray.m_Direction[1] = dy;
		ray.m_Direction[2] = dz;
		ray.m_TMin = 0.0f;
		ray.m_TMax = tmax;
		ray.m_Mask = 0xFF;
		return ray;
	}
}

TEST_F(optxContextTest, DeviceIsChosen)
{
	EXPECT_FALSE(optxContext::GetDeviceName().empty());
	EXPECT_NE(nullptr, optxContext::GetOptix());
	EXPECT_NE(nullptr, optxContext::GetStream());
}

TEST_F(optxContextTest, BufferRoundTrip)
{
	std::vector<uint32_t> data(1 << 18);
	for (size_t i = 0; i < data.size(); ++i)
	{
		data[i] = static_cast<uint32_t>(i * 2654435761u);
	}
	cuBuffer buffer;
	ASSERT_TRUE(buffer.Upload(data.data(), data.size() * sizeof(uint32_t)));
	EXPECT_EQ(data.size() * sizeof(uint32_t), buffer.GetSize());

	std::vector<uint32_t> back(data.size());
	ASSERT_TRUE(buffer.Download(back.data(), back.size() * sizeof(uint32_t)));
	EXPECT_EQ(data, back);

	//	A smaller Reserve keeps the allocation; Clear zeroes only what is in use
	ASSERT_TRUE(buffer.Reserve(16 * sizeof(uint32_t)));
	EXPECT_EQ(data.size() * sizeof(uint32_t), buffer.GetCapacity());
	ASSERT_TRUE(buffer.Clear());
	std::vector<uint32_t> cleared(16, 1);
	ASSERT_TRUE(buffer.Download(cleared.data(), cleared.size() * sizeof(uint32_t)));
	EXPECT_EQ(std::vector<uint32_t>(16, 0), cleared);
}

TEST_F(optxContextTest, RayQueryHitsTriangle)
{
	optxRayQuery query;
	std::string error;
	ASSERT_TRUE(query.Create(error)) << error;

	//	Two triangles in z = 0 and z = -2, both covering (0, 0)
	TestGas gas;
	ASSERT_TRUE(gas.Build({ -1, -1, 0,  1, -1, 0,  0, 1, 0,
							-1, -1, -2, 1, -1, -2, 0, 1, -2 },
						  { 0, 1, 2, 3, 4, 5 }));

	const std::vector<optxRayQueryRay> rays = {
		make_ray(0, 0, 5, 0, 0, -1),		// hits the first at t = 5
		make_ray(0, 0, -5, 0, 0, 1),		// from below: the second at t = 3
		make_ray(5, 5, 5, 0, 0, -1),		// misses
		make_ray(0, 0, 5, 0, 0, -1, 4.0f),	// stops short
	};
	std::vector<optxRayQueryHit> hits;
	ASSERT_TRUE(query.Trace(gas.m_Handle, rays, hits));
	ASSERT_EQ(rays.size(), hits.size());

	EXPECT_EQ(1u, hits[0].m_Hit);
	EXPECT_FLOAT_EQ(5.0f, hits[0].m_T);
	EXPECT_EQ(0u, hits[0].m_PrimitiveIndex);
	EXPECT_EQ(~0u, hits[0].m_InstanceId);	// a GAS traced directly: no instance

	EXPECT_EQ(1u, hits[1].m_Hit);
	EXPECT_FLOAT_EQ(3.0f, hits[1].m_T);
	EXPECT_EQ(1u, hits[1].m_PrimitiveIndex);

	EXPECT_EQ(0u, hits[2].m_Hit);
	EXPECT_EQ(0u, hits[3].m_Hit);
}
