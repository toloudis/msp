/*****************************************************************************
**  optxVector.h
**
**      Vector helpers the device programs share: float3 arithmetic, an
**	orthonormal basis, cosine-weighted directions, luminance and the power
**	heuristic. Device only (nvcc); include after optix.h.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXVECTOR_H
#error optxVector.h multiply included
#endif
#define GOX_OPTXVECTOR_H

namespace gox
{
namespace optxv
{
	constexpr float c_Pi = 3.14159265358979f;
	constexpr float c_Infinity = 1e30f;

	__forceinline__ __device__ float3 operator+(float3 a, float3 b) { return make_float3(a.x + b.x, a.y + b.y, a.z + b.z); }
	__forceinline__ __device__ float3 operator-(float3 a, float3 b) { return make_float3(a.x - b.x, a.y - b.y, a.z - b.z); }
	__forceinline__ __device__ float3 operator*(float3 a, float s) { return make_float3(a.x * s, a.y * s, a.z * s); }
	__forceinline__ __device__ float dot(float3 a, float3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
	__forceinline__ __device__ float3 cross(float3 a, float3 b)
	{
		return make_float3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	}
	__forceinline__ __device__ float3 normalize(float3 a) { return a * rsqrtf(dot(a, a)); }
	__forceinline__ __device__ float saturate(float x) { return fminf(fmaxf(x, 0.0f), 1.0f); }
	__forceinline__ __device__ float3 load3(const float* p) { return make_float3(p[0], p[1], p[2]); }
	__forceinline__ __device__ float3 mul3(float3 a, float3 b) { return make_float3(a.x * b.x, a.y * b.y, a.z * b.z); }
	__forceinline__ __device__ float3 lerp3(float3 a, float3 b, float t) { return a + (b - a) * t; }
	__forceinline__ __device__ float3 splat3(float s) { return make_float3(s, s, s); }
	__forceinline__ __device__ float max3(float3 a) { return fmaxf(a.x, fmaxf(a.y, a.z)); }
	__forceinline__ __device__ float3 min3(float3 a, float s) { return make_float3(fminf(a.x, s), fminf(a.y, s), fminf(a.z, s)); }

	//	Rec. 709 luminance of linear RGB
	__forceinline__ __device__ float luminance(float3 c) { return 0.2126f * c.x + 0.7152f * c.y + 0.0722f * c.z; }

	//	An orthonormal basis (s, t) about the unit vector n (Duff et al.,
	//	"Building an Orthonormal Basis, Revisited")
	__forceinline__ __device__ void basis(float3 n, float3& o_S, float3& o_T)
	{
		const float sign = copysignf(1.0f, n.z);
		const float a = -1.0f / (sign + n.z);
		const float b = n.x * n.y * a;
		o_S = make_float3(1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x);
		o_T = make_float3(b, sign + n.y * n.y * a, -n.y);
	}

	//	A direction about n, cosine-weighted
	__forceinline__ __device__ float3 cosine_direction(float3 n, float u1, float u2)
	{
		const float r = sqrtf(u1);
		const float phi = 2.0f * c_Pi * u2;
		float3 s, t;
		basis(n, s, t);
		return s * (r * __cosf(phi)) + t * (r * __sinf(phi)) + n * sqrtf(fmaxf(0.0f, 1.0f - u1));
	}

	__forceinline__ __device__ float power_heuristic(float a, float b)
	{
		const float a2 = a * a, b2 = b * b;
		return a2 + b2 > 0.0f ? a2 / (a2 + b2) : 0.0f;
	}
}
}
