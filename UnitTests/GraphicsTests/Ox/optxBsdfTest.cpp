/*****************************************************************************
**  optxBsdfTest.cpp
**
**      Part 3d of the OptiX plan (docs/plans/2026-11-20-optix-beauty.md,
**	step 24): the BRDF every traced surface uses (optx/private/Programs/
**	optxBsdf.h), evaluated and sampled in CUDA through optxBsdfProbe, on
**	a gtOptiX device:
**	- evaluation matches a CPU port of Pbr.hlsl's terms (GGX D,
**	  height-correlated Smith V, Schlick F, Lambert diffuse weighted by
**	  (1 - F(n.wo)) (1 - F(n.wi))) at
**	  sampled angles, roughnesses, metallic values and KHR specular and IOR
**	  values; without the extensions it is the core model's F0 = mix(0.04,
**	  baseColor, metallic);
**	- reciprocity: f(wi, wo) = f(wo, wi);
**	- the white furnace: a white surface reflects at most what arrives
**	  (the sample weights' mean), at every view angle; all of it for a
**	  Lambert surface, nearly all for rough diffuse and a mirror metal;
**	- the pdf integrates over the hemisphere to the share of samples that
**	  are directions, and a histogram of the samples matches the pdf
**	  (chi-square).
**
**	Extra Large Technology
\****************************************************************************/
#include "gtOptiX.hpp"

#ifndef GOX_OPTXBSDFPROBE_HPP
#include "GraphicsOptiX/optx/optxBsdfProbe.hpp"
#endif

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <random>
#include <string>
#include <vector>

using namespace gox;

namespace
{
	const double c_Pi = 3.14159265358979323846;

	struct V3
	{
		double x, y, z;
	};

