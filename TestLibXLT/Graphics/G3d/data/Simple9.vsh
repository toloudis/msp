;  Simple.vsh - 2 dir lights, 2 point lights
;
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
;
; c15	 -- diffuse color
; c16	 -- specular color
; c17	 -- specular power
; c18    -- ambient color
;
; c23    -- ambient light
; c24    -- color (dir light 1)
; c25    -- direction (dir light 1)
; c26    -- color (dir light 2)
; c27    -- direction (dir light 2)
;
; c32    -- color (pos light 1)
; c33    -- position (pos light 1)
; c34    -- attenuation (pos light 1)
; c35    -- color (pos light 2)
; c36    -- position (pos light 2)
; c37    -- attenuation (pos light 2)

; D3DXAssemble doesn't like preprocessor symbols?!
;#define CV_MAT_DIFFUSE 15
;#define CV_MAT_SPECULAR 16
;#define CV_MAT_SPECULARPOWER 17
;#define CV_MAT_AMBIENT 18

;#define CV_LIGHT_AMBIENT 23
;#define CV_DLIGHT1_COLOR 24
;#define CV_DLIGHT1_DIRECTION 25
;#define CV_DLIGHT2_COLOR 26
;#define CV_DLIGHT2_DIRECTION 27

;#define CV_PLIGHT1_COLOR 32
;#define CV_PLIGHT1_POSITION 33
;#define CV_PLIGHT1_ATTENUATION 34
;#define CV_PLIGHT2_COLOR 35
;#define CV_PLIGHT2_POSITION 36
;#define CV_PLIGHT2_ATTENUATION 37

;#define R_EYE_NORMAL r0
;#define R_EYE_VERTEX r1
;#define R_EYE_VECTOR r2
;#define R_ATTEN r5
;#define R_DIFFUSE r6
;#define R_TEMP r7
;#define R_HALF_VECTOR r8
;#define R_VERTEX_TO_LIGHT r9
;#define R_SPECULAR r10

vs.1.1

dcl_position v0
dcl_normal v3
dcl_texcoord0 v7
dcl_texcoord1 v8

;transform position
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3

;transform normal
dp3 r0.x, v3, c5
dp3 r0.y, v3, c6
dp3 r0.z, v3, c7

;normalize normal
dp3 r0.w, r0, r0
rsq r0.w, r0.w
mul r0, r0, r0.w

;compute world space position
dp4 r1.x, v0, c11
dp4 r1.y, v0, c12
dp4 r1.z, v0, c13
dp4 r1.w, v0, c14

;vector from point to eye
add r2, c10, -r1

;normalize e
dp3 r2.w, r2, r2
rsq r2.w, r2.w
mul r2, r2, r2.w

; output texture coordinates directly
mov oT0, v7
mov oT1, v8

; Get the material power
mov r3.w, c17.w

; Could do global ambient here, would need to 
; change first diffuse computation to "add" command

; *****************************
; ** Directional Light 1

; Dot normal with light vector
; This is the intensity of the diffuse component
dp3 r3.x, r0, c25

; This is the intensity of the specular component
; Calculate half vector (light vector + eye vector)
mov r8, c25
add r8, r8, r2

; normalize half-vector
dp3 r8.w, r8, r8
rsq r8.w, r8.w												
mul r8, r8, r8.w	

; Dot normal with half-vector.  
dp3 r3.yz, r0, r8

; Calculate the diffuse & specular factors
lit r4, r3

; add the (diffuse color * diffuse light color * diffuse intensity(R4.y))
mul oD0, r4.y, c24
mov oD0.w, c9.z

; specular (specular color * specular light color * specular intensity(R4.z))
mul oD1, r4.z, c24

