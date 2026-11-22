#ifndef G3D_POINTLIGHT_HPP
#include "Graphics/G3d/g3dPointLight.hpp"
#endif
#ifndef G3D_PROJECTEDLIGHT_HPP
#include "Graphics/G3d/g3dProjectedLight.hpp"
#endif

#include <gtest/gtest.h>

namespace
{
	template<class Light>
	void CheckInverseSquare()
	{
		Light light;
		EXPECT_FLOAT_EQ(1.0f, light.GetFalloff0());
		// Object creation sets the constant term before the quadratic term.
		light.SetFalloff0(0.0f);
		light.SetFalloff1(0.0f);
		light.SetFalloff2(1e-4f);
		EXPECT_FLOAT_EQ(0.0f, light.GetFalloff0());
		EXPECT_FLOAT_EQ(0.0f, light.GetFalloff1());
		EXPECT_FLOAT_EQ(1e-4f, light.GetFalloff2());
		const float distanceCm = 100.0f;
		EXPECT_FLOAT_EQ(1.0f, 1.0f / (light.GetFalloff0()
			+ light.GetFalloff1() * distanceCm + light.GetFalloff2() * distanceCm * distanceCm));
		light.SetFalloff0(2.0f);
		EXPECT_FLOAT_EQ(2.0f, light.GetFalloff0());
		light.SetFalloff0(0.0f);
		EXPECT_FLOAT_EQ(0.0f, light.GetFalloff0());
	}
}

TEST(g3dLightFalloff, PointLightAcceptsInverseSquare)
{
	CheckInverseSquare<g3dPointLight>();
}

TEST(g3dLightFalloff, ProjectedLightAcceptsInverseSquare)
{
	CheckInverseSquare<g3dProjectedLight>();
}
