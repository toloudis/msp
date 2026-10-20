/*****************************************************************************
**  vkdPipelineCacheTest.cpp
**
**      vkdPipelineCache: one pipeline per key, evicted with its shaders,
**      vertex inputs matched as D3D11 does, and the driver cache persisted
**      to a file that is ignored (not trusted) when it is damaged or from
**      another device.
**
**	Extra Large Technology
\****************************************************************************/
#include "Vk/gtVulkan.hpp"
#include "Dx11/gtD3D11.hpp"
#include "gtPlatform.hpp"
#include "gtTempDir.hpp"

#ifndef GVK_VKDBINDINGS_HPP
#include "GraphicsVulkan/vkd/vkdBindings.hpp"
#endif
#ifndef GVK_VKDPIPELINECACHE_HPP
#include "GraphicsVulkan/vkd/vkdPipelineCache.hpp"
#endif
#ifndef GVK_VKDSHADER_HPP
#include "GraphicsVulkan/vkd/vkdShader.hpp"
#endif

#include "gtSolidVS.h"
#include "gtSolidPS.h"

#include <d3dcompiler.h>

#include <cstring>
#include <fstream>
#include <functional>
#include <memory>
#include <vector>

using namespace gvk;

namespace
{
	class vkdPipelineCacheTest : public gtVulkan
	{
		protected:
			void SetUp() override
			{
				gtVulkan::SetUp();
				if (!m_pDevice)
				{
					return;
				}
				m_pBindings.reset(new vkdBindings(Device()));
				std::string error;
				m_pVS = vkdShader::Create(&Device(), VK_SHADER_STAGE_VERTEX_BIT, g_gtSolidVS, sizeof(g_gtSolidVS), "VSMain", "gtSolid VS", error);
				ASSERT_TRUE(m_pVS != nullptr) << error;
				m_pPS = vkdShader::Create(&Device(), VK_SHADER_STAGE_FRAGMENT_BIT, g_gtSolidPS, sizeof(g_gtSolidPS), "PSMain", "gtSolid PS", error);
				ASSERT_TRUE(m_pPS != nullptr) << error;
				m_pLayout.reset(new vkdVertexLayout({ { "POSITION", 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 0 },
													  { "COLOR", 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0, 12 } }, { 28 }));
			}

			void TearDown() override
			{
				m_pVS.reset();
				m_pPS.reset();
				m_pBindings.reset();
				gtVulkan::TearDown();
			}

			vkdPipelineKey Key(const vkdVertexLayout* i_pLayout = nullptr) const
			{
				vkdPipelineKey key;
				key.m_Shaders[0] = m_pVS->Id();
				key.m_Shaders[4] = m_pPS->Id();
				key.m_VertexLayout = (i_pLayout ? i_pLayout : m_pLayout.get())->Id();
				key.m_ColorCount = 1;
				key.m_Colors[0] = VK_FORMAT_R8G8B8A8_UNORM;
				return key;
			}

			const vkdShader* const* Stages()
			{
				m_Stages[0] = m_pVS.get();
				m_Stages[4] = m_pPS.get();
				return m_Stages;
			}

			std::unique_ptr<vkdBindings>		m_pBindings;
			std::unique_ptr<vkdShader>			m_pVS;
			std::unique_ptr<vkdShader>			m_pPS;
			std::unique_ptr<vkdVertexLayout>	m_pLayout;
			const vkdShader*					m_Stages[5] = {};
	};

