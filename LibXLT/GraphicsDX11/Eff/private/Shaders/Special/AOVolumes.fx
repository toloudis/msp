#include "..\Globals.h"
#include "..\Support.h"
#include "..\Tessellate.h"

// the AO occlusion range (obscurance distance)
float g_d;
// distance factor when computing AO falloff.
float g_Attenuation;
// AO contrast
float g_Contrast;
// -1 means flip the normals, +1 means leave alone
float g_DblSide = 1;

// must be set by app - allows for reduced resolution faster ao.
float4 g_AOTargetRes;

// store the reduce factors for the AO buffer size.
float2 g_UVScale; 

Texture2D<float> g_ACos;
Texture2D g_Positions;
Texture2D g_Normals;

SamplerState linearSampler
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

// can the AO rendertarget be smaller than the positions and normals targets?
// if they are always same size, then point sampling can work?
SamplerState sceneSampler
{
	FILTER = MIN_MAG_LINEAR_MIP_POINT;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

struct GS_INPUT
{
	float4 P : SV_POSITION;
//	float4 N : NORMAL;
};

GS_INPUT VS_Default(STANDARD_VERTEX IN)
{
    GS_INPUT OUT;
	float4 Po = float4(IN.Position, 1.0f);
    float4 Pv = mul(g_wv, Po);

	OUT.P = Pv;
//	OUT.P = float4(IN.Position, 1.0f);
//	OUT.N = float4(IN.Normal, 0.0f);

    return OUT;
}

struct HS_INPUT
{
	float4 P : TEXCOORD0;
	float4 N : NORMAL;
	float2 TexCoord0 : TEXCOORD1;
};

HS_INPUT VS_Tess(STANDARD_VERTEX IN)
{
    HS_INPUT OUT;

	float4 Po = float4(IN.Position, 1.0f);
    float4 Pw = mul(g_world, Po);

	float4 No = float4(IN.Normal, 0.0f);
    float4 Nw = mul(g_world, No);

	OUT.P = Pw;
    OUT.N = Nw;
    OUT.TexCoord0 = mul( g_uvTransform, float4(IN.UV,0,1)).xy;

    return OUT;
}

//--------------------------------------------------------------------------------------
// Hull shader
//--------------------------------------------------------------------------------------

struct DS_INPUT
{
	float4 P : TEXCOORD0;
	float4 N : NORMAL;
	float2 TexCoord0 : TEXCOORD1;
};

HS_CONSTANT_DATA_OUTPUT Constants_HS_AOV(InputPatch<HS_INPUT, 3> inputPatch)
{
	return AdaptiveTessellate(inputPatch[0].P.xyz,
							  inputPatch[1].P.xyz,
							  inputPatch[2].P.xyz);
}

[domain("tri")]
[partitioning(SGPU_PARTITIONING)]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("Constants_HS_AOV")]
DS_INPUT HS( InputPatch<HS_INPUT, 3> inputPatch, uint uCPID : SV_OutputControlPointID )
{
	return inputPatch[uCPID];
}

//--------------------------------------------------------------------------------------
// Domain Shader
//--------------------------------------------------------------------------------------

[domain("tri")]
GS_INPUT DS( HS_CONSTANT_DATA_OUTPUT input, float3 BarycentricCoordinates : SV_DomainLocation, 
						 const OutputPatch<DS_INPUT, 3> TrianglePatch, out float ClipDist : SV_ClipDistance0 )
{
	GS_INPUT output = (GS_INPUT)0;

	// Interpolate world space position with barycentric coordinates

	float3 vWorldPos = BarycentricCoordinates.x * TrianglePatch[0].P.xyz + 
		BarycentricCoordinates.y * TrianglePatch[1].P.xyz + 
		BarycentricCoordinates.z * TrianglePatch[2].P.xyz;

	float3 vNormal  = BarycentricCoordinates.x * TrianglePatch[0].N.xyz + 
		BarycentricCoordinates.y * TrianglePatch[1].N.xyz + 
		BarycentricCoordinates.z * TrianglePatch[2].N.xyz;

	float2 TexCoord0 = BarycentricCoordinates.x * TrianglePatch[0].TexCoord0 + 
		BarycentricCoordinates.y * TrianglePatch[1].TexCoord0 + 
		BarycentricCoordinates.z * TrianglePatch[2].TexCoord0;

	//need normal to displace
#ifdef APPLY_DISPLACEMENT
	vWorldPos = DisplaceVertex( vWorldPos, vNormal, TexCoord0 );
#endif

	// Transform world position with viewprojection matrix
	output.P = mul( g_view, float4( vWorldPos, 1.0 ) );

	ClipDist = ClipWorldPos( vWorldPos );

	return output;
}

