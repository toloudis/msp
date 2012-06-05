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

struct LightInfo
{
	float4 Pos;
	float4 Diffuse;
	float4 Specular;
};

/************* UN-TWEAKABLES **************/

float4x4 worldIT : WorldIT;
float4x4 wvp : WorldViewProjection;
float4x4 world : World;
float4x4 viewIT : ViewIT;
LightInfo lightArray[8] : LightArray;

/************* TWEAKABLES **************/

float4 eyePos : CameraPos;
float shininess : MaterialPower;

/************* DATA STRUCTS **************/

/* data from application vertex buffer */
struct appdata {
    float3 Position	: POSITION;
 //   float4 UV		: TEXCOORD0;
    float4 Normal	: NORMAL;
};

/* data passed from vertex shader to pixel shader */
struct vertexOutput {
    float4 HPosition	: POSITION;
//    float4 TexCoord0	: TEXCOORD0;
    float4 diffCol	: COLOR0;
    float4 specCol	: COLOR1;
};

/* Output pixel values */
struct pixelOutput {
  float4 col : COLOR;
};

/*********** support functions ******/

void diffuse_contrib(LightInfo lightInfo,
						float3 Pw,			// Position of vertex in world coords
						float3 Nn,			// Normalize vertex normal
						float3 V,			// Eye position, world - vertex position
						float shininess,	// Specular power
						
						out float4 diffContrib,
						out float4 specContrib)
{    
    float3 Ln = normalize(lightInfo.Pos - mul(Pw, lightInfo.Pos.w));
    float ldn = dot(Ln,Nn);
    float diffComp = max(0,ldn);
    
    diffContrib = diffComp * lightInfo.Diffuse;
    diffContrib.w = 1.0;
	
	float3 H = normalize(Ln + V);
	float specComp = pow(max(dot(Nn, H), 0), shininess);
	if (diffComp <= 0) specComp = 0;
	specContrib = specComp * lightInfo.Specular;
    specContrib.w = 1.0;
	//specContrib = float4(0,0,0, 1);
	
    //return float4(diffComp,diffComp,diffComp,1);
}   
    
/*********** vertex shader ******/

vertexOutput testVS(appdata IN,
    uniform float4x4 WorldViewProj,
    uniform float4x4 WorldIT,
    uniform float4x4 World,
    uniform float4x4 ViewIT,
    uniform LightInfo LightArray[8],
    uniform float4 EyePos,
    uniform float SpecularPower
) {
    vertexOutput OUT;
    
    float3 Nn = mul(WorldIT, IN.Normal).xyz;
    Nn = normalize(Nn);

    float4 Po = float4(IN.Position.xyz, 1.0);
    
    float3 Pw = mul(World, Po).xyz;
	float3 V = normalize(EyePos - Pw);
    
    OUT.diffCol = float4(0,0,0,1);
    OUT.specCol = float4(0,0,0,1);
    for (int i=0; i<8; i++)
    {
		float4 diffuse, specular;
		diffuse_contrib(LightArray[i], Pw, Nn, V, SpecularPower, diffuse, specular );
		OUT.diffCol += diffuse; 
		OUT.specCol += specular; 
    }
    
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
    float4 result = IN.diffCol + IN.specCol;
    OUT.col = result;
    return OUT;
}

/*************/

technique ps11
{
	pass p0 
	{		
		VertexShader = compile vs_2_0 testVS(wvp,worldIT,
					world,viewIT, lightArray, eyePos, shininess);
		ZEnable = true;
		ZWriteEnable = true;
		CullMode = None;
		PixelShader = compile ps_1_1 labPS();
	}
}

/***************************** eof ***/
