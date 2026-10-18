//--------------------------------------------------------------------------------------
// File: PostProcess.hlsl
//
// Copyright (c) StudioGPU. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// DX11 buffers
//--------------------------------------------------------------------------------------
StructuredBuffer<float4>	o_rays_srv;
RWStructuredBuffer<float4>	i_rays_uav;

//--------------------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------------------
#define RES_X					640
#define RES_Y					360
#define THREAD_REDUCE_FACTOR	2

//--------------------------------------------------------------------
// CopyBuffers() - Entry point
//--------------------------------------------------------------------
[numthreads( RES_Y/THREAD_REDUCE_FACTOR, 1, 1 )]
void CopyBuffers(uint3 blockIdx : SV_GroupID, uint3 threadIdx : SV_GroupThreadID) 
{
	// Get current thread id
	int canvas_idx = blockIdx.x * (RES_Y/THREAD_REDUCE_FACTOR) + threadIdx.x;		
	
	int ray_idx = canvas_idx << 1;
	i_rays_uav[ray_idx] = o_rays_srv[ray_idx];
	i_rays_uav[ray_idx+1] = o_rays_srv[ray_idx+1];
}
	