struct PS_INPUT
{
	float4 P : SV_POSITION;
	nointerpolation float4 P0 : TEXCOORD0;
	nointerpolation float4 P1 : TEXCOORD1;
	nointerpolation float4 P2 : TEXCOORD2;
	nointerpolation float4 N : TEXCOORD3;

	/** Inward-facing volume face normals */
	nointerpolation float area : TEXCOORD4;
	nointerpolation float3 m0 : TEXCOORD5;
	nointerpolation float3 m1 : TEXCOORD6;
	nointerpolation float3 m2 : TEXCOORD7;
};

[maxvertexcount(12)]
void GS(triangle GS_INPUT In[3], inout TriangleStream<PS_INPUT> OutStream)
{
	PS_INPUT vert;
	
	// inputs coming in in view space:
	vert.P0 = In[0].P;
	vert.P1 = In[1].P;
	vert.P2 = In[2].P;
	
	// edge vectors in view space
	float3 e01 = (vert.P1.xyz - vert.P0.xyz) * g_DblSide;
	float3 e12 = (vert.P2.xyz - vert.P1.xyz) * g_DblSide;
	float3 e20 = (vert.P0.xyz - vert.P2.xyz) * g_DblSide;
	
	// -m3
    float3 facenormal = cross(e01, -e20) * g_DblSide;
	vert.area = length(facenormal);
	facenormal = normalize(facenormal);
	vert.N = float4(facenormal,0);

	// edge normals pointing away from the polygon
	// all three are in the plane of the poly
	// -m0
	float3 en01 = normalize(cross(e01, facenormal));
	vert.m0 = -en01;
	// -m1
	float3 en12 = normalize(cross(e12, facenormal));
	vert.m1 = -en12;
	// -m2
	float3 en20 = normalize(cross(e20, facenormal));
	vert.m2 = -en20;

    // bias is used to force low mip level, which gives a useful average across the triangle.
//	static const float bias = 4.0;
//	vec4 LC = texture2D(lambertianCoverageMap, (texCoord[0] + texCoord[1] + texCoord[2]) / 3.0, bias) * lambertianCoverageConstant;
//	sharedMeanCoverage = LC.a;

	
	// A triangular prism can be created as a single triangle strip containing 12 vertices and 10 triangles,
	// two of which are degenerate, or as two triangle strips containing 12 vertices and 8 triangles total.
	// We use the second method.
	//
	// Input vertices:
	//                            2
	//                         __*
	//                   ___--- /
	//             ___---      /
	//            *-----------*
	//           0            1
	//
	// Output vertices:
	//                             2, 6  (P2)
	//                   (P1)  __*
	//                   ___--- /|
	//             ___---  1,10/ |
	//  (P0) 0, 8 *-----------*__* 4, 7
	//            |      ___--| /
	//            |___---     |/
	//            *-----------*
	//          5, 9           3, 11
	//
	// Output strips:
	//
	//                   2     4
	//                   *-----*              6   8  10
	//                 / |\    | \             *--*--*
	//               /   | \   |   \           | /| /|
	//           0 *     |  \  |    * 5        |/ |/ |
	//               \   |   \ |   /           *--*--*
	//                 \ |    \| /            7   9  11
	//                   *-----*
	//                   1     3
	//


	// if this prism contains the camera (0,0,0)
	// then just return a single quad at the camera near plane
	float d0 = dot(en01, vert.P0.xyz);
	float d1 = dot(en12, vert.P1.xyz);
	float d2 = dot(en20, vert.P2.xyz);
	float d3 = dot(facenormal, vert.P0.xyz);
	float d4 = dot(facenormal, vert.P0.xyz + g_d*facenormal);
	// if (0,0,0) is on the negative side of all the side planes, 
	// and the positive side of the polygon plane, and the negative side of 
	// the top plane:
	if (d0 < 0 && d1 < 0 && d2 < 0 && 
		d3 > 0 && d4 < 0)
	{
		// untransformed: z=0 is nearplane
		vert.P = float4(-1, 1, 0, 1);
		OutStream.Append(vert);
		vert.P = float4(-1, -1, 0, 1);
		OutStream.Append(vert);
		vert.P = float4(1, 1, 0, 1);
		OutStream.Append(vert);
		vert.P = float4(1, -1, 0, 1);
		OutStream.Append(vert);

		OutStream.RestartStrip();
	}
	else
	{
		// (g_d*2)^2
		float maxLen2 = g_d*g_d*4;

		float3 prism[3];

		float3 offset[3];
		offset[0] = g_d * (en01 + en20);
		offset[1] = g_d * (en12 + en01);
		offset[2] = g_d * (en20 + en12);

		for (int i = 0; i < 3; i++)
		{
			// Extrusion clamp
			float len2 = dot(offset[i], offset[i]);
			// Clamp to the maximum length
			if (len2 > maxLen2) 
			{
				offset[i] *= (g_d * 2.0 * rsqrt(len2));
			}
		}

		prism[0] = vert.P0.xyz + offset[0];
		prism[1] = vert.P1.xyz + offset[1];
		prism[2] = vert.P2.xyz + offset[2];

		// First strip
		{
			//  Top triangle
			vert.P = mul(g_proj, float4(prism[0] + g_d * facenormal, 1.0));
			OutStream.Append(vert);
			vert.P = mul(g_proj, float4(prism[1] + g_d * facenormal, 1.0));
			OutStream.Append(vert);
			vert.P = mul(g_proj, float4(prism[2] + g_d * facenormal, 1.0));
			OutStream.Append(vert);

			//  Right side quad
			vert.P = mul(g_proj, float4(prism[1], 1.0));
			OutStream.Append(vert);
			vert.P = mul(g_proj, float4(prism[2], 1.0));
			OutStream.Append(vert);

			//  Bottom triangle
			vert.P = mul(g_proj, float4(prism[0], 1.0));
			OutStream.Append(vert);

		}
		OutStream.RestartStrip();

		// Second strip
		{
			//  Back-left quad
			vert.P = mul(g_proj, float4(prism[2] + g_d * facenormal, 1.0));
			OutStream.Append(vert);
			vert.P = mul(g_proj, float4(prism[2], 1.0));
			OutStream.Append(vert);
			vert.P = mul(g_proj, float4(prism[0] + g_d * facenormal, 1.0));
			OutStream.Append(vert);
			vert.P = mul(g_proj, float4(prism[0], 1.0));
			OutStream.Append(vert);

			//  Front quad
			vert.P = mul(g_proj, float4(prism[1] + g_d * facenormal, 1.0));
			OutStream.Append(vert);
			vert.P = mul(g_proj, float4(prism[1], 1.0));
			OutStream.Append(vert);
		}
		OutStream.RestartStrip();
	}
}

