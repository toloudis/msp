//////////////////////////////////////////////////////////////////////////////
// Converted from AAEdgeFilter.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// AAEdgeFilter.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  AAEdgeFilter.fx
**
**  Runs an Anti-Aliasing Filter on all edges.  An edge is defined as a transition of Alpha from 0-1.
**  Only addition of pixels along the edge (no subtraction), so the edge width will grow.
**  Special edges are not currently handled (i.e. curve interior/exterior, flat)
**
**	John Schwab
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

// Explicit registers keep the binding layout identical for every entry point
// (and map directly onto a DX12 root signature / Vulkan descriptor set).

// full sized source image
Texture2D colorTexture : register(t0);

// sampler state is described in AAEdgeFilter.effect.json
SamplerState g_ColorSampler : register(s0);

cbuffer AAEdgeFilterParams : register(b0)
{
	float4 g_ViewportDimensions; //(width,height,1/width,1/height)
};

float4 Samp( float2 UV )
{
   return colorTexture.SampleLevel(g_ColorSampler, UV, 0);
}

float SampA( float2 UV )
{
   return colorTexture.SampleLevel(g_ColorSampler, UV, 0).a;
}

float2 EdgeWalk( float2 Cur, float2 Side, float2 Dir )
{
	//keep looping while there is an Alpha transition of 0-1
	while( SampA( Cur ) <= 0.0f && SampA( Side ) >= 1.0f )
	{
		Cur += Dir;
		Side += Dir;
	}
	return Cur;
}


float4 AAEdgeFilter( float2 UV )
{
	float2 PixSize25 = 2.5f * g_ViewportDimensions.zw;
	float2 PixSize = g_ViewportDimensions.zw;
	float4 Clr = Samp( UV );
	float2 UDir = float2( PixSize.x, 0 );
	float2 VDir = float2( 0, PixSize.y );

//----------------vertical walk--------------------
	float2 Side = UV - UDir;   //Texture offset for side edge
	float2 WDir = VDir;      //direction of walk
	float2 Dir = UDir;		 //direction of edge step (always orthogonal to walk)
	float size = PixSize25.y;
	for( int loop = 0; loop < 4; loop++)
	{
		if( loop == 2 )      //change orientation to horizontal walk
		{
			Side = UV - VDir;
			WDir = UDir;
			Dir = VDir;
			size = 0;
		}
		float4 SideClr = Samp( Side );
		if( Clr.a <= 0.0f && SideClr.a >= 1.0f )   //check for addition edge transition
		{
			//walk along the edge to find ends
			float2 Start = EdgeWalk( UV, Side, -WDir);
			float2 End = EdgeWalk( UV, Side, WDir);

			float StartA = SampA( Start );
			float EndA = SampA( End );

			float Len = length(End - Start);
			float Offset = length(UV - Start);
			float Center = (Start.y + End.y) * 0.5f;

			if( Len > size )	//ignore horizontal pixels below this size (the virtical will pick them up).
			{
				Clr.rgb = SideClr.rgb;
				Clr.a = saturate(lerp( StartA, EndA, Offset / Len));
			}
		}
		Side += Dir * 2;	//step to opposite side
	}
	return Clr;
}

struct VS_OUTPUT
{
   float4 Pos: SV_POSITION;
   float2 img: TEXCOORD0;
};

VS_OUTPUT VSMain(float4 Pos: SV_POSITION, float2 UV : TEXCOORD0 )
{
   VS_OUTPUT Out;

   // Clean up inaccuracies
   Pos.xy = sign(Pos.xy);

   Out.Pos = float4(Pos.xy, 0, 1);
   Out.img.x = 0.5 * (1 + Pos.x);
   Out.img.y = 0.5 * (1 - Pos.y);

   return Out;
}

float4 PS_AAEdgeFilter(VS_OUTPUT v_in) : SV_TARGET 
{
	return AAEdgeFilter( v_in.img );
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

/***************************** eof ***/
