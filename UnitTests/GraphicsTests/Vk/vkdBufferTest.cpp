/*****************************************************************************
**  vkdBufferTest.cpp
**
**      vkdBuffer: a device-local buffer uploaded through the ring feeds a
**      draw; a buffer destroyed right after the draw that reads it stays
**      alive until the GPU is done (validation, synchronization included,
**      is on and must stay silent).
**
**	Extra Large Technology
\****************************************************************************/
#include "Vk/gtVulkanContext.hpp"

#ifndef GVK_VKDBUFFER_HPP
#include "GraphicsVulkan/vkd/vkdBuffer.hpp"
#endif
#ifndef GVK_VKDSHADER_HPP
#include "GraphicsVulkan/vkd/vkdShader.hpp"
#endif

#include "gtSolidVS.h"
#include "gtSolidPS.h"

#include <cstring>
#include <memory>
#include <string>
#include <vector>

using namespace gvk;

namespace
{
	const uint32_t c_Size = 32;

	struct Vertex
	{
		float m_Pos[3];
		float m_Color[4];
	};

	//	A quad from i_X0 to i_X1 (clip space), full height, as a triangle list
	std::vector<Vertex> quad(const float i_Color[4], float i_X0, float i_X1)
	{
		std::vector<Vertex> v(6);
		const float p[6][2] = { { i_X0, 1 }, { i_X1, 1 }, { i_X0, -1 }, { i_X0, -1 }, { i_X1, 1 }, { i_X1, -1 } };
		for (int i = 0; i < 6; i++)
		{
			v[i] = { { p[i][0], p[i][1], 0.5f }, { i_Color[0], i_Color[1], i_Color[2], i_Color[3] } };
		}
		return v;
	}

	uint32_t pixel(const std::vector<uint8_t>& i_Image, uint32_t i_X, uint32_t i_Y)
	{
		uint32_t p;
		memcpy(&p, &i_Image[(i_Y * c_Size + i_X) * 4], 4);
		return p;
	}

	const uint32_t c_Red = 0xFF0000FF, c_Green = 0xFF00FF00, c_Blue = 0xFFFF0000;

	class vkdBufferTest : public gtVulkanContext
	{
		protected:
			void SetUp() override
			{
				gtVulkanContext::SetUp();
				if (!m_pDevice)
				{
					return;
				}
				std::string error;
				m_pVS = vkdShader::Create(&Device(), VK_SHADER_STAGE_VERTEX_BIT, g_gtSolidVS, sizeof(g_gtSolidVS), "VSMain", "gtSolid VS", error);
				m_pPS = vkdShader::Create(&Device(), VK_SHADER_STAGE_FRAGMENT_BIT, g_gtSolidPS, sizeof(g_gtSolidPS), "PSMain", "gtSolid PS", error);
				ASSERT_TRUE(m_pVS && m_pPS) << error;
				m_pLayout.reset(new vkdVertexLayout({ { "POSITION", 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 0 },
													  { "COLOR", 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0, 12 } }, { sizeof(Vertex) }));
			}

			void TearDown() override
			{
				m_pVS.reset();
				m_pPS.reset();
				gtVulkanContext::TearDown();
			}

			//	Target bound, cleared to blue, solid shaders, white tint
			vkdImage* Begin()
			{
				vkdImage* target = Target(c_Size, c_Size);
				vkdContext& c = Context();
				c.SetRenderTargets(1, &target, nullptr);
				vkdViewport viewport;
				viewport.m_Width = viewport.m_Height = (float)c_Size;
				c.SetViewports(1, &viewport);
				const vkdShader* stages[5] = { m_pVS.get(), nullptr, nullptr, nullptr, m_pPS.get() };
				c.SetShaders(stages);
				c.SetVertexLayout(m_pLayout.get());
				c.SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
				const float blue[4] = { 0, 0, 1, 1 };
				c.ClearRenderTarget(target, blue);
				const float one[4] = { 1, 1, 1, 1 };
				c.Bindings().SetConstantBuffer(0, c.Upload(one, sizeof(one)).m_Gpu, sizeof(one));
				return target;
			}

			std::unique_ptr<vkdBuffer> VertexBuffer(const std::vector<Vertex>& i_Vertices)
			{
				const VkDeviceSize size = i_Vertices.size() * sizeof(Vertex);
				std::unique_ptr<vkdBuffer> buffer = vkdBuffer::Create(Device(), size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, "vkdBufferTest vertices");
				buffer->Upload(Context(), 0, i_Vertices.data(), size);
				return buffer;
			}

			std::unique_ptr<vkdShader>			m_pVS, m_pPS;
			std::unique_ptr<vkdVertexLayout>	m_pLayout;
	};
}

TEST_F(vkdBufferTest, UploadThenDraw)
{
	vkdImage* target = Begin();
	const float red[4] = { 1, 0, 0, 1 };
	std::vector<Vertex> vertices = quad(red, -1, 0);
	const VkDeviceSize size = vertices.size() * sizeof(Vertex);
	//	Uploaded in two parts, at an offset, as DX11's UpdateSubresource with a box can
	std::unique_ptr<vkdBuffer> buffer = vkdBuffer::Create(Device(), size + 64, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, "UploadThenDraw");
	EXPECT_EQ(size + 64, buffer->Size());
	EXPECT_NE(0u, buffer->Address());
	buffer->Upload(Context(), 64, vertices.data(), 3 * sizeof(Vertex));
	buffer->Upload(Context(), 64 + 3 * sizeof(Vertex), &vertices[3], 3 * sizeof(Vertex));
	Context().SetVertexBuffer(0, buffer->Buffer(), 64);
	Context().Draw(6, 0);
	const std::vector<uint8_t> image = Read(target);
	EXPECT_EQ(c_Red, pixel(image, 4, 16));
	EXPECT_EQ(c_Blue, pixel(image, 28, 16));
}

TEST_F(vkdBufferTest, DestroyAfterDrawIsSafe)
{
	vkdImage* target = Begin();
	const float red[4] = { 1, 0, 0, 1 }, green[4] = { 0, 1, 0, 1 };
	{
		std::unique_ptr<vkdBuffer> left = VertexBuffer(quad(red, -1, 0));
		Context().SetVertexBuffer(0, left->Buffer(), 0);
		Context().Draw(6, 0);
	}	//	destroyed before the draw is even submitted
	//	A new buffer may get the old one's memory; the first draw must still read red
	std::unique_ptr<vkdBuffer> right = VertexBuffer(quad(green, 0, 1));
	Context().SetVertexBuffer(0, right->Buffer(), 0);
	Context().Draw(6, 0);
	right.reset();
	const std::vector<uint8_t> image = Read(target);
	EXPECT_EQ(c_Red, pixel(image, 4, 16));
	EXPECT_EQ(c_Green, pixel(image, 28, 16));
}
