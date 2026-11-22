//--------------------------------------------------------------------------------------
// File: RayTraceCS.hlsl
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
// Local Defines
//--------------------------------------------------------------------------------------
#define MAX_REFRACT_BOUNCES		2

#define PI						3.14159265
#define INVPI					0.318309886
#define INVTWOPI				0.159154943

#define COMPILE_REFRACTION_RAYS
#define COMPILE_REFLECTION_RAYS
#define COMPILE_SHADOW_RAYS

//--------------------------------------------------------------------------------------
// DX11 buffers
//--------------------------------------------------------------------------------------
Buffer<float4>		vertexBuffer[MAX_MESHES];
Buffer<uint>		indexBuffer[MAX_MESHES];
Buffer<float4>		bvhBuffer[MAX_MESHES];

Texture2D<float4>	g_textures[MAX_MESHES];
Texture2D<float4>	g_envtextures[MAX_MESHES];

StructuredBuffer<float4> i_rays_srv;// : register( t0 );
//StructuredBuffer<float4> o_rays_srv;// : register( t1 );

RWStructuredBuffer<float4> canvas		: register( u0 );
RWStructuredBuffer<float4> o_rays_uav   : register( u1 );
//RWStructuredBuffer<float4> i_rays_uav   : register( u7 );

//--------------------------------------------------------------------------------------
// Structs
//--------------------------------------------------------------------------------------
struct Vertex
{
    float4 Position	: POSITION;
    float4 Normal	: NORMAL;
    float4 UV		: TEXCOORD0;
    float4 T		: TANGENT;
    float4 B		: BINORMAL;
};
struct Ray 
{
	float3 o;
	float3 d;
};
struct IntersectionData
 {
	int hit;
	int meshIdx;
	float t;
	float3 P;		// Intersection point
	float3 N;		// P --> normal
	float3 V;		// P --> camera
	float2 UV;
	//float3 B;
	//float3 T;
} ;
struct OptimizedBvhNode
{
	// the bounding box
	float4 _aabbMin;
	float4 _aabbMax;
	float4 _indices; // < _escapeIndex , _subPart , _triangleIdx , _indexBufferIdx >	
} ;
SamplerState g_sampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

//--------------------------------------------------------------------------------------
// Constant buffer : meshes
//--------------------------------------------------------------------------------------
cbuffer cb0 : register( b0 ) // meshes
{
	float4 boxMin[MAX_MESHES];
	float4 boxMax[MAX_MESHES];
	float4x4 worldToObject[MAX_MESHES];
	float4x4 objectToWorld[MAX_MESHES];
	float4 g_diffuse[MAX_MESHES];
	float4 g_specular[MAX_MESHES];
	float4 g_ambient[MAX_MESHES];	
	float4 g_shininess[MAX_MESHES]; // x = shininess, y = reflectivity, z = diffuse map, w = env map	
	float4 g_nTriangles[MAX_MESHES]; // x = ntriangles, y = fresnelBias, z = frenelPower, w = IOR	
	float4 g_textureRes[MAX_MESHES]; // xy is main texture, zw is env texture
};
//--------------------------------------------------------------------------------------
// Constant buffer : world data
//--------------------------------------------------------------------------------------
cbuffer cb1 : register( b1 ) // world
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
// makeBVHNode()
//--------------------------------------------------------------------
OptimizedBvhNode makeBVHNode(int meshIdx, int i)
{
	OptimizedBvhNode node;
	node._aabbMin = bvhBuffer[meshIdx][i*3];
	node._aabbMax = bvhBuffer[meshIdx][i*3+1];
	node._indices = bvhBuffer[meshIdx][i*3+2];
	return node;
}

