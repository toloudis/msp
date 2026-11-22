//////////////////////////////////////////////////////////////////////////////
// Converted from Hair_SH.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// Hair_SH.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  Hair_SH.fx
**
**     This hair shader supports Shave and a Haircut rendering.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
**
**  Note: Geometry is assumed to be line segments where the normal is
**   tangent to the line direction.
\****************************************************************************/
// The SasGlobal effect description (gp : SasGlobal) and the SAS UI
// annotations of the material parameters are in Hair_SH.effect.json.
// Register layout shared by all materials: see Globals.hlsli; the hair
// system's own resources (b5 HairParams, t26-t38, u1, s10): see
// SupportHair.hlsli.

/*********** support data and functions ******/

#include "Support.hlsli"
#include "SupportHair.hlsli"

/************** Material Parameters*******/

// b4: this material's own parameters (defaults are in Hair_SH.effect.json)
cbuffer MaterialParams : register(b4)
{
	float g_Transparency;			// : Opacity, default 1
	float g_SpecularScale;			// default 1
	float g_GlossScale;				// default 1
	float g_AmbDiffScale;			// default 1
	float4 g_SpecColor;				// : MaterialSpecular, default (1,1,1,1)
	float g_densityScale;			// default 1
	float g_DepthBias;				// default 0
};

/************* DATA STRUCTS **************/

//Note that U and V have been spread to conform with packing rules
//This is requires so that the uint gets it's own register
struct VertexOutput
{
    float4 HPosition	: SV_POSITION;
	float3 WPosition	: TEXCOORD0;
	float  U            : TEXCOORD1;
	float3 WTangent     : TEXCOORD2;
	float  V            : TEXCOORD3;
	float4 Color		: COLOR0;
	float4 Material     : TEXCOORD4;
	nointerpolation uint Segment : TEXCOORD5;	//use to cull out degenerate strands
};

struct GeometryOutput
{
    float4 HPosition	: SV_POSITION;
	float3 WPosition	: TEXCOORD0;
	float  U            : TEXCOORD1;
	float3 WTangent     : TEXCOORD2;
	float  V            : TEXCOORD3;
	float4 Color		: COLOR0;
	float4 Material     : TEXCOORD4;
	float  ClipDist     : SV_ClipDistance0;
};

//ClipDist is consumed from GS
struct PixelInput
{
    float4 HPosition	: SV_POSITION;
	float3 WPosition	: TEXCOORD0;
	float  U            : TEXCOORD1;
	float3 WTangent     : TEXCOORD2;
	float  V            : TEXCOORD3;
	float4 Color		: COLOR0;
	float4 Material     : TEXCOORD4;
};

/*********** vertex shader ******/
VertexOutput HairVS_NoTess( uint VertID : SV_VertexID )
{
    VertexOutput OUT = (VertexOutput)0;

	uint nStrands = g_nStrandCPs-1;
	uint curSegment = VertID % g_nStrandCPs;
	uint curStrand = VertID / g_nStrandCPs;

	hairMaterial Mtl = g_HairMaterials[ curStrand ];

	//calculate hair vertex indices
	uint strandBaseID = curStrand * g_nStrandCPs;
	uint CPBase = strandBaseID + curSegment;

	float3 Pos = g_HairGeometry[ CPBase ];
	float3 Tan;

	//use next vertex if not at the end otherwise use previous
	if( curSegment >= nStrands )
	{
		Tan = Pos - g_HairGeometry[ CPBase-1 ];
	}
	else
	{
		Tan = g_HairGeometry[ CPBase+1 ] - Pos;
	}

	float3 vWorldPos = mul( g_world, float4( Pos, 1)).xyz;
	float3 vWorldTan = mul( (float3x3)g_world, Tan );

	float t = curSegment / (float)nStrands;

	OUT.Color.rgb = lerp( Mtl.m_RootColor, Mtl.m_TipColor, t);
	OUT.Color.a = 1.0f;
	float radius = lerp( Mtl.m_RootRadius, Mtl.m_TipRadius, t );

	OUT.WPosition = vWorldPos;
	OUT.WTangent  = vWorldTan;
	OUT.U = abs( radius );
	OUT.V = t;
	OUT.Material = float4( Mtl.m_Opacity, Mtl.m_Specular, Mtl.m_Gloss, Mtl.m_AmbientDiffuse );
	OUT.Segment = curSegment;

    return OUT;
}