float g_ClipPlaneEpsilon = 0.01f;
float g_NoClipPlaneEpsilon = 0.01f;
float g_AreaRatioEpsilon = 0.1f;
float g_BehindPlaneEpsilon = 0.01f;

/**
 Clips a triangle in \a v[0..2] to the plane through the origin with normal \a n
 (and projects it onto the hemisphere if preprocessor macro NORMALIZE is #defined.) 

 The result is a convex polygon in \a v[0..3]; the last vertex may be degenerate
 and equal to the first vertex.  If that is the case, the function returns false.
 The reason that the result is *always* returned in four vertices is that subsequent
 algorithms typically iterate over edges, and the quad and tri case can be handled
 without a branch for the first three edges under this ordering.

 \return true if the result is a triangle, false if it is a quad

 Optimized (by trial and error) for GeForce 280 under GLSL 1.50

 Optimization intuition:
 1. we want to maximize coherence (to keep all threads in a warp active) by quickly reducing to a small set of common cases,
 2. minimize peak register count (to enable a large number of simultaneous threads), and
 3. avoid non-constant array indexing (which expands to a huge set of branches on most GPUs)
*/
bool clipToPlane(const in float3 n, in out float3 v0, in out float3 v1, in out float3 v2, out float3 v3) 
{
    // Distances to the plane (this is an array parallel to v[], stored as a float3)
    float3 dist = float3(dot(v0, n), dot(v1, n), dot(v2, n));

    bool quad = false;

    // Perform this test conservatively since we want to eliminate
    // faces that are adjacent but below the point being shaded.
    // In order to be sure that two-sided surfaces don't slip and completly
    // occlude each other, we need a fairly large epsilon.  The same constant
    // appears in the ray tracer.

    if (! any( dist >= (g_ClipPlaneEpsilon)))
	{
        // All clipped; no occlusion from this triangle
        clip(-1);
    }
	else if (all( dist >= (-g_NoClipPlaneEpsilon))) 
	{
        // None clipped (original triangle vertices are unmodified)

    }
	else 
	{
        bool3 above = (dist >= 0.0);

        // There are either 1 or 2 vertices above the clipping plane.
        bool nextIsAbove;

        // Find the ccw-most vertex above the plane by cycling
        // the vertices in place.  There are three cases.
        if (above[1] && ! above[0]) 
		{
            nextIsAbove = above[2];
            // Cycle once CCW.  Use v[3] as a temp
            v3 = v0; v0 = v1; v1 = v2; v2 = v3;
            dist = dist.yzx;
        }
		else if (above[2] && ! above[1]) 
		{
            // Cycle once CW.  Use v3 as a temp.
            nextIsAbove = above[0];
            v3 = v2; v2 = v1; v1 = v0; v0 = v3;
            dist = dist.zxy;
        }
		else
		{
            nextIsAbove = above[1];
        }
        // Note: The above[] values are no longer in sync with v[] and dist[].

        // Both of the following branches require the same value, so we compute
        // it into v[3] and move it to v[2] if that was the required location.
        // This helps keep some more threads coherent.

        // Compute vertex 3 first so that we don't smash the data
        // we need to reuse in vertex 2 if this is a quad.
        v3 = lerp(v0, v2, dist[0] / (dist[0] - dist[2]));

        if (nextIsAbove) 
		{
            // There is a quad above the plane
            quad = true;

            //    i0---------i1
            //      \        |
            //   .....B......A...
            //          \    |
            //            \  |
            //              i2
            v2 = lerp(v1, v2, dist[1] / (dist[1] - dist[2]));

        }
		else
		{
            // There is a triangle above the plane

            //            i0
            //           / |
            //         /   |
            //   .....B....A...
            //      /      |
            //    i2-------i1

            v2 = v3;
            v1 = lerp(v0, v1, dist[0] / (dist[0] - dist[1]));
        }
    }

    // For triangle output, duplicate first vertex to avoid a branch
    // (and therefore, incoherence) later
    v3 = quad ? v3 : v0;

    return quad;
}

