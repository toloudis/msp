//#version 420

//#include "Globals.h"

// transforms/viewing
uniform mat4 g_worldIT;
uniform mat4 g_wvp;
uniform mat4 g_wv;
uniform mat4 g_world;
uniform mat4 g_viewIT;
uniform mat4 g_vp;
uniform vec4 g_eyePos;
// xres, yres, 1/xres, 1/yres
uniform vec4 g_targetRes; 

uniform float g_AlphaTestRef = 0.0f;

// material properties
uniform bool g_bDoubleSided = false;

/************* DATA STRUCTS **************/

/* data from application vertex buffer */
//#include "Tessellate.h"

/*********** support functions ******/

/*********** vertex shader ******/

TANGENT_VERTEX TangentVS( )
{
    TANGENT_VERTEX OUT;

	// decal and bump texture coords
	OUT.UV = InUV;
    OUT.TexCoord0 = (g_uvTransform * vec4(InUV,0,1)).xy;
    
	vec4 Po = vec4(InPosition, 1.0f);
    
    // transform position to world space and get vector to light
    vec3 Pw = (g_world * Po).xyz;
    OUT.WorldPos = Pw;

	OUT.WorldTan.X = mat3(g_world) * InT;
	OUT.WorldTan.Y = mat3(g_world) * InB;
	OUT.WorldTan.Z = mat3(g_world) * InNormal;
	
    return OUT;
}

out TANGENT_VERTEX_OUTPUT OUT;

void main( ) 
{
	OUT.V = TangentVS( );

	vec4 Po = vec4(InPosition, 1.0f);
	OUT.HPosition = g_wvp * Po;
	gl_Position = OUT.HPosition;
	OUT.ScreenPos = vec3(0,0,0);//not used

	gl_ClipDistance[0] = ClipWorldPos( OUT.V.WorldPos );
}