//NULL vertex shader
void HairVS_Tess(){}

/********* pixel shader ********/

#define HAIRSHADER 2
float4 AnisotropicLighting( float2 UV, float3 L, float3 V, float3 T, 
	float3 DifLight, float3 SpecLight, float3 hairColor )
{
#if HAIRSHADER == 1
	float cosang = cos( abs( acos(dot(T, L)) - acos(-dot(T,V)) ) );
	float3 Cspec = SpecLight * UV.y * pow(saturate(cosang), g_SpecularScale);
	float3 Cdiff = hairColor * DifLight * UV.y * dot(T, L);
/* We multipled by v to make it darker at the roots. This
* assumes v=0 at the root, v=1 at the tip.
*/
	return float4(Cspec+Cdiff, 1);

#elif HAIRSHADER == 2

// S&H shader
	float Ka = 0;
	float SHAVEambdiff = .6;
	float SHAVEspec = .35;
	float SHAVEgloss = .07;
	float SHAVEopacity = 1.0;
	float SHAVEselfshad=1;
	float3 SHAVEspec_color=1;
	//float3 rootcolor=1;
	//float3 tipcolor=1;

	float3 Cspec = 0, Cdiff = 0; /* collect specular & diffuse light */
//	float	tl;
	
/* Looping over lights, per apadoca paper, the math's a bit different */
	float sq2;
	float df2=dot(T,L);
	df2*=df2;
	df2=1.0-df2;
	if (df2<0)
		df2=0;
	if (df2>0)
		df2=sqrt(df2); 
	float diffterm=df2; /* diffuse */
	if (diffterm<0)
		diffterm=0;
	float vt =  dot(V,T);
	sq2=1.0-vt*vt;
	if (sq2<0) 
		sq2=0;
	if (sq2>0)
		sq2=sqrt(sq2);
	float rawspec = df2* sqrt( 1.0- vt * vt ) -  dot(L, T) * vt; /* raw specular */
	if (rawspec<0) 
		rawspec=0; 
	
	diffterm=(1.0-SHAVEambdiff)+diffterm*SHAVEambdiff; /* limits gamut of diffuse term */
	float3 Cl2s = SpecLight * SHAVEselfshad + (1 - SHAVEselfshad); /* limits the gamut of shadowing */
	float3 Cl2d = DifLight * SHAVEselfshad + (1 - SHAVEselfshad); /* limits the gamut of shadowing */
	Cspec += Cl2s*pow( abs(max(rawspec,0)), 1.0 / ( 3.0 * ( .101 - SHAVEgloss ) ) )*.5;   /* specular exponent x illumination */
	Cdiff += Cl2d*diffterm; /* diffuse x illumination */

	float3 mixed = hairColor;//lerp( rootcolor, tipcolor, v );
	float Oi = 1;//Os*SHAVEopacity;
	float3 Ci = Oi * mixed * (Cdiff) + ( Cspec * SHAVEspec_color *SHAVEspec);  /*sum terms and premult color x opac */
	
	return float4(Ci,1);
	
#elif HAIRSHADER == 3

	// beginning work on a sample kay-kajiya port
	// NOT READY TO RUN YET
	// this shader source comes from http://www.185vfx.com/resources/coursenotes.html
////////////////////////	
////////////////////////	
	float T_Dot_nL = dot(T, L);
	float T_Dot_e = dot(T, V);
	float Alpha = acos(T_Dot_nL);
	float Beta = acos(T_Dot_e);
	float Kajiya = T_Dot_nL * T_Dot_e + sin(Alpha) * sin(Beta);
    Cspec = SpecLight * pow(max(Kajiya,0), g_SpecularScale); 
//        ((SPEC1*Cl*pow(Kajiya, g_specularExp)) + 
  //      (SPEC2*Cl*pow(Kajiya, 1/roughness2)));
  
	float3 nSN = normalize( surface_normal );
	float3 S = cross(nSN, T);            /* Cross product of the tangent along the hair and surface normal */
    float3 N_hair = cross(T, S);       /* N_hair is a normal for the hair oriented "away" from the surface */
	float  l = clamp(dot(nSN,T),0,1);  /* Dot of surface_normal and T, used for blending */

 /* When the hair is exactly perpendicular to the surface, use the surface normal,
             when the hair is exactly tangent to the surface, use the hair normal 
             Otherwise, blend between the two normals in a linear fashion 
        */
    float3 norm_hair = (l * nSN) + ( (1-l) * N_hair);
    norm_hair = normalize(norm_hair);
  
	Cdiff = DifLight * saturate(dot(L, norm_hair));
#endif
////////////////////////	
////////////////////////	
}

