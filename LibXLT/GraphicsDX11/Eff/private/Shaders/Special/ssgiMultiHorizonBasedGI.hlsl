//////////////////////////////////////////////////////////////////////////////
// Converted from ssgiMultiHorizonBasedGI.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// ssgiMultiHorizonBasedGI.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------
// ported from nvidia sample
//----------------------------------------------------------------------------------

static const float3 LUMINANCE_VECTOR  = float3(0.265068,  0.67023428, 0.06409157);

Texture2D tRandom : register(t0); // float3s
Texture2D tDepths : register(t1); // floats (RGBA32F) (nearest in R)
Texture2D tColors : register(t2);

SamplerState samNearest : register(s0);

#define M_PI 3.14159265f

cbuffer SSGIParams : register(b0)
{
	float2 g_Dirs[32];
	float  g_NumSteps;
	float  g_NumDir;
	float2  g_R;
	float  g_AngleBias;
	float  g_TanAngleBias;
	float  g_Attenuation;
	float  g_Contrast;
	float2 g_FocalLen;
	float2 g_InvFocalLen;
	float2 g_InvResolution;
	float2 g_Resolution;
	float2 g_OverscanRatio;
	int    g_nLayers;
	float2 g_NearFar;
	float4 g_SSGITint;	// default (1,1,1,1) is in the manifest
};

//----------------------------------------------------------------------------------
struct PostProc_VSOut
{
    float4 pos   : SV_POSITION;
    float2 texUV : TEXCOORD0;
};

// Vertex shader that generates a full screen quad with texcoords
PostProc_VSOut FullScreenQuadVS(float4 Pos:SV_POSITION, float2 UV:TEXCOORD0 )
{
    PostProc_VSOut output = (PostProc_VSOut)0.0f;

	// Clean up inaccuracies
	Pos.xy = sign(Pos.xy);
    
    // -1..1, -1..1
    output.pos = float4( Pos.xy, 0.0f, 1.0f );
    
    // Bottom left pixel is (0,0) and top right is (1,1)
	output.texUV.xy = float2(UV.x, UV.y);
   
    return output;
}


//----------------------------------------------------------------------------------
float tangent(float3 P, float3 S)
{
    return (P.z - S.z) / length(S.xy - P.xy);
}


//----------------------------------------------------------------------------------
float3 uv_to_eye(float2 uv, float eye_z)
{
//	float worldZ = eye_z * (g_NearFar.y-g_NearFar.x) + g_NearFar.x;
// this represents the inversion of the view matrix.
	// put 0..1 uv into -1..1 screenspace
    uv = (uv * float2(2.0, -2.0) - float2(1.0, -1.0));
    // now use (z*tan(fovx), z*tan(fovx)/aspect) to get point back in view space.
    return float3(uv * g_InvFocalLen * eye_z, eye_z);
}

//----------------------------------------------------------------------------------
static const float2 offset = float2(0.5, 0.5);


float3 fetch_eye_pos(float2 uv)
{
	//adjust UV so depth buffer is aligned with target
	float2 depthUV = (uv - offset) * g_OverscanRatio + offset;
    float z = tDepths.SampleLevel(samNearest, depthUV, 0).x;
	//z = g_NearFar.x + z * (g_NearFar.y - g_NearFar.x);
    return uv_to_eye(uv, z);
}

//----------------------------------------------------------------------------------
float3 tangent_eye_pos(float2 uv, float4 tangentPlane)
{
    // view vector going through the surface point at uv
    float3 V = fetch_eye_pos(uv);
    float NdotV = dot(tangentPlane.xyz, V);
    // intersect with tangent plane except for silhouette edges
    if (NdotV < 0.0) V *= (tangentPlane.w / NdotV);
    return V;
}

float length2(float3 v) { return dot(v, v); } 


//----------------------------------------------------------------------------------
float3 min_diff(float3 P, float3 Pr, float3 Pl)
{
    float3 V1 = Pr - P;
    float3 V2 = P - Pl;
    return (length2(V1) < length2(V2)) ? V1 : V2;
}


//----------------------------------------------------------------------------------
float falloff(float r)
{
    return max(0, 1.0f - g_Attenuation*r*r);
}


//----------------------------------------------------------------------------------
float2 snap_uv_offset(float2 uv)
{
    return (round(uv * g_Resolution)) * g_InvResolution;
}

float2 snap_uv_coord(float2 uv)
{
    //return (floor(uv * g_Resolution) + 0.5f) * g_InvResolution;
    return uv - (frac(uv * g_Resolution) - 0.5f) * g_InvResolution;
    //return uv;
}

