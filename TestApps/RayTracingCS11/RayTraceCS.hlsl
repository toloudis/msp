//--------------------------------------------------------------------------------------
// File: RayTrace.hlsl
//
// Copyright (c) StudioGPU. All rights reserved.
//--------------------------------------------------------------------------------------
RWStructuredBuffer<float4> canvas : register( u0 );

// Constant buffer
cbuffer cb0
{
    float4  g_avSampleWeights[15];
    int2    g_outputsize;
    int2    g_inputsize;
}

// Defines
#define blockDim_X		320
#define blockDim_Y		1
#define K_EPSILON		0.005
#define VIEW_DIST		450

// Definition of a ray
typedef struct {
	float3 o;
	float3 d;
} Ray;

// Definition of a structure containing intersection point data
typedef struct {
	int hit;
	float4 color;
	float3 P;
	float3 N;
	float3 S;
	float3 R;
	float3 V;
	float3 H;
} IntersectionData;

// Definition of a sphere
typedef struct {
	float3 center;
	float4 color;
	float radius;
} Sphere;

// Definition of a triangle
typedef struct {
	float3 v0;
	float3 v1;
	float3 v2;
	float3 normal;
	float4 color;
} Triangle;

// Definition of a plane
typedef struct {
	float3 a;
	float3 b;
	float3 p0;
	float3 normal;
	float4 color;
	float a_len_squared;
	float b_len_squared;
} Rectangle;

// Definition of a camera
typedef struct {
	float3 eye;
	float3 lookat;
	float3 up;
	float3 u;
	float3 v;
	float3 w;
} Camera;

// Definition of a light source
typedef struct {
	float3 pos;
	float4 color;
} LightSource;

// Definition of a world
typedef struct {
	float4 backColor;
	Camera camera;
	LightSource lightSource;
	Sphere sphereOne;
	Sphere sphereTwo;
} World;

//--------------------------------------------------------------------
// IntersectSphere()
//--------------------------------------------------------------------
IntersectionData IntersectSphere( Ray ray , Sphere mySphere , float tmin ) {

	float t;
	IntersectionData id;	
	float3 temp = ray.o - mySphere.center;
	float a = dot(ray.d,ray.d);
	float b = 2.0 * dot(temp,ray.d);
	float c = dot(temp,temp) - mySphere.radius * mySphere.radius;
	float disc = b*b - 4.0*a*c;

	if ( disc < 0.0 ) {
		id.hit = 0;
	} else {
		float e = sqrt(disc);
		float denom = 2.0 * a;
		t = (-b - e) / denom;

		if (t > K_EPSILON) {
			tmin = t;
			id.hit = 1;
			id.N = (temp + ray.d*t) / mySphere.radius;
			id.P = ray.o + ray.d*t;
		}
		t = (-b + e) / denom;

		if (t > K_EPSILON) {
			tmin = t;
			id.hit = 1;
			id.N = (temp + ray.d*t) / mySphere.radius;
			id.P = ray.o + ray.d*t;
		}
	}
	return id;
}

//--------------------------------------------------------------------
// IntersectTriangle()
//--------------------------------------------------------------------
int IntersectTriangle( IntersectionData id, Ray ray, Triangle myTriangle, float tmin ) 
{
	float a = myTriangle.v0.x - myTriangle.v1.x;
	float b = myTriangle.v0.x - myTriangle.v2.x;
	float c = ray.d.x;
	float d = myTriangle.v0.x - ray.o.x;
	float e = myTriangle.v0.y - myTriangle.v1.y;
	float f = myTriangle.v0.y - myTriangle.v2.y;
	float g = ray.d.y;
	float h = myTriangle.v0.y - ray.o.y;
	float i = myTriangle.v0.z - myTriangle.v1.z;
	float j = myTriangle.v0.z - myTriangle.v2.z;
	float k = ray.d.z;
	float l = myTriangle.v0.z - ray.o.z;

	float m = f*k - g*j, n = h*k - g*l, p = f*l - h*j;
	float q = g*i - e*k, s = e*j - f*i;

	float inv_denom = 1.0 / (a*m + b*q + c*s);

	float e1 = d*m - b*n - c*p;
	float beta = e1*inv_denom;

	if ( beta < 0.0 ) {
		return 0;
	}

	float r = e*l - h*i;
	float e2 = a*n +d*q + c*r;
	float gamma = e2*inv_denom;

	if ( gamma < 0.0 ) {
		return 0;
	}
	if ( beta + gamma > 1.0 ) {
		return 0;
	}

	float e3 = a*p - b*r + d*s;
	float t = e3*inv_denom;

	if( t < K_EPSILON ) {
		return 0;
	}

	tmin = t;
	id.N = myTriangle.normal;
	id.P = ray.o + ray.d*t;

	return 1;	
}

//--------------------------------------------------------------------
// IntersectRectangle()
//--------------------------------------------------------------------
int IntersectRectangle( IntersectionData id , Ray ray , Rectangle myRectangle , float tmin ) {

	float t = dot(myRectangle.p0 - ray.o,myRectangle.normal) / dot(ray.d,myRectangle.normal);

	if ( t <= K_EPSILON ) {
		id.hit = 0;
		return 0;
	}

	float3 p = (ray.d*t) + ray.o;
	float3 d = p - myRectangle.p0;

	float ddota = dot(d,myRectangle.a);

	if ( ddota < 0.0 || ddota > myRectangle.a_len_squared ) {
		id.hit = 0;
		return 0;
	}

	float ddotb = dot(d,myRectangle.b);

	if ( ddotb < 0.0 || ddotb > myRectangle.b_len_squared ) {
		id.hit = 0;
		return 0;
	}

	tmin = t;
	id.N = myRectangle.normal;
	id.P = p;
	id.hit = 1;

	return 1;
}