//Shave and a Haircut shader modified for our lighting pipeline, ambient has been moved to environment pass
float4 ShaveLighting( PixelInput IN, float3 L, float3 V, float3 T, float3 DifLight, float3 SpecLight )
{
	float4 Out;

// S&H shader
	// Materials (Opacity, Specular, m_Gloss, AmbientDiffuse)
	float SHAVEopacity = IN.Material.x;
	float SHAVEspec = IN.Material.y;
	float SHAVEgloss = IN.Material.z;
	float SHAVEambdiff = IN.Material.w;

	float SHAVEselfshad=1;
	float3 SHAVEspec_color = g_SpecColor.rgb;

/* Looping over lights, per apadoca paper, the math's a bit different */
	float df2 = sqrt( 1 - dot( T, L )*dot( T, L ) );	//optimized since the vectors are normalized
	float sq2 = sqrt( 1 - dot( V, T )*dot( V, T ) );	//optimized since the vectors are normalized

	float rawspec = max( df2 * sq2 -  dot( T, L ) * dot( T, V ), 0); /* raw specular */
	float diffterm = (1.0 - SHAVEambdiff) + df2 * SHAVEambdiff; /* limits gamut of diffuse term */

	float3 Cl2s = SpecLight * SHAVEselfshad + (1 - SHAVEselfshad); /* limits the gamut of shadowing */
	float3 Cl2d = DifLight * SHAVEselfshad + (1 - SHAVEselfshad); /* limits the gamut of shadowing */

	float3 Cspec = Cl2s * pow( abs(max( rawspec,0)), 1.0 / ( 3.0 * ( .101 - SHAVEgloss ) ) )*.5;   /* specular exponent x illumination */
	float3 Cdiff = Cl2d * diffterm; /* diffuse x illumination */

	Out.a = IN.Color.a * SHAVEopacity;
	Out.rgb = ((IN.Color.rgb * Cdiff) + (Cspec * SHAVEspec_color * SHAVEspec));  /*sum terms and premult color x opac */

	return Out;
}

pixelOutput bumpReflectPS( PixelInput IN ) 
{
    pixelOutput OUT = (pixelOutput)0; 
	OUT.col = IN.Color;

	//alpha test
	if( g_AlphaTestRef >= OUT.col.a ) discard;

    return OUT;
}