//--------------------------------------------------------------------
// IntersectBox()
//--------------------------------------------------------------------
bool IntersectBox(Ray ray, float mins_x, float mins_y, float mins_z,
                           float maxes_x, float maxes_y, float maxes_z) 
{
	ray.d.xyz = 1/ray.d.xyz;

	float l1	= (mins_x - ray.o.x) * ray.d.x;
	float l2	= (maxes_x - ray.o.x) * ray.d.x;
	float lmin	= min(l1,l2);
	float lmax	= max(l1,l2);

	l1		= (mins_y - ray.o.y) * ray.d.y;
	l2		= (maxes_y - ray.o.y) * ray.d.y;
	lmin	= max(min(l1,l2), lmin);
	lmax	= min(max(l1,l2), lmax);
	
	l1		= (mins_z - ray.o.z) * ray.d.z;
	l2		= (maxes_z - ray.o.z) * ray.d.z;
	lmin	= max(min(l1,l2), lmin);
	lmax	= min(max(l1,l2), lmax);
	
	return ((lmax >= 0.f) & (lmax >= lmin));
}

//--------------------------------------------------------------------
// IntersectTriangle()
//--------------------------------------------------------------------
IntersectionData IntersectTriangle(const Ray ray, int meshIdx, int i){
		
	IntersectionData id;
	id.hit = 0;
		
	// Get triangle vertices in _p1_, _p2_, and _p3_
	float3 p1 = vertexBuffer[meshIdx][(indexBuffer[meshIdx][i]) * 5].xyz;
	float3 p2 = vertexBuffer[meshIdx][(indexBuffer[meshIdx][i+1]) * 5].xyz;
	float3 p3 = vertexBuffer[meshIdx][(indexBuffer[meshIdx][i+2]) * 5].xyz;

	float3 n1 = vertexBuffer[meshIdx][(indexBuffer[meshIdx][i]) * 5 + 1].xyz;
	float3 n2 = vertexBuffer[meshIdx][(indexBuffer[meshIdx][i+1]) * 5 + 1].xyz;
	float3 n3 = vertexBuffer[meshIdx][(indexBuffer[meshIdx][i+2]) * 5 + 1].xyz;
	
	float2 uv1 = vertexBuffer[meshIdx][(indexBuffer[meshIdx][i]) * 5 + 2].xy;
	float2 uv2 = vertexBuffer[meshIdx][(indexBuffer[meshIdx][i+1]) * 5 + 2].xy;
	float2 uv3 = vertexBuffer[meshIdx][(indexBuffer[meshIdx][i+2]) * 5 + 2].xy;
	
	//float3 tan1 = vertexBuffer[meshIdx][(indexBuffer[meshIdx][i]) * 5 + 3].xyz;
	//float3 tan2 = vertexBuffer[meshIdx][(indexBuffer[meshIdx][i+1]) * 5 + 3].xyz;
	//float3 tan3 = vertexBuffer[meshIdx][(indexBuffer[meshIdx][i+2]) * 5 + 3].xyz;
	//
	//float3 bn1 = vertexBuffer[meshIdx][(indexBuffer[meshIdx][i]) * 5 + 4].xyz;
	//float3 bn2 = vertexBuffer[meshIdx][(indexBuffer[meshIdx][i+1]) * 5 + 4].xyz;
	//float3 bn3 = vertexBuffer[meshIdx][(indexBuffer[meshIdx][i+2]) * 5 + 4].xyz;	
	
	float3 e1 = p2 - p1;
	float3 e2 = p3 - p1;
	float3 s1 = cross(ray.d, e2);
	
	float divisor = dot(s1, e1);
	if (divisor == 0.)
		return id;
	float invDivisor = 1.f / divisor;
	
	// Compute first barycentric coordinate
	float3 d = ray.o - p1;
	float b1 = dot(d, s1) * invDivisor;
	if (b1 < 0. || b1 > 1.)
		return id;
		
	// Compute second barycentric coordinate
	float3 s2 = cross(d, e1);
	float b2 = dot(ray.d, s2) * invDivisor;
	if (b2 < 0. || b1 + b2 > 1.)
		return id;
		
	// Compute _t_ to intersection point
	float t = dot(e2, s2) * invDivisor;	
	if (t < cpu_rtParams_1.x || t > cpu_rtParams_1.y)
		return id;
	
	/*	
	// Fill in _DifferentialGeometry_ from triangle hit
	// Compute triangle partial derivatives		
	float3 dpdu, dpdv;
	float uvs[3][2];
	
	int hasUVs = 1;
	if (hasUVs == 1) {
		uvs[0][0] = uv1.x;
		uvs[0][1] = uv1.y;
		uvs[1][0] = uv2.x;
		uvs[1][1] = uv2.y;
		uvs[2][0] = uv3.x;
		uvs[2][1] = uv3.y;
	} else {
		uvs[0][0] = 0.; uvs[0][1] = 0.;
		uvs[1][0] = 1.; uvs[1][1] = 0.;
		uvs[2][0] = 1.; uvs[2][1] = 1.;
	}
	
	// Compute deltas for triangle partial derivatives
	float du1 = uvs[0][0] - uvs[2][0];
	float du2 = uvs[1][0] - uvs[2][0];
	float dv1 = uvs[0][1] - uvs[2][1];
	float dv2 = uvs[1][1] - uvs[2][1];
	float3 dp1 = p1 - p3, dp2 = p2 - p3;
	float determinant = du1 * dv2 - dv1 * du2;
	if (determinant == 0.f) {
		// Handle zero determinant for triangle partial derivative matrix
		float3 v1 = normalize(cross(e2, e1));
		float3 v2 = dpdu;
		float3 v3 = dpdv;
		
		if ( abs(v1.x) > abs(v1.y) ) {
			float invLen = 1.f / sqrt(v1.x*v1.x + v1.z*v1.z);
			v2 = float3(v1.z * invLen, 0.f, v1.x *invLen);		
		}
		else {
			float invLen = 1.f / sqrt(v1.y*v1.y + v1.z*v1.z);
			v2 = float3(0.f, v1.z * invLen, -v1.y * invLen);		
		}
		v3 = cross(v1,v2);
		dpdu = v2;
		dpdv = v3;		
	}
	else {
		float invdet = 1.f / determinant;
		dpdu = ( dv2 * dp1 - dv1 * dp2) * invdet;
		dpdv = (-du2 * dp1 + du1 * dp2) * invdet;
	}
	// Interpolate $(u,v)$ triangle parametric coordinates
		
	float tu = b0*uvs[0][0] + b1*uvs[1][0] + b2*uvs[2][0];
	float tv = b0*uvs[0][1] + b1*uvs[1][1] + b2*uvs[2][1];
	*/
	float b0 = 1 - b1 - b2;

	id.hit = 1;
	id.t = t;
	id.N = b0*n1 + b1*n2 + b2*n3;
	id.UV = b0*uv1 + b1*uv2 + b2*uv3;
	//id.T = b0*tan1 + b1*tan2 + b2*tan3;
	//id.B = b0*bn1 + b1*bn2 + b2*bn3;
	id.P = b0*p1 + b1*p2 + b2*p3;
	id.V = ray.o - id.P;
	id.meshIdx = meshIdx;
	
	//id.T = normalize(mul(objectToWorld[meshIdx], float4(id.T,0)));
	//id.B = normalize(mul(objectToWorld[meshIdx], float4(id.B,0)));
	
	id.N = normalize(mul(objectToWorld[meshIdx], float4(id.N,0)));
	id.V = normalize(mul(objectToWorld[meshIdx], float4(id.V,0)));
	id.P = (mul(objectToWorld[meshIdx], float4(id.P,1))).xyz;
	
	return id;
}

