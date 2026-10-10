//////////////////////////////////////////////////////////////////////////////
// Converted from GIVolumes.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// GIVolumes.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

// Register layout shared with the material shaders: see Materials/Globals.hlsli.
#include "../Materials/Globals.hlsli"
#include "../Materials/Support.hlsli"
#include "../Materials/Lighting.hlsli"
#include "../Materials/Tessellate.hlsli"

// g_lightArrayNum (: LightArrayNum) and g_lightArray[8] (: LightArray), which
// this file used to declare, are members of LightParams (Globals.hlsli).

// b4: this shader's own parameters (defaults are in GIVolumes.effect.json)
cbuffer MaterialParams : register(b4)
{
	// the AO occlusion range (obscurance distance)
	float g_d;
	// distance factor when computing AO falloff.
	float g_Attenuation;
	// AO contrast
	float g_Contrast;

	// store the reduce factors for the AO buffer size.
	float2 g_UVScale;

	// Node color information
	float4 g_emissiveColor;			// default (0,0,0,0)
	float g_emissiveIntensity;		// default 1
	float4 g_diffuseColor;			// default (0,0,0,0)
	bool hasDiffuseMap;				// default false

	float4 g_GITint;				// default (0,0,0,0)
	bool hasAOBuffer;				// default false

	float2 g_Resolution;
	float2 g_InvResolution;
	float g_BlurRadius;
	float g_BlurFalloff;
	float g_Sharpness;
	float g_EdgeThreshold;

	float2 g_OverscanRatio;			// default (1,1)
};

Texture2D g_diffuseMap : register(t4);

Texture2D<float> g_ACos : register(t5);
Texture2D g_Positions : register(t6);
Texture2D g_Normals : register(t7);

Texture2D g_GIBuffer : register(t8);
Texture2D g_AOBuffer : register(t9);

SamplerState linearSampler : register(s10);

// can the AO rendertarget be smaller than the positions and normals targets?
// if they are always same size, then point sampling can work?
SamplerState sceneSampler : register(s11);

SamplerState samNearest : register(s12);

struct GS_INPUT
{
	float4 P : SV_POSITION;
	float2 TexCoord0 : TEXCOORD0;
//	float4 C : TEXCOORD0;
//	float4 N : NORMAL;
};

GS_INPUT VS_Default(STANDARD_VERTEX IN)
{
    GS_INPUT OUT;
	float4 Po = float4(IN.Position, 1.0f);
    float4 Pv = mul(g_wv, Po);
    
    //float4 Pw = mul(g_mW, Po);
    //float4 No = float4(IN.Normal, 0.0f);
    //float4 Nw = mul(g_mW, No);
    //
    //float3 diffuse = g_diffuseColor.rgb;//Tex2DCombine(hasDiffuseMap, g_diffuseMap, IN.UV, g_diffuseColor).rgb;
	//float4 emissive = g_emissiveColor * g_emissiveIntensity;
	//float3 accum_color = emissive.rgb;
    //int i = 0;
    //for (i = 0; i < g_lightArrayNum; i++)
    //{
		//float4 lightPos = g_lightArray[i].Pos;
		//float3 light_dir = normalize(lightPos.xyz - Pw.xyz);
		//accum_color += max(0, dot(light_dir.xyz, Nw.xyz)) * diffuse * g_lightArray[i].Diffuse.xyz;
    //}
    
	OUT.P = Pv;
	OUT.TexCoord0 = mul( g_uvTransform, float4(IN.UV,0,1)).xy;
	//OUT.C = float4(accum_color, 1);
//	OUT.P = float4(IN.Position, 1.0f);
//	OUT.N = float4(IN.Normal, 0.0f);

    return OUT;
}

struct HS_INPUT
{
	float4 P : TEXCOORD0;
	float4 N : NORMAL;
	float2 TexCoord0 : TEXCOORD1;
//	float4 C : TEXCOORD2;
};

