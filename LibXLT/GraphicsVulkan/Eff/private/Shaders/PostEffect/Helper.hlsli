//////////////////////////////////////////////////////////////////////////////
// Converted from Helper.h by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  Helper.h
**
**      Support functions for post effect shaders
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

// Sampler state is described in the manifest of each effect that includes
// this file (Sketch.effect.json).
SamplerState defaultSampler : register(s0);

/*********** support functions ******/
float3 desaturate(float3 c)
{
	float d = saturate((c.r + c.g + c.b) / 3);
	return float3(d, d, d);
}

float3 blur(in Texture2D Tex, in float2 uv, in int w)
{
	int bw = clamp(w, 0, 7);	// restrict blur width to be 7 pixels
	float3 csum = 0;

	for (int ix = -bw; ix <= bw; ix++)
	{
		for (int iy = -bw; iy <= bw; iy++)
		{
			float2 offset = clamp(int2(ix, iy), -7, 7);
			csum += Tex.Sample(defaultSampler, uv, offset).rgb;
		}
	}

	csum /= ((2 * bw + 1) * (2 * bw + 1));

	return csum;
}

/*************** eof ****************/