//--------------------------------------------------------------------
// getReflectRay()
//--------------------------------------------------------------------
float3 getReflectRay( float3 N , float3 S ) {
	return S - (N * (2 * dot(S,N)));
}

//--------------------------------------------------------------------
// PhongDiffuse()
//--------------------------------------------------------------------
float3 PhongDiffuse(float3 normal, float3 lightDir, float3 lDiffColor, float3 sDiffColor)
{
	float cosine = saturate(dot(normal,lightDir));
	return cosine * (lDiffColor * sDiffColor);
}

//--------------------------------------------------------------------
// PhongSpecular()
//--------------------------------------------------------------------
float3 PhongSpecular(float3 normal, float3 lightDir, float3 eyeDir, 
	float3 lSpecColor, float3 sSpecColor, float sSpecPower)
{
	float specComp = saturate(dot(normal,normalize(lightDir + eyeDir)));
    specComp = (dot(normal, lightDir)>0) * (pow(specComp, max(0.001, sSpecPower)));
	return specComp * (lSpecColor * sSpecColor);
}

//--------------------------------------------------------------------
// IntersectBVH()
//--------------------------------------------------------------------
IntersectionData IntersectBVH(Ray ray, int meshIdx)
{
	// transform ray into object space.
	// assumes ray is in world space to start.
	ray.o = mul(worldToObject[meshIdx], float4(ray.o, 1)).xyz;
	ray.d = mul(worldToObject[meshIdx], float4(ray.d, 0)).xyz;

	IntersectionData ret_id;
	IntersectionData theHit;
	ret_id.hit = 0;
	theHit.hit = 0;
	
	float tmin = cpu_rtParams_1.y;
	
    bool aabbOverlap = false;
    bool isLeafNode = false;

	int walkIterations = 0;	
	int escapeIndex = 0;
	int currIndex = 0;
    int rootNodeIndex = 0;
	int endIndex = (g_nTriangles[meshIdx].x * 2) - 1;
    
    OptimizedBvhNode rootNode = makeBVHNode(meshIdx, rootNodeIndex);  
    
	while (currIndex < endIndex)
	{
		//catch bugs in tree data
		if (walkIterations >= endIndex) break; // exception - should assert but this is a compute shader!
	
		walkIterations++;

		// test ray against aabb
		aabbOverlap = IntersectBox( ray , rootNode._aabbMin.x , rootNode._aabbMin.y, rootNode._aabbMin.z,
										  rootNode._aabbMax.x , rootNode._aabbMax.y, rootNode._aabbMax.z);
		isLeafNode = rootNode._indices.x < 0;

		// Ray hit box, so test against triangle in leaf
		if (aabbOverlap && isLeafNode)
		{
			theHit = IntersectTriangle( ray, meshIdx, rootNode._indices.w );
			if ( (theHit.hit == 1) && (theHit.t < tmin) ) {
				tmin = theHit.t;
				ret_id = theHit;
			}	
		}
		
		if (aabbOverlap || isLeafNode) // Move forward by ONE
		{
			rootNodeIndex++;
			currIndex++;
            if (rootNodeIndex < endIndex )
            {
                rootNode = makeBVHNode(meshIdx, rootNodeIndex);
            }
        }        
		else // Skip forward by ESCAPE index
		{
			rootNodeIndex += rootNode._indices.x;
			currIndex += rootNode._indices.x;
            if (rootNodeIndex < endIndex )
            {
                rootNode = makeBVHNode(meshIdx, rootNodeIndex);
            }
		}
	}

	return ret_id;
}