float ACosToUV(in float acosvalue)
{
	// acos is in [-1..1]
	// uv is in [0..1]
	return acosvalue*0.5 + 0.5;
}
float4 ACosToUV4(in float4 acosvalue)
{
	// acos is in [-1..1]
	// uv is in [0..1]
	return acosvalue*0.5 + 0.5;
}

// Do acos in texture fetch (saves about 10% but results changed?)
float fastacos(float x) 
{
	// ensure parameter is within bounds for acos.
	return acos(clamp(x,-1,1));
//	return g_ACos.SampleLevel(linearSampler, float2( (x * 0.5 + 0.5), 0.5 ), 0).x;
}

float ComputeFalloffWeight(in  float3 origin, 
						   in PS_INPUT In,
						   out float3 p0,
						   out float3 p1, 
						   out float3 p2)
{
    // Let pm[i] = p[i].dot(m[i]),
    // where p[i] is the polygon's vertex in tangent space
    // and m[i] is the normal to edge i.  
    //
    // pm[3] uses p[0] and m[3], which is the negative
    // normal to the entire occluding polygon.  That is,
    // pm[3] is the distance to the occluding polygon.
    float4 pm;
    p0  = In.P0.xyz - origin;

    // Always the top
    pm[3] = dot(p0, -In.N.xyz);

    // Two early-out tests.
    //
    // Corectness: If distanceToPlane < 0, we're *behind* the entire volume.  We need to add a small offset
    // to ensure that we don't discard corners where a surface point is exactly
    // in the plane of the source triangle and might round off to "behind" it.
    //
    // Optimization: If area / distanceToPlane < smallConstant, then this is a small triangle relative to 
    // the point, so it will produce minimal occlusion that will round off to zero at the 
    // alpha blender.  Making the constant larger will start to abruptly truncate some occlusion.
    // Making the constant smaller will increase precision; the test can be eliminated entirely without
    // affecting correctness.
	if ((pm[3] < g_BehindPlaneEpsilon) || (In.area < pm[3] * g_AreaRatioEpsilon)) 
	{
		clip(-1);
	}

    pm[0] = dot(p0, In.m0);

    p1  = In.P1.xyz - origin;
    pm[1] = dot(p1, In.m1);

    p2  = In.P2.xyz - origin;
    pm[2] = dot(p2, In.m2);

    // Let g[i] = max(0.0f, min(1.0f, 1.0f - pm[i] * invDelta));
	float invMaxObscuranceDistance = g_Attenuation/(g_d + 0.001);
    float4 g = clamp(1.0 - pm * invMaxObscuranceDistance, (0.0), (1.0));

//	float falloffExponent = 1.0/g_Attenuation;
//	g[3] = pow(g[3], falloffExponent);

    // Recall that meanCoverage is the average alpha value of the occluding polygon.
	static const float meanCoverage = 1.0;
    float f = g[0] * g[1] * g[2] * g[3] * meanCoverage;

    // If falloffWeight is low, there's no point in computing AO        
    if (f < 0.01) 
	{
        clip(-1);
    }

    return f;
}