HS_INPUT VS_Tess(STANDARD_VERTEX IN)
{
    HS_INPUT OUT;

	float4 Po = float4(IN.Position, 1.0f);
    float4 Pw = mul(g_world, Po);

	float4 No = float4(IN.Normal, 0.0f);
    float4 Nw = mul(g_world, No);
    
    //float3 diffuse = g_diffuseColor.rgb;//Tex2DCombine(hasDiffuseMap, g_diffuseMap, IN.UV, g_diffuseColor).rgb;
	//float4 emissive = g_emissiveColor * g_emissiveIntensity;
	//float3 accum_color = emissive.rgb + diffuse;
    //int i = 0;
    //for (i = 0; i < g_lightArrayNum; i++)
    //{
		//float4 lightPos = g_lightArray[i].Pos;
		//float3 light_dir = normalize(lightPos.xyz - Pw.xyz);
		//accum_color += max(0, dot(light_dir.xyz, Nw.xyz)) * diffuse * g_lightArray[i].Diffuse.xyz;
		////accum_color += diffuse * g_lightArray[i].Diffuse.xyz;
    //}

	OUT.P = Pw;
    OUT.N = Nw;
    OUT.TexCoord0 = mul( g_uvTransform, float4(IN.UV,0,1)).xy;
    //OUT.C = float4(accum_color, 1);
    
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
	//float4 C : TEXCOORD2;
};

HS_CONSTANT_DATA_OUTPUT Constants_HS_GIV(InputPatch<HS_INPUT, 3> inputPatch)
{
	return AdaptiveTessellate(inputPatch[0].P.xyz,
							  inputPatch[1].P.xyz,
							  inputPatch[2].P.xyz);
}

[domain("tri")]
[partitioning(SGPU_PARTITIONING)]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("Constants_HS_GIV")]
DS_INPUT HS( InputPatch<HS_INPUT, 3> inputPatch, uint uCPID : SV_OutputControlPointID )
{
	// HS_INPUT and DS_INPUT have the same layout; dxc does not convert
	// between distinct struct types implicitly, so copy member by member.
	DS_INPUT output;
	output.P = inputPatch[uCPID].P;
	output.N = inputPatch[uCPID].N;
	output.TexCoord0 = inputPatch[uCPID].TexCoord0;
	return output;
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
		
	//float4 vColor  = BarycentricCoordinates.x * TrianglePatch[0].C + 
		//BarycentricCoordinates.y * TrianglePatch[1].C + 
		//BarycentricCoordinates.z * TrianglePatch[2].C;

	//need normal to displace
#ifdef APPLY_DISPLACEMENT
	vWorldPos = DisplaceVertex( vWorldPos, vNormal, TexCoord0 );
#endif

	// Transform world position with viewprojection matrix
	output.P = mul( g_view, float4( vWorldPos, 1.0 ) );
	output.TexCoord0 = TexCoord0;
	//output.C = vColor;

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
	float4 C : TEXCOORD4;
};