//--------------------------------------------------------------------
// SampleDiffuseTexture()
//--------------------------------------------------------------------
float3 SampleDiffuseTexture(int meshIdx, float2 UV)
{
	float2 texRes = g_textureRes[meshIdx].xy;
	float2 uvPixels = UV * texRes;
	//int texIndex = uvPixels.y * texRes.x + uvPixels.x;
	
	float3 t = float3(1,1,1);//(g_textures[meshIdx])[texIndex].xyz;

	int i;
	for (i = 0; i < MAX_MESHES; i++)
	{
		if (i >= cpu_numMeshes) break;
		if (i == meshIdx)
		{
			t = g_textures[i][uvPixels].xyz;
			break;
		}
	}
	
	return t;
}

//--------------------------------------------------------------------
// CartesianToPolar()
//--------------------------------------------------------------------
float2 CartesianToPolar( float3 vec )
{
	float3 nvec = normalize( vec );
	return float2( (atan2( nvec.z, nvec.x )+PI)*INVTWOPI, acos( nvec.y )*INVPI );
}

//--------------------------------------------------------------------
// SampleEnvTexture()
//--------------------------------------------------------------------
float3 SampleEnvTexture(int meshIdx, float3 vec)
{
	float2 texRes = g_textureRes[meshIdx].zw;
	float2 UV = CartesianToPolar(vec);
	float2 uvPixels = UV * texRes;
	//int texIndex = uvPixels.y * texRes.x + uvPixels.x;
	
	float3 t = float3(1,1,1);//(g_envtextures[meshIdx])[texIndex].xyz;
	
	int i;
	for (i = 0; i < MAX_MESHES; i++)
	{
		if (i >= cpu_numMeshes) break;
		if (i == meshIdx)
		{
			t = g_envtextures[i][uvPixels].xyz;
			break;
		}
	}

	return t;
}

