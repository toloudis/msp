;  StencilShadow.vsh 

#define CV_ZERO				0
#define CV_ONE				1
#define CV_OSPACELIGHTPOS	2
#define CV_WVP0				3
#define CV_WVP1				4
#define CV_WVP2				5
#define CV_WVP3				6
#define CV_FATNESS_SCALE	7

#define IV_POSITION  v0
#define IV_NORMAL    v1

#define V_LIGHT_TO_VERT		r0
#define V_VERT_TO_LIGHT		r1
#define V_POSITION			r2
#define V_FACEDOT			r3
#define V_ANTIFACEDOT		r4
#define V_TEMP				r5

vs.1.1

dcl_position v0
dcl_normal v1

//----------------------------------------------------------------------------
//	Get normalized light direction
//----------------------------------------------------------------------------
mul V_TEMP, IV_POSITION, c[CV_OSPACELIGHTPOS].w
sub V_VERT_TO_LIGHT, c[CV_OSPACELIGHTPOS], V_TEMP
dp3 V_TEMP.w, V_VERT_TO_LIGHT, V_VERT_TO_LIGHT
rsq V_TEMP.w, V_TEMP.w
mul V_VERT_TO_LIGHT, V_VERT_TO_LIGHT, V_TEMP.w

//----------------------------------------------------------------------------
//	Inset the position along the normal vector direction
//	This moves the shadow volume points inside the model
//	slightly to minimize popping of shadowed areas as
//	each facet comes in and out of shadow.
//	CV_FATNESS_SCALE should be negative
//	The amount we must shift goes as z squared.
//----------------------------------------------------------------------------
dp4 V_TEMP.w, IV_POSITION, c[CV_WVP3]
//mul V_TEMP.w, V_TEMP.w, V_TEMP.w
mul V_TEMP.w, V_TEMP.w, c[CV_FATNESS_SCALE]
mul V_POSITION, IV_NORMAL, V_TEMP.wwww
add V_POSITION, IV_POSITION, V_POSITION

mov V_POSITION.w, IV_POSITION.w

//----------------------------------------------------------------------------
//	decide if front or back face
//	if back face, V_FACEDOT = 1, else V_FACEDOT = 0
//----------------------------------------------------------------------------
dp3 V_FACEDOT, IV_NORMAL, V_VERT_TO_LIGHT
slt V_FACEDOT, V_FACEDOT,  c[CV_ZERO].z
sub V_ANTIFACEDOT, c[CV_ONE], V_FACEDOT

//----------------------------------------------------------------------------
//	V_LIGHT_TO_VERT will later be used to move the backfacing vertices
//	infinitely far away, so we set the w to zero
//----------------------------------------------------------------------------
mov V_VERT_TO_LIGHT.w, c[CV_ZERO]

//----------------------------------------------------------------------------
//	set position = homogeneous infinite light direction point if back facing, regular position if
//	front facing
//----------------------------------------------------------------------------
mul V_POSITION, V_ANTIFACEDOT, V_POSITION
mad V_POSITION, V_FACEDOT, -V_VERT_TO_LIGHT, V_POSITION

//----------------------------------------------------------------------------
// transform to hclip space
//----------------------------------------------------------------------------
dp4 oPos.x, V_POSITION, c[CV_WVP0]
dp4 oPos.y, V_POSITION, c[CV_WVP1]
dp4 oPos.z, V_POSITION, c[CV_WVP2]
dp4 oPos.w, V_POSITION, c[CV_WVP3]
