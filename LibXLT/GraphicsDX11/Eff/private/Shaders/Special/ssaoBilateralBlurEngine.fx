//--------------------------------------------------------------------------------------
// ported from nvidia sample.
//--------------------------------------------------------------------------------------

//#ifndef NUM_MSAA_SAMPLES
//#define NUM_MSAA_SAMPLES 2
//#endif

Texture2D tColor;
//Texture2D<float4> tColor;
sampler2D samNearestColor = sampler_state
{
    Texture   = <tColor>;
    FILTER = MIN_MAG_MIP_POINT;
    //MipFilter = POINT;
    //MinFilter = POINT;
    //MagFilter = POINT;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

Texture2D tSource;
//Texture2D<float>  tSource;
sampler2D samNearestSource = sampler_state
{
    Texture   = <tSource>;
    FILTER = MIN_MAG_MIP_POINT;
    //MipFilter = POINT;
    //MinFilter = POINT;
    //MagFilter = POINT;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

// this is the rgba normals and depths from the prior pass. 
// depth in x.
Texture2D tDepth;
//Texture2D<float>  tDepth;
sampler2D samNearestDepth = sampler_state
{
    Texture   = <tDepth>;
    FILTER = MIN_MAG_MIP_POINT;
    //MipFilter = POINT;
    //MinFilter = POINT;
    //MagFilter = POINT;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

//texture tDepthBuffer;
//Texture2D<float>  tDepthBuffer;
//texture tMSAADepth;
//Texture2DMS<float, NUM_MSAA_SAMPLES> tMSAADepth;
SamplerState samNearest
{
    Filter   = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};
SamplerState samLinear
{
    Filter   = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

float2 g_Resolution;
float2 g_InvResolution;
float g_BlurRadius;
float g_BlurFalloff;
float g_Sharpness;
float g_EdgeThreshold;

float2 g_OverscanRatio;

//--------------------------------------------------------------------------------------
struct PostProc_VSOut
{
    float4 pos : SV_Position;
    float2 tex : TEXCOORD0;
};

//Vertex shader that generates a full screen quad with texcoords g_Rom vertIDs
//To use draw 3 vertices with primitive type triangle strip
PostProc_VSOut FullScreenQuadVS( float4 Pos:SV_POSITION, float2 UV:TEXCOORD0  )
{
    PostProc_VSOut output = (PostProc_VSOut)0.0f;

	output.pos = Pos;//float4(Pos.xy, 0, 1);
	output.tex = UV;

    return output;
}

//-------------------------------------------------------------------------
static const float2 offset = float2(0.5, 0.5);

float fetch_eye_z(float2 uv)
{
	//adjust UV so depth buffer is aligned with target
	float2 depthUV = (uv - offset) * g_OverscanRatio + offset;

    float z = tDepth.SampleLevel(samNearest, depthUV, 0).x;
    return z;
}

//-------------------------------------------------------------------------
float4 BlurFunction(float2 uv, float r, float4 center_c, float center_d, inout float w_total)
{
    float4 c = tSource.SampleLevel( samNearest, uv, 0 );
    float d = fetch_eye_z(uv);

    float ddiff = d - center_d;
    float w = exp(-r*r*g_BlurFalloff - ddiff*ddiff*g_Sharpness);
    w_total += w;

    return w*c;
}

//-------------------------------------------------------------------------
float4 BlurX( PostProc_VSOut IN ): SV_TARGET
{
    float4 b = 0;
    float w_total = 0;
    float4 center_c = tSource.Sample( samNearest, IN.tex );
    float center_d = fetch_eye_z(IN.tex);
    
    for (float r = -g_BlurRadius; r <= g_BlurRadius; ++r)
    {
        float2 uv = IN.tex.xy + float2(r*g_InvResolution.x , 0);
        b += BlurFunction(uv, r, center_c, center_d, w_total);	
    }

    return b/w_total;
}
/*
float4 BlurX_SS( PostProc_VSOut IN ): SV_TARGET
{
    float b = 0;
    float w_total = 0;
    float center_c = tSource.Sample( samNearest, IN.tex ).x;

    for (float r = -g_BlurRadius; r <= g_BlurRadius; ++r) 
    {
        for (int sample = 0; sample < NUM_MSAA_SAMPLES; ++sample)
        {
            float center_d = tMSAADepth.Load( int2(IN.pos.xy), sample );
            float2 uv = IN.tex.xy + float2(r*g_InvResolution.x , 0);
            b += BlurFunction(uv, r, center_c, center_d, w_total);	
        }
    }

    return b/w_total;
}
*/
//-------------------------------------------------------------------------
float4 BlurY( uniform bool combine, PostProc_VSOut IN): SV_TARGET
{
    float4 b = 0;
    float w_total = 0;
    float4 center_c = tSource.Sample( samNearest, IN.tex, 0 );
    float center_d = fetch_eye_z(IN.tex);
    
    for (float r = -g_BlurRadius; r <= g_BlurRadius; ++r)
    {
        float2 uv = IN.tex.xy + float2(0, r*g_InvResolution.y); 
        b += BlurFunction(uv, r, center_c, center_d, w_total);
    }
    if (combine) {
        return  b/w_total * tColor.Sample(samNearest, IN.tex);
    }
    return b/w_total;	
}
/*
float4 BlurY_SS( uniform bool combine, PostProc_VSOut IN): SV_TARGET
{
    //return 0;
    float b = 0;
    float w_total = 0;
    float center_c = tSource.Sample( samNearest, IN.tex, 0 );
 
    for (float r = -g_BlurRadius; r <= g_BlurRadius; ++r)
    {
        for (int sample = 0; sample < NUM_MSAA_SAMPLES; ++sample)
        {
            float center_d = tMSAADepth.Load( int2(IN.pos.xy), sample );
            float2 uv = IN.tex.xy + float2(0, r*g_InvResolution.y);
            b += BlurFunction(uv, r, center_c, center_d, w_total);
        }
    }

    if (combine)
    {
        return  b/w_total*tColor.Sample(samNearest, float3(IN.tex, 0), 0);
    }
    return b/w_total;	
}
*/

//-------------------------------------------------------------------------
float4 Passthrough_PS( PostProc_VSOut IN ): SV_TARGET
{
    return tColor.Sample(samNearest, IN.tex);
}

//-------------------------------------------------------------------------
technique11 BlurPass
{
    pass pX
    {
        VertexShader	= compile vs_5_0 FullScreenQuadVS();
        PixelShader		= compile ps_5_0 BlurX();
    }

    pass pY
    {
        VertexShader	= compile vs_5_0 FullScreenQuadVS();
        PixelShader		= compile ps_5_0 BlurY(false);
    }
}

technique11 BlurPassWithDiffuse
{
    pass pX
    {
        VertexShader	= compile vs_5_0 FullScreenQuadVS();
        PixelShader		= compile ps_5_0 BlurX();
    }

    pass pY
    {
        VertexShader	= compile vs_5_0 FullScreenQuadVS();
        PixelShader		= compile ps_5_0 BlurY(true);
    }
}

//technique11 BlurPassSupersampling
//{
//    pass pX
//    {
//        VertexShader	= compile vs_5_0 FullScreenQuadVS();
//        PixelShader		= compile ps_5_0 BlurX_SS();
//    }
//
//    pass pY
//    {
//        VertexShader	= compile vs_5_0 FullScreenQuadVS();
//        PixelShader		= compile ps_5_0 BlurY_SS(false);
//    }
//}

//technique11 BlurPassWithDiffuseSupersampling
//{
//    pass pX
//    {
//        VertexShader	= compile vs_5_0 FullScreenQuadVS();
//        PixelShader		= compile ps_5_0 BlurX_SS();
//    }
//
//    pass pY
//    {
//        VertexShader	= compile vs_5_0 FullScreenQuadVS();
//        PixelShader		= compile ps_5_0 BlurY_SS(true);
//    }
//}

//-------------------------------------------------------------------------
technique11 BlurPassthrough
{
    pass p0
    {
        VertexShader	= compile vs_5_0 FullScreenQuadVS();
        PixelShader		= compile ps_5_0 Passthrough_PS();
    }
}

//--------------------------------------------------------------------------
float edgeDetectScalar(float sx, float sy, float threshold)
{
    float dist = (sx*sx+sy*sy);
    float e = (dist > threshold)? 1: 0;
    return e;
}

//--------------------------------------------------------------------------
float4 edgeDetectPS( uniform bool combine, PostProc_VSOut IN ): SV_TARGET
{
    // We need eight samples (the centre has zero weight in both kernels).
    float3 offset = {1, -1 , 0};

    float g00 = tDepth.Sample( samNearest, IN.tex.xy + offset.yy * g_InvResolution).x;
    float g01 = tDepth.Sample( samNearest, IN.tex.xy + offset.zy * g_InvResolution).x;
    float g02 = tDepth.Sample( samNearest, IN.tex.xy + offset.xy * g_InvResolution).x;
    float g10 = tDepth.Sample( samNearest, IN.tex.xy + offset.yz * g_InvResolution).x;
    float g11 = tDepth.Sample( samNearest, IN.tex.xy + offset.zz * g_InvResolution).x;
    float g12 = tDepth.Sample( samNearest, IN.tex.xy + offset.xz * g_InvResolution).x;
    float g20 = tDepth.Sample( samNearest, IN.tex.xy + offset.yx * g_InvResolution).x;
    float g21 = tDepth.Sample( samNearest, IN.tex.xy + offset.zx * g_InvResolution).x;
    float g22 = tDepth.Sample( samNearest, IN.tex.xy + offset.xx * g_InvResolution).x;

    // Sobel in horizontal dir.
    float sx = 0;
    sx -= g00;
    sx -= g01 * 2;
    sx -= g02;
    sx += g20;
    sx += g21 * 2;
    sx += g22;
    
    // Sobel in vertical dir - weights are just rotated 90 degrees.
    float sy = 0;
    sy -= g00;
    sy += g02;
    sy -= g10 * 2;
    sy += g12 * 2;
    sy -= g20;
    sy += g22;
    
    //return g11;

    // In theory, dist should use a sqrt.  This is a common approx.
    //float greySx = 0.333 * (sx.r + sx.g + sx.b);
    //float greySy = 0.333 * (sy.r + sy.g + sy.b);
    //float eR = edgeDetectScalar(greySx, greySy);
    if( !edgeDetectScalar(sx, sy, g_EdgeThreshold)){
        discard;    
    }
    
    return 1;
    
}

//-------------------------------------------------------------------------
technique11 BlurEdgeDetection
{
    pass p0
    {
        VertexShader	= compile vs_5_0 FullScreenQuadVS();
        PixelShader		= compile ps_5_0 edgeDetectPS(false);
    }
}
