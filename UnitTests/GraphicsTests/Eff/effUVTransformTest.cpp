/*****************************************************************************
**  effUVTransformTest.cpp
**
**      effUVTransform::FromKhrTextureTransform: the material UV transform
**      made from a glTF KHR_texture_transform moves texture coordinates as
**      KHR's uv' = T * R * S * uv does.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/Mat/matShaderEffect.hpp"
#endif

#include <gtest/gtest.h>

#include <cmath>

namespace
{
	struct KhrTransform
	{
		float m_Offset[2], m_Rotation, m_Scale[2];
	};

	// KHR_texture_transform: uv' = T * R * S * uv, R = [[cos r, sin r], [-sin r, cos r]]
	void Khr(const KhrTransform& i_T, const float i_UV[2], float o_UV[2])
	{
		const float su = i_T.m_Scale[0] * i_UV[0], sv = i_T.m_Scale[1] * i_UV[1];
		const float c = cosf(i_T.m_Rotation), s = sinf(i_T.m_Rotation);
		o_UV[0] = c * su + s * sv + i_T.m_Offset[0];
		o_UV[1] = -s * su + c * sv + i_T.m_Offset[1];
	}

	// As the material vertex shaders apply g_uvTransform (Materials/Blinn.hlsl:72,
	// OUT.TexCoord0 = mul(g_uvTransform, float4(In.UV,0,1)).xy): the effect's
	// SetMatrix takes the matrix row-major, so HLSL's M[r][c] is Ptr()[r * 4 + c],
	// and mul(M, v) is sum over c of M[r][c] * v[c].
	void Ours(const maMatrix4x4& i_M, const float i_UV[2], float o_UV[2])
	{
		const float* m = i_M.GetPtr();
		const float v[4] = { i_UV[0], i_UV[1], 0, 1 };
		for (int r = 0; r < 2; r++)
		{
			o_UV[r] = m[r * 4 + 0] * v[0] + m[r * 4 + 1] * v[1] + m[r * 4 + 2] * v[2] + m[r * 4 + 3] * v[3];
		}
	}
}

TEST(effUVTransform, MatchesKhrTextureTransform)
{
	const KhrTransform transforms[] =
	{
		{ { 0, 0 }, 0, { 1, 1 } },
		{ { 0.25f, -0.5f }, 0, { 1, 1 } },
		{ { 0, 0 }, 0, { 2, 0.5f } },
		{ { 0, 0 }, 0.6f, { 1, 1 } },
		{ { 0.1f, 0.2f }, 1.2f, { 3, 0.75f } },
	};
	const float uvs[][2] = { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 0.3f, 0.7f } };

	for (const KhrTransform& t : transforms)
	{
		const maMatrix4x4 m = effUVTransform::FromKhrTextureTransform(
			t.m_Offset[0], t.m_Offset[1], t.m_Rotation, t.m_Scale[0], t.m_Scale[1]).MakeUVTransform();
		for (const auto& uv : uvs)
		{
			float khr[2], ours[2];
			Khr(t, uv, khr);
			Ours(m, uv, ours);
			EXPECT_NEAR(khr[0], ours[0], 1e-5f) << "rotation " << t.m_Rotation << " uv " << uv[0] << "," << uv[1];
			EXPECT_NEAR(khr[1], ours[1], 1e-5f) << "rotation " << t.m_Rotation << " uv " << uv[0] << "," << uv[1];
		}
	}
}
