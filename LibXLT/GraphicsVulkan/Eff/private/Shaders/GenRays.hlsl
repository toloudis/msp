//--------------------------------------------------------------------------------------
// File: GenRays.hlsl
//
// Copyright (c) StudioGPU. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Global Defines - SHOULD ALWAYS MATCH CPP & HLSL
//--------------------------------------------------------------------------------------
#define RES_X					640
#define RES_Y					360
#define THREAD_REDUCE_FACTOR	4
#define MAX_LIGHTS				4
#define MAX_MESHES				16
#define NUM_RAY_ELEMENTS		4

//--------------------------------------------------------------------------------------
// DX11 buffers
//--------------------------------------------------------------------------------------
RWStructuredBuffer<float4> i_rays_uav;
RWStructuredBuffer<float4> canvas;

//--------------------------------------------------------------------------------------
// Structs
//--------------------------------------------------------------------------------------
typedef struct {
	float4 o;
	float4 d;
} Ray;

//--------------------------------------------------------------------------------------
// Constant buffer : world data
//--------------------------------------------------------------------------------------
cbuffer cb1
{    
    float4	cpu_light_pos[MAX_LIGHTS];  // .W component = whether it casts a shadow or not
    float4	cpu_light_diffuseColor[MAX_LIGHTS];
	float4	cpu_light_specularColor[MAX_LIGHTS];
	float4	cpu_light_fallOff[MAX_LIGHTS];
	float4	cpu_light_coneInfo[MAX_LIGHTS];
	
	float4x4 cpu_CameraToScreen;
	float4x4 cpu_WorldToScreen;
	float4x4 cpu_RasterToCamera;
	float4x4 cpu_ScreenToRaster;
	float4x4 cpu_RasterToScreen;
	float4x4 cpu_CameraToWorld;
	float4x4 cpu_RasterToWorld;
    
	float4	cpu_camera_eye;
	float4	cpu_camera_lookat;
	float4	cpu_camera_up;
	float4	cpu_camera_U;
	float4	cpu_camera_V;
	float4	cpu_camera_W;	
    
    float4	cpu_backColor;
    
    float4  cpu_rtParams_1;	// < K_EPSILON , K_LARGEVAL , 0 , enable shadows >    
    float4	cpu_rtParams_2; // << numBounces , secondary pass, reflection attenuation, enable reflections >>
	float4	cpu_rtParams_3; // << refract_toggle , refraction attenuation, 0, 0 >>
    
    int2	cpu_resolution;
    
    int		cpu_numLights;
    int		cpu_numMeshes;
    
   // int		padding_one;		
   // int		padding_two;		
   // int		padding_three;   
};

//--------------------------------------------------------------------
// ComputeRays() - Entry point
//--------------------------------------------------------------------
[numthreads( RES_Y/THREAD_REDUCE_FACTOR, 1, 1 )]
void ComputeRays(uint3 blockIdx : SV_GroupID, uint3 threadIdx : SV_GroupThreadID) 
{
	// Get current thread id
	int canvas_idx = blockIdx.x * (RES_Y/THREAD_REDUCE_FACTOR) + threadIdx.x;		
	
	// Figure out thread's (x,y) locs
	uint x = canvas_idx%cpu_resolution.x;
	uint y = canvas_idx/cpu_resolution.x;
	
	float4 viewplane_point = float4(x, y, 0, 1);
	viewplane_point = mul(cpu_RasterToWorld,viewplane_point);
	viewplane_point -= cpu_camera_eye;
		
	// Setup primary ray
	Ray primaryRay;
	primaryRay.o = cpu_camera_eye;	
	primaryRay.d = normalize( viewplane_point );
		
	int ray_idx = canvas_idx * NUM_RAY_ELEMENTS;
	i_rays_uav[ray_idx] = primaryRay.o;
	i_rays_uav[ray_idx+1] = primaryRay.d;
	
	// Reset color back to black
	canvas[canvas_idx] = float4(0,0,0,0);
}
	
