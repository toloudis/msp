//////////////////////////////////////////////////////////////////////////////
// Converted from NormalMap.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// NormalMap.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------
// ported from nvidia sample
//--------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// PARAMETERS
//-----------------------------------------------------------------------------

#include "../Materials/Support.hlsli"
#include "../Materials/Tessellate.hlsli"

// Register layout shared with the materials: see ../Materials/Globals.hlsli.
// b4: this shader's own parameters (defaults are in NormalMap.effect.json)
cbuffer MaterialParams : register(b4)
{
	float g_Transparency;			// : Opacity, default 1
	bool hasTransparencyMap;		// default false
};

Texture2D TransparencyMap : register(t4);		// : OpacityTexture

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

TANGENT_VERTEX_OUTPUT NormalMap_VS_Default( STANDARD_VERTEX In, out float ClipDist : SV_ClipDistance0 )
{
    TANGENT_VERTEX_OUTPUT Out;

	float4 Pos = float4(In.Position, 1.0f);
	Out.V.WorldPos = mul(g_world, Pos ).xyz;

	Out.V.UV = In.UV;
	Out.V.TexCoord0 = mul(g_uvTransform, float4(In.UV,0,1)).xy;
	Out.V.WorldTan = TransformTangents( g_world, In.T, In.B, In.Normal );

	Out.ScreenPos = mul( g_wv, Pos ).xyz;
    Out.HPosition = TransformVertex( Pos, In.UV, g_wvp);

	ClipDist = ClipWorldPos( Out.V.WorldPos );

    return Out;
}

TANGENT_TESS_OUTPUT NormalMap_VS_Tess( STANDARD_VERTEX In )
{
    TANGENT_TESS_OUTPUT Out;

	float4 Pos = float4(In.Position, 1.0f);

    Out.V.WorldPos = mul(g_world, Pos).xyz;

	Out.V.UV = In.UV;
	Out.V.TexCoord0 = mul(g_uvTransform, float4(In.UV,0,1)).xy;
	Out.V.WorldTan = TransformTangents( g_world, In.T, In.B, In.Normal );

    return Out;
}

//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------
pixelOutput WorldNormal_PS( TANGENT_VERTEX_OUTPUT In, bool vFace : SV_ISFRONTFACE )
{
	pixelOutput OUT = (pixelOutput)0;
	//early alpha test
	float Alpha = Tex2DCombine(hasTransparencyMap, TransparencyMap, In.V.TexCoord0, g_Transparency).r;
	if( g_AlphaTestRef >= Alpha ) discard;

	//fetch bump normal
	float3 norm = GetBumpNormal( In.V, hasNormalMap, normalMap, g_bumpMapScale, vFace );
	OUT.col.rgb = norm;

	return OUT;
}

pixelOutput WorldNormal_PS_Tess( TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return WorldNormal_PS( IN, vFace );
}

pixelOutput WorldNormal_PS_Default( TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE )
{
	return WorldNormal_PS( IN, vFace );
}

//-----------------------------------------------------------------------------

pixelOutput ViewNormal_PS( TANGENT_VERTEX_OUTPUT In, bool vFace : SV_ISFRONTFACE )
{   
    pixelOutput Out = (pixelOutput)0;

	//early alpha test
	float Alpha = Tex2DCombine(hasTransparencyMap, TransparencyMap, In.V.TexCoord0, g_Transparency).r;
	if( g_AlphaTestRef >= Alpha ) discard;

	//fetch bump normal
	float3 norm = GetBumpNormal( In.V, hasNormalMap, normalMap, g_bumpMapScale, vFace );

	//negate the normal so that the normals view is facing us and not away.
	float3 vnorm = -mul( (float3x3)g_view, norm );
	Out.col.rgb = normalize(vnorm);

	return Out;
}

pixelOutput ViewNormal_PS_Tess( TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return ViewNormal_PS( IN, vFace );
}

pixelOutput ViewNormal_PS_Default( TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE )
{
	return ViewNormal_PS( IN, vFace );
}

//-----------------------------------------------------------------------------

pixelOutput TangentNormal_PS( TANGENT_VERTEX_OUTPUT In, bool vFace : SV_ISFRONTFACE )
{   
    pixelOutput Out = (pixelOutput)0;

	//early alpha test
	float Alpha = Tex2DCombine(hasTransparencyMap, TransparencyMap, In.V.TexCoord0, g_Transparency).r;
	if( g_AlphaTestRef >= Alpha ) discard;


	//fetch bump normal
	float3 norm = GetBumpNormal( In.V, hasNormalMap, normalMap, g_bumpMapScale, vFace );

	Out.col.rgb = mul( (float3x3)g_worldIT, norm );

	return Out;
}

pixelOutput TangentNormal_PS_Tess( TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return TangentNormal_PS( IN, vFace );
}

pixelOutput TangentNormal_PS_Default( TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE )
{
	return TangentNormal_PS( IN, vFace );
}

//-----------------------------------------------------------------------------

pixelOutput ViewSpacePos_PS( TANGENT_VERTEX_OUTPUT In, bool vFace : SV_ISFRONTFACE )
{   
    pixelOutput Out = (pixelOutput)0;

	//early alpha test
	float Alpha = Tex2DCombine(hasTransparencyMap, TransparencyMap, In.V.TexCoord0, g_Transparency).r;
	if( g_AlphaTestRef >= Alpha ) discard;

	Out.col.rgb = mul( g_view, float4(In.V.WorldPos, 1) ).xyz;

	return Out;
}
pixelOutput ViewSpacePos_PS_Tess( TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return ViewSpacePos_PS( IN, vFace );
}

pixelOutput ViewSpacePos_PS_Default( TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE )
{
	return ViewSpacePos_PS( IN, vFace );
}

//-----------------------------------------------------------------------------
// TECHNIQUES
//-----------------------------------------------------------------------------

//technique to view the normal vectors in world space


//technique to view the normal vectors in view (camera) space


//technique to view the normal vectors in tangent space


//technique to write the positions in view (camera) space
