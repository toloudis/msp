/*****************************************************************************
**  effPlainEffectVkTest.cpp
**
**      The fxEffect API the material code drives must behave the same on
**      both backends: for every embedded effect, effPlainEffectVk and
**      GraphicsDX11's effPlainEffect list the same variables (names,
**      semantics, types, annotations), techniques and passes, treat missing
**      names the same, and the setters write the same bytes.
**
**      Needs no Vulkan device (reflection only); DX11 runs on the hardware
**      adapter or WARP.
**
**	Extra Large Technology
\****************************************************************************/
#include "Dx11/gtD3D11.hpp"

#ifndef GVK_EFFPLAINEFFECTVK_HPP
#include "GraphicsVulkan/Eff/effPlainEffectVk.hpp"
#endif
#ifndef GVK_FXEMBEDDEDSHADERSVK_HPP
#include "GraphicsVulkan/Fx/fxEmbeddedShadersVk.hpp"
#endif
#ifndef EFF_PLAINEFFECT_HPP
#include "GraphicsDX11/Eff/effPlainEffect.hpp"
#endif
#ifndef FX_EMBEDDEDSHADERS_HPP
#include "GraphicsDX11/Fx/fxEmbeddedShaders.hpp"
#endif

#include <gtest/gtest.h>

#include <cstring>
#include <memory>
#include <string>
#include <vector>

namespace
{
	std::vector<std::string> effect_names()
	{
		std::vector<std::string> names;
		for (int i = 0; i < gvk::fxEmbeddedShadersVk::k_NumEffects; i++)
		{
			names.push_back(gvk::fxEmbeddedShadersVk::k_Effects[i].m_Name);
		}
		return names;
	}

	std::string str(const char* i_p)
	{
		return i_p ? i_p : "(null)";
	}

	class effPlainEffectVkTest : public ::testing::TestWithParam<std::string>
	{
		protected:
			void SetUp() override
			{
				const gvk::fxEmbeddedEffect* vk = gvk::fxEmbeddedShadersVk::Find(GetParam());
				const fxEmbeddedEffect* dx = fxEmbeddedShaders::Find(GetParam());
				ASSERT_TRUE(vk && dx);
				std::string error;
				std::unique_ptr<gvk::fxEffectVk> vkEffect = gvk::fxEmbeddedShadersVk::CreateVk(*vk, nullptr, error);
				ASSERT_TRUE(vkEffect != nullptr) << error;
				m_pVk.reset(new gvk::effPlainEffectVk(std::move(vkEffect), vk->m_Source));
				if (!m_D3D11.Create(error))
				{
					GTEST_SKIP() << "No Direct3D 11 device: " << error;
				}
				std::unique_ptr<fxEffectDX11> dxEffect = fxEmbeddedShaders::CreateDX11(*dx, m_D3D11.Device(), error);
				ASSERT_TRUE(dxEffect != nullptr) << error;
				m_pDX11.reset(new effPlainEffect(std::move(dxEffect), dx->m_Source));
			}

			static void same_type(fxEffectType* i_pDX11, gvk::fxEffectType* i_pVk, const std::string& i_What)
			{
				fxEffectTypeDesc a;
				gvk::fxEffectTypeDesc b;
				i_pDX11->GetDesc(&a);
				i_pVk->GetDesc(&b);
				EXPECT_EQ(str(a.TypeName), str(b.TypeName)) << i_What;
				EXPECT_EQ((int)a.Class, (int)b.Class) << i_What;
				EXPECT_EQ((int)a.Type, (int)b.Type) << i_What;
				EXPECT_EQ(a.Elements, b.Elements) << i_What;
				EXPECT_EQ(a.Members, b.Members) << i_What;
				EXPECT_EQ(a.Rows, b.Rows) << i_What;
				EXPECT_EQ(a.Columns, b.Columns) << i_What;
			}

			gtD3D11							m_D3D11;
			std::unique_ptr<gvk::effPlainEffectVk>	m_pVk;
			std::unique_ptr<effPlainEffect>	m_pDX11;
	};
}