	std::vector<char> read_file(const std::string& i_Path)
	{
		std::ifstream in(i_Path, std::ios::binary);
		return std::vector<char>(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
	}

	void write_file(const std::string& i_Path, const std::vector<char>& i_Data)
	{
		std::ofstream out(i_Path, std::ios::binary | std::ios::trunc);
		out.write(i_Data.data(), i_Data.size());
	}
}

TEST(vkdPipelineKey, EqualityAndHash)
{
	vkdPipelineKey base;
	base.m_Shaders[0] = 1;
	base.m_Shaders[4] = 2;
	base.m_VertexLayout = 3;
	base.m_ColorCount = 1;
	base.m_Colors[0] = VK_FORMAT_R8G8B8A8_UNORM;

	const std::vector<std::function<void(vkdPipelineKey&)>> changes =
	{
		[](vkdPipelineKey& k) { k.m_Shaders[3] = 9; },
		[](vkdPipelineKey& k) { k.m_VertexLayout = 4; },
		[](vkdPipelineKey& k) { k.m_TopologyClass = vkd_Lines; },
		[](vkdPipelineKey& k) { k.m_TopologyClass = vkd_Patches; k.m_PatchControlPoints = 3; },
		[](vkdPipelineKey& k) { k.m_ColorCount = 2; k.m_Colors[1] = VK_FORMAT_R16G16B16A16_SFLOAT; },
		[](vkdPipelineKey& k) { k.m_Colors[0] = VK_FORMAT_B8G8R8A8_UNORM; },
		[](vkdPipelineKey& k) { k.m_Depth = VK_FORMAT_D24_UNORM_S8_UINT; },
		[](vkdPipelineKey& k) { k.m_Samples = VK_SAMPLE_COUNT_4_BIT; },
		[](vkdPipelineKey& k) { k.m_Fill = VK_POLYGON_MODE_LINE; },
		[](vkdPipelineKey& k) { k.m_bDepthClip = 0; },
		[](vkdPipelineKey& k) { k.m_Blend.m_bAlphaToCoverage = true; },
		[](vkdPipelineKey& k) { k.m_Blend.m_Targets[0].m_bEnable = true; },
		[](vkdPipelineKey& k) { k.m_Blend.m_Targets[0].m_Dst = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; },
		[](vkdPipelineKey& k) { k.m_Blend.m_Targets[0].m_WriteMask = 0x7; },
	};
	for (size_t i = 0; i < changes.size(); i++)
	{
		vkdPipelineKey other = base;
		EXPECT_TRUE(other == base);
		EXPECT_EQ(base.Hash(), other.Hash());
		changes[i](other);
		EXPECT_FALSE(other == base) << "change " << i;
		EXPECT_NE(base.Hash(), other.Hash()) << "change " << i;
	}

	// a target the blend state doesn't use doesn't make a difference
	vkdPipelineKey unused = base;
	unused.m_Blend.m_Targets[5].m_bEnable = true;
	EXPECT_TRUE(unused == base);
	EXPECT_EQ(base.Hash(), unused.Hash());
}

TEST_F(vkdPipelineCacheTest, SameStateSamePipeline)
{
	vkdPipelineCache cache(Device(), *m_pBindings, "");
	VkPipeline a = cache.Get(Key(), Stages(), m_pLayout.get());
	VkPipeline b = cache.Get(Key(), Stages(), m_pLayout.get());
	EXPECT_NE(VK_NULL_HANDLE, a);
	EXPECT_EQ(a, b);
	EXPECT_EQ(1u, cache.Creations());
	EXPECT_EQ(1u, cache.Count());

	vkdPipelineKey blended = Key();
	blended.m_Blend.m_Targets[0].m_bEnable = true;
	EXPECT_NE(a, cache.Get(blended, Stages(), m_pLayout.get()));
	EXPECT_EQ(2u, cache.Creations());
}

TEST_F(vkdPipelineCacheTest, EvictOnShaderDestroy)
{
	vkdPipelineCache cache(Device(), *m_pBindings, "");
	ASSERT_NE(VK_NULL_HANDLE, cache.Get(Key(), Stages(), m_pLayout.get()));
	EXPECT_EQ(1u, cache.Count());
	m_pPS.reset();
	EXPECT_EQ(0u, cache.Count());
}

TEST_F(vkdPipelineCacheTest, MissingVertexInputFails)
{
	vkdPipelineCache cache(Device(), *m_pBindings, "");
	vkdVertexLayout noColor({ { "POSITION", 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 0 } }, { 12 });
	EXPECT_EQ(VK_NULL_HANDLE, cache.Get(Key(&noColor), Stages(), &noColor));
	// a semantic in another case still matches, as in D3D
	vkdVertexLayout lower({ { "position", 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 0 },
							{ "Color", 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0, 12 } }, { 28 });
	EXPECT_NE(VK_NULL_HANDLE, cache.Get(Key(&lower), Stages(), &lower));
}

TEST_F(vkdPipelineCacheTest, SvPositionMatchesPositionAsD3D11)
{
	//	GraphicsDX11 makes the bump meshes' input layout once, against the
	//	default effect's signature, which names the position SV_POSITION;
	//	some material shaders (Skin, BlinnSkin) read it as POSITION. D3D11
	//	rejects that pairing in CreateInputLayout, but a layout made against
	//	one shader feeds another by register when drawing, so they draw.
	//	The pipeline cache matches by name, so it must take one for the other.
	GT_D3D11(d3d);
	ID3D11Device* device = d3d.Device();
	ID3D11DeviceContext* context = d3d.Context();
	const std::string root = gtPlatform::RepoRoot();
	const std::wstring file = std::wstring(root.begin(), root.end()) + L"UnitTests\\GraphicsTests\\Vk\\Shaders\\gtSolid.hlsl";
	Microsoft::WRL::ComPtr<ID3DBlob> vs, ps, reference, errors;
	ASSERT_TRUE(SUCCEEDED(D3DCompileFromFile(file.c_str(), NULL, NULL, "VSMain", "vs_5_0", 0, 0, &vs, &errors)));
	ASSERT_TRUE(SUCCEEDED(D3DCompileFromFile(file.c_str(), NULL, NULL, "PSMain", "ps_5_0", 0, 0, &ps, &errors)));
	const char* c_Reference = "float4 main(float3 p : SV_POSITION, float4 c : COLOR0) : SV_Position { return float4(p, 1) * c.x; }";
	ASSERT_TRUE(SUCCEEDED(D3DCompile(c_Reference, strlen(c_Reference), "reference", NULL, NULL, "main", "vs_5_0", 0, 0, &reference, &errors)))
		<< (errors ? (const char*)errors->GetBufferPointer() : "");
	const D3D11_INPUT_ELEMENT_DESC elements[] =
	{
		{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	Microsoft::WRL::ComPtr<ID3D11InputLayout> rejected, layout;
	EXPECT_FALSE(SUCCEEDED(device->CreateInputLayout(elements, 2, vs->GetBufferPointer(), vs->GetBufferSize(), &rejected)));
	ASSERT_TRUE(SUCCEEDED(device->CreateInputLayout(elements, 2, reference->GetBufferPointer(), reference->GetBufferSize(), &layout)));

	//	one triangle over the whole 4x4 target, drawn by gtSolid (POSITION):
	//	its colour times an unbound tint (0) replaces the blue clear
	const float vertices[3][7] = { { -1, -1, 0, 1, 1, 1, 1 }, { -1, 3, 0, 1, 1, 1, 1 }, { 3, -1, 0, 1, 1, 1, 1 } };
	D3D11_BUFFER_DESC bufferDesc = { sizeof(vertices), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
	D3D11_SUBRESOURCE_DATA init = { vertices, 0, 0 };
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	ASSERT_TRUE(SUCCEEDED(device->CreateBuffer(&bufferDesc, &init, &buffer)));
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	device->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), NULL, &vertexShader);
	device->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), NULL, &pixelShader);
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> view;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> target = d3d.CreateTexture(4, 4, NULL, true, NULL, &view);
	const float blue[4] = { 0, 0, 1, 1 };
	context->ClearRenderTargetView(view.Get(), blue);
	context->OMSetRenderTargets(1, view.GetAddressOf(), NULL);
	const D3D11_VIEWPORT viewport = { 0, 0, 4, 4, 0, 1 };
	context->RSSetViewports(1, &viewport);
	const UINT stride = sizeof(vertices[0]), offset = 0;
	context->IASetVertexBuffers(0, 1, buffer.GetAddressOf(), &stride, &offset);
	context->IASetInputLayout(layout.Get());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->VSSetShader(vertexShader.Get(), NULL, 0);
	context->PSSetShader(pixelShader.Get(), NULL, 0);
	context->Draw(3, 0);
	const std::vector<uint8_t> pixels = d3d.ReadBack(target.Get());
	ASSERT_EQ(4u * 4 * 4, pixels.size());
	EXPECT_EQ(0, pixels[0] + pixels[1] + pixels[2] + pixels[3]) << "D3D11 didn't draw: blue " << (int)pixels[2];

