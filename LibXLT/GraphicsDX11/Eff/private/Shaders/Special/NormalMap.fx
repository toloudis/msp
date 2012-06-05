//--------------------------------------------------------------------------------------
// ported from nvidia sample
//--------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// PARAMETERS
//-----------------------------------------------------------------------------

#include "..\Support.h"
#include "..\Tessellate.h"

float g_Transparency : Opacity = 1.0f;
bool hasTransparencyMap = false;
Texture2D TransparencyMap		: OpacityTexture;

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
technique11 WorldSpaceNormal
{
#define PASS_WORLDSPACE(PassName)	\
	pass P##PassName			\
	{						\
        SetVertexShader( CompileShader( vs_5_0, NormalMap_VS_##PassName()));\
        SetPixelShader( CompileShader( ps_5_0, WorldNormal_PS_##PassName()));\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_WORLDSPACE(Default)
PASS_WORLDSPACE(Tess)
}

//technique to view the normal vectors in view (camera) space
technique11 ViewSpaceNormal
{
#define PASS_VIEWSPACE(PassName)	\
	pass P##PassName			\
	{						\
        SetVertexShader( CompileShader( vs_5_0, NormalMap_VS_##PassName()));\
        SetPixelShader( CompileShader( ps_5_0,ViewNormal_PS_##PassName()));\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_VIEWSPACE(Default)
PASS_VIEWSPACE(Tess)
}

//technique to view the normal vectors in tangent space
technique11 TangentSpaceNormal
{
#define PASS_TANGENTSPACE(PassName)	\
	pass P##PassName			\
	{						\
        SetVertexShader( CompileShader( vs_5_0, NormalMap_VS_##PassName()));\
        SetPixelShader( CompileShader( ps_5_0,TangentNormal_PS_##PassName()));\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_TANGENTSPACE(Default)
PASS_TANGENTSPACE(Tess)
}

//technique to write the positions in view (camera) space
technique11 ViewSpacePos
{
#define PASS_VIEWSPACEPOS(PassName)	\
	pass P##PassName			\
	{						\
        SetVertexShader( CompileShader( vs_5_0, NormalMap_VS_##PassName()));\
        SetPixelShader( CompileShader( ps_5_0,ViewSpacePos_PS_##PassName()));\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_VIEWSPACEPOS(Default)
PASS_VIEWSPACEPOS(Tess)
}