TEST_P(effPlainEffectVkTest, SameVariables)
{
	fxEffectDesc_ a;
	gvk::fxEffectDesc_ b;
	m_pDX11->GetDesc(&a);
	m_pVk->GetDesc(&b);
	ASSERT_EQ(a.GlobalVariables, b.GlobalVariables);

	for (UINT i = 0; i < a.GlobalVariables; i++)
	{
		fxEffectVariable* dx = m_pDX11->GetVariableByIndex(i);
		gvk::fxEffectVariable* vk = m_pVk->GetVariableByIndex(i);
		fxEffectVariableDesc da;
		gvk::fxEffectVariableDesc db;
		dx->GetDesc(&da);
		vk->GetDesc(&db);
		const std::string name = str(da.Name);
		ASSERT_EQ(name, str(db.Name)) << "variable " << i;
		EXPECT_EQ(str(da.Semantic), str(db.Semantic)) << name;
		ASSERT_EQ(da.Annotations, db.Annotations) << name;
		same_type(dx->GetType(), vk->GetType(), name);
		if (da.Semantic)
		{
			EXPECT_EQ(vk, m_pVk->GetVariableBySemantic(da.Semantic)) << name;
		}
		EXPECT_EQ(vk, m_pVk->GetVariableByName(da.Name));

		//	annotations, by the names the manifest gives
		const fxVariableDesc* manifest = m_pDX11->GetEffectDX11()->GetDesc().FindVariable(name);
		for (size_t n = 0; manifest && n < manifest->m_Annotations.size(); n++)
		{
			const char* annotation = manifest->m_Annotations[n].m_Name.c_str();
			fxEffectVariable* ax = dx->GetAnnotationByName(annotation);
			gvk::fxEffectVariable* av = vk->GetAnnotationByName(annotation);
			const std::string what = name + "@" + annotation;
			ASSERT_EQ(ax->IsValid(), av->IsValid()) << what;
			same_type(ax->GetType(), av->GetType(), what);
			LPCSTR sx = nullptr;
			const char* sv = nullptr;
			EXPECT_EQ(SUCCEEDED(ax->GetString(&sx)), av->GetString(&sv)) << what;
			EXPECT_EQ(str(sx), str(sv)) << what;
			float fx[4] = {}, fv[4] = {};
			EXPECT_EQ(SUCCEEDED(ax->GetFloatVector(fx)), av->GetFloatVector(fv)) << what;
			EXPECT_EQ(0, memcmp(fx, fv, sizeof(fx))) << what;
		}
	}
}

TEST_P(effPlainEffectVkTest, SameTechniquesAndPasses)
{
	fxEffectDesc_ a;
	gvk::fxEffectDesc_ b;
	m_pDX11->GetDesc(&a);
	m_pVk->GetDesc(&b);
	ASSERT_EQ(a.Techniques, b.Techniques);
	for (UINT t = 0; t < a.Techniques; t++)
	{
		fxEffectTechniqueDesc ta;
		gvk::fxEffectTechniqueDesc tb;
		m_pDX11->GetTechniqueByIndex(t)->GetDesc(&ta);
		m_pVk->GetTechniqueByIndex(t)->GetDesc(&tb);
		EXPECT_EQ(str(ta.Name), str(tb.Name));
		ASSERT_EQ(ta.Passes, tb.Passes);
		EXPECT_EQ(m_pVk->GetTechniqueByIndex(t), m_pVk->GetTechniqueByName(tb.Name));
		for (UINT p = 0; p < ta.Passes; p++)
		{
			fxEffectPassDesc pa;
			gvk::fxEffectPassDesc pb;
			m_pDX11->GetTechniqueByIndex(t)->GetPassByIndex(p)->GetDesc(&pa);
			m_pVk->GetTechniqueByIndex(t)->GetPassByIndex(p)->GetDesc(&pb);
			EXPECT_EQ(str(pa.Name), str(pb.Name));
			EXPECT_EQ(m_pVk->GetTechniqueByIndex(t)->GetPassByIndex(p), m_pVk->GetTechniqueByIndex(t)->GetPassByName(pb.Name));
		}
	}
}