	vkdPipelineCache cache(Device(), *m_pBindings, "");
	vkdVertexLayout svPosition({ { "SV_POSITION", 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 0 },
								 { "COLOR", 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0, 12 } }, { 28 });
	EXPECT_NE(VK_NULL_HANDLE, cache.Get(Key(&svPosition), Stages(), &svPosition));
}

TEST_F(vkdPipelineCacheTest, SaveAndReload)
{
	gtTempDir dir;
	const std::string file = (dir.GetPath() / "pipelines.bin").string();
	{
		vkdPipelineCache cache(Device(), *m_pBindings, file);
		EXPECT_FALSE(cache.LoadedFromFile());
		for (int i = 0; i < 3; i++)
		{
			vkdPipelineKey key = Key();
			key.m_Blend.m_Targets[0].m_WriteMask = 0xF >> i;
			ASSERT_NE(VK_NULL_HANDLE, cache.Get(key, Stages(), m_pLayout.get()));
		}
		EXPECT_TRUE(cache.Save());
	}
	EXPECT_GT(read_file(file).size(), sizeof(VkPipelineCacheHeaderVersionOne));
	vkdPipelineCache reloaded(Device(), *m_pBindings, file);
	EXPECT_TRUE(reloaded.LoadedFromFile());
	EXPECT_NE(VK_NULL_HANDLE, reloaded.Get(Key(), Stages(), m_pLayout.get()));
}

