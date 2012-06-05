;  ReflectRefract.vsh 

; This shader generates the camera space reflection coordinates in T0, and
; a reasonable approximation of refraction coordinates in T1. The approximation 
; for refraction is set up by shortening the vertex normal and passing it 
; through the standard reflection calculation.

; v0  -- position
; v3  -- normal
; v7  -- tex coord
; v8  -- tex coord1
;
; c0-3   -- world/view/proj matrix
; c4     -- light vector
; c5-8   -- inverse/transpose world matrix
; c9     -- {0.0, 0.5, 1.0, -1.0}
; c10    -- eye point
; c11-14 -- world matrix


#define CV_WORLDVIEWPROJ_0 0
#define CV_WORLDVIEWPROJ_1 1
#define CV_WORLDVIEWPROJ_2 2
#define CV_WORLDVIEWPROJ_3 3

#define CV_WORLDEYEPOS 10


#define CV_WORLD_0 5
#define CV_WORLD_1 6
#define CV_WORLD_2 7
#define CV_WORLD_3 8

#define CV_CONSTANTS 9
#define CV_REFRACT 20


#define R_WORLDSPACE_NORMAL r9
#define R_WORLDSPACE_NORMAL_REFRACT r7
#define R_EYE_VECTOR r3
#define R_DOT2 r4 

#define R_TEMP r1 

vs.1.1

dcl_position v0
dcl_normal v3
dcl_texcoord0 v7
dcl_texcoord1 v8


; Transform position
dp4 oPos.x, v0, c[CV_WORLDVIEWPROJ_0]
dp4 oPos.y, v0, c[CV_WORLDVIEWPROJ_1]
dp4 oPos.z, v0, c[CV_WORLDVIEWPROJ_2]
dp4 oPos.w, v0, c[CV_WORLDVIEWPROJ_3]

; Get world space vertex position
dp4 r0.x, v0, c[CV_WORLD_0]
dp4 r0.y, v0, c[CV_WORLD_1]
dp4 r0.z, v0, c[CV_WORLD_2]
dp4 r0.w, v0, c[CV_WORLD_3]

; Transform normal to world space
dp3 R_WORLDSPACE_NORMAL.x, v3, c[CV_WORLD_0]
dp3 R_WORLDSPACE_NORMAL.y, v3, c[CV_WORLD_1]
dp3 R_WORLDSPACE_NORMAL.z, v3, c[CV_WORLD_2]

; Need to re-normalize normal
dp3 R_WORLDSPACE_NORMAL.w, R_WORLDSPACE_NORMAL, R_WORLDSPACE_NORMAL
rsq R_WORLDSPACE_NORMAL.w, R_WORLDSPACE_NORMAL.w
mul R_WORLDSPACE_NORMAL, R_WORLDSPACE_NORMAL, R_WORLDSPACE_NORMAL.w

; Get vector from eye to vertex
sub R_TEMP, r0, c[CV_WORLDEYEPOS]
dp3 R_EYE_VECTOR.w, R_TEMP, R_TEMP
rsq R_EYE_VECTOR.w, R_EYE_VECTOR.w		
mul R_EYE_VECTOR, R_TEMP, R_EYE_VECTOR.w

; Calculate E - 2*(E dot N)*N 
dp3 R_DOT2, R_EYE_VECTOR, R_WORLDSPACE_NORMAL
add R_DOT2, R_DOT2, R_DOT2
mad oT0.xyz,R_WORLDSPACE_NORMAL, -R_DOT2, R_EYE_VECTOR
; constants.z == 1
mov oT0.w, c[CV_CONSTANTS].z

; Get refraction normal
mul R_WORLDSPACE_NORMAL_REFRACT.xyz, R_WORLDSPACE_NORMAL, c[CV_REFRACT]

; Calculate E - 2*(E dot N)*N 
dp3 R_DOT2, R_EYE_VECTOR, R_WORLDSPACE_NORMAL_REFRACT
add R_DOT2, R_DOT2, R_DOT2
mad oT1.xyz,R_WORLDSPACE_NORMAL_REFRACT, -R_DOT2, R_EYE_VECTOR
mov oT1.w, c[CV_CONSTANTS].z

;pure white diffuse color - use constant to get (1,1,1,1)
mov oD0, c9.zzzz
