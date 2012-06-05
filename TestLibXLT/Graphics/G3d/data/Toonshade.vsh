;  Toonshade.vsh 
; ToonShade.nvv
;
;Algorithm:
;	-Compute world-space position, find vector from eye to vertex
;	-Normalize this eye vector
;	-Use this eye vector dotted with the surface normal (also in world space)
;	  as a texture coordinate (this will index into a 1D texture).
;	  This dot product effectively finds the silhouette of the object.
;	-Use the light vector dotted with the surface normal to index into a 
;	  second 1D texture.  This lookup generates the constant-tone toon
;	  shading depending on how close the surface normal coincides with the
;	  light vector.
;
;Copyright (C) 1999, 2000 NVIDIA Corporation
;

// v0  -- position
// v3  -- normal
// v7,8  -- tex coord

#define CV_WORLDVIEWPROJ_0 0
#define CV_WORLDVIEWPROJ_1 1
#define CV_WORLDVIEWPROJ_2 2
#define CV_WORLDVIEWPROJ_3 3

#define CV_LIGHT_DIRECTION 4

#define CV_WORLD_IT_0 5
#define CV_WORLD_IT_1 6
#define CV_WORLD_IT_2 7
#define CV_WORLD_IT_3 8

#define CV_CONSTANTS 9

#define CV_EYE 10

#define CV_WORLD_0 11
#define CV_WORLD_1 12
#define CV_WORLD_2 13
#define CV_WORLD_3 14

vs.1.1

dcl_position v0
dcl_normal v3
dcl_texcoord0 v7
dcl_texcoord1 v8



// transform position
dp4 oPos.x, v0, c[CV_WORLDVIEWPROJ_0]
dp4 oPos.y, v0, c[CV_WORLDVIEWPROJ_1]
dp4 oPos.z, v0, c[CV_WORLDVIEWPROJ_2]
dp4 oPos.w, v0, c[CV_WORLDVIEWPROJ_3]

// transform normal
dp3 r0.x, v3, c[CV_WORLD_IT_0]
dp3 r0.y, v3, c[CV_WORLD_IT_1]
dp3 r0.z, v3, c[CV_WORLD_IT_2]

// normalize normal
dp3 r0.w, r0, r0
rsq r0.w, r0.w
mul r0, r0, r0.w

// compute world space position
dp4 r1.x, v0, c[CV_WORLD_0]
dp4 r1.y, v0, c[CV_WORLD_1]
dp4 r1.z, v0, c[CV_WORLD_2]
dp4 r1.w, v0, c[CV_WORLD_3]

// vector from point to eye
add r2, c[CV_EYE], -r1

// normalize e
dp3 r2.w, r2, r2
rsq r2.w, r2.w
mul r2, r2, r2.w

// e dot n
dp3 oT1.x, r0, r2

// l dot n
dp3 oT0.x, r0, -c[CV_LIGHT_DIRECTION]

;pure white diffuse color 
; - use constant to get (1,1,1,1)
mov oD0, c9.zzzz