//--------------------------------------------------------------------
// fastFresnel() - fresnel approximation
//--------------------------------------------------------------------
float fastFresnel(float NdotE, float R0, float power)
{
	// R(theta) = (1/2) ((g-c)/(g+c))^2 (1 + [ (c(g+c)-(eta)^2)/(c(g-c)+ (eta)^2) ]2)
	// approximate:
	// R(theta) ~= Ra(theta) = R(0) + (1-R(0))*(1-cos(theta))^5
   return R0 + (1.0-R0)*pow(abs(1.0 - saturate(NdotE)), power);
} 

//--------------------------------------------------------------------
// traceRay()
//--------------------------------------------------------------------
IntersectionData traceRay( Ray ray, float t_dist )
{
	IntersectionData mesh_id;	
	IntersectionData ret_id;
	mesh_id.hit = 0;	
	ret_id.hit = 0;
				
	// Check intersect with boxes in scene
	int i;
	for ( i = 0 ; i < MAX_MESHES ; i++ ) 
	{
		if (i >= cpu_numMeshes) break;
	
		mesh_id = IntersectBVH(ray, i);
	
		// Ray hit mesh in box
		if (( mesh_id.hit == 1 ) && (mesh_id.t < t_dist)) 
		{	
			t_dist = mesh_id.t;
			ret_id = mesh_id;
		}
	}	
	return ret_id;	
}

//--------------------------------------------------------------------
// shade()
//--------------------------------------------------------------------
float4 shade( IntersectionData id ) 
{
	// Ensure everything is normalized
	float3 V = normalize(id.V);
	float3 N = normalize(id.N);

	// Compute ambient, diffuse, and specular components	
	float3 t = (g_shininess[id.meshIdx].z == 1) ?  SampleDiffuseTexture(id.meshIdx, id.UV) : float3(1,1,1);
	float3 e = (g_shininess[id.meshIdx].w == 1) ? SampleEnvTexture(id.meshIdx, N) : float3(1,1,1);
	
	// pixelColor starts off as ambient
	float4 pixelColor = g_ambient[id.meshIdx] * g_diffuse[id.meshIdx] * float4(e*t,1);

	int i;
	for ( i = 0 ; i < MAX_LIGHTS ; i++ ) {
	
		if (i >= cpu_numLights) break;
			
		// Calculate light vector
		float3 L = cpu_light_pos[i] - id.P;
		float dist = length(L);
		L = normalize( L );
		
		IntersectionData shadow_id;
		shadow_id.hit = 0;
				
#ifdef COMPILE_SHADOW_RAYS
		if( cpu_rtParams_1.w == 1 && cpu_light_pos[i].w == 1 ) 
		{		
			Ray shadowRay;
			shadowRay.o = id.P;
			shadowRay.d = L;
			shadow_id = traceRay( shadowRay, dist );
		}
#endif
		
		if ( shadow_id.hit == 0 ) 
		{			
			float3 d = PhongDiffuse(N, L, cpu_light_diffuseColor[i].xyz, g_diffuse[id.meshIdx].xyz);
			float3 s = PhongSpecular(N, L, V, cpu_light_specularColor[i].xyz, g_specular[id.meshIdx].xyz, g_shininess[id.meshIdx].x);
			float4 diffuse = float4(t*d, 0);
			float4 specular = float4(s, 0);
			
			// Falloff
			float atten = 1 / (cpu_light_fallOff[i].x + cpu_light_fallOff[i].y * dist + cpu_light_fallOff[i].z * dist * dist);			
			diffuse *= atten;
			specular *= atten;
			
			pixelColor += diffuse + specular;
		} 		
				
	}	
		
	return pixelColor;
}