//----------------------------------------------------------------------------------
float tan_to_sin(float x)
{
    return x / sqrt(1.0f + x*x);
}

//----------------------------------------------------------------------------------
float3 tangent_vector(float2 deltaUV, float3 dPdu, float3 dPdv)
{
    return deltaUV.x * dPdu + deltaUV.y * dPdv;
}


//----------------------------------------------------------------------------------
float tangent(float3 T)
{
    return -T.z / length(T.xy);
}

float SampleHBGI2(float3 P, float3 S, float2 uv0, float2 snapped_uv,
                 float3 dPdu, float3 dPdv,
                 float tanH, float h0,
				 float sqr_R, float inv_R,
                 out float tanS, out float hS)
{
    float gi = 0;
    // Ignore any samples outside the radius of influence
    float d2 = length2(S - P);
    hS = h0;
    if (d2 < sqr_R)
	{
        tanS = tangent(P, S);

        if (tanS > tanH)
		{
            // Compute tangent vector associated with snapped_uv
            float2 snapped_duv = snapped_uv - uv0;
            float3 T = tangent_vector(snapped_duv, dPdu, dPdv);
            float tanT = tangent(T) + g_TanAngleBias;

            // Compute GI between tangent T and sample S
            float sinS = tan_to_sin(tanS);
            float sinT = tan_to_sin(tanT);
            float r = sqrt(d2) * inv_R;
            float h = sinS - sinT;
            gi += falloff(r) * (h - h0);

            hS = h;
        }
    }
    return gi;
}

float3 MultiAccumulatedHorizonOcclusion_Quality(float2 deltaUV, 
                                          float2 uv0, 
                                          float3 P, 
                                          float numSteps, 
                                          float randstep,
                                          float3 dPdu,
                                          float3 dPdv,
										  float sqr_R,
										  float inv_R)
{
    // Jitter starting point within the first sample distance
    float2 uv = (uv0 + deltaUV) + randstep * deltaUV;
    
    // Snap first sample uv and initialize horizon tangent
    float2 snapped_duv = snap_uv_offset(uv - uv0);
    float3 T = tangent_vector(snapped_duv, dPdu, dPdv);
    float tanH = tangent(T) + g_TanAngleBias;

    float3 gi = 0;
    float h0 = 0;
    for(float j = 0; j < numSteps; ++j)
	{
        float2 snapped_uv = snap_uv_coord(uv);
		float2 depthUV = (snapped_uv - offset) * g_OverscanRatio + offset;
        float4 PackedDepths = tDepths.SampleLevel( samNearest, depthUV, 0);
        float3 PackedColors = tColors.SampleLevel( samNearest, snapped_uv, 0).rgb;

        float maxGI = 0.0f;
        float3 maxGIC = 0;
        float maxTanH = tanH;
        float maxH0 = h0;

        float hS, tanS;
        float3 S = uv_to_eye(uv, PackedDepths.x);
        float giS = SampleHBGI2(P, S, uv0, snapped_uv, dPdu, dPdv, tanH, h0, sqr_R, inv_R, tanS, hS);
        float3 giC = PackedColors * giS;
        if (giS > maxGI)
        {
            maxGI = giS;
            maxTanH = tanS;
            maxH0 = hS;
            maxGIC = giC;
        }

        if (g_nLayers >= 2)
		{
	        float hS2, tanS2;
            float3 S = uv_to_eye(uv, PackedDepths.y);
            float giS2 = SampleHBGI2(P, S, uv0, snapped_uv, dPdu, dPdv, tanH, h0, sqr_R, inv_R, tanS2, hS2);
            float3 giC2 = PackedColors * giS2;
            if (giS2 > maxGI)
            {
                maxGI = giS2;
                maxTanH = tanS2;
                maxH0 = hS2;
                maxGIC = giC2;
            }
        }

        if (g_nLayers >= 3)
		{
	        float hS3, tanS3;
            S = uv_to_eye(uv, PackedDepths.z);
            float giS3 = SampleHBGI2(P, S, uv0, snapped_uv, dPdu, dPdv, tanH, h0, sqr_R, inv_R, tanS3, hS3);
            float3 giC3 = PackedColors * giS3;
            if (giS3 > maxGI)
            {
                maxGI = giS3;
                maxTanH = tanS3;
                maxH0 = hS3;
                maxGIC = giC3;
            }
        }
                
        if (g_nLayers >= 4)
		{
	        float hS4, tanS4;
            S = uv_to_eye(uv, PackedDepths.w);
            float giS4 = SampleHBGI2(P, S, uv0, snapped_uv, dPdu, dPdv, tanH, h0, sqr_R, inv_R, tanS4, hS4);
            float3 giC4 = PackedColors * giS4;
            if (giS4 > maxGI)
            {
                maxGI = giS4;
                maxTanH = tanS4;
                maxH0 = hS4;
                maxGIC = giC4;
            }
        }

        tanH = maxTanH;
        h0 = maxH0;
        gi += maxGIC;

        uv += deltaUV;
    }
    return gi;
}

