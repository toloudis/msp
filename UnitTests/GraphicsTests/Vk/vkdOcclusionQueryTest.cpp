/*****************************************************************************
**  vkdOcclusionQueryTest.cpp
**
**      vkdOcclusionQuery as depth-peeled transparency uses it (D3D11's
**      occlusion query): the samples a 10x10 quad passes on a 64x64 target,
**      none when a nearer depth hides it, and one query reused, each result
**      read before the next Begin.
**
**	Extra Large Technology
\****************************************************************************/
#include "Vk/gtVulkanContext.hpp"

#ifndef GVK_VKDOCCLUSIONQUERY_HPP
#include "GraphicsVulkan/vkd/vkdOcclusionQuery.hpp"
#endif
#ifndef GVK_VKDSHADER_HPP
#include "GraphicsVulkan/vkd/vkdShader.hpp"
#endif

#include "gtSolidVS.h"
#include "gtSolidPS.h"

#include <memory>
#include <string>
#include <vector>

using namespace gvk;

namespace
{
	const uint32_t c_Size = 64;

	struct Vertex
	{
		float m_Pos[3];
		float m_Color[4];
	};

	//	A quad from (i_X0, i_Y0) to (i_X1, i_Y1) in clip space at depth i_Z
	std::vector<Vertex> quad(float i_X0, float i_Y0, float i_X1, float i_Y1, float i_Z)
	{
		const float p[6][2] = { { i_X0, i_Y1 }, { i_X1, i_Y1 }, { i_X0, i_Y0 }, { i_X0, i_Y0 }, { i_X1, i_Y1 }, { i_X1, i_Y0 } };
		std::vector<Vertex> v(6);
		for (int i = 0; i < 6; i++)
		{
			v[i] = { { p[i][0], p[i][1], i_Z }, { 1, 1, 1, 1 } };
		}
		return v;
	}

	class vkdOcclusionQueryTest : public gtVulkanContext
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
				m_pColor = Target(c_Size, c_Size);
				m_pDepth = vkdImage::Create2D(Device(), c_Size, c_Size, VK_FORMAT_D24_UNORM_S8_UINT,
					VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, "vkdOcclusionQueryTest depth",
					VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
			}

			void TearDown() override
			{
				if (m_pDevice)
				{
					Context().Flush();
					Device().WaitIdle();
				}
				m_pDepth.reset();
				m_pVS.reset();
				m_pPS.reset();
				m_pLayout.reset();
				gtVulkanContext::TearDown();
			}

			//	Targets bound, full viewport, solid shaders, depth cleared to 1
			void Begin()
			{
				vkdContext& c = Context();
				vkdImage* color = m_pColor;
				c.SetRenderTargets(1, &color, m_pDepth.get());
				vkdViewport viewport;
				viewport.m_Width = (float)c_Size;
				viewport.m_Height = (float)c_Size;
				c.SetViewports(1, &viewport);
				const vkdShader* stages[5] = { m_pVS.get(), nullptr, nullptr, nullptr, m_pPS.get() };
				c.SetShaders(stages);
				c.SetVertexLayout(m_pLayout.get());
				c.SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
				const float black[4] = { 0, 0, 0, 1 };
				c.ClearRenderTarget(m_pColor, black);
				c.ClearDepthStencil(m_pDepth.get(), true, 1.0f, true, 0);
				const float one[4] = { 1, 1, 1, 1 };
				vkdRingBuffer::Allocation tint = c.Upload(one, sizeof(one));
				c.Bindings().SetConstantBuffer(0, tint.m_Gpu, sizeof(one));
			}

			void Draw(const std::vector<Vertex>& i_Vertices)
			{
				vkdRingBuffer::Allocation vb = Context().Upload(i_Vertices.data(), i_Vertices.size() * sizeof(Vertex), 16);
				Context().SetVertexBuffer(0, vb.m_Buffer, vb.m_Offset);
				Context().Draw((uint32_t)i_Vertices.size(), 0);
			}

			std::unique_ptr<vkdShader>		m_pVS, m_pPS;
			std::unique_ptr<vkdVertexLayout> m_pLayout;
			vkdImage*						m_pColor = nullptr;
			std::unique_ptr<vkdImage>		m_pDepth;
	};

	//	10x10 pixels: 10/32 of clip space, from the centre
	const float c_Ten = 10.0f / 32.0f;
}

TEST_F(vkdOcclusionQueryTest, CountsSamples)
{
	vkdOcclusionQuery query(Device());
	const std::vector<Vertex> small = quad(0, 0, c_Ten, c_Ten, 0.5f);
	const std::vector<Vertex> nearer = quad(-1, -1, 1, 1, 0.25f);

	for (int pass = 0; pass < 3; pass++)
	{
		SCOPED_TRACE(pass);

		//	the quad alone
		Begin();
		query.Begin(Context());
		Draw(small);
		query.End(Context());
		EXPECT_EQ(100u, query.GetResult(Context()));

		//	behind a nearer depth (drawn before the query begins)
		Begin();
		Draw(nearer);
		query.Begin(Context());
		Draw(small);
		query.End(Context());
		EXPECT_EQ(0u, query.GetResult(Context()));
	}
}

TEST_F(vkdOcclusionQueryTest, SpansTargetChanges)
{
	//	DX11's peel loop begins the query, draws through a pass that binds
	//	its own target, and ends it: the count covers every draw between
	vkdOcclusionQuery query(Device());
	const std::vector<Vertex> small = quad(0, 0, c_Ten, c_Ten, 0.5f);
	Begin();
	query.Begin(Context());
	Draw(small);
	Begin();			// targets rebound and cleared: a new rendering scope
	Draw(quad(-c_Ten, -c_Ten, 0, 0, 0.5f));
	query.End(Context());
	EXPECT_EQ(200u, query.GetResult(Context()));
}
