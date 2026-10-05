//////////////////////////////////////////////////////////////////////////////
// Converted from ssaoMultiHorizonBasedAO.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// ssaoMultiHorizonBasedAO.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------
// ported from nvidia sample
//----------------------------------------------------------------------------------

Texture2D tRandom : register(t0); // float3s
Texture2D tDepths : register(t1); // floats (RGBA32F) (nearest in R)

SamplerState samNearest : register(s0);

#define M_PI 3.14159265f

cbuffer SSAOParams : register(b0)
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
	float4 g_SSAOTint;
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

float SampleHBAO2(float3 P, float3 S, float2 uv0, float2 snapped_uv,
                 float3 dPdu, float3 dPdv,
                 float tanH, float h0,
				 float sqr_R, float inv_R,
                 out float tanS, out float hS)
{
    float ao = 0;
    // Ignore any samples outside the radius of influence
    float d2 = length2(S - P);
    tanS = tangent(P, S);
    hS = h0;
    if ((d2 < sqr_R) && (tanS > tanH))
	{
        // Compute tangent vector associated with snapped_uv
        float2 snapped_duv = snapped_uv - uv0;
        float3 T = tangent_vector(snapped_duv, dPdu, dPdv);
        float tanT = tangent(T) + g_TanAngleBias;

        // Compute AO between tangent T and sample S
        float sinS = tan_to_sin(tanS);
        float sinT = tan_to_sin(tanT);
        float r = sqrt(d2) * inv_R;
        float h = sinS - sinT;
        ao += falloff(r) * (h - h0);

        hS = h;
    }
    return ao;
}

float MultiAccumulatedHorizonOcclusion_Quality(float2 deltaUV, 
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

    float ao = 0;
    float h0 = 0;
    for(float j = 0; j < numSteps; ++j)
	{
        float2 snapped_uv = snap_uv_coord(uv);
		float2 depthUV = (snapped_uv - offset) * g_OverscanRatio + offset;
        float4 PackedDepths = tDepths.SampleLevel(samNearest, depthUV, 0);

        float maxAO = 0;
        float maxTanH = tanH;
        float maxH0 = h0;

        float hS, tanS;
        float3 S = uv_to_eye(uv, PackedDepths.x );
        float aoS = SampleHBAO2(P, S, uv0, snapped_uv, dPdu, dPdv, tanH, h0, sqr_R, inv_R, tanS, hS);
        if (aoS > maxAO)
        {
            maxAO = aoS;
            maxTanH = tanS;
            maxH0 = hS;
        }

        if (g_nLayers >= 2)
		{
	        float hS2, tanS2;
            S = uv_to_eye(uv, PackedDepths.y);
            float aoS2 = SampleHBAO2(P, S, uv0, snapped_uv, dPdu, dPdv, tanH, h0, sqr_R, inv_R, tanS2, hS2);

            if (aoS2 > maxAO)
            {
                maxAO = aoS2;
                maxTanH = tanS2;
                maxH0 = hS2;
            }
        }

        if (g_nLayers >= 3)
		{
	        float hS3, tanS3;
            S = uv_to_eye(uv, PackedDepths.z);
            float aoS3 = SampleHBAO2(P, S, uv0, snapped_uv, dPdu, dPdv, tanH, h0, sqr_R, inv_R, tanS3, hS3);
            if (aoS3 > maxAO)
            {
                maxAO = aoS3;
                maxTanH = tanS3;
                maxH0 = hS3;
            }
        }
                
        if (g_nLayers >= 4)
		{
	        float hS4, tanS4;
            S = uv_to_eye(uv, PackedDepths.w);
            float aoS4 = SampleHBAO2(P, S, uv0, snapped_uv, dPdu, dPdv, tanH, h0, sqr_R, inv_R, tanS4, hS4);
            if (aoS4 > maxAO)
            {
                maxAO = aoS4;
                maxTanH = tanS4;
                maxH0 = hS4;
            }
        }

        tanH = maxTanH;
        h0 = maxH0;
        ao += maxAO;

        uv += deltaUV;
    }
    return ao;
}

