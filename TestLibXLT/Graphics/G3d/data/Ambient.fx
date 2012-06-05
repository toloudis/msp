/*********************************************************************NVMH3****
Path:  NVSDK\Common\media\cgfx
File:  $Id: //sw/devrel6/SDK/MEDIA/CgFX/velvety.fx#1 $

Copyright NVIDIA Corporation 2002
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
*AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.


Comments:
    Simple single color diffuse

******************************************************************************/

string Category = "Effects\\Cg\\Lighting";
string keywords = "dx8,pointlight";
string description = "Simple single color diffuse";

/************* UN-TWEAKABLES **************/

float4x4 worldIT : WorldIT;
float4x4 wvp : WorldViewProjection;
float4x4 world : World;
float4x4 viewIT : ViewIT;

/************* TWEAKABLES **************/

float4 lightPos : LightPos
<
	string Object = "PointLight";
	string Space = "World";
> = {100.0f, 100.0f, 100.0f, 0.0f};

/************* DATA STRUCTS **************/

/* data from application vertex buffer */
struct appdata {
    float3 Position	: POSITION;
    float3 Normal	: NORMAL;
    float4 Color	: COLOR;
    float2 UV		: TEXCOORD0;
    float3 T		: TANGENT;
    float3 B		: BINORMAL;
};

/* data passed from vertex shader to pixel shader */
struct vertexOutput {
    float4 HPosition	: POSITION;
//    float4 TexCoord0	: TEXCOORD0;
    float4 diffCol	: COLOR0;
};

/* Output pixel values */
struct pixelOutput {
  float4 col : COLOR;
};

/*********** vertex shader ******/

vertexOutput testVS(appdata IN,
    uniform float4x4 WorldViewProj,
    uniform float4x4 WorldIT,
    uniform float4x4 World,
    uniform float4x4 ViewIT,
    uniform float3 LightPos
) {
    vertexOutput OUT;
    
    float3 Nn = mul(WorldIT, IN.Normal).xyz;
    Nn = normalize(Nn);

    float4 Po = float4(IN.Position.xyz, 1.0);
    
    float3 Pw = mul(World, Po).xyz;
    float3 Ln = normalize(LightPos - Pw);
    float ldn = dot(Ln,Nn);
    float diffComp = max(0,ldn);
    //float4 diffContrib = diffComp * DiffColor;
    //diffContrib.w = 1.0;

    //OUT.TexCoord0 = IN.UV;
 //   OUT.diffCol = float4(diffComp,diffComp,diffComp,1);
   OUT.diffCol = IN.Color;
   OUT.diffCol.a = 1;
 //   OUT.diffCol = float4(IN.T.x,IN.Color.g,IN.Color.b,1);
    
    // Cg version : 
    OUT.HPosition = mul(WorldViewProj, Po);
    
    // DX9 version:
    //OUT.HPosition = mul(Po, WorldViewProj);
    
    return OUT;
}

/********* pixel shader ********/

pixelOutput labPS(vertexOutput IN
) {
    pixelOutput OUT; 
    float4 result = IN.diffCol;
    OUT.col = result;
    return OUT;
}

/*************/

technique ps11
{
	pass p0 
	{		
		VertexShader = compile vs_1_1 testVS(wvp,worldIT,
					world,viewIT, lightPos);
		//PixelShader = compile ps_1_1 labPS();
		PixelShader = null;
	}
}

/***************************** eof ***/
