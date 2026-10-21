/*****************************************************************************
**  fxEffectVkTest.cpp
**
**      fxEffectVk on the device: every embedded effect creates, manifest
**      sampler states map to Vulkan, and Apply() gets constants, textures and
**      samplers to the shaders (drawn with the Special/Solid effect), also
**      when the ring buffer memory of an earlier upload has been reused.
**
**	Extra Large Technology
\****************************************************************************/
#include "Vk/gtVulkan.hpp"

#ifndef GVK_EFFPLAINEFFECTVK_HPP
#include "GraphicsVulkan/Eff/effPlainEffectVk.hpp"
#endif
#ifndef GVK_FXEMBEDDEDSHADERSVK_HPP
#include "GraphicsVulkan/Fx/fxEmbeddedShadersVk.hpp"
#endif
#ifndef GVK_FXMATERIALINSTANCE_HPP
#include "GraphicsVulkan/Fx/fxMaterialInstance.hpp"
#endif
#ifndef GVK_VKDBINDINGS_HPP
#include "GraphicsVulkan/vkd/vkdBindings.hpp"
#endif
#ifndef GVK_VKDBUFFER_HPP
#include "GraphicsVulkan/vkd/vkdBuffer.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif

#include <cstring>
#include <memory>
#include <string>
#include <vector>

using namespace gvk;

namespace
{
	const uint32_t c_Size = 64;

	//	STANDARD_VERTEX (Materials/Globals.hlsli)
	struct Vertex
	{
		float m_Pos[3];
		float m_Normal[3];
		float m_UV[2];
		float m_Tangent[3];
		float m_Binormal[3];
	};