//--------------------------------------------------------------------
// phong()
//--------------------------------------------------------------------
float4 phong(IntersectionData id, float Ka, float Kd, float Ks, float Ke, float4 objColor, float4 lightColor, float3 lightPos, float3 rayOrig ) {

	id.S = lightPos - id.P;
	id.V = rayOrig - id.P;
	id.H = id.V + id.S;

	// Ensure everything is normalized
	id.S = normalize(id.S);
	id.V = normalize(id.V);
	id.H = normalize(id.H);
	id.N = normalize(id.N);

	// Compute ambient, diffuse, and specular components
	float4 ambient = objColor*Ka;
	float4 diffuse = lightColor*dot(id.N,id.S)*Kd;				
	float4 specular_blinn = lightColor*pow(dot(id.H,id.N),Ke)*Ks;
	
	return ambient + diffuse + specular_blinn;
}

//--------------------------------------------------------------------
// traceRay()
//--------------------------------------------------------------------
float4 traceRay(Ray ray, World world){
	
	float4 pixelColor = world.backColor;
	
	IntersectionData id;
	
	id = IntersectSphere( ray , world.sphereOne , 0 );		
	if ( id.hit == 1 ) {
		//pixelColor = phong( id, 0.25, 0.25, 0.2, 20, world.sphereOne.color, world.lightSource.color, world.lightSource.pos, ray.o );
		return world.sphereOne.color;
	} 	
	
	id = IntersectSphere( ray , world.sphereTwo , 0 );	
	if ( id.hit == 1 ) {
		//pixelColor = phong( id, 0.25, 0.25, 0.2, 20, world.sphereTwo.color, world.lightSource.color, world.lightSource.pos, ray.o );
		return world.sphereTwo.color;
	} 
	
	return pixelColor;
}

//--------------------------------------------------------------------
// Utility functions
//--------------------------------------------------------------------
Camera makeCamera( float3 eye, float3 lookat, float3 up) {
	Camera myCamera;
	myCamera.eye = eye;
	myCamera.lookat = lookat;
	myCamera.up = up;	
	myCamera.w = normalize( myCamera.eye - myCamera.lookat );
	myCamera.u = normalize(cross(myCamera.up , myCamera.w));
	myCamera.v = cross(myCamera.w , myCamera.u);
	return myCamera;
}
LightSource makeLight( float3 pos , float4 color ) {
	LightSource myLight;
	myLight.pos = pos;
	myLight.color = color;
	return myLight;
}
World makeWorld( LightSource lightSource, Camera camera, float4 backColor, Sphere sphereOne, Sphere sphereTwo ) { 
	World myWorld;
	myWorld.lightSource = lightSource;
	myWorld.camera = camera;
	myWorld.sphereOne = sphereOne;
	myWorld.sphereTwo = sphereTwo;
	myWorld.backColor = backColor;
	return myWorld;
}
Sphere makeSphere(float3 pos , float radius, float4 color) {
	Sphere mySphere;
	mySphere.center = pos;
	mySphere.radius = radius;
	mySphere.color = color;
	return mySphere;
}

//--------------------------------------------------------------------
// render() - Main kernel
//--------------------------------------------------------------------
[numthreads( blockDim_X, blockDim_Y, 1 )]
void Render( uint3 blockIdx : SV_GroupID, uint3 threadIdx : SV_GroupThreadID )
{	
	// Utility colors
    float4 color = float4(0,0,0,1);
	float4 black = float4(0,0,0,1);
	float4 red = float4(1,0,0,1);
	float4 green = float4(0,1,0,1);
	float4 blue = float4(0,0,1,1);
	float4 white = float4(1,1,1,1);	
	
	// Setup world
	Sphere myBall_One = makeSphere( float3(-50, 200 , -500), 300, blue );	
	Sphere myBall_Two = makeSphere( float3(450, -200 , -1000), 150, red );	
	LightSource lightSource = makeLight(float3(200,275,-100),white);
	Camera camera = makeCamera(float3(0,300,1000),float3(0,0,-200),float3(0,1,0));
	World world = makeWorld(lightSource, camera, black, myBall_One, myBall_Two);
	
	// Get current thread id
	int t_idx = blockIdx.x * blockDim_X + threadIdx.x;

	// Figure out thread's (x,y) locs
	int threads_per_row = (blockDim_X * g_inputsize.x) / g_inputsize.y;
	int x_loc = t_idx%threads_per_row;
	int y_loc = t_idx/threads_per_row;
	
	// Setup viewplane
	float2 viewPlane;
	viewPlane.x = x_loc - 0.5 * (g_inputsize.x - 1);
	viewPlane.y = y_loc - 0.5 * (g_inputsize.y - 1);
	
	// Setup primary ray
	Ray primaryRay;
	primaryRay.o = world.camera.eye;	
	primaryRay.d = world.camera.u*viewPlane.x +	world.camera.v*viewPlane.y - world.camera.w*VIEW_DIST;
	primaryRay.d = normalize(primaryRay.d);
	
	// Fire primary ray
	int canvasIdx = y_loc*g_inputsize.x + x_loc;
	canvas[canvasIdx] = traceRay(primaryRay,world);
	
}