pixelOutput iblPS( PixelInput IN, uniform bool bPremultAlpha = true ) 
{
    pixelOutput OUT; 

	float SHAVEopacity = IN.Material.x;

	float3 worldEyeDir = normalize(IN.WPosition - g_eyePos.xyz);

//	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, tNormalMapSampler, g_bumpMapScale, vFace );
	float3 worldNormal = normalize( IN.WTangent );

	float4 spec = g_envSpecularColor*g_specularFactor;
	if (g_bHasSpecularEnvMap) 
	{
		spec *= SampleEnvironment((-worldEyeDir), worldNormal, 
				specularEnvMap, g_specularEnvAngle);
	}

//	float4 diff = Tex2DCombine(hasBaseMap, tBaseSampler, IN.V.TexCoord0, g_hairBaseColor*g_envDiffuseColor*g_diffuseFactor);
	float4 diff = IN.Color*g_envDiffuseColor*g_diffuseFactor;
	if (g_bHasDiffuseEnvMap)
	{
		diff *= float4(SampleEnvDiffuse(worldNormal, 
				diffuseEnvMap, g_diffuseEnvAngle).rgb,1);
	}

	float alpha = IN.Color.a * SHAVEopacity;
	OUT.col.a = alpha;
	OUT.col.rgb = (spec.rgb + diff.rgb) * (bPremultAlpha ? alpha : 1);	//premult alpha

	//alpha test
	if( g_AlphaTestRef >= OUT.col.a ) discard;
		
//	OUT.col = float4( spec.rgb + diff.rgb, IN.Color.a );
    return OUT;
}