//--------------------------------------------------------------------
// getRefractRay()
//--------------------------------------------------------------------
float3 getRefractRay( float3 N, float3 I, float n1, float n2, int bounceNum )
{	
	// determine backface or not
	int backFace = (bounceNum%2 == 0) ? -1 : 1;

	//Ratio of object / air
	float n = (backFace == 1) ? (n1 / n2) : (n2 / n1);

	//cosine of angle of dir to normal
	float cosI = dot(-I, N);
	
	float cosT2 = 1.0f - n * n * (1.0f - cosI * cosI);
	
	//If cosT2 < 0, total internal reflection occured so no refraction
	if (cosT2 < 0)
		return float3(0,0,0);

	// determine backface or not
	//int backFace = (cosI > 0) ? -1 : 1;

	//Otherwise, return refraction ray	
	return normalize((I * n) + (N * (n * cosI - sqrt(cosT2)*backFace )));
}

//--------------------------------------------------------------------
// ZeroOutRays()
//--------------------------------------------------------------------
void ZeroOutRays(int ray_idx) {
	o_rays_uav[ray_idx] = float4(0,0,0,0);
	o_rays_uav[ray_idx+1] = float4(0,0,0,0);
	o_rays_uav[ray_idx+2] = float4(0,0,0,0);
	o_rays_uav[ray_idx+3] = float4(0,0,0,0);
}

//--------------------------------------------------------------------
// traceRay()
//--------------------------------------------------------------------
IntersectionData traceScene( Ray ray, int canvas_idx, int ray_idx )
{
	IntersectionData id;
	id.hit = 0;

	if( any(ray.d) )
	{	
		// Trace ray
		id = traceRay( ray, cpu_rtParams_1.y );

		// If hit
		if ( id.hit == 1 )
		{
			o_rays_uav[ray_idx] = float4( id.P , 0 );
			o_rays_uav[ray_idx+1] = float4( id.N , 0 );
			o_rays_uav[ray_idx+2] = float4( ray.d , 0 );
			o_rays_uav[ray_idx+3] = float4( id.meshIdx, 0 , 0 , 0 );	
		}
		
		// If miss
		else 
		{
			ZeroOutRays(ray_idx);
		}
	}
	
	return id;
}


//--------------------------------------------------------------------
// RenderPrimary() - Entry point
//--------------------------------------------------------------------
[numthreads( RES_Y/THREAD_REDUCE_FACTOR, 1, 1 )]
void RenderPrimary( uint3 blockIdx : SV_GroupID, uint3 threadIdx : SV_GroupThreadID )
{		
	// Get current thread id
	int canvas_idx = blockIdx.x * (RES_Y/THREAD_REDUCE_FACTOR) + threadIdx.x;
	int ray_idx = canvas_idx * NUM_RAY_ELEMENTS;
	
	Ray ray;
	ray.o = i_rays_srv[ray_idx];
	ray.d = i_rays_srv[ray_idx+1];
	
	IntersectionData id = traceScene( ray, canvas_idx, ray_idx );
	
	if ( id.hit == 1 && cpu_rtParams_2.y == 0)
	{
		canvas[canvas_idx] += shade( id );
	}
}


//--------------------------------------------------------------------
// RenderReflect() - Entry point
//--------------------------------------------------------------------
[numthreads( RES_Y/THREAD_REDUCE_FACTOR, 1, 1 )]
void RenderReflect( uint3 blockIdx : SV_GroupID, uint3 threadIdx : SV_GroupThreadID )
{		
	// Get current thread id
	int canvas_idx = blockIdx.x * (RES_Y/THREAD_REDUCE_FACTOR) + threadIdx.x;
	int ray_idx = canvas_idx * NUM_RAY_ELEMENTS;
	
	float3 O = i_rays_srv[ray_idx];
	float3 N = i_rays_srv[ray_idx+1];
	float3 prev_D = i_rays_srv[ray_idx+2];
	float meshIdx = i_rays_srv[ray_idx+3].x;
	
	Ray ray;
	ray.o = O;
	ray.d = getReflectRay( N, prev_D );
	
	if ( g_shininess[meshIdx].y > 0 ) // If mesh is reflective
	{
		IntersectionData id = traceScene( ray, canvas_idx, ray_idx );
	
		if ( id.hit == 1 ) 
		{
			canvas[canvas_idx] += shade( id ) * 
				                  cpu_rtParams_2.z * 
					              g_shininess[meshIdx].y * 
						          fastFresnel(dot(-ray.d, id.N), g_nTriangles[meshIdx].y, g_nTriangles[meshIdx].z);
		}
	}
}