float VECTORIZED_SampleHBAO2(float3 P, float3 S0, float3 S1, float3 S2, float3 S3,
	float2 uv0, 
	float2 snapped_uv0, float2 snapped_uv1, float2 snapped_uv2, float2 snapped_uv3,
	float3 dPdu, float3 dPdv,
	float4 tanH, float4 h0,
	float sqr_R, float inv_R,
	out float4 vtanS, out float4 hS, out float4 vTanH)
{
    float ao = 0;
    // Ignore any samples outside the radius of influence
    float4 vd2  = float4(length2(S0 - P), length2(S1 - P), length2(S2 - P), length2(S3 - P));

	vtanS = float4(tangent(P, S0), tangent(P, S1), tangent(P, S2), tangent(P, S3));
	float4 vsinS = vtanS / sqrt(1.0f + vtanS*vtanS);
    
	float4 vr = sqrt(vd2) * inv_R;
	float4 vfalloffr = float4( falloff(vr.x), falloff(vr.y), falloff(vr.z), falloff(vr.w) );

	// init before comparison tests
	vTanH = tanH;

	hS = h0;
    if ((vd2.x < sqr_R) && (vtanS.x > tanH.x))
	{
        // Compute tangent vector associated with snapped_uv
        float2 snapped_duv = snapped_uv0 - uv0;
        float3 T = tangent_vector(snapped_duv, dPdu, dPdv);
        float tanT = tangent(T) + g_TanAngleBias;

        // Compute AO between tangent T and sample S
        float sinT = tan_to_sin(tanT);
        float h = vsinS.x - sinT;
        ao += vfalloffr.x * (h - h0.x);

        hS.x = h;
		vTanH.x = vtanS.x;
    }
    if ((vd2.y < sqr_R) && (vtanS.y > tanH.y))
	{
        // Compute tangent vector associated with snapped_uv
        float2 snapped_duv = snapped_uv1 - uv0;
        float3 T = tangent_vector(snapped_duv, dPdu, dPdv);
        float tanT = tangent(T) + g_TanAngleBias;

        // Compute AO between tangent T and sample S
        float sinT = tan_to_sin(tanT);
        float h = vsinS.y - sinT;
        ao += vfalloffr.y * (h - h0.y);

        hS.y = h;
		vTanH.y = vtanS.y;
    }
    if ((vd2.z < sqr_R) && (vtanS.z > tanH.z))
	{
        // Compute tangent vector associated with snapped_uv
        float2 snapped_duv = snapped_uv2 - uv0;
        float3 T = tangent_vector(snapped_duv, dPdu, dPdv);
        float tanT = tangent(T) + g_TanAngleBias;

        // Compute AO between tangent T and sample S
        float sinT = tan_to_sin(tanT);
        float h = vsinS.z - sinT;
        ao += vfalloffr.z * (h - h0.z);

        hS.z = h;
		vTanH.z = vtanS.z;
    }
    if ((vd2.w < sqr_R) && (vtanS.w > tanH.w))
	{
        // Compute tangent vector associated with snapped_uv
        float2 snapped_duv = snapped_uv3 - uv0;
        float3 T = tangent_vector(snapped_duv, dPdu, dPdv);
        float tanT = tangent(T) + g_TanAngleBias;

        // Compute AO between tangent T and sample S
        float sinT = tan_to_sin(tanT);
        float h = vsinS.w - sinT;
        ao += vfalloffr.w * (h - h0.w);

        hS.w = h;
		vTanH.w = vtanS.w;
    }
    return ao;
}
float VECTORIZED_MultiAccumulatedHorizonOcclusion_Quality(float2 deltaUV0, float2 deltaUV1,float2 deltaUV2,float2 deltaUV3,
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
    float2 uv_0 = (uv0 + deltaUV0) + randstep * deltaUV0;
    float2 uv_1 = (uv0 + deltaUV1) + randstep * deltaUV1;
    float2 uv_2 = (uv0 + deltaUV2) + randstep * deltaUV2;
    float2 uv_3 = (uv0 + deltaUV3) + randstep * deltaUV3;
    
    // Snap first sample uv and initialize horizon tangent
    float2 snapped_duv0 = snap_uv_offset(uv_0 - uv0);
    float2 snapped_duv1 = snap_uv_offset(uv_1 - uv0);
    float2 snapped_duv2 = snap_uv_offset(uv_2 - uv0);
    float2 snapped_duv3 = snap_uv_offset(uv_3 - uv0);

	float3 T0 = tangent_vector(snapped_duv0, dPdu, dPdv);
	float3 T1 = tangent_vector(snapped_duv1, dPdu, dPdv);
	float3 T2 = tangent_vector(snapped_duv2, dPdu, dPdv);
	float3 T3 = tangent_vector(snapped_duv3, dPdu, dPdv);

	float4 tanH;
	tanH.x = tangent(T0) + g_TanAngleBias;
	tanH.y = tangent(T1) + g_TanAngleBias;
	tanH.z = tangent(T2) + g_TanAngleBias;
	tanH.w = tangent(T3) + g_TanAngleBias;

    float ao = 0;
    float4 h0 = 0;
    for(float j = 0; j < numSteps; ++j)
	{
        float2 snapped_uv0 = snap_uv_coord(uv_0);
        float2 snapped_uv1 = snap_uv_coord(uv_1);
        float2 snapped_uv2 = snap_uv_coord(uv_2);
        float2 snapped_uv3 = snap_uv_coord(uv_3);

		float2 depthUV0 = (snapped_uv0 - offset) * g_OverscanRatio + offset;
		float2 depthUV1 = (snapped_uv1 - offset) * g_OverscanRatio + offset;
		float2 depthUV2 = (snapped_uv2 - offset) * g_OverscanRatio + offset;
		float2 depthUV3 = (snapped_uv3 - offset) * g_OverscanRatio + offset;

        float4 PackedDepths0 = tDepths.SampleLevel(samNearest, depthUV0, 0);
        float4 PackedDepths1 = tDepths.SampleLevel(samNearest, depthUV1, 0);
        float4 PackedDepths2 = tDepths.SampleLevel(samNearest, depthUV2, 0);
        float4 PackedDepths3 = tDepths.SampleLevel(samNearest, depthUV3, 0);

        float maxAO = 0;
        float4 maxTanH = tanH;
        float4 maxH0 = h0;

        float4 hS, tanS;
		float3 S0, S1, S2, S3;

        S0 = uv_to_eye(uv_0, PackedDepths0.x );
        S1 = uv_to_eye(uv_1, PackedDepths1.x );
        S2 = uv_to_eye(uv_2, PackedDepths2.x );
        S3 = uv_to_eye(uv_3, PackedDepths3.x );
        float aoS = VECTORIZED_SampleHBAO2(P, S0, S1, S2, S3, uv0, 
			snapped_uv0, snapped_uv1, snapped_uv2, snapped_uv3,
			dPdu, dPdv, tanH, h0, sqr_R, inv_R, tanS, hS, maxTanH);
        if (aoS > maxAO)
        {
            maxAO = aoS;
            maxH0 = hS;
        }

        if (g_nLayers >= 2)
		{
	        float4 hS2, tanS2;
			S0 = uv_to_eye(uv_0, PackedDepths0.y );
			S1 = uv_to_eye(uv_1, PackedDepths1.y );
			S2 = uv_to_eye(uv_2, PackedDepths2.y );
			S3 = uv_to_eye(uv_3, PackedDepths3.y );
			float aoS2 = VECTORIZED_SampleHBAO2(P, S0, S1, S2, S3, uv0, 
				snapped_uv0, snapped_uv1, snapped_uv2, snapped_uv3,
				dPdu, dPdv, tanH, h0, sqr_R, inv_R, tanS2, hS2, maxTanH);

            if (aoS2 > maxAO)
            {
                maxAO = aoS2;
                maxH0 = hS2;
            }
        }

        if (g_nLayers >= 3)
		{
	        float4 hS3, tanS3;
			S0 = uv_to_eye(uv_0, PackedDepths0.z );
			S1 = uv_to_eye(uv_1, PackedDepths1.z );
			S2 = uv_to_eye(uv_2, PackedDepths2.z );
			S3 = uv_to_eye(uv_3, PackedDepths3.z );
			float aoS3 = VECTORIZED_SampleHBAO2(P, S0, S1, S2, S3, uv0, 
				snapped_uv0, snapped_uv1, snapped_uv2, snapped_uv3,
				dPdu, dPdv, tanH, h0, sqr_R, inv_R, tanS3, hS3, maxTanH);
            if (aoS3 > maxAO)
            {
                maxAO = aoS3;
                maxH0 = hS3;
            }
        }
                
        if (g_nLayers >= 4)
		{
	        float4 hS4, tanS4;
			S0 = uv_to_eye(uv_0, PackedDepths0.w );
			S1 = uv_to_eye(uv_1, PackedDepths1.w );
			S2 = uv_to_eye(uv_2, PackedDepths2.w );
			S3 = uv_to_eye(uv_3, PackedDepths3.w );
			float aoS4 = VECTORIZED_SampleHBAO2(P, S0, S1, S2, S3, uv0, 
				snapped_uv0, snapped_uv1, snapped_uv2, snapped_uv3,
				dPdu, dPdv, tanH, h0, sqr_R, inv_R, tanS4, hS4, maxTanH);
            if (aoS4 > maxAO)
            {
                maxAO = aoS4;
                maxH0 = hS4;
            }
        }

        tanH = maxTanH;
        h0 = maxH0;
        ao += maxAO;

        uv_0 += deltaUV0;
        uv_1 += deltaUV1;
        uv_2 += deltaUV2;
        uv_3 += deltaUV3;
    }
    return ao;
}