	V3 operator+(V3 a, V3 b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
	V3 operator*(V3 a, double s) { return { a.x * s, a.y * s, a.z * s }; }
	double dot(V3 a, V3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
	V3 normalize(V3 a) { return a * (1.0 / std::sqrt(dot(a, a))); }

	//	A unit direction from polar angles about z
	V3 polar(double i_Theta, double i_Phi)
	{
		return { std::sin(i_Theta) * std::cos(i_Phi), std::sin(i_Theta) * std::sin(i_Phi), std::cos(i_Theta) };
	}

	struct Material
	{
		double	m_Base[3] = { 1, 1, 1 };
		double	m_Metallic = 0;
		double	m_Roughness = 1;
		double	m_Specular = 1;
		double	m_SpecularColor[3] = { 1, 1, 1 };
		double	m_Ior = 1.5;
	};

	optxBsdfQuery query(const Material& i_M, V3 i_N, V3 i_Wo, V3 i_Wi, double u0 = 0, double u1 = 0, double u2 = 0)
	{
		optxBsdfQuery q = {};
		for (int c = 0; c < 3; ++c)
		{
			q.m_BaseColor[c] = float(i_M.m_Base[c]);
			q.m_SpecularColor[c] = float(i_M.m_SpecularColor[c]);
		}
		q.m_Metallic = float(i_M.m_Metallic);
		q.m_Roughness = float(i_M.m_Roughness);
		q.m_Specular = float(i_M.m_Specular);
		q.m_Ior = float(i_M.m_Ior);
		q.m_U[0] = float(u0);
		q.m_U[1] = float(u1);
		q.m_U[2] = float(u2);
		const V3 v[3] = { i_N, i_Wo, i_Wi };
		float* out[3] = { q.m_Normal, q.m_Wo, q.m_Wi };
		for (int k = 0; k < 3; ++k)
		{
			out[k][0] = float(v[k].x);
			out[k][1] = float(v[k].y);
			out[k][2] = float(v[k].z);
		}
		return q;
	}

	//	Pbr.hlsl's terms (PbrLightTerms, PbrDirect), with the plan's KHR
	//	specular and IOR F0 and F90: the diffuse lobe (1 - F(NdotV))
	//	(1 - F(NdotL)) diffuse / pi and the specular lobe D V F, per channel
	void reference(const Material& i_M, V3 n, V3 wo, V3 wi, double o_Diffuse[3], double o_Specular[3])
	{
		const double metallic = std::min(1.0, std::max(0.0, i_M.m_Metallic));
		const double roughness = std::min(1.0, std::max(0.045, i_M.m_Roughness));
		const double a = roughness * roughness, a2 = a * a;
		const V3 h = wi + wo;
		const V3 H = h * (1.0 / std::sqrt(std::max(dot(h, h), 1e-8)));
		const double NdotL = std::min(1.0, std::max(0.0, dot(n, wi)));
		const double NdotV = std::max(dot(n, wo), 1e-4);
		const double NdotH = std::min(1.0, std::max(0.0, dot(n, H)));
		const double VdotH = std::min(1.0, std::max(0.0, dot(wo, H)));
		const double d = NdotH * NdotH * (a2 - 1) + 1;
		const double D = a2 / (c_Pi * d * d);
		const double gv = NdotL * std::sqrt(NdotV * NdotV * (1 - a2) + a2);
		const double gl = NdotV * std::sqrt(NdotL * NdotL * (1 - a2) + a2);
		const double Vis = 0.5 / std::max(gv + gl, 1e-5);
		const double r = (i_M.m_Ior - 1) / (i_M.m_Ior + 1);
		const double specular = std::min(1.0, std::max(0.0, i_M.m_Specular));
		for (int c = 0; c < 3; ++c)
		{
			const double dielectric = std::min(r * r * i_M.m_SpecularColor[c], 1.0) * specular;
			const double f0 = dielectric + (i_M.m_Base[c] - dielectric) * metallic;
			const double f90 = specular + (1.0 - specular) * metallic;
			const double F = f0 + (f90 - f0) * std::pow(1 - VdotH, 5);
			const double Fv = f0 + (f90 - f0) * std::pow(1 - std::min(1.0, NdotV), 5);
			const double Fl = f0 + (f90 - f0) * std::pow(1 - NdotL, 5);
			o_Specular[c] = D * Vis * F;
			o_Diffuse[c] = (1 - Fv) * (1 - Fl) * i_M.m_Base[c] * (1 - metallic) / c_Pi;
		}
	}

	bool close(double i_Expected, double i_Got, double i_Relative)
	{
		return std::fabs(i_Expected - i_Got) <= i_Relative * std::fabs(i_Expected) + 1e-5;
	}

	class optxBsdfTest : public gtOptiX
	{
		protected:
			void SetUp() override
			{
				gtOptiX::SetUp();
				if (IsSkipped() || !m_pDevice)
				{
					return;
				}
				std::string error;
				ASSERT_TRUE(m_Probe.Create(error)) << error;
			}

			void TearDown() override
			{
				m_Probe.Destroy();
				gtOptiX::TearDown();
			}

			std::vector<optxBsdfResult> run(const std::vector<optxBsdfQuery>& i_Queries)
			{
				std::vector<optxBsdfResult> results = m_Probe.Run(i_Queries);
				EXPECT_EQ(i_Queries.size(), results.size());
				return results;
			}

			//	The sample weights' mean for a view (the reflected share of
			//	white light arriving from everywhere), over i_N samples
			double furnace(const Material& i_M, double i_ViewTheta, int i_N)
			{
				const V3 n = { 0, 0, 1 };
				const V3 wo = polar(i_ViewTheta, 0.3);
				std::mt19937 rng(7);
				std::uniform_real_distribution<double> u(0.0, 1.0);
				std::vector<optxBsdfQuery> queries;
				queries.reserve(i_N);
				for (int i = 0; i < i_N; ++i)
				{
					queries.push_back(query(i_M, n, wo, n, u(rng), u(rng), u(rng)));
				}
				const std::vector<optxBsdfResult> results = run(queries);
				double sum = 0;
				for (const optxBsdfResult& r : results)
				{
					//	a grey material: the green channel stands for all
					sum += r.m_Lobe ? r.m_SampleWeight[1] : 0.0;
				}
				return results.empty() ? 0.0 : sum / results.size();
			}

			optxBsdfProbe	m_Probe;
	};

	const Material c_Materials[] = {
		[]{ Material m; m.m_Base[0] = 0.8; m.m_Base[1] = 0.5; m.m_Base[2] = 0.2; m.m_Roughness = 0.6; return m; }(),
		[]{ Material m; m.m_Base[0] = 0.9; m.m_Base[1] = 0.7; m.m_Base[2] = 0.3; m.m_Metallic = 1; m.m_Roughness = 0.25; return m; }(),
		[]{ Material m; m.m_Base[0] = 0.3; m.m_Base[1] = 0.6; m.m_Base[2] = 0.9; m.m_Metallic = 0.5; m.m_Roughness = 0.045; return m; }(),
		[]{ Material m; m.m_Base[0] = 0.5; m.m_Roughness = 1.0; m.m_Specular = 0.3; m.m_SpecularColor[0] = 1; m.m_SpecularColor[1] = 0.5; m.m_SpecularColor[2] = 0.25; return m; }(),
		[]{ Material m; m.m_Base[2] = 0.4; m.m_Roughness = 0.35; m.m_Ior = 1.33; return m; }(),
		[]{ Material m; m.m_Roughness = 0.15; m.m_Ior = 2.4; m.m_SpecularColor[1] = 8.0; return m; }(),
		[]{ Material m; m.m_Base[0] = 0.7; m.m_Roughness = 0.5; m.m_Specular = 0; return m; }(),
	};
}

TEST_F(optxBsdfTest, EvaluationMatchesPbrHlsl)
{
	std::mt19937 rng(3);
	std::uniform_real_distribution<double> u(0.0, 1.0);
	std::vector<optxBsdfQuery> queries;
	std::vector<int> which;
	std::vector<V3> views, lights, normals;
	for (int m = 0; m < int(sizeof(c_Materials) / sizeof(c_Materials[0])); ++m)
	{
		for (int i = 0; i < 64; ++i)
		{
			//	a tilted normal, and a view and light above it
			const V3 n = normalize(V3{ u(rng) - 0.5, u(rng) - 0.5, 2.0 });
			V3 wo = polar(std::acos(0.02 + 0.98 * u(rng)), 2 * c_Pi * u(rng));
			V3 wi = polar(std::acos(0.02 + 0.98 * u(rng)), 2 * c_Pi * u(rng));
			if (dot(wo, n) < 0.02 || dot(wi, n) < 0.02)
			{
				continue;
			}
			queries.push_back(query(c_Materials[m], n, wo, wi));
			which.push_back(m);
			views.push_back(wo);
			lights.push_back(wi);
			normals.push_back(n);
		}
	}
	const std::vector<optxBsdfResult> results = run(queries);
	ASSERT_EQ(queries.size(), results.size());
	int mismatches = 0;
	for (size_t i = 0; i < results.size(); ++i)
	{
		double diffuse[3], specular[3];
		reference(c_Materials[which[i]], normals[i], views[i], lights[i], diffuse, specular);
		for (int c = 0; c < 3; ++c)
		{
			if (!close(diffuse[c], results[i].m_Diffuse[c], 2e-3) || !close(specular[c], results[i].m_Specular[c], 2e-3))
			{
				if (++mismatches <= 10)
				{
					ADD_FAILURE() << "material " << which[i] << " query " << i << " channel " << c << ": diffuse "
								  << diffuse[c] << " vs " << results[i].m_Diffuse[c] << ", specular " << specular[c]
								  << " vs " << results[i].m_Specular[c];
				}
			}
		}
		EXPECT_GT(results[i].m_Pdf, 0.0f);
	}
	EXPECT_EQ(0, mismatches);
	EXPECT_GT(results.size(), 300u);
}

TEST_F(optxBsdfTest, WithoutTheExtensionsItIsTheCoreModel)
{
	//	specular 1, white specular colour, IOR 1.5: F0 = mix(0.04, base,
	//	metallic) and F90 = 1, Pbr.hlsl's own; seen and lit head-on, so the
	//	half vector is the normal and F = F0
	const V3 n = { 0, 0, 1 };
	std::vector<optxBsdfQuery> queries;
	std::vector<Material> materials;
	for (double metallic : { 0.0, 0.3, 1.0 })
	{
		Material m;
		m.m_Base[0] = 0.9;
		m.m_Base[1] = 0.4;
		m.m_Base[2] = 0.1;
		m.m_Metallic = metallic;
		m.m_Roughness = 0.5;
		materials.push_back(m);
		queries.push_back(query(m, n, n, n));
	}
	const std::vector<optxBsdfResult> results = run(queries);
	ASSERT_EQ(queries.size(), results.size());
	const double a = 0.25, a2 = a * a;
	const double D = 1.0 / (c_Pi * a2);			// NdotH = 1
	const double Vis = 0.5 / 2.0;				// NdotL = NdotV = 1
	for (size_t i = 0; i < results.size(); ++i)
	{
		for (int c = 0; c < 3; ++c)
		{
			const double f0 = 0.04 + (materials[i].m_Base[c] - 0.04) * materials[i].m_Metallic;
			EXPECT_TRUE(close(D * Vis * f0, results[i].m_Specular[c], 2e-3)) << i << " " << c;
			EXPECT_TRUE(close((1 - f0) * (1 - f0) * materials[i].m_Base[c] * (1 - materials[i].m_Metallic) / c_Pi, results[i].m_Diffuse[c], 2e-3))
				<< i << " " << c;
		}
	}
}

TEST_F(optxBsdfTest, Reciprocity)
{
	std::mt19937 rng(5);
	std::uniform_real_distribution<double> u(0.0, 1.0);
	const V3 n = { 0, 0, 1 };
	std::vector<optxBsdfQuery> forward, backward;
	for (const Material& m : c_Materials)
	{
		for (int i = 0; i < 32; ++i)
		{
			const V3 a = polar(std::acos(0.05 + 0.95 * u(rng)), 2 * c_Pi * u(rng));
			const V3 b = polar(std::acos(0.05 + 0.95 * u(rng)), 2 * c_Pi * u(rng));
			forward.push_back(query(m, n, a, b));
			backward.push_back(query(m, n, b, a));
		}
	}
	const std::vector<optxBsdfResult> f = run(forward), g = run(backward);
	ASSERT_EQ(f.size(), g.size());
	for (size_t i = 0; i < f.size(); ++i)
	{
		for (int c = 0; c < 3; ++c)
		{
			const double fa = double(f[i].m_Diffuse[c]) + f[i].m_Specular[c];
			const double fb = double(g[i].m_Diffuse[c]) + g[i].m_Specular[c];
			EXPECT_TRUE(close(fa, fb, 1e-3)) << i << " channel " << c << ": " << fa << " vs " << fb;
		}
	}
}

TEST_F(optxBsdfTest, WhiteFurnace)
{
	const int n = 1 << 16;
	for (double theta : { 0.0, 0.6, 1.2, 1.45 })
	{
		SCOPED_TRACE("view angle " + std::to_string(theta));
		//	Lambert: every sample's weight is the albedo, 1
		Material lambert;
		lambert.m_Specular = 0;
		EXPECT_NEAR(1.0, furnace(lambert, theta, n), 1e-3);

		//	never more than arrives (a little noise allowed)
		for (double roughness : { 1.0, 0.5, 0.2, 0.045 })
		{
			for (double metallic : { 0.0, 1.0 })
			{
				Material m;
				m.m_Roughness = roughness;
				m.m_Metallic = metallic;
				const double albedo = furnace(m, theta, n);
				EXPECT_LE(albedo, 1.01) << "roughness " << roughness << " metallic " << metallic;
				if (theta < 1.0)
				{
					//	single scattering loses energy: a fully rough metal (alpha
					//	1) keeps 1 - ln 2 = 0.307 seen head-on, smooth ones and
					//	rough diffuse (whose diffuse lobe carries most) nearly all
					const double floor = metallic == 0.0 ? 0.85 : roughness >= 0.9 ? 0.28 : roughness >= 0.4 ? 0.85 : 0.93;
					EXPECT_GE(albedo, floor) << "roughness " << roughness << " metallic " << metallic;
				}
			}
		}
	}
}

TEST_F(optxBsdfTest, PdfIntegratesToTheShareOfDirections)
{
	const V3 n = { 0, 0, 1 };
	for (const Material& m : c_Materials)
	{
		if (m.m_Roughness < 0.2)
		{
			continue;	// too peaked for the quadrature
		}
		for (double theta : { 0.3, 1.1 })
		{
			const V3 wo = polar(theta, 0.0);
			//	the pdf over the hemisphere, midpoint rule in (cos theta, phi)
			const int nc = 256, np = 256;
			std::vector<optxBsdfQuery> queries;
			queries.reserve(nc * np);
			for (int i = 0; i < nc; ++i)
			{
				for (int j = 0; j < np; ++j)
				{
					const double ct = (i + 0.5) / nc;
					queries.push_back(query(m, n, wo, polar(std::acos(ct), 2 * c_Pi * (j + 0.5) / np)));
				}
			}
			const std::vector<optxBsdfResult> grid = run(queries);
			double integral = 0;
			for (const optxBsdfResult& r : grid)
			{
				integral += r.m_Pdf;
			}
			integral *= 2 * c_Pi / (double(nc) * np);

			//	the share of samples that are directions
			std::mt19937 rng(9);
			std::uniform_real_distribution<double> u(0.0, 1.0);
			queries.clear();
			for (int i = 0; i < (1 << 16); ++i)
			{
				queries.push_back(query(m, n, wo, n, u(rng), u(rng), u(rng)));
			}
			const std::vector<optxBsdfResult> samples = run(queries);
			double valid = 0;
			for (const optxBsdfResult& r : samples)
			{
				valid += r.m_Lobe ? 1 : 0;
			}
			valid /= std::max<size_t>(1, samples.size());
			EXPECT_NEAR(valid, integral, 0.01) << "roughness " << m.m_Roughness << " metallic " << m.m_Metallic << " view " << theta;
		}
	}
}

TEST_F(optxBsdfTest, SamplesFollowThePdf)
{
	const V3 n = { 0, 0, 1 };
	struct Case
	{
		double	m_Roughness;
		double	m_Metallic;
		double	m_Theta;
	};
	const Case cases[] = { { 0.3, 0.0, 0.5 }, { 0.7, 0.0, 1.2 }, { 0.3, 1.0, 1.0 }, { 0.6, 1.0, 0.2 } };
	for (const Case& k : cases)
	{
		SCOPED_TRACE("roughness " + std::to_string(k.m_Roughness) + " metallic " + std::to_string(k.m_Metallic) +
					 " view " + std::to_string(k.m_Theta));
		Material m;
		m.m_Base[0] = 0.6;
		m.m_Base[1] = 0.6;
		m.m_Base[2] = 0.6;
		m.m_Roughness = k.m_Roughness;
		m.m_Metallic = k.m_Metallic;
		const V3 wo = polar(k.m_Theta, 0.0);

		//	bins in (cos theta, phi)
		const int bc = 12, bp = 16, sub = 12;
		std::vector<double> expected(bc * bp, 0.0);
		std::vector<optxBsdfQuery> queries;
		for (int i = 0; i < bc * sub; ++i)
		{
			for (int j = 0; j < bp * sub; ++j)
			{
				queries.push_back(query(m, n, wo, polar(std::acos((i + 0.5) / (bc * sub)), 2 * c_Pi * (j + 0.5) / (bp * sub))));
			}
		}
		const std::vector<optxBsdfResult> grid = run(queries);
		ASSERT_EQ(queries.size(), grid.size());
		const double cell = 2 * c_Pi / (double(bc * sub) * (bp * sub));
		for (int i = 0; i < bc * sub; ++i)
		{
			for (int j = 0; j < bp * sub; ++j)
			{
				expected[(i / sub) * bp + j / sub] += grid[size_t(i) * bp * sub + j].m_Pdf * cell;
			}
		}

		const int count = 200000;
		std::mt19937 rng(13);
		std::uniform_real_distribution<double> u(0.0, 1.0);
		queries.clear();
		for (int i = 0; i < count; ++i)
		{
			queries.push_back(query(m, n, wo, n, u(rng), u(rng), u(rng)));
		}
		const std::vector<optxBsdfResult> samples = run(queries);
		ASSERT_EQ(queries.size(), samples.size());
		std::vector<double> observed(bc * bp, 0.0);
		double invalid = 0;
		for (const optxBsdfResult& r : samples)
		{
			if (!r.m_Lobe)
			{
				invalid += 1;
				continue;
			}
			const double ct = std::min(0.999999, std::max(0.0, double(r.m_SampleWi[2])));
			double phi = std::atan2(double(r.m_SampleWi[1]), double(r.m_SampleWi[0]));
			if (phi < 0)
			{
				phi += 2 * c_Pi;
			}
			const int i = std::min(bc - 1, int(ct * bc));
			const int j = std::min(bp - 1, int(phi / (2 * c_Pi) * bp));
			observed[i * bp + j] += 1;
		}

		//	chi-square, small bins pooled
		double chi2 = 0, pooled_e = 0, pooled_o = 0, total_e = 0;
		int bins = 0;
		for (int b = 0; b < bc * bp; ++b)
		{
			const double e = expected[b] * count;
			total_e += expected[b];
			if (e < 20)
			{
				pooled_e += e;
				pooled_o += observed[b];
				continue;
			}
			chi2 += (observed[b] - e) * (observed[b] - e) / e;
			++bins;
		}
		pooled_e += (1.0 - total_e) * count;		// the samples that aren't directions
		pooled_o += invalid;
		if (pooled_e >= 20)
		{
			chi2 += (pooled_o - pooled_e) * (pooled_o - pooled_e) / pooled_e;
			++bins;
		}
		const double dof = bins - 1;
		EXPECT_GT(bins, 20);
		EXPECT_LT(chi2, dof + 6.0 * std::sqrt(2.0 * dof)) << "chi-square " << chi2 << " over " << bins << " bins";
	}
}