// No normalize or shared products
float projArea(const in float3 a, const in float3 b, const in float3 n) 
{
    float3 bXa = (cross(b, a));
    float cosine = dot(a, b);
    float theta = fastacos(cosine * rsqrt(dot(a, a) * dot(b, b)));

    return theta * dot(n, bXa) * rsqrt(dot(bXa, bXa));
}
float projArea2(const in float3 a, const in float3 b, const in float3 n) 
{
	float3 _a = normalize(a);
	float3 _b = normalize(b);
	float3 S = normalize(cross (_a,_b));
	float3 ab2 = _a - _b;
	// factoring out a factor of 0.5 to fold in after summing.
	return ( - dot(n, S) * fastacos(1 - (0.5 * dot(ab2, ab2))) );
}

/** Computes the form factor of polygon \a p[0..2] and a point at the origin with normal \a n.
    The result is on the scale 0..1.  DISCARDs if the form factor is zero. */
float ComputeFormFactor(in float3 n, in float3 p0, in float3 p1, in float3 p2) 
{
    float3 p3;

    // Clip to the plane of the deferred shading pixel.  If the triangle
    // is entirely clipped, the function will DISCARD.

    // Will discard on zero area
    bool quad = clipToPlane(n, p0, p1, p2, p3);

    float result = 0.0;
    if (quad)
	{
        result += projArea(p3, p0, n);
    }
    
    result += projArea(p0, p1, n);
    result += projArea(p1, p2, n);
    result += projArea(p2, p3, n);

    // Constants factored out of projArea
    const float adjust = 1.0 / (2.0 * 3.1415927);
    return result * adjust;
}


