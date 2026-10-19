/*****************************************************************************
**  effShaderVkTest.cpp
**
**      GraphicsVulkan's material shader classes (effShaderBaseVk, the eff*
**      classes, effShaderArray) against GraphicsDX11's: the same code runs
**      through matShaderMgr under each layer (gtBackend), and what it sees
**      must match:
**
**      - the effects registered (names, classes, data types), and the
**        material and post-effect lists;
**      - every effect's parameters as the material and post-effect
**        dialogs see them (BuildPrtyObject, GetParamIndex);
**      - the bytes of every constant buffer after SetupMatrices,
**        SetupMaterial and the lighting setup, for materials with
**        non-default and edge values (Review Focus 4);
**      - which textures are bound to which parameters.
**
**      Skipped without a Vulkan device (the DX11 half still runs).
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"

#ifndef MAT_SHADERMGR_HPP
#include "Graphics/Mat/matShaderMgr.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/Mat/matShaderEffect.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/Mat/matTextureMgr.hpp"
#endif
#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/Eff/effShaderParams.hpp"
#endif
#ifndef EFF_SOLIDDATA_HPP
#include "Graphics/Eff/effSolidData.hpp"
#endif
#ifndef EFF_PHONGDATA_HPP
#include "Graphics/Eff/effPhongData.hpp"
#endif
#ifndef G3D_LIGHTMGR_HPP
#include "Graphics/G3d/g3dLightMgr.hpp"
#endif
#ifndef G3D_POINTLIGHT_HPP
#include "Graphics/G3d/g3dPointLight.hpp"
#endif
#ifndef G3D_DIRECTIONALLIGHT_HPP
#include "Graphics/G3d/g3dDirectionalLight.hpp"
#endif
#ifndef G3D_RENDERSTATE_HPP
#include "Graphics/G3d/g3dRenderState.hpp"
#endif
#ifndef MA_MATRIX4X4_HPP
#include "Core/Ma/maMatrix4x4.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/Ma/maAxisBox.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef IT_STRINGUTIL_HPP
#include "Core/it/itStringUtil.hpp"
#endif

//	DX11
#ifndef EFF_SHADERBASEDX11_HPP
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#endif
#ifndef EFF_PLAINSHADERDX11_HPP
#include "GraphicsDX11/eff/effPlainShaderDX11.hpp"
#endif
#ifndef EFF_PLAINEFFECT_HPP
#include "GraphicsDX11/eff/effPlainEffect.hpp"
#endif

//	Vulkan
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
#ifndef GVK_EFF_PLAINSHADERVK_HPP
#include "GraphicsVulkan/Eff/effPlainShaderVk.hpp"
#endif
#ifndef GVK_EFFPLAINEFFECTVK_HPP
#include "GraphicsVulkan/Eff/effPlainEffectVk.hpp"
#endif
#ifndef GVK_FXEFFECTVK_HPP
#include "GraphicsVulkan/Fx/fxEffectVk.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif

#include <d3d11.h>

#include <gtest/gtest.h>

#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <memory>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>

namespace
{
	//	A class name without the namespace and backend suffix:
	//	"class gvk::effShaderBaseVk" and "class effShaderBaseDX11" -> "effShaderBase"
	std::string class_name(const void* i_pObject, const std::type_info& i_Type)
	{
		if (!i_pObject)
			return "(null)";
		std::string name = i_Type.name();
		for (const char* prefix : { "class ", "struct ", "gvk::" })
		{
			const size_t at = name.find(prefix);
			if (at != std::string::npos)
				name.erase(at, strlen(prefix));
		}
		for (const char* suffix : { "DX11", "Vk" })
		{
			const size_t n = strlen(suffix);
			if (name.size() > n && name.compare(name.size() - n, n, suffix) == 0)
				name.erase(name.size() - n);
		}
		return name;
	}

	template <class T> std::string class_of(T* i_p)
	{
		return i_p ? class_name(i_p, typeid(*i_p)) : std::string("(null)");
	}

	//	Every effect matShaderMgr knows: the registered ones, by key, then the
	//	material shaders and post effects it lists, loaded by name
	std::vector<std::pair<std::string, matShaderEffect*>> all_effects()
	{
		struct Visitor : matShaderMgr::matShaderMapVisitor
		{
			std::vector<std::pair<std::string, matShaderEffect*>> m_Effects;
			void visit(const std::string& i_Key, matShaderEffect* i_pEffect) override
			{
				m_Effects.push_back({ i_Key, i_pEffect });
			}
		} visitor;
		matShaderMgr::visitShaders(&visitor);
		for (const matShaderInfo& info : matShaderMgr::GetMaterialShaders())
		{
			const std::string name = itStringUtil::GetStdString(info.m_Name);
			visitor.m_Effects.push_back({ "material " + name, matShaderMgr::GetEffect(fsLocator(itString(name.c_str()))) });
		}
		for (const matShaderInfo& info : matShaderMgr::GetPostEffectShaders())
		{
			const std::string name = itStringUtil::GetStdString(info.m_Name);
			visitor.m_Effects.push_back({ "post " + name, matShaderMgr::GetPostEffect(fsLocator(itString(name.c_str()))) });
		}
		return visitor.m_Effects;
	}

	std::string describe_registry()
	{
		std::ostringstream out;
		struct Visitor : matShaderMgr::matShaderMapVisitor
		{
			std::ostringstream* m_pOut;
			void visit(const std::string& i_Key, matShaderEffect* i_pEffect) override
			{
				std::unique_ptr<effShaderData> data(matShaderMgr::CreateData(i_Key));
				*m_pOut << i_Key << ": " << class_of(i_pEffect) << ", data " << class_of(data.get())
						<< ", name '" << (i_pEffect ? i_pEffect->GetName() : std::string()) << "'\n";
			}
		} visitor;
		visitor.m_pOut = &out;
		matShaderMgr::visitShaders(&visitor);
		for (const matShaderInfo& info : matShaderMgr::GetMaterialShaders())
		{
			out << "material " << itStringUtil::GetStdString(info.m_Name) << " '" << info.m_UIName << "' '"
				<< info.m_HelpString << "' outline " << info.m_bSupportsOutline << "\n";
		}
		for (const matShaderInfo& info : matShaderMgr::GetPostEffectShaders())
		{
			out << "post " << itStringUtil::GetStdString(info.m_Name) << " '" << info.m_UIName << "'\n";
		}
		return out.str();
	}

	std::string describe_params(effShaderParams& i_Params)
	{
		std::ostringstream out;
		std::vector<effParamFloat*> floats;
		i_Params.GetAllFloatParams(floats);
		for (effParamFloat* p : floats)
			out << " float " << p->GetName() << "=" << p->GetProperty().GetValue();
		std::vector<effParamInt*> ints;
		i_Params.GetAllIntParams(ints);
		for (effParamInt* p : ints)
			out << " int " << p->GetName() << "=" << p->GetProperty().GetValue();
		std::vector<effParamBool*> bools;
		i_Params.GetAllBoolParams(bools);
		for (effParamBool* p : bools)
			out << " bool " << p->GetName() << "=" << p->GetProperty().GetValue();
		std::vector<effParamColor*> colors;
		i_Params.GetAllColorParams(colors);
		for (effParamColor* p : colors)
		{
			const maFloatRGBA c = p->GetProperty().GetValue();
			out << " color " << p->GetName() << "=" << c.GetRed() << "," << c.GetGreen() << "," << c.GetBlue() << "," << c.GetAlpha();
		}
		std::vector<effParamTexture*> textures;
		i_Params.GetAllTextureParams(textures);
		for (effParamTexture* p : textures)
			out << " texture " << p->GetName();
		return out.str();
	}

	//	One effect's parameters, as the material and post-effect dialogs see
	//	them (BuildPrtyObject), with each one's GetParamIndex. GetAllParamUIs
	//	and GetParamUI aren't compared: nothing calls them, and GraphicsDX11's
	//	crash (they read a missing annotation's string as NULL; annotation
	//	lookups never return NULL, so the checks for one never fail).
	std::string describe_effect(matShaderEffect* i_pEffect)
	{
		std::ostringstream out;
		effShaderParams params;
		const int built = i_pEffect->BuildPrtyObject(&params);
		out << "  prty " << built << describe_params(params) << "\n";
		std::vector<std::string> names;
		std::vector<effParamFloat*> floats;
		params.GetAllFloatParams(floats);
		for (effParamFloat* p : floats) names.push_back(p->GetName());
		std::vector<effParamInt*> ints;
		params.GetAllIntParams(ints);
		for (effParamInt* p : ints) names.push_back(p->GetName());
		std::vector<effParamBool*> bools;
		params.GetAllBoolParams(bools);
		for (effParamBool* p : bools) names.push_back(p->GetName());
		std::vector<effParamColor*> colors;
		params.GetAllColorParams(colors);
		for (effParamColor* p : colors) names.push_back(p->GetName());
		std::vector<effParamTexture*> textures;
		params.GetAllTextureParams(textures);
		for (effParamTexture* p : textures) names.push_back(p->GetName());
		out << "  indices";
		for (const std::string& name : names)
			out << " " << name << "=" << i_pEffect->GetParamIndex(name);
		out << "\n";
		out << "  reflection " << i_pEffect->HasReflectionMap() << " tess " << i_pEffect->HasHardwareTessellation()
			<< " outline " << i_pEffect->GetSupportOutline() << " skin " << i_pEffect->GetHasSkinning() << "\n";
		return out.str();
	}

	//	Every effect's parameters
	std::string describe_params_and_ui()
	{
		std::ostringstream out;
		for (const auto& entry : all_effects())
		{
			matShaderEffect* effect = entry.second;
			out << "== " << entry.first << ": " << class_of(effect) << "\n";
			if (effect)
				out << describe_effect(effect);
		}
		return out.str();
	}

	//--------------------------------------------------------------------
	//	The constant buffers and bound textures of an effect, by backend
	//--------------------------------------------------------------------
	struct EffectState
	{
		std::map<std::string, std::vector<uint8_t>>	m_Buffers;		// by cbuffer name
		std::map<std::string, std::string>			m_Resources;	// name -> "none" or "<w>x<h>"
	};

	std::string size_of(ID3D11ShaderResourceView* i_pView)
	{
		if (!i_pView)
			return "none";
		ID3D11Resource* resource = NULL;
		i_pView->GetResource(&resource);
		std::string size = "?";
		ID3D11Texture2D* texture = NULL;
		if (resource && SUCCEEDED(resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&texture)))
		{
			D3D11_TEXTURE2D_DESC desc;
			texture->GetDesc(&desc);
			size = std::to_string(desc.Width) + "x" + std::to_string(desc.Height);
			texture->Release();
		}
		if (resource)
			resource->Release();
		return size;
	}

	std::string size_of(gvk::vkdImage* i_pImage)
	{
		return i_pImage ? std::to_string(i_pImage->Width()) + "x" + std::to_string(i_pImage->Height()) : std::string("none");
	}

	//	The backend's fxEffect behind a matShaderEffect (NULL if none)
	fxEffectDX11* dx11_effect(matShaderEffect* i_pEffect)
	{
		if (effPlainShaderDX11* plain = dynamic_cast<effPlainShaderDX11*>(i_pEffect))
			return plain->GetEffect();
		if (effShaderBaseDX11* base = dynamic_cast<effShaderBaseDX11*>(i_pEffect))
			if (effPlainEffect* wrapped = dynamic_cast<effPlainEffect*>(base->GetFxEffect()))
				return wrapped->GetEffectDX11();
		return NULL;
	}

	gvk::fxEffectVk* vk_effect(matShaderEffect* i_pEffect)
	{
		if (gvk::effPlainShaderVk* plain = dynamic_cast<gvk::effPlainShaderVk*>(i_pEffect))
			return plain->GetEffect();
		if (gvk::effShaderBaseVk* base = dynamic_cast<gvk::effShaderBaseVk*>(i_pEffect))
			if (gvk::effPlainEffectVk* wrapped = dynamic_cast<gvk::effPlainEffectVk*>(base->GetFxEffect()))
				return wrapped->GetEffectVk();
		return NULL;
	}

	//	Constant buffer names come from the Vulkan effect (GraphicsDX11's
	//	fxEffectDX11 can't list them; fxEffectVkReflectionTest checks both
	//	reflect the same buffers)
	std::map<std::string, std::vector<std::string>> l_BufferNames;	// effect name -> cbuffer names

	EffectState read_state(gtBackend::Kind i_Kind, matShaderEffect* i_pEffect)
	{
		EffectState state;
		const std::string key = i_pEffect->GetName();
		if (i_Kind == gtBackend::e_Vulkan)
		{
			gvk::fxEffectVk* effect = vk_effect(i_pEffect);
			EXPECT_TRUE(effect) << key;
			if (!effect)
				return state;
			l_BufferNames[key] = effect->GetConstantBufferNames();
			for (const std::string& name : l_BufferNames[key])
			{
				const int b = effect->FindConstantBuffer(name);
				const uint8_t* data = static_cast<const uint8_t*>(effect->GetConstantBufferData(b));
				state.m_Buffers[name].assign(data, data + effect->GetConstantBufferSize(b));
			}
			for (const std::string& name : effect->GetResourceNames())
				state.m_Resources[name] = size_of(effect->GetResource(effect->FindResource(name)));
		}
		else
		{
			fxEffectDX11* effect = dx11_effect(i_pEffect);
			EXPECT_TRUE(effect) << key;
			if (!effect)
				return state;
			for (const std::string& name : l_BufferNames[key])
			{
				const int b = effect->FindConstantBuffer(name);
				EXPECT_GE(b, 0) << key << ": no cbuffer " << name << " on DX11";
				if (b < 0)
					continue;
				const uint8_t* data = static_cast<const uint8_t*>(effect->GetConstantBufferData(b));
				state.m_Buffers[name].assign(data, data + effect->GetConstantBufferSize(b));
			}
			for (const std::string& name : effect->GetResourceNames())
				state.m_Resources[name] = size_of(effect->GetResource(effect->FindResource(name)));
		}
		return state;
	}

	//--------------------------------------------------------------------
	//	Materials: a built-in shader (effect data) or a material shader
	//	(shader parameters), with values written to every parameter
	//--------------------------------------------------------------------
	enum Values
	{
		e_NonDefault,		// every float and colour moved off its default
		e_Zero,				// floats 0 (zero specular power), colours transparent black
		e_Huge,				// floats 1e6 (very large specular power), colours white
	};

	float float_value(Values i_Values, float i_Default, int i_Index)
	{
		switch (i_Values)
		{
			case e_Zero:	return 0.0f;
			case e_Huge:	return 1.0e6f;
			default:		return i_Default * 1.37f + 0.11f + 0.01f * i_Index;
		}
	}

	maFloatRGBA color_value(Values i_Values, int i_Index)
	{
		switch (i_Values)
		{
			case e_Zero:	return maFloatRGBA(0, 0, 0, 0);
			case e_Huge:	return maFloatRGBA(1, 1, 1, 1);
			default:		return maFloatRGBA(0.2f + 0.05f * i_Index, 0.5f, 0.8f - 0.05f * i_Index, 0.9f);
		}
	}

	//	A material on a material shader ("Blinn.fx"): parameters built by
	//	its effect, then set; i_Textures go to its texture parameters in order
	matMaterial* shader_material(const std::string& i_Shader, Values i_Values,
								 const std::vector<matTexture*>& i_Textures = std::vector<matTexture*>())
	{
		matMaterial* material = new matMaterial();
		shared_ptr<effShaderParams> params(new effShaderParams());
		params->SetShaderName(itString(i_Shader.c_str()));
		material->SetShaderParams(params);
		matShaderEffect* effect = params->GetShader();
		EXPECT_TRUE(effect) << i_Shader;
		if (effect)
			effect->BuildPrtyObject(params.get());
		std::vector<effParamFloat*> floats;
		params->GetAllFloatParams(floats);
		for (size_t i = 0; i < floats.size(); i++)
			floats[i]->Property().SetValue(float_value(i_Values, floats[i]->GetProperty().GetValue(), (int)i));
		std::vector<effParamColor*> colors;
		params->GetAllColorParams(colors);
		for (size_t i = 0; i < colors.size(); i++)
			colors[i]->Property().SetValue(color_value(i_Values, (int)i));
		std::vector<effParamTexture*> textures;
		params->GetAllTextureParams(textures);
		for (size_t i = 0; i < textures.size() && i < i_Textures.size(); i++)
			textures[i]->SetTexture(i_Textures[i]);
		return material;
	}

	//	A material on a built-in shader with effect data ("Phong.fx", "Solid.fx")
	matMaterial* data_material(const std::string& i_Shader, Values i_Values)
	{
		matMaterial* material = new matMaterial(i_Shader, matShaderMgr::CreateData(i_Shader));
		if (effPhongData* phong = dynamic_cast<effPhongData*>(material->GetEffectData()))
		{
			phong->m_ColorAmbient = color_value(i_Values, 0);
			phong->m_ColorDiffuse = color_value(i_Values, 1);
			phong->m_ColorSpecular = color_value(i_Values, 2);
			phong->m_ColorEmissive = color_value(i_Values, 3);
			phong->m_SpecularPower = float_value(i_Values, phong->m_SpecularPower, 0);
			phong->m_Transparency = i_Values == e_Zero ? 0.0f : (i_Values == e_Huge ? 1.0f : 0.7f);
		}
		if (effSolidData* solid = dynamic_cast<effSolidData*>(material->GetEffectData()))
		{
			solid->m_Color = color_value(i_Values, 4);
		}
		return material;
	}

	//	The setup a renderer does before drawing a node (as g3dSceneRendererDX11's DrawNode)
	void setup(matShaderEffect* i_pEffect, const matMaterial* i_pMaterial)
	{
		maMatrix4x4 world;
		world.Identity();
		world(0, 3) = 0.25f; world(1, 3) = -0.5f; world(2, 3) = 1.0f;
		world(0, 0) = 1.5f; world(1, 1) = 0.75f;
		maMatrix4x4 camera;
		camera.Identity();
		camera(2, 3) = -5.0f; camera(0, 1) = 0.1f;
		maMatrix4x4 projection;
		projection.Identity();
		projection(0, 0) = 1.2f; projection(1, 1) = 1.6f; projection(2, 2) = 1.01f; projection(3, 2) = 1.0f; projection(2, 3) = -0.1f;
		const maAxisBox box(maPoint3d(-1, -1, -1), maPoint3d(1, 1, 1));

		i_pEffect->SetTechnique(matShaderEffect::e_Default);
		i_pEffect->SetTime(1.25f);
		i_pEffect->SetVertexUVBakeMode(false);
		i_pEffect->SetIsolateReflections(false);
		i_pEffect->SetIsDoubleSided(false);
		i_pEffect->SetBakingFactors(maPoint2d(0.5f, 2.0f), maPoint2d(0.25f, -0.125f));
		i_pEffect->SetupMatrices(world, camera, projection, maPoint3d(0.5f, 1.0f, 5.0f));
		i_pEffect->SetupMaterial(i_pMaterial);
		i_pEffect->SetupLighting(box);
		i_pEffect->SetupAmbientLighting(box);
		static const g3dAmbientEnvState l_NoAmbientEnvState;
		i_pEffect->SetupAmbientPass(l_NoAmbientEnvState);
	}

	//	A point and a directional light, for the lighting setup
	struct Lights
	{
		g3dPointLight* m_pPoint;
		g3dDirectionalLight* m_pDirectional;
		Lights()
		{
			m_pPoint = g3dLightMgr::CreatePointLight();
			m_pPoint->SetPosition(maPoint3d(2, 3, 4));
			m_pPoint->SetIntensity(maFloatRGBA(0.9f, 0.8f, 0.7f, 1));
			m_pPoint->SetRange(25.0f);
			m_pPoint->SetEnable(true);
			m_pDirectional = g3dLightMgr::CreateDirectionalLight();
			m_pDirectional->SetDirection(maVector3d(-0.3f, -0.8f, -0.5f));
			m_pDirectional->SetIntensity(maFloatRGBA(0.4f, 0.5f, 0.6f, 1));
			m_pDirectional->SetEnable(true);
			g3dLightMgr::SetAmbient(maFloatRGBA(0.1f, 0.15f, 0.2f, 1));
		}
		~Lights()
		{
			g3dLightMgr::DestroyLight(m_pDirectional);
			g3dLightMgr::DestroyLight(m_pPoint);
		}
	};

	struct MaterialCase
	{
		std::string m_Name;
		std::function<matMaterial*()> m_Make;
	};

	std::vector<MaterialCase> material_cases()
	{
		std::vector<MaterialCase> cases;
		for (const char* shader : { "Blinn.fx", "Phong.fx", "Lambert.fx" })
		{
			const std::string s = shader;
			cases.push_back({ "material " + s, [s]() { return shader_material(s, e_NonDefault); } });
			cases.push_back({ "material " + s + " zero", [s]() { return shader_material(s, e_Zero); } });
			cases.push_back({ "material " + s + " huge", [s]() { return shader_material(s, e_Huge); } });
		}
		for (const char* shader : { "Phong.fx", "Solid.fx", "default" })
		{
			const std::string s = shader;
			cases.push_back({ "built-in " + s, [s]() { return data_material(s, e_NonDefault); } });
			cases.push_back({ "built-in " + s + " zero", [s]() { return data_material(s, e_Zero); } });
			cases.push_back({ "built-in " + s + " huge", [s]() { return data_material(s, e_Huge); } });
		}
		return cases;
	}

	//	The effect state after setting up each material case
	std::vector<EffectState> setup_cases(gtBackend::Kind i_Kind)
	{
		std::vector<EffectState> states;
		Lights lights;
		for (const MaterialCase& c : material_cases())
		{
			SCOPED_TRACE(c.m_Name);
			std::unique_ptr<matMaterial> material(c.m_Make());
			matShaderEffect* effect = matShaderMgr::GetEffect(*material);
			EXPECT_TRUE(effect);
			if (!effect)
			{
				states.push_back(EffectState());
				continue;
			}
			setup(effect, material.get());
			states.push_back(read_state(i_Kind, effect));
		}
		return states;
	}
}

TEST(effShaderVkTest, SameShadersRegistered)
{
	std::string dx11, vulkan;
	gtBackend::Run(gtBackend::e_DX11, [&]() { dx11 = describe_registry(); });
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { vulkan = describe_registry(); }))
		return;
	EXPECT_FALSE(dx11.empty());
	EXPECT_EQ(dx11, vulkan);
}

TEST(effShaderVkTest, SameParamsAndUI)
{
	std::string dx11, vulkan;
	gtBackend::Run(gtBackend::e_DX11, [&]() { dx11 = describe_params_and_ui(); });
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { vulkan = describe_params_and_ui(); }))
		return;
	EXPECT_FALSE(dx11.empty());
	EXPECT_EQ(dx11, vulkan);
}

TEST(effShaderVkTest, SetupWritesSameConstants)
{
	//	Vulkan first: it lists the constant buffer names DX11's are read by
	std::vector<EffectState> dx11, vulkan;
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { vulkan = setup_cases(gtBackend::e_Vulkan); }))
		return;
	gtBackend::Run(gtBackend::e_DX11, [&]() { dx11 = setup_cases(gtBackend::e_DX11); });

	const std::vector<MaterialCase> cases = material_cases();
	ASSERT_EQ(cases.size(), dx11.size());
	ASSERT_EQ(cases.size(), vulkan.size());
	for (size_t i = 0; i < cases.size(); i++)
	{
		SCOPED_TRACE(cases[i].m_Name);
		EXPECT_FALSE(vulkan[i].m_Buffers.empty());
		ASSERT_EQ(dx11[i].m_Buffers.size(), vulkan[i].m_Buffers.size());
		for (const auto& buffer : vulkan[i].m_Buffers)
		{
			const std::vector<uint8_t>& other = dx11[i].m_Buffers[buffer.first];
			ASSERT_EQ(other.size(), buffer.second.size()) << buffer.first;
			for (size_t b = 0; b < other.size(); b += 4)
			{
				EXPECT_EQ(0, memcmp(&other[b], &buffer.second[b], 4))
					<< "cbuffer " << buffer.first << " byte " << b << ": DX11 "
					<< *reinterpret_cast<const float*>(&other[b]) << " Vulkan "
					<< *reinterpret_cast<const float*>(&buffer.second[b]);
			}
		}
		EXPECT_EQ(dx11[i].m_Resources, vulkan[i].m_Resources);
	}
}

TEST(effShaderVkTest, TexturesBoundAtSameSlots)
{
	auto run = [](gtBackend::Kind i_Kind)
	{
		std::vector<EffectState> states;
		Lights lights;
		//	Textures of distinct sizes, so each binding says which it is
		matTexture* diffuse = matTextureMgr::CreateRenderTargetTexture(32, 16, false, NULL, false, false);
		matTexture* normal = matTextureMgr::CreateRenderTargetTexture(8, 4, false, NULL, false, false);
		for (const char* shader : { "Blinn.fx", "Phong_wBump.fx", "Lambert.fx" })
		{
			SCOPED_TRACE(shader);
			std::unique_ptr<matMaterial> material(shader_material(shader, e_NonDefault, { diffuse, normal }));
			matShaderEffect* effect = matShaderMgr::GetEffect(*material);
			if (!effect)
				continue;
			setup(effect, material.get());
			states.push_back(read_state(i_Kind, effect));
			//	and with the textures missing
			std::unique_ptr<matMaterial> bare(shader_material(shader, e_NonDefault));
			setup(matShaderMgr::GetEffect(*bare), bare.get());
			states.push_back(read_state(i_Kind, matShaderMgr::GetEffect(*bare)));
		}
		matTextureMgr::ReleaseTexture(normal);
		matTextureMgr::ReleaseTexture(diffuse);
		return states;
	};
	std::vector<EffectState> dx11, vulkan;
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { vulkan = run(gtBackend::e_Vulkan); }))
		return;
	gtBackend::Run(gtBackend::e_DX11, [&]() { dx11 = run(gtBackend::e_DX11); });
	ASSERT_EQ(dx11.size(), vulkan.size());
	bool anyBound = false;
	for (size_t i = 0; i < dx11.size(); i++)
	{
		SCOPED_TRACE(i);
		EXPECT_EQ(dx11[i].m_Resources, vulkan[i].m_Resources);
		for (const auto& r : vulkan[i].m_Resources)
			anyBound |= (r.second != "none");
	}
	EXPECT_TRUE(anyBound) << "no texture was bound";
}
