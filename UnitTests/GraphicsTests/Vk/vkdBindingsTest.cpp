/*****************************************************************************
**  vkdBindingsTest.cpp
**
**      The descriptor-model prototype: a shader using DX-style registers
**      (b0, b3, t0, t37, s13), compiled from HLSL by DXC with the
**      GraphicsVulkan register shifts, draws with resources bound through
**      vkdBindings (descriptor heap + per-draw binding table). Runs with
**      core, synchronization and GPU-assisted validation.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtVulkan.hpp"

#ifndef GVK_VKDBINDINGS_HPP
#include "GraphicsVulkan/vkd/vkdBindings.hpp"
#endif
#ifndef GVK_VKDRINGBUFFER_HPP
#include "GraphicsVulkan/vkd/vkdRingBuffer.hpp"
#endif
#ifndef GVK_VKDBUFFER_HPP
#include "GraphicsVulkan/vkd/vkdBuffer.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef GVK_VKDINFO_HPP
#include "GraphicsVulkan/vkd/vkdInfo.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif

#include "gtBindingProtoVS.h"	// g_gtBindingProtoVS (SPIR-V, built from Shaders/gtBindingProto.hlsl)
#include "gtBindingProtoPS.h"	// g_gtBindingProtoPS
#include "gtStructuredReadVS.h"	// g_gtStructuredReadVS (Shaders/gtStructuredRead.hlsl)
#include "gtStructuredReadPS.h"	// g_gtStructuredReadPS

#include <cstring>

using namespace gvk;

namespace
{
	struct Float4 { float x, y, z, w; };

	class gtBindings : public gtVulkan
	{
		protected:
			bool UseGpuAssistedValidation() const override { return true; }

			void SetUp() override
			{
				gtVulkan::SetUp();
				if (!m_pDevice)
				{
					return;
				}
				m_pRing.reset(new vkdRingBuffer(Device(), 64 * 1024));
				m_pBindings.reset(new vkdBindings(Device()));
				m_Pipeline = make_pipeline(g_gtBindingProtoVS, sizeof(g_gtBindingProtoVS),
											g_gtBindingProtoPS, sizeof(g_gtBindingProtoPS), VK_FORMAT_R8G8B8A8_UNORM);
			}

			void TearDown() override
			{
				if (m_pDevice)
				{
					Device().WaitIdle();
					vkDestroyPipeline(Device().Device(), m_Pipeline, NULL);
					m_pBindings.reset();
					m_pRing.reset();
				}
				gtVulkan::TearDown();
			}

			VkShaderModule module(const unsigned char* i_pCode, size_t i_Size)
			{
				VkShaderModuleCreateInfo info = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
				info.codeSize = i_Size;
				info.pCode = reinterpret_cast<const uint32_t*>(i_pCode);
				VkShaderModule module = VK_NULL_HANDLE;
				VK_CHECK(vkCreateShaderModule(Device().Device(), &info, NULL, &module));
				return module;
			}

			VkPipeline make_pipeline(const unsigned char* i_pVS, size_t i_VSSize, const unsigned char* i_pPS, size_t i_PSSize,
									 VkFormat i_ColorFormat)
			{
				VkShaderModule vs = module(i_pVS, i_VSSize);
				VkShaderModule ps = module(i_pPS, i_PSSize);
				VkPipelineShaderStageCreateInfo stages[2] = {};
				stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				stages[0].pNext = &m_pBindings->ShaderMapping();
				stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
				stages[0].module = vs;
				stages[0].pName = "VSMain";
				stages[1] = stages[0];
				stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				stages[1].module = ps;
				stages[1].pName = "PSMain";

				VkPipelineVertexInputStateCreateInfo vertex = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
				VkPipelineInputAssemblyStateCreateInfo assembly = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
				assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
				VkPipelineViewportStateCreateInfo viewport = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
				viewport.viewportCount = 1;
				viewport.scissorCount = 1;
				VkPipelineRasterizationStateCreateInfo raster = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
				raster.polygonMode = VK_POLYGON_MODE_FILL;
				raster.cullMode = VK_CULL_MODE_NONE;
				raster.lineWidth = 1.0f;
				VkPipelineMultisampleStateCreateInfo multisample = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
				multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
				VkPipelineColorBlendAttachmentState attachment = {};
				attachment.colorWriteMask = 0xF;
				VkPipelineColorBlendStateCreateInfo blend = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
				blend.attachmentCount = 1;
				blend.pAttachments = &attachment;
				VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
				VkPipelineDynamicStateCreateInfo dynamic = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
				dynamic.dynamicStateCount = 2;
				dynamic.pDynamicStates = dynamic_states;

				const VkFormat color_format = i_ColorFormat;
				VkPipelineRenderingCreateInfo rendering = {VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
				rendering.colorAttachmentCount = 1;
				rendering.pColorAttachmentFormats = &color_format;
				VkPipelineCreateFlags2CreateInfo flags = {VK_STRUCTURE_TYPE_PIPELINE_CREATE_FLAGS_2_CREATE_INFO};
				flags.pNext = &rendering;
				flags.flags = VK_PIPELINE_CREATE_2_DESCRIPTOR_HEAP_BIT_EXT;

				VkGraphicsPipelineCreateInfo info = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
				info.pNext = &flags;
				info.stageCount = 2;
				info.pStages = stages;
				info.pVertexInputState = &vertex;
				info.pInputAssemblyState = &assembly;
				info.pViewportState = &viewport;
				info.pRasterizationState = &raster;
				info.pMultisampleState = &multisample;
				info.pColorBlendState = &blend;
				info.pDynamicState = &dynamic;
				info.layout = VK_NULL_HANDLE;		// descriptor heaps: no pipeline layout
				VkPipeline pipeline = VK_NULL_HANDLE;
				VK_CHECK(vkCreateGraphicsPipelines(Device().Device(), VK_NULL_HANDLE, 1, &info, NULL, &pipeline));
				vkDestroyShaderModule(Device().Device(), vs, NULL);
				vkDestroyShaderModule(Device().Device(), ps, NULL);
				return pipeline;
			}

			//	A 2x2 texture of one colour, uploaded through the ring.
			std::unique_ptr<vkdImage> texture(const char* i_Name, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
			{
				std::unique_ptr<vkdImage> image = vkdImage::Create2D(Device(), 2, 2, VK_FORMAT_R8G8B8A8_UNORM,
					VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, i_Name);
				vkdRingBuffer::Allocation staging = m_pRing->Allocate(16, 16);
				uint8_t* pixels = static_cast<uint8_t*>(staging.m_pCpu);
				for (int i = 0; i < 4; ++i)
				{
					pixels[i * 4 + 0] = r; pixels[i * 4 + 1] = g; pixels[i * 4 + 2] = b; pixels[i * 4 + 3] = a;
				}
				VkCommandBuffer commands = Device().Commands();
				vkdFullBarrier(commands);
				VkBufferImageCopy region = {};
				region.bufferOffset = staging.m_Offset;
				region.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
				region.imageExtent = {2, 2, 1};
				vkCmdCopyBufferToImage(commands, staging.m_Buffer, image->Image(), VK_IMAGE_LAYOUT_GENERAL, 1, &region);
				return image;
			}

			VkDeviceAddress constants(const Float4& i_Value)
			{
				vkdRingBuffer::Allocation a = m_pRing->Allocate(sizeof(Float4));
				memcpy(a.m_pCpu, &i_Value, sizeof(Float4));
				return a.m_Gpu;
			}

			//	Draws one full-screen triangle into i_Target with the current bindings.
			void draw(vkdImage& i_Target)
			{
				draw(i_Target, m_Pipeline);
			}

			void draw(vkdImage& i_Target, VkPipeline i_Pipeline)
			{
				VkCommandBuffer commands = Device().Commands();
				vkdFullBarrier(commands);
				VkRenderingAttachmentInfo color = {VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
				color.imageView = i_Target.View();
				color.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
				color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				VkRenderingInfo rendering = {VK_STRUCTURE_TYPE_RENDERING_INFO};
				rendering.renderArea = {{0, 0}, {i_Target.Width(), i_Target.Height()}};
				rendering.layerCount = 1;
				rendering.colorAttachmentCount = 1;
				rendering.pColorAttachments = &color;
				vkCmdBeginRendering(commands, &rendering);
				vkCmdBindPipeline(commands, VK_PIPELINE_BIND_POINT_GRAPHICS, i_Pipeline);
				m_pBindings->BindHeaps(commands);
				m_pBindings->Apply(commands, *m_pRing);
				//	D3D's Y direction: a negative viewport height
				VkViewport viewport = {0, static_cast<float>(i_Target.Height()), static_cast<float>(i_Target.Width()),
									   -static_cast<float>(i_Target.Height()), 0, 1};
				vkCmdSetViewport(commands, 0, 1, &viewport);
				VkRect2D scissor = {{0, 0}, {i_Target.Width(), i_Target.Height()}};
				vkCmdSetScissor(commands, 0, 1, &scissor);
				vkCmdDraw(commands, 3, 1, 0, 0);
				vkCmdEndRendering(commands);
			}

			//	A device-local buffer holding i_Size bytes of i_pData, uploaded through the ring.
			std::unique_ptr<vkdBuffer> buffer(const char* i_Name, const void* i_pData, VkDeviceSize i_Size)
			{
				std::unique_ptr<vkdBuffer> buffer = vkdBuffer::Create(Device(), i_Size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, i_Name);
				vkdRingBuffer::Allocation staging = m_pRing->Allocate(i_Size, 16);
				memcpy(staging.m_pCpu, i_pData, static_cast<size_t>(i_Size));
				VkCommandBuffer commands = Device().Commands();
				vkdFullBarrier(commands);
				VkBufferCopy region = {staging.m_Offset, 0, i_Size};
				vkCmdCopyBuffer(commands, staging.m_Buffer, buffer->Buffer(), 1, &region);
				return buffer;
			}

			std::unique_ptr<vkdImage> target(const char* i_Name)
			{
				return vkdImage::Create2D(Device(), 16, 16, VK_FORMAT_R8G8B8A8_UNORM,
					VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, i_Name);
			}

			void expect_all(const std::vector<uint8_t>& i_Pixels, int r, int g, int b, int a)
			{
				ASSERT_EQ(16u * 16u * 4u, i_Pixels.size());
				for (size_t i = 0; i < i_Pixels.size(); i += 4)
				{
					ASSERT_NEAR(r, i_Pixels[i + 0], 1) << "pixel " << i / 4;
					ASSERT_NEAR(g, i_Pixels[i + 1], 1) << "pixel " << i / 4;
					ASSERT_NEAR(b, i_Pixels[i + 2], 1) << "pixel " << i / 4;
					ASSERT_NEAR(a, i_Pixels[i + 3], 1) << "pixel " << i / 4;
				}
			}

			std::unique_ptr<vkdRingBuffer>	m_pRing;
			std::unique_ptr<vkdBindings>	m_pBindings;
			VkPipeline						m_Pipeline = VK_NULL_HANDLE;
	};

	VkSamplerCreateInfo point_sampler()
	{
		VkSamplerCreateInfo info = {VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
		info.magFilter = VK_FILTER_NEAREST;
		info.minFilter = VK_FILTER_NEAREST;
		info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		info.maxLod = VK_LOD_CLAMP_NONE;
		return info;
	}
}

TEST_F(gtBindings, DrawsWithSlotBindings)
{
	//	tex0 (t0) = (128, 64, 255, 255), tex1 (t37) = (51, 102, 0, 0)
	std::unique_ptr<vkdImage> tex0 = texture("tex0", 128, 64, 255, 255);
	std::unique_ptr<vkdImage> tex1 = texture("tex1", 51, 102, 0, 0);
	std::unique_ptr<vkdImage> out = target("out");

	m_pBindings->SetTexture(0, m_pBindings->AddImage(tex0->ViewInfo(), VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE));
	m_pBindings->SetTexture(37, m_pBindings->AddImage(tex1->ViewInfo(), VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE));
	m_pBindings->SetSampler(13, m_pBindings->AddSampler(point_sampler()));
	m_pBindings->SetConstantBuffer(0, constants({0.5f, 1.0f, 0.5f, 1.0f}), sizeof(Float4));	// scale
	m_pBindings->SetConstantBuffer(3, constants({1.0f, 0.5f, 1.0f, 0.0f}), sizeof(Float4));	// bias
	draw(*out);

	//	tex0 * scale + tex1 * bias = (0.251+0.2, 0.251+0.2, 0.5+0, 1+0)
	expect_all(out->ReadBack(), 115, 115, 128, 255);
}

TEST_F(gtBindings, TwoDrawsDifferentBindings)
{
	std::unique_ptr<vkdImage> red = texture("red", 255, 0, 0, 255);
	std::unique_ptr<vkdImage> blue = texture("blue", 0, 0, 255, 255);
	std::unique_ptr<vkdImage> black = texture("black", 0, 0, 0, 0);
	std::unique_ptr<vkdImage> a = target("a");
	std::unique_ptr<vkdImage> b = target("b");
	uint32_t red_index = m_pBindings->AddImage(red->ViewInfo(), VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
	uint32_t blue_index = m_pBindings->AddImage(blue->ViewInfo(), VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);

	m_pBindings->SetSampler(13, m_pBindings->AddSampler(point_sampler()));
	m_pBindings->SetTexture(37, m_pBindings->AddImage(black->ViewInfo(), VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE));
	m_pBindings->SetConstantBuffer(3, constants({0, 0, 0, 0}), sizeof(Float4));

	m_pBindings->SetTexture(0, red_index);
	m_pBindings->SetConstantBuffer(0, constants({1, 1, 1, 1}), sizeof(Float4));
	draw(*a);
	m_pBindings->SetTexture(0, blue_index);
	m_pBindings->SetConstantBuffer(0, constants({0.5f, 0.5f, 0.5f, 1}), sizeof(Float4));
	draw(*b);

	expect_all(a->ReadBack(), 255, 0, 0, 255);
	expect_all(b->ReadBack(), 0, 0, 128, 255);
}

//	StructuredBuffer<float4> at t3 and StructuredBuffer<uint> at t7, next to a
//	texture at t0, read through storage-buffer descriptors in the resource
//	heap (the hair shaders' g_HairMaterials and g_HairGeometry). Then the null
//	buffer at both slots reads as zeros, as a NULL view does in D3D11.
TEST_F(gtBindings, StructuredBufferReads)
{
	const Float4 values[4] = {{0.25f, 1.5f, -2.0f, 0.0f}, {1.25f, 2.5f, -3.0f, 0.5f},
							  {2.25f, 3.5f, -4.0f, 0.25f}, {3.25f, 4.5f, 1e6f, -0.75f}};
	const uint32_t counts[4] = {7, 0, 65536, 3};
	std::unique_ptr<vkdBuffer> value_buffer = buffer("values", values, sizeof(values));
	std::unique_ptr<vkdBuffer> count_buffer = buffer("counts", counts, sizeof(counts));
	std::unique_ptr<vkdImage> tex = texture("tex", 0, 0, 0, 255);
	std::unique_ptr<vkdImage> out = vkdImage::Create2D(Device(), 4, 1, VK_FORMAT_R32G32B32A32_SFLOAT,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, "out");
	VkPipeline pipeline = make_pipeline(g_gtStructuredReadVS, sizeof(g_gtStructuredReadVS),
										g_gtStructuredReadPS, sizeof(g_gtStructuredReadPS), VK_FORMAT_R32G32B32A32_SFLOAT);

	m_pBindings->SetTexture(0, m_pBindings->AddImage(tex->ViewInfo(), VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE));
	const uint32_t value_index = m_pBindings->AddBuffer(value_buffer->Address(), value_buffer->Size());
	const uint32_t count_index = m_pBindings->AddBuffer(count_buffer->Address(), count_buffer->Size());
	m_pBindings->SetTexture(3, value_index);
	m_pBindings->SetTexture(7, count_index);
	draw(*out, pipeline);
	std::vector<uint8_t> pixels = out->ReadBack();
	ASSERT_EQ(sizeof(values), pixels.size());
	const float* p = reinterpret_cast<const float*>(pixels.data());
	for (int x = 0; x < 4; ++x)
	{
		EXPECT_EQ(values[x].x + (float)counts[x], p[x * 4 + 0]) << "pixel " << x;
		EXPECT_EQ(values[x].y, p[x * 4 + 1]) << "pixel " << x;
		EXPECT_EQ(values[x].z, p[x * 4 + 2]) << "pixel " << x;
		EXPECT_EQ(values[x].w + 1.0f, p[x * 4 + 3]) << "pixel " << x;
	}

	m_pBindings->SetTexture(3, m_pBindings->NullBufferIndex());
	m_pBindings->SetTexture(7, m_pBindings->NullBufferIndex());
	draw(*out, pipeline);
	pixels = out->ReadBack();
	p = reinterpret_cast<const float*>(pixels.data());
	for (int x = 0; x < 4; ++x)
	{
		EXPECT_EQ(0.0f, p[x * 4 + 0]) << "pixel " << x;
		EXPECT_EQ(0.0f, p[x * 4 + 1]) << "pixel " << x;
		EXPECT_EQ(0.0f, p[x * 4 + 2]) << "pixel " << x;
		EXPECT_EQ(1.0f, p[x * 4 + 3]) << "pixel " << x;
	}

	m_pBindings->RemoveResource(value_index);
	m_pBindings->RemoveResource(count_index);
	Device().WaitIdle();
	vkDestroyPipeline(Device().Device(), pipeline, NULL);
	EXPECT_EQ(0, m_Validation.Stats().m_Errors);
	EXPECT_EQ(0, m_Validation.Stats().m_Warnings);
}

//	An image and a buffer share the resource heap: the index a removed buffer
//	gave back is handed to the next image
TEST_F(gtBindings, RemovedBufferIndexIsReusedAfterGpu)
{
	const uint32_t data[4] = {1, 2, 3, 4};
	std::unique_ptr<vkdBuffer> buf = buffer("buf", data, sizeof(data));
	const uint32_t index = m_pBindings->AddBuffer(buf->Address(), buf->Size());
	m_pBindings->RemoveResource(index);
	Device().WaitFor(Device().Submit());
	std::unique_ptr<vkdImage> tex = texture("tex", 1, 2, 3, 4);
	EXPECT_EQ(index, m_pBindings->AddImage(tex->ViewInfo(), VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE));
}

TEST_F(gtBindings, SamplerDedup)
{
	uint32_t first = m_pBindings->AddSampler(point_sampler());
	EXPECT_EQ(first, m_pBindings->AddSampler(point_sampler()));
	VkSamplerCreateInfo linear = point_sampler();
	linear.magFilter = VK_FILTER_LINEAR;
	EXPECT_NE(first, m_pBindings->AddSampler(linear));
}

TEST_F(gtBindings, RemovedImageIndexIsReusedAfterGpu)
{
	std::unique_ptr<vkdImage> tex = texture("tex", 1, 2, 3, 4);
	uint32_t index = m_pBindings->AddImage(tex->ViewInfo(), VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
	m_pBindings->RemoveResource(index);
	Device().WaitFor(Device().Submit());
	EXPECT_EQ(index, m_pBindings->AddImage(tex->ViewInfo(), VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE));
}

//	An image removed just before its vkdBindings is destroyed: the deferred
//	return of its index runs later, when the device destroys, and must not
//	touch the destroyed vkdBindings (MachStudio deletes the bindings before
//	the device, and so do tests).
TEST_F(gtBindings, RemovalOutlivesBindings)
{
	std::unique_ptr<vkdImage> tex = texture("tex", 1, 2, 3, 4);
	m_pBindings->RemoveResource(m_pBindings->AddImage(tex->ViewInfo(), VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE));
	m_pBindings.reset();
	tex.reset();
	Device().WaitIdle();
	vkDestroyPipeline(Device().Device(), m_Pipeline, NULL);
	m_pRing.reset();
	m_pDevice.reset();		// runs the deferred removal; with the bug: use after free (hang or crash)
	EXPECT_EQ(0, m_Validation.Stats().m_Errors);
	EXPECT_EQ(0, m_Validation.Stats().m_Warnings);
}