TEST_P(effPlainEffectVkTest, InvalidLookupsAreInvalidObjects)
{
	EXPECT_FALSE(m_pVk->GetVariableByName("no such variable")->IsValid());
	EXPECT_FALSE(m_pVk->GetVariableBySemantic("NoSuchSemantic")->IsValid());
	EXPECT_FALSE(m_pVk->GetVariableByIndex(100000)->IsValid());
	EXPECT_FALSE(m_pVk->GetTechniqueByName("no such technique")->IsValid());
	EXPECT_FALSE(m_pVk->GetTechniqueByIndex(1000)->IsValid());
	EXPECT_FALSE(m_pVk->GetTechniqueByIndex(0)->GetPassByName("no such pass")->IsValid());
	EXPECT_FALSE(m_pVk->GetTechniqueByIndex(0)->GetPassByIndex(1000)->IsValid());
	gvk::fxEffectVariable* invalid = m_pVk->GetVariableByName("no such variable");
	EXPECT_FALSE(invalid->SetFloat(1.0f));
	EXPECT_FALSE(invalid->GetMemberByName("x")->IsValid());
	EXPECT_FALSE(invalid->GetElement(0)->IsValid());
}

//	Each scalar, vector and matrix variable set through the API, with values
//	that differ per variable, gives the same constant buffer bytes
TEST_P(effPlainEffectVkTest, SettersRoundTrip)
{
	fxEffectDesc_ desc;
	m_pDX11->GetDesc(&desc);
	for (UINT i = 0; i < desc.GlobalVariables; i++)
	{
		fxEffectVariable* dx = m_pDX11->GetVariableByIndex(i);
		gvk::fxEffectVariable* vk = m_pVk->GetVariableByIndex(i);
		fxEffectTypeDesc type;
		dx->GetType()->GetDesc(&type);
		float values[16];
		for (int v = 0; v < 16; v++)
		{
			values[v] = (float)(i * 16 + v) * 0.25f - 3.0f;
		}
		switch (type.Class)
		{
			case D3D_SVC_SCALAR:
				if (type.Elements == 0)
				{
					EXPECT_EQ(SUCCEEDED(dx->SetFloat(values[0])), vk->SetFloat(values[0]));
				}
				break;
			case D3D_SVC_VECTOR:
				if (type.Elements == 0)
				{
					EXPECT_EQ(SUCCEEDED(dx->SetFloatVector(values)), vk->SetFloatVector(values));
				}
				else
				{
					EXPECT_EQ(SUCCEEDED(dx->SetFloatVectorArray(values, 0, 2)), vk->SetFloatVectorArray(values, 0, 2));
				}
				break;
			case D3D_SVC_MATRIX_ROWS:
			case D3D_SVC_MATRIX_COLUMNS:
				if (type.Elements == 0)
				{
					EXPECT_EQ(SUCCEEDED(dx->SetMatrix(values)), vk->SetMatrix(values));
				}
				break;
			default:
				break;
		}
	}

	gvk::fxEffectVk* vkEffect = m_pVk->GetEffectVk();
	fxEffectDX11* dxEffect = m_pDX11->GetEffectDX11();
	for (const std::string& name : vkEffect->GetConstantBufferNames())
	{
		const int a = dxEffect->FindConstantBuffer(name), b = vkEffect->FindConstantBuffer(name);
		ASSERT_GE(a, 0) << name;
		ASSERT_EQ(dxEffect->GetConstantBufferSize(a), vkEffect->GetConstantBufferSize(b)) << name;
		EXPECT_EQ(0, memcmp(dxEffect->GetConstantBufferData(a), vkEffect->GetConstantBufferData(b), vkEffect->GetConstantBufferSize(b)))
			<< "constant buffer " << name;
	}
}

INSTANTIATE_TEST_SUITE_P(Embedded, effPlainEffectVkTest, ::testing::ValuesIn(effect_names()),
						 [](const ::testing::TestParamInfo<std::string>& i_Info) { return i_Info.param; });