float2 GetTile64TexCoord(float2 pixelpos)
{
	return frac(pixelpos*g_Resolution/64.0);
}

static const float luminanceThreshold = 254.0f/255.0f;

//----------------------------------------------------------------------------------
float4 HORIZON_BASED_GI_PS( PostProc_VSOut IN ) : SV_TARGET
{
//	INtexUV.y = 1 - INtexUV.y;
//	return float4(INtexUV,0,1);
//	return float4(tex2D(samDepths, INtexUV).xxx,1);
//	return tex2D(samDepths, INtexUV).r;
//	return (tex2D(samDepths, INtexUV).r - g_NearFar.x) / (g_NearFar.y-g_NearFar.x);
//	return tex2D(samDepths, INtexUV).r;

    float3 P = fetch_eye_pos(IN.texUV);

    float4 color = tColors.SampleLevel( samNearest, IN.texUV, 0 );

    float luminance = dot(color.xyz, LUMINANCE_VECTOR);
    luminance = max(luminance, luminanceThreshold);

	float r = luminance * lerp(g_R.x, g_R.y, (P.z-g_NearFar.x)/(g_NearFar.y-g_NearFar.x) );
	float inv_r = 1.0/r;
	float sqr_r = r*r;    
	
    // Project the radius of influence g_R from eye space to texture space.
    // The scaling by 0.5 is to go from [-1,1] to [0,1].
    float2 step_size = 0.5 * r * g_FocalLen / P.z;

    // Early out if the projected radius is smaller than 1 pixel.
    float numSteps = min ( g_NumSteps, min(step_size.x * g_Resolution.x, step_size.y * g_Resolution.y));

	if( numSteps < 1.0 ) return 1.0;
    step_size /= numSteps + 1;

    // Nearest neighbor pixels on the tangent plane
    float3 Pr, Pl, Pt, Pb;
    float4 tangentPlane;

    Pr = fetch_eye_pos(IN.texUV + float2(g_InvResolution.x, 0));
    Pl = fetch_eye_pos(IN.texUV + float2(-g_InvResolution.x, 0));
    Pt = fetch_eye_pos(IN.texUV + float2(0, g_InvResolution.y));
    Pb = fetch_eye_pos(IN.texUV + float2(0, -g_InvResolution.y));
    float3 N = normalize(cross(Pr - Pl, Pt - Pb));
    tangentPlane = float4(N, dot(P, N));

   
    // Screen-aligned basis for the tangent plane
    float3 dPdu = min_diff(P, Pr, Pl);
    float3 dPdv = min_diff(P, Pt, Pb) * (g_Resolution.y * g_InvResolution.x);

    // (cos(alpha),sin(alpha),jitter)
    float3 rand_Dir = tRandom.SampleLevel(samNearest, GetTile64TexCoord(IN.texUV), 0).xyz;

    float3 gi = 0;
    for( float d = 0; d < g_NumDir; ++d)
	{
        float2 deltaUV = float2(g_Dirs[d].x*rand_Dir.x - g_Dirs[d].y*rand_Dir.y, 
                                g_Dirs[d].x*rand_Dir.y + g_Dirs[d].y*rand_Dir.x) 
                                * step_size.xy;
         gi += MultiAccumulatedHorizonOcclusion_Quality(deltaUV, IN.texUV, P, numSteps, rand_Dir.z, dPdu, dPdv, sqr_r, inv_r );
    }

	//return lerp(float4(0,0,0,1), float4(1,0,0,1), (ao / g_NumDir * g_Contrast));
	//return float4(gi * luminance * g_Contrast/ g_NumDir, 1);
	return float4(gi * g_SSGITint.rgb * luminance *  g_Contrast/ g_NumDir, 1);
//    return (ao / g_NumDir * g_Contrast);
}

//----------------------------------------------------------------------------------