float4 PS(PS_INPUT In) : SV_TARGET
{
	// uncomment to visualize ao volumes
//	return float4(0, 1, 1, 0.5);

	float2 uv = In.P.xy / g_AOTargetRes.xy; // componentwise divide 

	//return float4(uv.x, uv.y, 0, 1);
	//uv.y = 1 - uv.y;

	// get the scene surface position under this pixel
	// let X = sample from position buffer.

	//float3 X = g_Positions.Load(float3(In.P.xy,0)).xyz;
	float3 X = g_Positions.SampleLevel(sceneSampler, uv, 0).xyz;


	//float3 N = (-g_Normals.Load(float3(In.P.xy,0)).xyz);
	float3 N = (-g_Normals.SampleLevel(sceneSampler, uv, 0).xyz);

	// empty space?
	if (dot(N,N) == 0)
		clip(-1);
	N = normalize(N);

	float3 p0, p1, p2, p3;

	float g = ComputeFalloffWeight(X, In, p0,p1,p2);

//	if (g_DblSide < 0)
//	{
//		// swap
//		float3 t = p2;
//		p2 = p1;
//		p1 = t;
//	}

	bool isQuad = clipToPlane(N, p0, p1, p2, p3);

	float ao = 0;
	ao += projArea(p0, p1, N);
	ao += projArea(p1, p2, N);
	ao += projArea(p2, p3, N);
	if (isQuad)
		ao += projArea(p3, p0, N);


//	if (isnan(In.P0.x) || isnan(In.P0.y) || isnan(In.P0.z) ||
//		isnan(In.P1.x) || isnan(In.P1.y) || isnan(In.P1.z) ||
//		isnan(In.P2.x) || isnan(In.P2.y) || isnan(In.P2.z) ||
//		isnan(In.N.x) || isnan(In.N.y) || isnan(In.N.z) ||
//		isnan(In.area) ||
//		isnan(In.m0.x) || isnan(In.m0.y) || isnan(In.m0.z) ||
//		isnan(In.m1.x) || isnan(In.m1.y) || isnan(In.m1.z) ||
//		isnan(In.m2.x) || isnan(In.m2.y) || isnan(In.m2.z))
//		return float4(1,1,0,1);
//	else 
//		if (isnan(ao))
//			return float4(0,1,1,1);
//		else
//			if (isinf(ao))
//				return float4(1,0,1,1);

	float rao = saturate(g * ao * 0.5 / 3.14159265);
	return float4(rao,rao,rao,1);
}

#define AOVOLUMES_HULL_AND_DOMAIN_Default

#define AOVOLUMES_HULL_AND_DOMAIN_Tess			\
	SetHullShader	(CompileShader(hs_5_0, HS()));	\
	SetDomainShader	(CompileShader(ds_5_0, DS()));