//--------------------------------------------------------------------
// RenderRefract() - Entry point
//--------------------------------------------------------------------
[numthreads( RES_Y/THREAD_REDUCE_FACTOR, 1, 1 )]
void RenderRefract( uint3 blockIdx : SV_GroupID, uint3 threadIdx : SV_GroupThreadID )
{		
	// Get current thread id
	int canvas_idx = blockIdx.x * (RES_Y/THREAD_REDUCE_FACTOR) + threadIdx.x;
	int ray_idx = canvas_idx * NUM_RAY_ELEMENTS;
	
	float3 O = i_rays_srv[ray_idx];
	float3 N = i_rays_srv[ray_idx+1];
	float3 prev_D = i_rays_srv[ray_idx+2];
	float meshIdx = i_rays_srv[ray_idx+3].x;
	
	Ray ray;
	ray.o = O;
	ray.d = getRefractRay( N, prev_D, 1.0, g_nTriangles[meshIdx].w, cpu_rtParams_1.z );
	
	if ( g_nTriangles[meshIdx].w >= 1 ) // If mesh is refractive
	{
		IntersectionData id = traceScene( ray, canvas_idx, ray_idx );
	
		if ( id.hit == 1 ) 
		{
			canvas[canvas_idx] += shade( id ) * cpu_rtParams_3.y;
		}
	}
}


/*
//--------------------------------------------------------------------
// traceSceneSinglePass()
//--------------------------------------------------------------------
void traceSceneSinglePass(Ray ray, int canvas_idx) {

	// Trace ray
	IntersectionData prim_Id = traceRay( ray, cpu_rtParams_1.y );
	
	// Shade intersected mesh
	if ( prim_Id.hit == 1 && cpu_rtParams_2.y == 0 ) 
	{
		canvas[canvas_idx] = shade( prim_Id );
	}	

	// If primary ray hit
	if ( prim_Id.hit == 1 )
	{
		int prevMeshId = prim_Id.meshIdx;
		
		IntersectionData id = prim_Id;

#ifdef COMPILE_REFRACTION_RAYS		
		for ( int refractionBounceNum = 1 ; refractionBounceNum < MAX_REFRACT_BOUNCES+1 ; refractionBounceNum++ ) 
		{	
			if ( g_nTriangles[prim_Id.meshIdx].w >= 1 &&	// If mesh is refractive
				 cpu_rtParams_3.x == 1 )					// If refraction checkbox 
			{
						
				// Calculate new ray
				Ray refractRay;
				refractRay.o = id.P;				
				refractRay.d = getRefractRay( id.N, ray.d, 1.0, g_nTriangles[id.meshIdx].w, refractionBounceNum );				
				
				// Trace ray
				id = traceRay( refractRay, cpu_rtParams_1.y );				
					
				// Shade intersected mesh
				if ( id.hit == 1 && refractionBounceNum != 1  ) 
				{				
					canvas[canvas_idx] += shade( id ) * cpu_rtParams_3.y;
				}
			}
		}
#endif

#ifdef COMPILE_REFLECTION_RAYS	
		if ( g_shininess[prim_Id.meshIdx].y > 0 &&	// If mesh is reflective
			 cpu_rtParams_2.w == 1 )				// If reflection checkbox
		{
			IntersectionData id = prim_Id;
			
			// Calculate new ray
			Ray reflectRay;
			reflectRay.o = id.P;
			reflectRay.d = getReflectRay( id.N, ray.d );
			
			// Trace ray
			id = traceRay( reflectRay, cpu_rtParams_1.y );				
				
			// Shade intersected mesh
			if ( id.hit == 1 ) 
			{				
				canvas[canvas_idx] += shade( id ) * cpu_rtParams_2.z * g_shininess[prevMeshId].y * fastFresnel(dot(-ray.d, id.N), g_nTriangles[prevMeshId].y, g_nTriangles[prevMeshId].z);						
			}
		}
#endif

	}
	
	// No hits at all
	if  ( prim_Id.t == cpu_rtParams_1.y ) canvas[canvas_idx] = cpu_backColor;
	
}
*/