[maxvertexcount(12)]
void GS(triangle GS_INPUT In[3], inout TriangleStream<PS_INPUT> OutStream)
{
	PS_INPUT vert;
	
	// inputs coming in in view space:
	vert.P0 = In[0].P;
	vert.P1 = In[1].P;
	vert.P2 = In[2].P;
	float4 Pc = (vert.P0 + vert.P1 + vert.P2) / 3;
	float2 TexCoord0 = (In[0].TexCoord0 + In[1].TexCoord0 + In[2].TexCoord0) / 3;
	
	// edge vectors in view space
	float3 e01 = vert.P1.xyz - vert.P0.xyz;
	float3 e12 = vert.P2.xyz - vert.P1.xyz;
	float3 e20 = vert.P0.xyz - vert.P2.xyz;
	
	// -m3
	float3 facenormal = normalize(cross(e01, -e20));
	
	float3 diffuse = g_diffuseColor.rgb;//Tex2DCombine(hasDiffuseMap, g_diffuseMap, IN.UV, g_diffuseColor).rgb;
	if (hasDiffuseMap)
		diffuse *= g_diffuseMap.SampleLevel(g_DefaultSampler, TexCoord0, 0).rgb;
	float4 emissive = g_emissiveColor * g_emissiveIntensity;
	float3 accum_color = emissive.rgb;
    int i = 0;
    for (i = 0; i < g_lightArrayNum; i++)
    {
		float4 lightPos = (mul(g_view, float4(g_lightArray[i].Pos.xyz, 1)));
		float3 light_dir = normalize(lightPos.xyz - Pc.xyz);
		float light_falloff = pow(length(lightPos.xyz - Pc.xyz), 0.4f) + 0.01f;
		float3 light_int = g_lightArray[i].Diffuse.xyz / light_falloff;
		accum_color += max(0, dot(light_dir.xyz, facenormal)) * diffuse * light_int;
		//accum_color += diffuse * g_lightArray[i].Diffuse.xyz;
    }
    vert.C = float4(accum_color, 1);
	
	// edge normals pointing away from the polygon
	// all three are in the plane of the poly
	// -m0
	float3 en01 = normalize(cross(e01, facenormal));
	// -m1
	float3 en12 = normalize(cross(e12, facenormal));
	// -m2
	float3 en20 = normalize(cross(e20, facenormal));
	
	vert.N = float4(facenormal,0);
	
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
		// First strip
		{
			//  Top triangle
			vert.P = mul(g_proj, float4(vert.P0.xyz + g_d * (facenormal + en01 + en20), 1.0));
			OutStream.Append(vert);
			vert.P = mul(g_proj, float4(vert.P1.xyz + g_d * (facenormal + en12 + en01), 1.0));
			OutStream.Append(vert);
			vert.P = mul(g_proj, float4(vert.P2.xyz + g_d * (facenormal + en20 + en12), 1.0));
			OutStream.Append(vert);

			//  Right side quad
			vert.P = mul(g_proj, float4(vert.P1.xyz + g_d * (en12 + en01), 1.0));
			OutStream.Append(vert);
			vert.P = mul(g_proj, float4(vert.P2.xyz + g_d * (en20 + en12), 1.0));
			OutStream.Append(vert);

			//  Bottom triangle
			vert.P = mul(g_proj, float4(vert.P0.xyz + g_d * (en01 + en20), 1.0));
			OutStream.Append(vert);

		}
		OutStream.RestartStrip();

		// Second strip
		{
			//  Back-left quad
			vert.P = mul(g_proj, float4(vert.P2.xyz + g_d * (facenormal + en20 + en12), 1.0));
			OutStream.Append(vert);
			vert.P = mul(g_proj, float4(vert.P2.xyz + g_d * (en20 + en12), 1.0));
			OutStream.Append(vert);
			vert.P = mul(g_proj, float4(vert.P0.xyz + g_d * (facenormal + en01 + en20), 1.0));
			OutStream.Append(vert);
			vert.P = mul(g_proj, float4(vert.P0.xyz + g_d * (en01 + en20), 1.0));
			OutStream.Append(vert);

			//  Front quad
			vert.P = mul(g_proj, float4(vert.P1.xyz + g_d * (facenormal + en12 + en01), 1.0));
			OutStream.Append(vert);
			vert.P = mul(g_proj, float4(vert.P1.xyz + g_d * (en12 + en01), 1.0));
			OutStream.Append(vert);
		}
		OutStream.RestartStrip();
	}
}

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
//#define NORMALIZE 1
bool clipToPlane(const in float3 n, const in float3 m3, inout float3 v[4]) 
{
    // Distances to the plane (this is an array parallel to v[], stored as a vec3)
    float3 dist;
	uint i;
    for (i = 0; i < 3; ++i) 
    {
        dist[i] = dot(v[i], n);
    }

    const float epsilon = 0.00001;

    bool quad = false;

    // Offset volumes slightly *after* clipping but before normalization 
    // to avoid degeneracies in the projArea computation, which manifest 
    // as white at cracks
    float3 offset = m3 * 0.005;

    // Normalization occurs at the bottom to keep it outside of the branches
    // and ensure thread coherence.

    // Perform this test conservatively since we want to eliminate
    // faces that are adjacent but below the point being shaded.
    // In order to be sure that two-sided surfaces don't slip and completly
    // occlude each other, we need a fairly large epsilon.  The same constant
    // appears in the ray tracer.

    if (! any( dist >= (0.01) )) 
    {
        // All clipped; no occlusion from this triangle
        clip(-1);
    }
    else if (all( dist >= (-epsilon) )) 
    {
        // None clipped (original triangle vertices are unmodified)

    }
    else 
    {
        bool3 above = (dist >= (0.0));

        // There are either 1 or 2 vertices above the clipping plane.
        bool nextIsAbove;

        // Find the ccw-most vertex above the plane by cycling
        // the vertices in place.  There are three cases.
        if (above[1] && ! above[0]) 
        {
            nextIsAbove = above[2];
            // Cycle once CCW.  Use v[3] as a temp
            v[3] = v[0]; v[0] = v[1]; v[1] = v[2]; v[2] = v[3];
            dist = dist.yzx;
        }
        else if (above[2] && ! above[1]) 
        {
            // Cycle once CW.  Use v3 as a temp.
            nextIsAbove = above[0];
            v[3] = v[2]; v[2] = v[1]; v[1] = v[0]; v[0] = v[3];
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
        v[3] = lerp(v[0], v[2], dist[0] / (dist[0] - dist[2]));

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

            v[2] = lerp(v[1], v[2], dist[1] / (dist[1] - dist[2]));
#ifdef NORMALIZE
            v[3] = normalize(v[3] + offset);
#endif

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

            v[1] = lerp(v[0], v[1], dist[0] / (dist[0] - dist[1]));
            v[2] = v[3];
        }
    }

    // v[3] was already normalized, if it is used
#ifdef NORMALIZE
    for (i = 0; i < 3; ++i) 
    {
        v[i] = normalize(v[i] + offset);
    }
#endif

    // For triangle output, duplicate first vertex to avoid a branch
    // (and therefore, incoherence) later
    v[3] = quad ? v[3] : v[0];


#ifndef NORMALIZE
    for (i = 0; i < 4; ++i) 
    {
        v[i] += offset;
    }
#endif

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

float4 PS(PS_INPUT In) : SV_TARGET
{
	//float2 uv = In.P.xy / g_TargetRes; // componentwise divide 
	//return float4(uv.x, uv.y, 0, 1);
	//uv.y = 1 - uv.y;

	// get the scene surface position under this pixel
	// let X = sample from position buffer.
	float3 N = (-g_Normals.Load(float3(In.P.xy,0)).xyz);
	// empty space?
	if (dot(N,N) == 0)
		clip(-1);
	N = normalize(N);

	float3 X = g_Positions.Load(float3(In.P.xy,0)).xyz;
	// get P0,P1,P2 in same space as X. (assuming view space for now)
	// p0 = P0-X; p1 = P1-X; p2 = P2-X (center on X)
	float3 p[4];
	p[0] = (In.P0.xyz - X);
	p[1] = (In.P1.xyz - X);
	p[2] = (In.P2.xyz - X);
	p[3] = p[0];

	// clip to positive half-space of plane using N from normal buffer
//	float3 dclip = float3(dot(p[0], N), dot(p[1], N), dot(p[2], N));
//	if (all(dclip <= float3(0,0,0)))
//		clip(-1);

	float3 m3 = -In.N.xyz;

	// this will modify p	
	bool isQuad = clipToPlane(N, m3, p);
//	if (isQuad)
//		clip(-1);

	// edge vectors
	float3 e01 = p[1] - p[0];
	float3 e12 = p[2] - p[1];
	float3 e23 = p[3] - p[2];
	float3 e30 = p[0] - p[3];
	// edge normals pointing into the polygon
	// all three are in the plane of the poly
	float3 m01 = normalize(cross(e01, m3));
	float3 m12 = normalize(cross(e12, m3));
	float3 m23 = normalize(cross(e23, m3));
	float3 m30 = isQuad ? normalize(cross(e30, m3)) : 0;
	
	// can compute g(X) now
	float g = 1;
	g = g * max(0, min(1, 1 - (g_Attenuation*dot(p[0], m01)/g_d))); 
	g = g * max(0, min(1, 1 - (g_Attenuation*dot(p[1], m12)/g_d))); 
	g = g * max(0, min(1, 1 - (g_Attenuation*dot(p[2], m23)/g_d))); 
	g = g * max(0, min(1, 1 - (g_Attenuation*dot(p[3], m30)/g_d))); 
	g = g * max(0, min(1, 1 - (g_Attenuation*dot(p[0], m3)/g_d)));// m3 is normal to orig face
	if (g <= 0)
		clip(-1);
	
	// normalize p0,p1,p2 to put them on unit sphere 
	p[0] = normalize(p[0]);
	p[1] = normalize(p[1]);
	p[2] = normalize(p[2]);
	p[3] = normalize(p[3]);
	
	float3 ab0 = p[0]-p[1];
	float3 ab1 = p[1]-p[2];
	float3 ab2 = p[2]-p[3];
	float3 ab3 = p[3]-p[0];
	// then can sum up the AO functions A(p0,p1), A(p1,p2), A(p2,p0)
	float ao = 0;
//	ao = ao + ( -0.5 * dot(N, normalize(cross(pn0, pn1))) * acos(1 - (0.5 * dot(ab0, ab0))) );
//	ao = ao + ( -0.5 * dot(N, normalize(cross(pn1, pn2))) * acos(1 - (0.5 * dot(ab1, ab1))) );
//	ao = ao + ( -0.5 * dot(N, normalize(cross(pn2, pn0))) * acos(1 - (0.5 * dot(ab2, ab2))) );
	
	float4 n = float4(
		dot(N, normalize(cross(p[0], p[1]))),
		dot(N, normalize(cross(p[1], p[2]))),
		dot(N, normalize(cross(p[2], p[3]))),
		isQuad ? dot(N, normalize(cross(p[3], p[0]))) : 0
	);
	// vectorized!
//#define	VECTORIZEACOS 1
//#define	USEACOSLOOKUP 1
#ifdef VECTORIZEACOS
#ifdef USEACOSLOOKUP
	// this formula comes from:
	//		(1-0.5*dp)*0.5 + 0.5, just like what ACosToUV4 does.
	float4 cosvalues = float4(1,1,1,1) - 0.25*float4(dot(ab0, ab0),dot(ab1, ab1),dot(ab2, ab2),dot(ab3, ab3));
	//cosvalues = ACosToUV4(cosvalues);	
	float4 c = float4(
		g_ACos.SampleLevel(linearSampler, float2( (cosvalues.x),0), 0),
		g_ACos.SampleLevel(linearSampler, float2( (cosvalues.y),0), 0),
		g_ACos.SampleLevel(linearSampler, float2( (cosvalues.z),0), 0),
		isQuad ? g_ACos.SampleLevel(linearSampler, float2( (cosvalues.w),0), 0) : 0
	);
#else //USEACOSLOOKUP
	float4 cosvalues = float4(1,1,1,1) - 0.5*float4(dot(ab0, ab0),dot(ab1, ab1),dot(ab2, ab2),dot(ab3, ab3));
	float4 c = float4(
		acos(cosvalues.x),
		acos(cosvalues.y),
		acos(cosvalues.z),
		isQuad ? acos(cosvalues.w) : 0
	);
#endif //USEACOSLOOKUP
#else //VECTORIZEACOS
#ifdef USEACOSLOOKUP
	// this formula comes from:
	//		(1-0.5*dp)*0.5 + 0.5, just like what ACosToUV does.
	float4 c = float4(
		g_ACos.SampleLevel(linearSampler, float2( (1-0.25*dot(ab0,ab0)),0), 0),
		g_ACos.SampleLevel(linearSampler, float2( (1-0.25*dot(ab1,ab1)),0), 0),
		g_ACos.SampleLevel(linearSampler, float2( (1-0.25*dot(ab2,ab2)),0), 0),
		isQuad ? g_ACos.SampleLevel(linearSampler, float2( (1-0.25*dot(ab3,ab3)),0), 0) : 0
	);
#else //USEACOSLOOKUP
	float4 c = float4(
		acos(1-0.5*dot(ab0,ab0)),
		acos(1-0.5*dot(ab1,ab1)),
		acos(1-0.5*dot(ab2,ab2)),
		isQuad ? acos(1-0.5*dot(ab3,ab3)) : 0
	);
#endif //USEACOSLOOKUP
#endif //VECTORIZEACOS
	
	ao = -0.5 * dot(n,c);
	
	float3 rao = saturate(g*(ao/3.14159265)) * In.C.rgb;
	return float4(rao,1);
}

// g_GITint, hasAOBuffer, g_GIBuffer and g_AOBuffer are declared at the top.

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

float4 GetTintedAOSample(float2 uv)
{
    //float4 ao = saturate(g_Contrast * g_GIBuffer.SampleLevel(samNearest, uv * g_UVScale, 0).x);
    float3 gi = saturate(g_Contrast * g_GIBuffer.SampleLevel(samNearest, uv * g_UVScale, 0).rgb);
    float ao;
    if (hasAOBuffer)
		ao = 1 - g_AOBuffer.Sample(linearSampler, uv * g_UVScale).r;
	else
		ao = 1.0f;
	float3 gicol = gi * ao;
	return float4(gicol, 1);
}

float4 TintAOPS
    (
    in float4 vPos : SV_POSITION,
    in float2 vScreenPosition : TEXCOORD0
    ) : SV_TARGET
{
	float3 gi = GetTintedAOSample(vScreenPosition).rgb;
    //float3 gi = saturate(g_Contrast * g_GIBuffer.Sample(linearSampler, vScreenPosition).rgb);
    //float ao;
    //if (hasAOBuffer)
		//ao = 1 - g_AOBuffer.Sample(linearSampler, vScreenPosition).r;
	//else
		//ao = 1.0f;
	////float3 aocol = g_GITint.rgb * ao;
	//float3 gicol = gi * ao;
    return float4(gi, 1);
}


// the blur parameters (g_Resolution ... g_OverscanRatio) are in MaterialParams.

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
    float4 center_c = g_GIBuffer.Sample( samNearest, vScreenPosition, 0 );
    float center_d = fetch_eye_z(vScreenPosition);
    
    for (float r = -g_BlurRadius; r <= g_BlurRadius; ++r)
    {
        float2 uv = vScreenPosition + float2(0, r*g_InvResolution.y); 
	    float4 c = g_GIBuffer.SampleLevel(samNearest, uv, 0);
        b += c*BlurFunction(uv, r, center_c, center_d, w_total);
    }
    return b/w_total;	
}