technique11 Default
{
#define PASS_AOVOLUMES(PassName)	\
	pass P##PassName				\
	{								\
		SetVertexShader		(CompileShader(vs_5_0, VS_##PassName()));	\
		AOVOLUMES_HULL_AND_DOMAIN_##PassName							\
		SetGeometryShader	(CompileShader(gs_5_0, GS()));				\
		SetPixelShader		(CompileShader(ps_5_0, PS()));				\
	}
PASS_AOVOLUMES(Default)
PASS_AOVOLUMES(Tess)
/*
	pass PDefault
	{
		SetVertexShader		(CompileShader(vs_5_0, VS_Default()));	
		SetGeometryShader	(CompileShader(gs_5_0, GS()));				 
		SetPixelShader		(CompileShader(ps_5_0, PS()));				
	}
	pass PTess
	{	
		SetVertexShader		(CompileShader(vs_5_0, VS_Tess()));	
		SetHullShader		(CompileShader(hs_5_0, HS()));
		SetDomainShader		(CompileShader(ds_5_0, DS()));
		SetGeometryShader	(CompileShader(gs_5_0, GS()));				 
		SetPixelShader		(CompileShader(ps_5_0, PS()));				
	}
*/
}

float4 g_AOTint = float4(0,0,0,0);
Texture2D g_AOBuffer;

struct VS_OUTPUT
{
   float4 Pos: SV_POSITION;
   float2 img: TEXCOORD0;
};
VS_OUTPUT VSMain(float4 Pos: SV_POSITION, float2 UV : TEXCOORD0 )
{
	VS_OUTPUT Out;
	Out.Pos = Pos;
	Out.img = UV;
	return Out;
}

SamplerState samNearest
{
    Filter   = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};
float4 GetTintedAOSample(float2 uv)
{
	float4 baseAO = g_AOBuffer.SampleLevel(samNearest, uv * g_UVScale, 0);
    float3 ao = saturate(g_Contrast * baseAO.rgb);
	float3 aocol = lerp(g_AOTint.rgb, float3(1,1,1), 1-ao);
	return float4(aocol, 1);
}
float4 TintAOPS
    (
    in float4 vPos : SV_POSITION,
    in float2 vScreenPosition : TEXCOORD0
    ) : SV_TARGET
{
	float4 aocol = GetTintedAOSample(vScreenPosition);
	// testing acos:
//	aocol = acos(vScreenPosition.x * 2 - 1)/3.14159265;
//	aocol = g_ACos.SampleLevel(linearSampler, float2( vScreenPosition.x,0), 0)/3.14159265;
    return aocol;
}
technique11 FinalPass
{
	pass P0
	{
		SetVertexShader		(CompileShader(vs_5_0, VSMain()));	
		SetHullShader		(NULL);
		SetDomainShader		(NULL);
		SetGeometryShader	(NULL);				 
		SetPixelShader		(CompileShader(ps_5_0, TintAOPS()));				
	}
}


float2 g_Resolution;
float2 g_InvResolution;
float g_BlurRadius;
float g_BlurFalloff;
float g_Sharpness;
float g_EdgeThreshold;

float2 g_OverscanRatio = {1,1};

//-------------------------------------------------------------------------
static const float2 offset = float2(0.5, 0.5);
float fetch_eye_z(float2 uv)
{
	//adjust UV so depth buffer is aligned with target
	float2 depthUV = (uv - offset) * g_OverscanRatio + offset;
    float z = g_Positions.SampleLevel(samNearest, depthUV, 0).z;
    return z;
}
//-------------------------------------------------------------------------
float BlurFunction(float2 uv, float r, float4 center_c, float center_d, inout float w_total)
{
    float d = fetch_eye_z(uv);

    float ddiff = d - center_d;
    float w = exp(-r*r*g_BlurFalloff - ddiff*ddiff*g_Sharpness);
    w_total += w;

    return w;
}
// this pass will upsample from the lower res aobuffer.
// Since we used a subviewport, we use g_DownwsampleRatio to scale uvs 
// to sample from it.
float4 BlurX_TintAOPS
    (
    in float4 vPos : SV_POSITION,
    in float2 vScreenPosition : TEXCOORD0
    ) : SV_TARGET
{
    float4 b = 0;
    float w_total = 0;
    float4 center_c = GetTintedAOSample(vScreenPosition);
    float center_d = fetch_eye_z(vScreenPosition);
    
    for (float r = -g_BlurRadius; r <= g_BlurRadius; ++r)
    {
        float2 uv = vScreenPosition + float2(r*g_InvResolution.x , 0);
        float4 c = GetTintedAOSample(uv);
        b += c*BlurFunction(uv, r, center_c, center_d, w_total);	
    }

    return b/w_total;
}
//-------------------------------------------------------------------------
// dont tint now since the results of BlurX are already tinted as appropriate.
//-------------------------------------------------------------------------
float4 BlurY_PS
    (
    in float4 vPos : SV_POSITION,
    in float2 vScreenPosition : TEXCOORD0
    ) : SV_TARGET
{
    float4 b = 0;
    float w_total = 0;
    float4 center_c = g_AOBuffer.Sample( samNearest, vScreenPosition, 0 );
    float center_d = fetch_eye_z(vScreenPosition);
    
    for (float r = -g_BlurRadius; r <= g_BlurRadius; ++r)
    {
        float2 uv = vScreenPosition + float2(0, r*g_InvResolution.y); 
	    float4 c = g_AOBuffer.SampleLevel(samNearest, uv, 0);
        b += c*BlurFunction(uv, r, center_c, center_d, w_total);
    }
    return b/w_total;	
}

technique11 FinalPassUpsample
{
	pass P0
	{
		SetVertexShader		(CompileShader(vs_5_0, VSMain()));	
		SetHullShader		(NULL);
		SetDomainShader		(NULL);
		SetGeometryShader	(NULL);				 
		SetPixelShader		(CompileShader(ps_5_0, BlurX_TintAOPS()));				
	}
	pass P1
	{
		SetVertexShader		(CompileShader(vs_5_0, VSMain()));	
		SetHullShader		(NULL);
		SetDomainShader		(NULL);
		SetGeometryShader	(NULL);				 
		SetPixelShader		(CompileShader(ps_5_0, BlurY_PS()));				
	}
}