float2 GetTile64TexCoord(float2 pixelpos)
{
	return frac(pixelpos*g_Resolution/64.0);
}

//----------------------------------------------------------------------------------
float4 HORIZON_BASED_AO_PS( PostProc_VSOut IN ) : SV_TARGET
{
//	INtexUV.y = 1 - INtexUV.y;
//	return float4(INtexUV,0,1);
//	return float4(tex2D(samDepths, INtexUV).xxx,1);
//	return tex2D(samDepths, INtexUV).r;
//	return (tex2D(samDepths, INtexUV).r - g_NearFar.x) / (g_NearFar.y-g_NearFar.x);
//	return tex2D(samDepths, INtexUV).r;

    float3 P = fetch_eye_pos(IN.texUV);

	float r = lerp(g_R.x, g_R.y, (P.z-g_NearFar.x)/(g_NearFar.y-g_NearFar.x) );
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
    //float3 rand_Dir = tRandom.Load(int3((int)IN.pos.x&63, (int)IN.pos.y&63, 0)).xyz;
    //float3 rand_Dir = tex2D(samNearestRandom, GetTile64TexCoord(INtexUV)).xyz;

    float ao = 0;

#define VECTORIZED_CODE 1
#if VECTORIZED_CODE == 1

    // Handle 4 directions at once. 
    for (uint d = 0; d < (uint)floor(g_NumDir/4.0); d++) 
    {
		float2 deltaUV0 = float2(g_Dirs[4*d+0].x*rand_Dir.x - g_Dirs[4*d+0].y*rand_Dir.y, 
								g_Dirs[4*d+0].x*rand_Dir.y + g_Dirs[4*d+0].y*rand_Dir.x) 
								* step_size.xy;
		float2 deltaUV1 = float2(g_Dirs[4*d+1].x*rand_Dir.x - g_Dirs[4*d+1].y*rand_Dir.y, 
								g_Dirs[4*d+1].x*rand_Dir.y + g_Dirs[4*d+1].y*rand_Dir.x) 
								* step_size.xy;
		float2 deltaUV2 = float2(g_Dirs[4*d+2].x*rand_Dir.x - g_Dirs[4*d+2].y*rand_Dir.y, 
								g_Dirs[4*d+2].x*rand_Dir.y + g_Dirs[4*d+2].y*rand_Dir.x) 
								* step_size.xy;
		float2 deltaUV3 = float2(g_Dirs[4*d+3].x*rand_Dir.x - g_Dirs[4*d+3].y*rand_Dir.y, 
								g_Dirs[4*d+3].x*rand_Dir.y + g_Dirs[4*d+3].y*rand_Dir.x) 
								* step_size.xy;

		ao += VECTORIZED_MultiAccumulatedHorizonOcclusion_Quality(deltaUV0, deltaUV1, deltaUV2, deltaUV3,
			IN.texUV, P, numSteps, rand_Dir.z, dPdu, dPdv, sqr_r, inv_r );
    }
        
#else

    for( uint d = 0; d < (uint)g_NumDir; ++d)
	{
        float2 deltaUV = float2(g_Dirs[d].x*rand_Dir.x - g_Dirs[d].y*rand_Dir.y, 
                                g_Dirs[d].x*rand_Dir.y + g_Dirs[d].y*rand_Dir.x) 
                                * step_size.xy;
         ao += MultiAccumulatedHorizonOcclusion_Quality(deltaUV, IN.texUV, P, numSteps, rand_Dir.z, dPdu, dPdv, sqr_r, inv_r );
    }

#endif

	return lerp(float4(1,1,1,1), g_SSAOTint, (ao / g_NumDir * g_Contrast));
//    return (ao / g_NumDir * g_Contrast);
}

//----------------------------------------------------------------------------------