pixelOutput singleLightPS( PixelInput IN, uniform LightInfo i_Light )
{
    pixelOutput OUT = (pixelOutput)0;
    
	IncidentLight light;
	IlluminatePointLight(IN.WPosition.xyz, i_Light, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(g_eyePos.xyz - IN.WPosition.xyz);
	float3 WorldTan = normalize(IN.WTangent);

#if HAIRSHADER == 1
	float4 Diff = AnisotropicLighting( IN.UV, lightDir, worldEyeDir, WorldTan,
		light.Cld, light.Cls, IN.Color);
#elif HAIRSHADER == 2
	float4 Diff = ShaveLighting( IN, lightDir, worldEyeDir, WorldTan,
		light.Cld, light.Cls );
#endif

	if( g_FirstLight == true )	//environment contribution
	{
		OUT = iblPS( IN, false );	//use environment without premult alpha
		OUT.col.rgb += Diff.rgb;	//add Diff to Env
	}
	else
	{
		OUT.col = Diff;	//Only Diff lighting
	}

	//alpha test
	if( g_AlphaTestRef >= OUT.col.a ) discard;

	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput projLightPS( PixelInput IN,		
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap )
{
    pixelOutput OUT = (pixelOutput)0;
	IncidentLight light;
	IlluminateProjLightHair(IN.WPosition, i_Light, i_ProjLight, 
							 g_bHasProjMap, ProjTextureMap,
							 g_densityScale, g_DepthBias, light );

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.WPosition - g_eyePos.xyz);
	float3 WorldTan = normalize(IN.WTangent);

#if HAIRSHADER == 1
	float4 Diff = AnisotropicLighting( IN.UV, lightDir, worldEyeDir, WorldTan,
		light.Cld, light.Cls, IN.Color);
#elif HAIRSHADER == 2
	float4 Diff = ShaveLighting( IN, lightDir, worldEyeDir, WorldTan,
		light.Cld, light.Cls );
#endif

	if( g_FirstLight == true )	//environment contribution
	{
		OUT = iblPS( IN, false );	//use environment without premult alpha
		OUT.col.rgb += Diff.rgb;	//add Diff to Env
	}
	else
	{
		OUT.col = Diff;	//Only Diff lighting
	}

	//alpha test
	if( g_AlphaTestRef >= OUT.col.a ) discard;

	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput glowPS( PixelInput IN,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap )
{
    pixelOutput OUT; 
    OUT.col = float4(0,0,0,0);
    
    if (g_bConstGlow)
    {
		OUT.col = float4(1,1,1,1);
    }
    else //if (mask.r+mask.g+mask.b > 0)
    {
		IncidentLight light;
		if( g_bProjLt )
		{
			IlluminateProjLightHair(IN.WPosition, i_Light, i_ProjLight,
									 g_bHasProjMap, ProjTextureMap,
									 g_densityScale, g_DepthBias, light );
		}
		else
			IlluminatePointLight(IN.WPosition, i_Light, light);
		
		float3 lightDir = normalize(light.L);
		float3 worldEyeDir = normalize(IN.WPosition - g_eyePos.xyz);
		float3 WorldTan = normalize(IN.WTangent);
		
		// zero out the diffuse factors here.
		OUT.col = AnisotropicLighting( float2( IN.U, IN.V), lightDir, worldEyeDir, WorldTan,
			float3(0,0,0), light.Cls, IN.Color.rgb);

		OUT.col.a = 1;
	}
    return OUT;
}

pixelOutput DOFHair_PS( PixelInput IN ) 
{
    pixelOutput OUT;
	float Depth = mul( g_view, float4( IN.WPosition, 1.0f)).z;	//transform to view space z
    OUT.col = ComputeDepthBlur( Depth ); 
    return OUT;
}

//--------------------------------------------------------------------------------------
// Domain Shader
//--------------------------------------------------------------------------------------
[domain("isoline")]
VertexOutput Hair_DS( HS_HAIR_CONSTANT_DATA_OUTPUT input, float2 UV : SV_DomainLocation, 
					 const OutputPatch<hairOutput, 4> CP, uint PatchID : SV_PrimitiveID )
{
	VertexOutput output;
	float3 vWorldPos;
	float3 vWorldTan;

	float t = InterpolateHairStrand( CP, PatchID, UV, vWorldPos, vWorldTan );
	hairMaterial Mtl = GetMaterial( PatchID );

	float radius = lerp( Mtl.m_RootRadius, Mtl.m_TipRadius, t );
	output.Color.rgb = lerp( Mtl.m_RootColor, Mtl.m_TipColor, t);
	output.Color.a = Mtl.m_Opacity * g_Transparency;
	output.HPosition = float4( vWorldPos, 0);
	output.WPosition = vWorldPos;
	output.WTangent = vWorldTan;
	output.U = abs( radius );
	output.V = t;
	output.Material = float4( Mtl.m_Opacity, Mtl.m_Specular, Mtl.m_Gloss, Mtl.m_AmbientDiffuse );
	output.Segment = PatchID % g_nStrandCPs;

	return output;
}

[domain("isoline")]
PixelInput HairLinesLit_DS( HS_HAIR_CONSTANT_DATA_OUTPUT input, float2 UV : SV_DomainLocation, 
					       const OutputPatch<hairOutput, 4> CP, uint PatchID : SV_PrimitiveID )
{
	PixelInput output;
	float3 vWorldPos;
	float3 vWorldTan;

	float t = InterpolateHairStrand( CP, PatchID, UV, vWorldPos, vWorldTan );
	hairMaterial Mtl = GetMaterial( PatchID );

	float radius = lerp( Mtl.m_RootRadius, Mtl.m_TipRadius, t );
	output.Color.rgb = lerp( Mtl.m_RootColor, Mtl.m_TipColor, t);
	output.Color.a = Mtl.m_Opacity * g_Transparency;
	output.HPosition = mul( g_vp, float4( vWorldPos, 1));
	output.WPosition = vWorldPos;
	output.WTangent = vWorldTan;
	output.U = abs( radius );
	output.V = t;
	output.Material = float4( Mtl.m_Opacity, Mtl.m_Specular, Mtl.m_Gloss, Mtl.m_AmbientDiffuse );

	return output;
}

//--------------------------------------------------------------------------------------
// Geometry Shader
//--------------------------------------------------------------------------------------
[maxvertexcount(4)]
void HairLit_GS( line VertexOutput input[2], inout TriangleStream<GeometryOutput> TriStream )
{
	GeometryOutput output;

	if( input[0].Segment >= (uint)(g_nStrandCPs-1) ) return;	//cull out degenerate segments

	//expand each vertex out perpendicular to the tangent by the radius to form a 2 sided line
	//the tangent gurantees that shared vertices get the same expansion.
	//these two lines then form a quad
	for( uint i = 0; i < 2; i++ )
	{
		output.U = input[i].U;
		output.V = input[i].V;
		output.Material = input[i].Material;

		float radius = input[i].U;
		float3 newPos = input[i].WPosition;
		float3 WTan = input[i].WTangent;

		float3 Offset;
		float SubPixel = HairLimitExpand( newPos, WTan, radius, Offset );
		output.WTangent = WTan;
		output.Color = input[i].Color * SubPixel;

		float3 WPos = newPos + (Offset * radius);	//position.w is signed radius
		output.HPosition = mul( g_vp, float4( WPos, 1) );
		output.WPosition = WPos;
		output.ClipDist = ClipWorldPos( WPos );
		TriStream.Append( output );

		WPos = newPos + (Offset * -radius);	//position.w is signed radius
		output.HPosition = mul( g_vp, float4( WPos, 1) );
		output.WPosition = WPos;
		output.ClipDist = ClipWorldPos( WPos );
		TriStream.Append( output );
	}

    TriStream.RestartStrip();
}

[maxvertexcount(2)]
void HairLinesLit_GS( line VertexOutput input[2], inout LineStream<GeometryOutput> Lines )
{
	GeometryOutput output;

	if( input[0].Segment >= (uint)(g_nStrandCPs-1) ) return;	//cull out degenerate segments

	for( uint i = 0; i < 2; i++ )
	{
		float3 WPos = input[i].WPosition;
		float3 WTan = input[i].WTangent;
		output.U = input[i].U;
		output.V = input[i].V;
		output.Material = input[i].Material;
		output.WTangent = WTan;
		output.Color = input[i].Color;
		output.HPosition = mul( g_vp, float4( WPos, 1) );
		output.WPosition = WPos;
		output.ClipDist = ClipWorldPos( WPos );
		Lines.Append( output );
	}

    Lines.RestartStrip();
}


#define HAIR_LIT_HULL_AND_DOMAIN_Default\
	SetVertexShader(CompileShader( vs_5_0, HairVS_NoTess()));\
	SetGeometryShader(CompileShader(gs_5_0, HairLit_GS()));

#define HAIR_LIT_HULL_AND_DOMAIN_Tess\
	SetVertexShader(CompileShader( vs_5_0, HairVS_Tess()));\
	SetHullShader(CompileShader(hs_5_0, Hair_HS()));\
	SetDomainShader(CompileShader(ds_5_0, Hair_DS()));\
	SetGeometryShader(CompileShader(gs_5_0, HairLit_GS()));

#define HAIR_LIT_HULL_AND_DOMAIN_Lines\
	SetVertexShader(CompileShader( vs_5_0, HairVS_NoTess()));\
	SetGeometryShader(CompileShader(gs_5_0, HairLinesLit_GS()));\

#define HAIR_LIT_HULL_AND_DOMAIN_LinesTess\
	SetVertexShader(CompileShader( vs_5_0, HairVS_Tess()));\
	SetHullShader(CompileShader(hs_5_0, Hair_HS()));\
	SetDomainShader(CompileShader(ds_5_0, HairLinesLit_DS()));


//--------------------------------------------------------------------------------------
// Shader Techniques
//--------------------------------------------------------------------------------------


/***************************** eof ***/

//////////////////////////////////////////////////////////////////////////////
// Entry points generated from the technique/pass compile statements.
// Each one binds the uniform arguments the .fx passed in its compile call.
//////////////////////////////////////////////////////////////////////////////

pixelOutput SingleLight_PDefault_PS(PixelInput IN)
{
    return singleLightPS(IN, g_lightInfo);
}

pixelOutput ProjectedLight_PDefault_PS(PixelInput IN)
{
    return projLightPS(IN, g_lightInfo, g_projLight, projShadowMap);
}

pixelOutput Glow_PDefault_PS(PixelInput IN)
{
    return glowPS(IN, g_lightInfo, g_projLight, projShadowMap);
}

pixelOutput Environment_PDefault_PS(PixelInput IN)
{
    return iblPS(IN, true);
}
