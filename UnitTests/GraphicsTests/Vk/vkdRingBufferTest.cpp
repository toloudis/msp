/*****************************************************************************
**  vkdRingBufferTest.cpp
**
**      vkdRingBuffer: aligned, distinct allocations; nothing reused while
**      the GPU may still read it; growing when full.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtVulkan.hpp"

#ifndef GVK_VKDRINGBUFFER_HPP
#include "GraphicsVulkan/vkd/vkdRingBuffer.hpp"
#endif

#include <map>
#include <set>

using namespace gvk;

TEST_F(gtVulkan, RingAllocationsAreAlignedAndDistinct)
{
	vkdRingBuffer ring(Device(), 64 * 1024);
	std::set<VkDeviceAddress> seen;
	for (int i = 0; i < 20; ++i)
	{
		vkdRingBuffer::Allocation a = ring.Allocate(100 + i);
		ASSERT_NE(nullptr, a.m_pCpu);
		EXPECT_EQ(0u, a.m_Gpu % 256) << "allocation " << i;
		EXPECT_EQ(0u, a.m_Offset % 256);
		EXPECT_TRUE(seen.insert(a.m_Gpu).second) << "allocation " << i << " reused an address";
	}
	vkdRingBuffer::Allocation small = ring.Allocate(4, 16);
	EXPECT_EQ(0u, small.m_Gpu % 16);
}

TEST_F(gtVulkan, RingDoesNotReuseWhileInFlight)
{
	vkdRingBuffer ring(Device(), 1024);
	//	An address may be handed out again only once the GPU has finished
	//	the submission that last used it.
	std::map<VkDeviceAddress, uint64_t> last_use;
	for (int frame = 0; frame < 20; ++frame)
	{
		for (int i = 0; i < 3; ++i)
		{
			vkdRingBuffer::Allocation a = ring.Allocate(256);
			auto previous = last_use.find(a.m_Gpu);
			if (previous != last_use.end())
			{
				EXPECT_GE(Device().CompletedValue(), previous->second) << "frame " << frame << " reused memory in flight";
			}
			last_use[a.m_Gpu] = Device().PendingValue();
		}
		Device().Commands();
		Device().Submit();
	}

	//	Nothing submitted: everything handed out must be distinct.
	std::set<VkDeviceAddress> open;
	for (int i = 0; i < 12; ++i)
	{
		EXPECT_TRUE(open.insert(ring.Allocate(256).m_Gpu).second) << "allocation " << i;
	}
}

TEST_F(gtVulkan, RingReusesAfterGpuFinished)
{
	vkdRingBuffer ring(Device(), 1024);
	//	when the GPU keeps up, blocks are recycled instead of added
	for (int i = 0; i < 10; ++i)
	{
		ring.Allocate(1024);
		Device().Commands();
		Device().WaitFor(Device().Submit());
	}
	EXPECT_LE(ring.BlockCount(), 2u);
}

TEST_F(gtVulkan, RingGrowsForLargeAllocations)
{
	vkdRingBuffer ring(Device(), 1024);
	vkdRingBuffer::Allocation big = ring.Allocate(10 * 1024);
	ASSERT_NE(nullptr, big.m_pCpu);
	memset(big.m_pCpu, 0x5a, 10 * 1024);
}