TEST_F(vkdPipelineCacheTest, CorruptCacheFileIsIgnored)
{
	gtTempDir dir;
	const std::string file = (dir.GetPath() / "pipelines.bin").string();
	const std::vector<std::vector<char>> bad =
	{
		std::vector<char>(1000, 'x'),		// garbage
		std::vector<char>(10, 0),			// shorter than a header
		std::vector<char>(),				// empty
	};
	for (const std::vector<char>& data : bad)
	{
		write_file(file, data);
		vkdPipelineCache cache(Device(), *m_pBindings, file);
		EXPECT_FALSE(cache.LoadedFromFile());
		EXPECT_NE(VK_NULL_HANDLE, cache.Get(Key(), Stages(), m_pLayout.get()));
	}
}

TEST_F(vkdPipelineCacheTest, ForeignHeaderIsIgnored)
{
	gtTempDir dir;
	const std::string file = (dir.GetPath() / "pipelines.bin").string();
	{
		vkdPipelineCache cache(Device(), *m_pBindings, file);
		ASSERT_NE(VK_NULL_HANDLE, cache.Get(Key(), Stages(), m_pLayout.get()));
	}
	std::vector<char> data = read_file(file);
	ASSERT_GE(data.size(), sizeof(VkPipelineCacheHeaderVersionOne));
	VkPipelineCacheHeaderVersionOne header;
	memcpy(&header, data.data(), sizeof(header));
	header.vendorID ^= 0x1234;
	memcpy(data.data(), &header, sizeof(header));
	write_file(file, data);

	vkdPipelineCache cache(Device(), *m_pBindings, file);
	EXPECT_FALSE(cache.LoadedFromFile());
	EXPECT_NE(VK_NULL_HANDLE, cache.Get(Key(), Stages(), m_pLayout.get()));
}

//	A valid header over damaged data passes our checks; the driver must cope
//	(the Vulkan spec has it ignore incompatible data). No crash, no messages.
TEST_F(vkdPipelineCacheTest, DamagedBodyDoesNotCrash)
{
	gtTempDir dir;
	const std::string file = (dir.GetPath() / "pipelines.bin").string();
	{
		vkdPipelineCache cache(Device(), *m_pBindings, file);
		ASSERT_NE(VK_NULL_HANDLE, cache.Get(Key(), Stages(), m_pLayout.get()));
	}
	std::vector<char> data = read_file(file);
	ASSERT_GT(data.size(), sizeof(VkPipelineCacheHeaderVersionOne) + 16);
	for (size_t i = sizeof(VkPipelineCacheHeaderVersionOne); i < data.size(); i++)
	{
		data[i] = (char)0xAB;
	}
	data.resize(data.size() / 2);
	write_file(file, data);

	vkdPipelineCache cache(Device(), *m_pBindings, file);
	EXPECT_NE(VK_NULL_HANDLE, cache.Get(Key(), Stages(), m_pLayout.get()));
}