	const float c_Identity[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

	//	The whole target as two triangles, u from 0 (left) to 1 (right)
	std::vector<Vertex> full_quad()
	{
		const float p[6][2] = { { -1, 1 }, { 1, 1 }, { -1, -1 }, { -1, -1 }, { 1, 1 }, { 1, -1 } };
		std::vector<Vertex> v(6);
		for (int i = 0; i < 6; i++)
		{
			v[i] = { { p[i][0], p[i][1], 0.5f }, { 0, 0, 1 }, { (p[i][0] + 1) / 2, (1 - p[i][1]) / 2 }, { 1, 0, 0 }, { 0, 1, 0 } };
		}
		return v;
	}

	class fxEffectVkTest : public gtVulkan
	{
		protected:
			//	A small ring, so a few frames reuse its memory
			virtual VkDeviceSize RingBlockSize() const { return 1 << 20; }

			void SetUp() override
			{
				gtVulkan::SetUp();
				if (!m_pDevice)
				{
					return;
				}
				m_pBindings.reset(new vkdBindings(Device()));
				m_pRing.reset(new vkdRingBuffer(Device(), RingBlockSize()));
				m_pPipelines.reset(new vkdPipelineCache(Device(), *m_pBindings, ""));
				m_pContext.reset(new vkdContext(Device(), *m_pRing, *m_pBindings, *m_pPipelines));
				vkdGlobal::Set(m_pDevice.get(), m_pRing.get(), m_pBindings.get(), m_pPipelines.get(), m_pContext.get());
				m_pLayout.reset(new vkdVertexLayout({ { "SV_POSITION", 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 0 },
													  { "NORMAL", 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 12 },
													  { "TEXCOORD", 0, VK_FORMAT_R32G32_SFLOAT, 0, 24 },
													  { "TANGENT", 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 32 },
													  { "BINORMAL", 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 44 } }, { sizeof(Vertex) }));
				m_pTarget = vkdImage::Create2D(Device(), c_Size, c_Size, VK_FORMAT_R8G8B8A8_UNORM,
					VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, "fxEffectVkTest target");
			}

			void TearDown() override
			{
				m_pSolid.reset();
				m_pTarget.reset();
				m_pTexture.reset();
				vkdGlobal::Set(NULL, NULL, NULL, NULL, NULL);
				m_pContext.reset();
				m_pPipelines.reset();
				m_pRing.reset();
				m_pBindings.reset();
				gtVulkan::TearDown();
			}

			//	Special/Solid on the device, with identity transforms
			fxEffectVk& Solid()
			{
				if (!m_pSolid)
				{
					std::string error;
					m_pSolid = fxEmbeddedShadersVk::CreateVk(*fxEmbeddedShadersVk::Find("Solid"), &Device(), error);
					EXPECT_TRUE(m_pSolid != nullptr) << error;
					for (const char* name : { "g_wvp", "g_world", "g_uvTransform" })
					{
						m_pSolid->SetMatrix(m_pSolid->FindConstant(name), c_Identity);
					}
				}
				return *m_pSolid;
			}

			void SetColor(float r, float g, float b, float a)
			{
				const float color[4] = { r, g, b, a };
				Solid().SetFloatVector(Solid().FindConstant("solidColor"), color);
			}

			//	Clear to blue, Apply Solid's first pass, draw the full quad
			void Draw()
			{
				vkdContext& c = *m_pContext;
				vkdImage* target = m_pTarget.get();
				c.SetRenderTargets(1, &target, nullptr);
				vkdViewport viewport;
				viewport.m_Width = viewport.m_Height = (float)c_Size;
				c.SetViewports(1, &viewport);
				const float blue[4] = { 0, 0, 1, 1 };
				c.ClearRenderTarget(target, blue);
				Solid().Apply(Solid().FindTechnique("Default"), 0);
				c.SetVertexLayout(m_pLayout.get());
				c.SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
				const std::vector<Vertex> quad = full_quad();
				vkdRingBuffer::Allocation vb = c.Upload(quad.data(), quad.size() * sizeof(Vertex), 16);
				c.SetVertexBuffer(0, vb.m_Buffer, vb.m_Offset);
				c.Draw((uint32_t)quad.size(), 0);
			}

			std::vector<uint8_t> Read()
			{
				m_pContext->Flush();
				return m_pTarget->ReadBack();
			}

			//	A 4x1 R8G8B8A8 texture, uploaded through the ring
			vkdImage* Texture(const uint8_t (&i_Texels)[16])
			{
				m_pTexture = vkdImage::Create2D(Device(), 4, 1, VK_FORMAT_R8G8B8A8_UNORM,
					VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, "fxEffectVkTest texture");
				vkdRingBuffer::Allocation staging = m_pContext->Upload(i_Texels, sizeof(i_Texels), 16);
				VkBufferImageCopy region = {};
				region.bufferOffset = staging.m_Offset;
				region.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
				region.imageExtent = { 4, 1, 1 };
				vkCmdCopyBufferToImage(m_pContext->Commands(), staging.m_Buffer, m_pTexture->Image(), VK_IMAGE_LAYOUT_GENERAL, 1, &region);
				return m_pTexture.get();
			}

			static bool near_color(const std::vector<uint8_t>& i_Image, uint32_t x, uint32_t y, int r, int g, int b, int a)
			{
				const uint8_t* p = &i_Image[(y * c_Size + x) * 4];
				const int expected[4] = { r, g, b, a };
				for (int i = 0; i < 4; i++)
				{
					if (abs(p[i] - expected[i]) > 1)
					{
						return false;
					}
				}
				return true;
			}

			static std::string describe(const std::vector<uint8_t>& i_Image, uint32_t x, uint32_t y)
			{
				const uint8_t* p = &i_Image[(y * c_Size + x) * 4];
				return "(" + std::to_string(p[0]) + "," + std::to_string(p[1]) + "," + std::to_string(p[2]) + "," + std::to_string(p[3]) + ")";
			}

			std::unique_ptr<vkdBindings>		m_pBindings;
			std::unique_ptr<vkdRingBuffer>		m_pRing;
			std::unique_ptr<vkdPipelineCache>	m_pPipelines;
			std::unique_ptr<vkdContext>			m_pContext;
			std::unique_ptr<vkdVertexLayout>	m_pLayout;
			std::unique_ptr<vkdImage>			m_pTarget;
			std::unique_ptr<vkdImage>			m_pTexture;
			std::unique_ptr<fxEffectVk>			m_pSolid;
	};

	class fxEffectVkSmallRingTest : public fxEffectVkTest
	{
		protected:
			VkDeviceSize RingBlockSize() const override { return 4096; }
	};
}

TEST_F(fxEffectVkTest, AllEffectsCreateOnTheDevice)
{
	int deferred = 0;
	for (int e = 0; e < fxEmbeddedShadersVk::k_NumEffects; e++)
	{
		std::string error;
		std::unique_ptr<fxEffectVk> effect = fxEmbeddedShadersVk::CreateVk(fxEmbeddedShadersVk::k_Effects[e], &Device(), error);
		EXPECT_TRUE(effect != nullptr) << error;
		for (int t = 0; effect && t < effect->GetTechniqueCount(); t++)
		{
			for (int p = 0; p < effect->GetPassCount(t); p++)
			{
				deferred += effect->IsPassDeferred(t, p) ? 1 : 0;
			}
		}
	}
	EXPECT_GT(deferred, 0);		// the hair hull shader passes
	std::cout << "deferred passes (hair hull shader): " << deferred << std::endl;
}

TEST(fxSamplerToVulkan, Mapping)
{
	VkSamplerCreateInfo info;
	std::string error;

	fxSamplerDesc linear;		// the D3D defaults
	ASSERT_TRUE(fxSamplerToVulkan(linear, info, error)) << error;
	EXPECT_EQ(VK_FILTER_LINEAR, info.minFilter);
	EXPECT_EQ(VK_FILTER_LINEAR, info.magFilter);
	EXPECT_EQ(VK_SAMPLER_MIPMAP_MODE_LINEAR, info.mipmapMode);
	EXPECT_EQ(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, info.addressModeU);
	EXPECT_FALSE(info.anisotropyEnable);
	EXPECT_FALSE(info.compareEnable);
	EXPECT_EQ(0.0f, info.minLod);
	EXPECT_EQ(VK_LOD_CLAMP_NONE, info.maxLod);
	EXPECT_EQ(VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE, info.borderColor);

	fxSamplerDesc point;
	point.m_Filter = "min_mag_linear_mip_point";		// names are case-insensitive, as in Effects
	point.m_Address[0] = "Wrap";
	point.m_Address[1] = "MIRROR";
	point.m_Address[2] = "MIRROR_ONCE";
	ASSERT_TRUE(fxSamplerToVulkan(point, info, error)) << error;
	EXPECT_EQ(VK_FILTER_LINEAR, info.minFilter);
	EXPECT_EQ(VK_SAMPLER_MIPMAP_MODE_NEAREST, info.mipmapMode);
	EXPECT_EQ(VK_SAMPLER_ADDRESS_MODE_REPEAT, info.addressModeU);
	EXPECT_EQ(VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT, info.addressModeV);
	EXPECT_EQ(VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE, info.addressModeW);

	fxSamplerDesc minPoint;
	minPoint.m_Filter = "MIN_POINT_MAG_LINEAR_MIP_POINT";
	ASSERT_TRUE(fxSamplerToVulkan(minPoint, info, error)) << error;
	EXPECT_EQ(VK_FILTER_NEAREST, info.minFilter);
	EXPECT_EQ(VK_FILTER_LINEAR, info.magFilter);

	fxSamplerDesc aniso;
	aniso.m_Filter = "ANISOTROPIC";
	aniso.m_MaxAnisotropy = 16;
	ASSERT_TRUE(fxSamplerToVulkan(aniso, info, error)) << error;
	EXPECT_TRUE(info.anisotropyEnable);
	EXPECT_EQ(16.0f, info.maxAnisotropy);

	fxSamplerDesc shadow;
	shadow.m_Filter = "COMPARISON_MIN_MAG_MIP_LINEAR";
	shadow.m_ComparisonFunc = "LESS_EQUAL";
	shadow.m_Address[0] = shadow.m_Address[1] = "BORDER";
	for (float& c : shadow.m_BorderColor)
	{
		c = 0.0f;
	}
	shadow.m_MinLOD = 1.0f;
	shadow.m_MaxLOD = 4.0f;
	ASSERT_TRUE(fxSamplerToVulkan(shadow, info, error)) << error;
	EXPECT_TRUE(info.compareEnable);
	EXPECT_EQ(VK_COMPARE_OP_LESS_OR_EQUAL, info.compareOp);
	EXPECT_EQ(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER, info.addressModeU);
	EXPECT_EQ(VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK, info.borderColor);
	EXPECT_EQ(1.0f, info.minLod);
	EXPECT_EQ(4.0f, info.maxLod);

	fxSamplerDesc oddBorder = shadow;
	oddBorder.m_BorderColor[0] = 0.5f;
	EXPECT_FALSE(fxSamplerToVulkan(oddBorder, info, error));
	EXPECT_NE(std::string::npos, error.find("border")) << error;

	fxSamplerDesc badFilter;
	badFilter.m_Filter = "BILINEAR";
	EXPECT_FALSE(fxSamplerToVulkan(badFilter, info, error));
}

TEST_F(fxEffectVkTest, ConstantsReachTheShader)
{
	SetColor(0.25f, 0.5f, 0.75f, 1.0f);
	Draw();
	const std::vector<uint8_t> image = Read();
	EXPECT_TRUE(near_color(image, 32, 32, 64, 128, 191, 255)) << describe(image, 32, 32);
	EXPECT_TRUE(near_color(image, 0, 63, 64, 128, 191, 255)) << describe(image, 0, 63);
}

//	Constants uploaded for frame 1 and not changed since are uploaded again
//	for later frames: the ring has reused frame 1's memory by then
TEST_F(fxEffectVkSmallRingTest, UnchangedConstantsSurviveRingReuse)
{
	SetColor(0.25f, 0.5f, 0.75f, 1.0f);
	Draw();
	Read();
	for (int frame = 0; frame < 4; frame++)
	{
		std::vector<uint8_t> filler(3000, 0xAB);		// fills the 4 KB blocks with garbage
		m_pContext->Upload(filler.data(), filler.size(), 16);
		m_pContext->Upload(filler.data(), filler.size(), 16);
		Device().WaitFor(m_pContext->Flush());
	}
	EXPECT_GT(m_pRing->BlockCount(), 1u);
	Draw();
	const std::vector<uint8_t> image = Read();
	EXPECT_TRUE(near_color(image, 32, 32, 64, 128, 191, 255)) << describe(image, 32, 32);
}

TEST_F(fxEffectVkTest, MaterialInstanceRestores)
{
	fxMaterialInstance red(&Solid()), green(&Solid());
	ASSERT_TRUE(red.IsValid());
	red.Restore();
	SetColor(1, 0, 0, 1);
	red.Capture();
	green.Restore();
	SetColor(0, 1, 0, 1);
	green.Capture();

	red.Restore();
	Draw();
	std::vector<uint8_t> image = Read();
	EXPECT_TRUE(near_color(image, 32, 32, 255, 0, 0, 255)) << describe(image, 32, 32);
	green.Restore();
	Draw();
	image = Read();
	EXPECT_TRUE(near_color(image, 32, 32, 0, 255, 0, 255)) << describe(image, 32, 32);
}

//	Solid's opacity map: the alpha test discards where the texture's red is
//	below g_AlphaTestRef (left half), so the texture and sampler are bound
TEST_F(fxEffectVkTest, TextureAndSamplerBound)
{
	const uint8_t texels[16] = { 0, 0, 0, 255, 0, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255 };
	fxEffectVk& solid = Solid();
	solid.SetResource(solid.FindResource("TransparencyMap"), Texture(texels));
	solid.SetConstant(solid.FindConstant("hasTransparencyMap"), (uint32_t)1);
	solid.SetConstant(solid.FindConstant("g_Transparency"), 1.0f);
	solid.SetConstant(solid.FindConstant("g_AlphaTestRef"), 0.5f);
	SetColor(1, 1, 0, 1);
	Draw();
	const std::vector<uint8_t> image = Read();
	EXPECT_TRUE(near_color(image, 2, 32, 0, 0, 255, 255)) << "left: " << describe(image, 2, 32);
	EXPECT_TRUE(near_color(image, 61, 32, 255, 255, 0, 255)) << "right: " << describe(image, 61, 32);

	// a NULL texture reads as zeros, as in D3D11: everything is discarded
	solid.SetResource(solid.FindResource("TransparencyMap"), nullptr);
	Draw();
	const std::vector<uint8_t> none = Read();
	EXPECT_TRUE(near_color(none, 61, 32, 0, 0, 255, 255)) << describe(none, 61, 32);
}

//	A texture destroyed while an effect still has it bound (a material's map
//	released while the effect variable keeps it, as shdwPassAlphaFill's
//	shared effect did): D3D11 keeps the view alive by its reference; here
//	the effect lets it go, so it reads as the null texture rather than a
//	freed image
TEST_F(fxEffectVkTest, DestroyedTextureIsUnbound)
{
	const uint8_t texels[16] = { 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255 };
	fxEffectVk& solid = Solid();
	const fxEffectVk::Resource map = solid.FindResource("TransparencyMap");
	solid.SetResource(map, Texture(texels));
	solid.SetConstant(solid.FindConstant("hasTransparencyMap"), (uint32_t)1);
	solid.SetConstant(solid.FindConstant("g_Transparency"), 1.0f);
	solid.SetConstant(solid.FindConstant("g_AlphaTestRef"), 0.5f);
	SetColor(1, 1, 0, 1);
	Draw();
	const std::vector<uint8_t> drawn = Read();
	EXPECT_TRUE(near_color(drawn, 32, 32, 255, 255, 0, 255)) << describe(drawn, 32, 32);

	m_pTexture.reset();
	EXPECT_EQ(nullptr, solid.GetResource(map));
	Draw();
	const std::vector<uint8_t> image = Read();
	EXPECT_TRUE(near_color(image, 32, 32, 0, 0, 255, 255)) << describe(image, 32, 32);
}

//	A StructuredBuffer variable (HairDefault's g_HairGeometry, at t38) takes a
//	vkdBuffer through the fxEffect API, and a texture variable doesn't (nor
//	does the buffer variable take an image), as DX11's type check refuses.
//	A buffer destroyed while bound is let go, as images are: the effect then
//	binds the null buffer, and drawing stays clean
TEST_F(fxEffectVkTest, BufferVariableBindsBuffer)
{
	std::string error;
	std::unique_ptr<fxEffectVk> created = fxEmbeddedShadersVk::CreateVk(*fxEmbeddedShadersVk::Find("HairDefault"), &Device(), error);
	ASSERT_TRUE(created != nullptr) << error;
	fxEffectVk* hair = created.get();
	effPlainEffectVk effect(std::move(created), "Special/HairDefault.effect.json");
	const fxEffectVk::Resource geometry_handle = hair->FindResource("g_HairGeometry");
	ASSERT_TRUE(geometry_handle.IsValid());
	ASSERT_TRUE(hair->FindResource("depthMap").IsValid());

	const float points[2][3] = { { -0.5f, 0, 0.5f }, { 0.5f, 0, 0.5f } };
	std::unique_ptr<vkdBuffer> buffer = vkdBuffer::Create(Device(), sizeof(points), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, "g_HairGeometry");
	buffer->Upload(*m_pContext, 0, points, sizeof(points));
	fxEffectVariable* geometry = effect.GetVariableByName("g_HairGeometry")->AsShaderResource();
	fxEffectVariable* depth = effect.GetVariableByName("depthMap")->AsShaderResource();
	EXPECT_TRUE(geometry->SetResource(buffer.get()));
	EXPECT_EQ(buffer.get(), hair->GetBuffer(geometry_handle));
	EXPECT_FALSE(depth->SetResource(buffer.get()));
	std::unique_ptr<vkdImage> image = vkdImage::Create2D(Device(), 1, 1, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT, "image");
	EXPECT_FALSE(geometry->SetResource(image.get()));
	EXPECT_EQ(buffer.get(), hair->GetBuffer(geometry_handle));
	EXPECT_TRUE(depth->SetResource(image.get()));
	EXPECT_TRUE(depth->SetResource(NULL));		// D3D code unbinds with NULL, of either kind
	EXPECT_TRUE(geometry->SetResource(NULL));
	EXPECT_EQ(nullptr, hair->GetBuffer(geometry_handle));
	EXPECT_TRUE(geometry->SetResource(buffer.get()));

	//	Simple / PLines: VS, GS and PS (no hull shader); the VS reads g_HairGeometry
	const int technique = hair->FindTechnique("Simple");
	auto draw = [&]()
	{
		vkdContext& c = *m_pContext;
		vkdImage* target = m_pTarget.get();
		c.SetRenderTargets(1, &target, nullptr);
		vkdViewport viewport;
		viewport.m_Width = viewport.m_Height = (float)c_Size;
		c.SetViewports(1, &viewport);
		const float blue[4] = { 0, 0, 1, 1 };
		c.ClearRenderTarget(target, blue);
		hair->Apply(technique, 2);		// PDefault, PTess, PLines, PLinesTess
		c.SetVertexLayout(nullptr);
		c.SetTopology(VK_PRIMITIVE_TOPOLOGY_LINE_STRIP);
		c.Draw(2, 0);
		m_pContext->Flush();
	};
	draw();
	buffer.reset();
	EXPECT_EQ(nullptr, hair->GetBuffer(geometry_handle));
	draw();
	Device().WaitIdle();
	EXPECT_EQ(0, m_Validation.Stats().m_Errors);
	EXPECT_EQ(0, m_Validation.Stats().m_Warnings);
}
