/*****************************************************************************
**  SupportHair.h
**
**      Support functions for "hair" .fx functions
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "Lighting.h"
#include "Tessellate.h"

//This calculates the alpha based on the hair width below 1 screen pixel
//#define USE_SUBPIXEL_ATTENUATION

bool hasHairSupport = true;	//if this flag is defined in a shader then rendering of hair is supported

bool g_bHasOSM = false;			//when we have an Opacity Shadow Map
bool g_bHasOSMMulti = false;	//when using all 4 OSM
bool g_bHasOSMMulti32 = false;	//when using all 8 OSM
bool g_HasDepthMap = false;		//when we have a depth map for deep opacity maps
bool g_bDrawOddTessellated = false;	//Odd quads for hacked Quad Strips
int g_nStrandCPs = 5;				//number of Control Points in a Strand

Texture2D projOSM;	//Opacity Shadow Map
Texture2D projOSM2; //other OSM's
Texture2D projOSM3;
Texture2D projOSM4;
Texture2D projOSM5;
Texture2D projOSM6;
Texture2D projOSM7;
Texture2D projOSM8;
Texture2D depthMap;	//the depth map (R32f actual depths)

RWTexture3D<float> OpacityShadowBuffer3D;

bool g_HasOpacityShadowTexture3D = false;
Texture3D OpacityShadowTexture3D;

//these will be around the object, except if the light planes cross the bounds
//then these will be reduced by the lights bounds
float g_ZNear = 1;		//minimal bounds near plane
float g_ZFar = 1000;		//maximal bounds far plane
float2 g_InvScreenSize;	//inverse of screen resolution

float4 g_LightViewPlane;		//plane of the light camera view
//float2 g_Aspect = {1,1};				//projection X,Y scale factors
float  g_SubPixelPower = 1.0f;		//0 = none, 1 = linear, 2 = squared, etc...

Texture2D hairDataTexture;
float4 g_HairDataTextureSize = {1024,1024,1.0f/1024,1.0f/1024};	//(Width,Height,1/Width,1/Height)

//hardware tessellation value
float2 g_HairTessellationValue = {1.0f,1.0f};

//clump interpolation
float g_ClumpRadius = 0.0f;

float2 random2[64] =
{
	-0.415757954121f, 	-0.307007312775f,
	-0.999989032745f, 	0.123504042625f,
	-0.324011802673f, 	0.33845436573f,
	-0.969086050987f, 	-0.997798800468f,
	-0.181707203388f, 	-0.0354183316231f,
	0.9620013237f, 	0.746612906456f,
	0.634776711464f, 	-0.359547495842f,
	0.686866641045f, 	0.570747971535f,
	-0.00195515155792f, 	-0.605298280716f,
	0.0248702764511f, 	0.37431037426f,
	-0.0493814945221f, 	0.33611869812f,
	0.402388095856f, 	-0.887425601482f,
	0.920067548752f, 	0.192143321037f,
	-0.360320627689f, 	-0.868100583553f,
	0.0381577014923f, 	-0.111639559269f,
	0.539649248123f, 	0.668017029762f,
	0.0927656888962f, 	0.384810805321f,
	0.651272773743f, 	0.127217888832f,
	0.206308484077f, 	0.607260704041f,
	0.79118001461f, 	0.677464842796f,
	0.139808416367f, 	-0.963199913502f,
	-0.22441393137f, 	-0.851792991161f,
	0.0195223093033f, 	-0.482260942459f,
	0.342718839645f, 	0.930730938911f,
	-0.0997076034546f, 	0.204538822174f,
	-0.378837943077f, 	0.659482836723f,
	0.00894856452942f, 	0.505936384201f,
	0.651379942894f, 	-0.00585699081421f,
	-0.837081551552f, 	0.951209783554f,
	-0.290645718575f, 	-0.701874494553f,
	0.0917477607727f, 	0.152590155602f,
	-0.503503918648f, 	-0.72034406662f,
	0.703035116196f, 	0.0749887228012f,
	-0.96193087101f, 	0.780337452888f,
	-0.770724475384f, 	-0.285424351692f,
	-0.521964728832f, 	0.859279155731f,
	-0.264045059681f, 	0.0244901180267f,
	-0.287628412247f, 	-0.221044540405f,
	-0.726094305515f, 	0.877244234085f,
	-0.0579364299774f, 	0.575763344765f,
	-0.89625442028f, 	0.213267683983f,
	0.921410679817f, 	0.599874258041f,
	0.405699491501f, 	0.167470216751f,
	-0.266678452492f, 	-0.486743867397f,
	-0.200808227062f, 	-0.28875541687f,
	-0.407266914845f, 	0.879252076149f,
	-0.610522627831f, 	-0.502714037895f,
	0.190292000771f, 	-0.375916957855f,
	0.432513713837f, 	0.337543964386f,
	-0.599826455116f, 	-0.362500607967f,
	0.122844338417f, 	-0.674452006817f,
	-0.0707055330276f, 	-0.804758310318f,
	0.561969161034f, 	0.534954190254f,
	-0.706166863441f, 	0.713750720024f,
	0.50043463707f, 	-0.15829706192f,
	-0.931690633297f, 	0.597908616066f,
	-0.0521476864815f, 	0.0349020957947f,
	-0.997243881226f, 	-0.141711473465f,
	-0.962580561638f, 	0.935882329941f,
	0.4426176548f, 	-0.195207297802f,
	0.749297738075f, 	-0.0625938177109f,
	0.908566951752f, 	-0.0246162414551f,
	-0.637861430645f, 	-0.923464179039f,
	-0.79311645031f, 	0.809995889664f,
/*	0.887435913086f, 	0.544613480568f,
	0.946038246155f, 	0.121113061905f,
	-0.593244731426f, 	0.739417433739f,
	0.46822810173f, 	-0.0841932892799f,
	0.529536962509f, 	-0.901707589626f,
	0.92825114727f, 	-0.577435731888f,
	0.600801944733f, 	0.924322366714f,
	0.665665864944f, 	0.993400335312f,
	0.774976491928f, 	0.0103375911713f,
	0.62262237072f, 	-0.705139458179f,
	0.493190407753f, 	0.288145065308f,
	-0.864136457443f, 	-0.975541055202f,
	0.625094771385f, 	-0.61847358942f,
	0.324367523193f, 	0.309258699417f,
	-0.658737182617f, 	-0.149027884007f,
	-0.476232171059f, 	-0.254612147808f,
	0.449486494064f, 	0.591151356697f,
	0.315091848373f, 	-0.569881975651f,
	0.325016498566f, 	-0.0642792582512f,
	0.484638929367f, 	0.160881996155f,
	-0.73237580061f, 	0.206281661987f,
	0.111445307732f, 	0.0115730762482f,
	-0.457200288773f, 	-0.359537184238f,
	0.752270579338f, 	0.145184874535f,
	-0.26561653614f, 	-0.290171980858f,
	-0.86328625679f, 	-0.506230592728f,
	0.908035159111f, 	-0.188288152218f,
	-0.353170394897f, 	-0.747117161751f,
	0.189489364624f, 	0.399418950081f,
	-0.872706890106f, 	0.0921521186829f,
	-0.727744817734f, 	-0.860645651817f,
	-0.948987066746f, 	-0.441547632217f,
	0.401406764984f, 	-0.469781160355f,
	-0.26779872179f, 	-0.0837929844856f,
	0.937297105789f, 	-0.730389237404f,
	0.0744278430939f, 	0.0924748182297f,
	-0.704310297966f, 	0.354593992233f,
	0.155883431435f, 	0.644702553749f,
	-0.636761069298f, 	-0.0448909401894f,
	0.420086026192f, 	0.921164751053f,
	-0.251735448837f, 	-0.144667387009f,
	0.189654707909f, 	-0.225541591644f,
	0.501892805099f, 	0.495574474335f,
	0.346584320068f, 	0.539728045464f,
	0.141563415527f, 	0.998720407486f,
	-0.506407439709f, 	-0.885527908802f,
	-0.540439724922f, 	0.238392114639f,
	-0.305917322636f, 	0.130444765091f,
	0.17865550518f, 	0.905961751938f,
	-0.514184236526f, 	0.381422758102f,
	0.0286765098572f, 	0.179683923721f,
	-0.712139964104f, 	-0.478293836117f,
	-0.521578788757f, 	-0.358527898788f,
	0.323919296265f, 	-0.903955638409f,
	-0.250787854195f, 	-0.774267852306f,
	-0.958730041981f, 	0.393280744553f,
	-0.635471224785f, 	-0.309784889221f,
	-0.248264908791f, 	0.699278116226f,
	0.250646710396f, 	-0.535455822945f,
	0.521033525467f, 	0.373806238174f,
	0.348887324333f, 	-0.270278334618f,
	-0.394953966141f, 	0.139647841454f,
	0.0216785669327f, 	0.919688224792f,
	0.371087551117f, 	0.46440577507f,
	0.556820988655f, 	-0.339622676373f,
	0.738476872444f, 	0.477137684822f,
	0.771810650826f, 	0.93575835228f,
	-0.205457806587f, 	0.182102441788f,
	-0.903442919254f, 	-0.576662361622f,
	-0.421976387501f, 	-0.214109063148f,
	-0.3824852705f, 	0.41473531723f,
	0.88427066803f, 	0.845234274864f,
	-0.597764253616f, 	-0.794075012207f,
	0.679982304573f, 	-0.802298903465f,
	-0.238126754761f, 	0.403813362122f,
	0.6050812006f, 	-0.543026208878f,
	-0.145884990692f, 	-0.211347818375f,
	-0.745605826378f, 	-0.999535262585f,
	-0.384266376495f, 	0.295287370682f,
	0.271909594536f, 	-0.0186420679092f,
	0.0944821834564f, 	0.740437626839f,
	-0.266018867493f, 	-0.962322890759f,
	0.0610929727554f, 	-0.268881261349f,
	-0.697465002537f, 	-0.720154285431f,
	-0.308974266052f, 	0.838204264641f,
	0.582984209061f, 	0.220297574997f,
	0.484161615372f, 	0.869464635849f,
	0.835884928703f, 	-0.217734217644f,
	0.866134524345f, 	0.962750196457f,
	0.359741687775f, 	-0.157439529896f,
	-0.978330790997f, 	0.503557443619f,
	0.515821576118f, 	0.0977878570557f,
	-0.781891405582f, 	0.660185813904f,
	-0.467448890209f, 	0.0832816362381f,
	-0.502683162689f, 	-0.151012897491f,
	0.876625180244f, 	-0.895189762115f,
	-0.900877118111f, 	-0.658530831337f,
	-0.367899179459f, 	0.79533624649f,
	0.32302069664f, 	-0.365221679211f,
	0.470696091652f, 	0.72997879982f,
	-0.167205512524f, 	-0.500283718109f,
	-0.93359130621f, 	0.148563742638f,
	0.395768165588f, 	-0.0961967110634f,
	0.395884156227f, 	0.74974834919f,
	0.080939412117f, 	0.599899888039f,
	-0.0910460948944f, 	-0.67810177803f,
	0.524116873741f, 	-0.644667506218f,
	0.672092199326f, 	-0.292624533176f,
	0.76543033123f, 	0.551385998726f,
	0.443885326385f, 	-0.527751803398f,
	0.301817536354f, 	0.707025885582f,
	0.107479453087f, 	-0.202602922916f,
	-0.160126268864f, 	-0.582996606827f,
	0.402836322784f, 	-0.633915960789f,
	-0.523296117783f, 	-0.95643222332f,
	0.452896356583f, 	-0.841628909111f,
	-0.865230441093f, 	0.794190526009f,
	0.522953748703f, 	-0.397499680519f,
	-0.717108011246f, 	-0.234159350395f,
	0.0981377363205f, 	0.841750502586f,
	-0.810242652893f, 	0.19634604454f,
	-0.905021548271f, 	0.730925798416f,
	0.211004734039f, 	0.215678215027f,
	-0.441374003887f, 	0.262136340141f,
	0.0325330495834f, 	0.0389763116837f,
	-0.812033832073f, 	0.536602258682f,
	0.0334894657135f, 	-0.386205136776f,
	0.200131177902f, 	0.0127446651459f,
	-0.027800142765f, 	0.781478524208f,
	0.415264368057f, 	0.258855342865f,
	0.306766033173f, 	0.480510234833f,
	0.349022507668f, 	-0.77649140358f,
	0.136532664299f, 	0.520867466927f,
	0.812962174416f, 	-0.559707403183f,
	-0.0205968022346f, 	0.986451745033f,
	-0.509413957596f, 	0.560806512833f,
	-0.550288438797f, 	0.476644039154f,
	0.939857363701f, 	0.456780433655f,
	-0.898642957211f, 	0.863899350166f,
	0.786417841911f, 	-0.890314102173f,
	-0.076176404953f, 	0.922913551331f,
	-0.860582351685f, 	-0.122878789902f,
	0.600135326385f, 	0.861961245537f,
	0.698167085648f, 	0.89713537693f,
	0.492275953293f, 	-0.0186278223991f,
	-0.841658473015f, 	-0.596030235291f,
	0.905914902687f, 	0.337806820869f,
	-0.721285104752f, 	-0.0184351801872f,
	-0.0693228840828f, 	-0.993405759335f,
	0.715800404549f, 	-0.203863024712f,
	0.117646455765f, 	-0.843482613564f,
	-0.443656802177f, 	-0.815229892731f,
	0.597274661064f, 	-0.766075611115f,
	-0.762543559074f, 	0.747245788574f,
	0.193806886673f, 	0.733397245407f,
	-0.985407471657f, 	-0.356724023819f,
	-0.738691449165f, 	-0.371147453785f,
	-0.537099838257f, 	-0.810829877853f,
	0.740815758705f, 	-0.994248151779f,
	0.807778120041f, 	-0.664702653885f,
	0.582011938095f, 	0.792075037956f,
	0.136256337166f, 	0.264344453812f,
	-0.18073785305f, 	-0.428484141827f,
	0.00223731994629f, 	-0.956182062626f,
	-0.00850087404251f, 	-0.251703977585f,
	0.756610989571f, 	-0.620826601982f,
	-0.911600470543f, 	-0.279290378094f,
	-0.653744339943f, 	0.645545363426f,
	0.657974839211f, 	0.625242829323f,
	0.58411192894f, 	0.132586479187f,
	0.879118680954f, 	0.107514619827f,
	-0.133203148842f, 	-0.907719731331f,
	0.583663225174f, 	-0.442635238171f,
	0.918936729431f, 	-0.316513895988f,
	-0.244584262371f, 	-0.625973284245f,
	-0.808946669102f, 	-0.828397154808f,
	0.271717905998f, 	-0.104681670666f,
	-0.229069411755f, 	-0.698700726032f,
	0.267337203026f, 	-0.172531247139f,
	0.671711683273f, 	-0.569589614868f,
	0.477365732193f, 	-0.997455060482f,
	0.292757153511f, 	-0.426412165165f,
	0.393433690071f, 	-0.725052118301f,
	-0.469008982182f, 	-0.452587723732f,
	-0.594199061394f, 	0.0838865041733f,
	0.171688318253f, 	-0.522022247314f,
	0.991615891457f, 	0.225111961365f,
	0.370167851448f, 	0.396544337273f,
	-0.218688964844f, 	0.797135591507f,
	-0.57398968935f, 	0.347099304199f,
	0.68114900589f, 	0.757608890533f,
	0.745979785919f, 	-0.698154687881f,
*/
};


//************Must be in sync with mdlHairMaterial****************
struct hairMaterial
{
	float m_RootRadius;
	float m_TipRadius;
	float3 m_RootColor;
	float3 m_TipColor;
	float3 m_SurfaceNormal;
	float m_Opacity;
	float m_Specular;
	float m_Gloss;
	float m_AmbientDiffuse;
};

StructuredBuffer< hairMaterial > g_HairMaterials;
StructuredBuffer< float3 > g_HairGeometry;

/* data from application vertex buffer */
struct hairVertex
{
	float4 Position	: SV_POSITION;
	float3 Tangent	: TANGENT;
	float4 Color    : COLOR;
	float2 Interp	: TEXCOORD0;	// 0-1 down strand, radius (width of hair)
	float4 Material : TEXCOORD1;	// (Opacity, Specular, m_Gloss, AmbientDiffuse)
};

struct hairVertexTessDX11
{
	float4 Position	: SV_POSITION;
};

struct hairOutput
{
	float4 Position	: TEXCOORD0;
};

struct hairVertexTess
{
	// Cartesian coordinates
	float4 vCartesianCoordinates : BLENDWEIGHT0;
	// Indices
	uint4 vIndices		: BLENDINDICES0;

	float4 Position0 : POSITION0;
	float4 Position1 : POSITION4;
	float4 Position2 : POSITION8;
	float4 Position3 : POSITION12;
};

//-----------------Vertex Shader functions-------------------------
/*
//returns the screen aligned world space transformed vertex of an expanded ribbon
void StrandVertex( in float4x4 World, in float4 Pos, in float3 Tan, out float3 WPos, out float3 WTan )
{
	float3 newPos = mul( World, float4( Pos.xyz, 1 )).xyz;

	float3 nTan = normalize( Tan );
	WTan = mul( (float3x3)World, nTan );

	float3 ViewVec = g_eyePos.xyz - newPos;
	float3 nViewVec = normalize( ViewVec );

	float3 Side = normalize(cross( ViewVec, WTan ));
	WPos = newPos + (Side * Pos.w);	//position.w is signed radius
}

void StrandVertexLimit( in float4 Pos, in float3 Tan, in float radius, out float3 WPos, out float3 WTan, out float SubPixel )
{
	SubPixel = 1;
	float ProjPlane = mul( g_wvp, float4( Pos.xyz, 1 ) ).w;   //Plane to flatten ribbon on

	// put pos and tan into world space
	float3 newPos = mul( g_world, float4( Pos.xyz, 1 )).xyz;
	WTan = mul( (float3x3)g_world, Tan );

	// then compute Side vector from view and tangent (in world space)
	float3 ViewVec = g_eyePos.xyz - newPos;
	float3 nViewVec = normalize( ViewVec );

	// remember Pos.w is a radius value on either side of the spline of the hair strand.
	// so it is either pos or neg.
	float3 Side = normalize(cross( ViewVec, WTan )) * sign( Pos.w );
//	float radius = abs( Pos.w );

#ifdef USE_SUBPIXEL_ATTENUATION
	float dist = radius / ProjPlane;                  //project radius to screen

	float2 pix = mul( g_proj, float4(1,1,0,0) ).xy;
	float PixelSize = length( g_InvScreenSize / pix );
	if( ProjPlane > 0 && dist < PixelSize )         //test if within limit
	{
		radius = PixelSize * ProjPlane;
		SubPixel = saturate(1 - pow( abs((PixelSize - dist) / PixelSize), g_SubPixelPower ));
//		SubPixel = saturate(1 - ((PixelSize - dist) / PixelSize));
	}
#endif

	WPos = newPos + (Side * radius);
}
*/
float HairLimitExpand( in float3 Pos, in float3 Tan, in float Radius, out float3 Offset )
{
	float SubPixel = 1.0f;

	float3 ViewVec = g_eyePos.xyz - Pos;

	float3 Side = normalize(cross( ViewVec, Tan ));

#ifdef USE_SUBPIXEL_ATTENUATION
	float2 pix = mul( g_proj, float4(1,1,0,0) ).xy;
	float ProjPlane = mul( g_vp, float4( Pos, 1 ) ).w;   //Plane to flatten ribbon on
	float dist = Radius / ProjPlane;                  //project radius to screen
	float PixelSize = length( g_InvScreenSize / pix );
	if( ProjPlane > 0 && dist < PixelSize )         //test if within limit
	{
		radius = PixelSize * ProjPlane;
		SubPixel = saturate(1 - pow( abs((PixelSize - dist) / PixelSize), g_SubPixelPower ));
		//		SubPixel = saturate(1 - ((PixelSize - dist) / PixelSize));
	}
#endif

	Offset = Side;

	return SubPixel;
}

void ClumpDisplace( in float Instance, in float3 Normal, inout float3 Pos )
{
	float2 Off = random2[ (uint)(Instance*64) ] * g_ClumpRadius;

	float3 U = cross( Normal, float3( 0, 1, 0) );
	float3 V = cross( U, Normal );
	Pos += normalize(U) * Off.x;
	Pos += normalize(V) * Off.y;
}



//--------------pixel shader functions--------------------------

SamplerState HairSampler
{
	FILTER = MIN_MAG_LINEAR_MIP_POINT;
	AddressU = Border;
	AddressV = Border;
	AddressW = Border;
};

float SampleOpacityShadowTexture3D( in ProjLightInfo projLight, in float depth, in float4 ProjTexCoord, in float densityScale, in float i_depthBias )
{
	int c = 0;
	float density = 0;

	uint3 dims;
	OpacityShadowTexture3D.GetDimensions( dims.x, dims.y, dims.z );

	//calculate delta Z
//	float DZ = (g_ZFar - g_ZNear) / (float)dims.z;
	float DZ = g_ZFar - g_ZNear;
//	float d = g_ZNear;
//	d += i_depthBias;

	float3 TexCoord = ProjTexCoord.xyz / ProjTexCoord.w;
	TexCoord.z = (depth - g_ZNear) / DZ;	//map depth with near/far range

	density = OpacityShadowTexture3D.SampleLevel( HairSampler, TexCoord, 0).r;

	return exp( densityScale * density * -6.0f );
}

//4 layer opacity map with option for deep opacity map using depth map
float SampleOpacityShadowMap4( in ProjLightInfo projLight, in float depth, in float4 ProjTexCoord, in float densityScale, in float i_depthBias )
{
	int c = 0;
	float density = 0;

	//calculate delta Z
	float DZ = (g_ZFar - g_ZNear) / 4;
	float d = g_ZNear;

	if( g_HasDepthMap )
	{
//		float LNear = projLight.NearFar.x;
//		float LFar = projLight.NearFar.y;
//		d = (tex2Dproj( DepthSampler, ProjTexCoord ).r * (LFar - LNear)) + LNear;	//rescale to actual depth;
//		d = (tex2Dproj( DepthSampler, ProjTexCoord ).r * (g_ZFar - g_ZNear)) + g_ZNear;	//rescale to actual depth;
		d = (depthMap.SampleLevel( HairSampler, ProjTexCoord.xy/ProjTexCoord.w, 0 ).r * (g_ZFar - g_ZNear)) + g_ZNear;	//rescale to actual depth;
	}
	d += i_depthBias;

	//acquire opacity   
//	float4 O = tex2Dproj( OSMSampler, ProjTexCoord);
	float4 O = projOSM.SampleLevel( HairSampler, ProjTexCoord.xy/ProjTexCoord.w,0);
	float4 D = {d+DZ, d+DZ*2, d+DZ*3, d+DZ*4};
	float4 W = saturate( (depth - D) / DZ );
	density += dot( O, W );

	return exp( densityScale * density * -6.0f );
}

//16 layer opacity shadow map with optional deep opacity map using depth map
float SampleOpacityShadowMap16( in ProjLightInfo projLight, in float depth,
							   in float4 ProjTexCoord, in float densityScale, in float i_depthBias )
{
	int c = 0;
	float density = 0;

	//calculate delta Z
	float DZ = (g_ZFar - g_ZNear) / 16;
	float d = g_ZNear;

	if( g_HasDepthMap )
	{
//		float LNear = projLight.NearFar.x;
//		float LFar = projLight.NearFar.y;
//		d = (tex2Dproj( DepthSampler, ProjTexCoord ).r * (LFar - LNear)) + LNear;	//rescale to actual depth;
//		d = (tex2Dproj( DepthSampler, ProjTexCoord ).r * (g_ZFar - g_ZNear)) + g_ZNear;	//rescale to actual depth;
		d = (depthMap.SampleLevel( HairSampler, ProjTexCoord.xy/ProjTexCoord.w, 0 ).r * (g_ZFar - g_ZNear)) + g_ZNear;	//rescale to actual depth;
	}
	d += i_depthBias;

	//acquire opacity   
//	float4 O1 = tex2Dproj( OSMSampler, ProjTexCoord);
//	float4 O2 = tex2Dproj( OSMSampler2, ProjTexCoord);
//	float4 O3 = tex2Dproj( OSMSampler3, ProjTexCoord);
//	float4 O4 = tex2Dproj( OSMSampler4, ProjTexCoord);
	float4 O1 = projOSM.SampleLevel( HairSampler, ProjTexCoord.xy/ProjTexCoord.w, 0);
	float4 O2 = projOSM2.SampleLevel( HairSampler, ProjTexCoord.xy/ProjTexCoord.w, 0);
	float4 O3 = projOSM3.SampleLevel( HairSampler, ProjTexCoord.xy/ProjTexCoord.w, 0);
	float4 O4 = projOSM4.SampleLevel( HairSampler, ProjTexCoord.xy/ProjTexCoord.w, 0);
	float4 D1 = {d+DZ  , d+DZ*2, d+DZ*3, d+DZ*4};
	float4 D2 = {d+DZ*5, d+DZ*6, d+DZ*7, d+DZ*8};
	float4 D3 = {d+DZ*9, d+DZ*10, d+DZ*11, d+DZ*12};
	float4 D4 = {d+DZ*13, d+DZ*14, d+DZ*15, d+DZ*16};
	float4 W1 = saturate( (depth - D1) / DZ );
	float4 W2 = saturate( (depth - D2) / DZ );
	float4 W3 = saturate( (depth - D3) / DZ );
	float4 W4 = saturate( (depth - D4) / DZ );
	density += dot( O1, W1 );
	density += dot( O2, W2 );
	density += dot( O3, W3 );
	density += dot( O4, W4 );

	return exp( densityScale * density * -6.0f/4.0f );
}

//32 layer opacity shadow map with optional deep opacity map using depth map
float SampleOpacityShadowMap32( in ProjLightInfo projLight, in float depth,
							   in float4 ProjTexCoord, in float densityScale, in float i_depthBias )
{
	int c = 0;
	float density = 0;

	//calculate delta Z
	float DZ = (g_ZFar - g_ZNear) / 32;
	float d = g_ZNear;

	if( g_HasDepthMap )
	{
		//		float LNear = projLight.NearFar.x;
		//		float LFar = projLight.NearFar.y;
		//		d = (tex2Dproj( DepthSampler, ProjTexCoord ).r * (LFar - LNear)) + LNear;	//rescale to actual depth;
		//		d = (tex2Dproj( DepthSampler, ProjTexCoord ).r * (g_ZFar - g_ZNear)) + g_ZNear;	//rescale to actual depth;
		d = (depthMap.SampleLevel( HairSampler, ProjTexCoord.xy/ProjTexCoord.w, 0 ).r * (g_ZFar - g_ZNear)) + g_ZNear;	//rescale to actual depth;
	}
	d += i_depthBias;

	//acquire opacity   
	//	float4 O1 = tex2Dproj( OSMSampler, ProjTexCoord);
	//	float4 O2 = tex2Dproj( OSMSampler2, ProjTexCoord);
	//	float4 O3 = tex2Dproj( OSMSampler3, ProjTexCoord);
	//	float4 O4 = tex2Dproj( OSMSampler4, ProjTexCoord);
	float4 O1 = projOSM.SampleLevel( HairSampler, ProjTexCoord.xy/ProjTexCoord.w, 0);
	float4 O2 = projOSM2.SampleLevel( HairSampler, ProjTexCoord.xy/ProjTexCoord.w, 0);
	float4 O3 = projOSM3.SampleLevel( HairSampler, ProjTexCoord.xy/ProjTexCoord.w, 0);
	float4 O4 = projOSM4.SampleLevel( HairSampler, ProjTexCoord.xy/ProjTexCoord.w, 0);
	float4 O5 = projOSM5.SampleLevel( HairSampler, ProjTexCoord.xy/ProjTexCoord.w, 0);
	float4 O6 = projOSM6.SampleLevel( HairSampler, ProjTexCoord.xy/ProjTexCoord.w, 0);
	float4 O7 = projOSM7.SampleLevel( HairSampler, ProjTexCoord.xy/ProjTexCoord.w, 0);
	float4 O8 = projOSM8.SampleLevel( HairSampler, ProjTexCoord.xy/ProjTexCoord.w, 0);
	float4 D1 = {d+DZ  , d+DZ*2, d+DZ*3, d+DZ*4};
	float4 D2 = {d+DZ*5, d+DZ*6, d+DZ*7, d+DZ*8};
	float4 D3 = {d+DZ*9, d+DZ*10, d+DZ*11, d+DZ*12};
	float4 D4 = {d+DZ*13, d+DZ*14, d+DZ*15, d+DZ*16};
	float4 W1 = saturate( (depth - D1) / DZ );
	float4 W2 = saturate( (depth - D2) / DZ );
	float4 W3 = saturate( (depth - D3) / DZ );
	float4 W4 = saturate( (depth - D4) / DZ );
	density += dot( O1, W1 );
	density += dot( O2, W2 );
	density += dot( O3, W3 );
	density += dot( O4, W4 );

	return exp( densityScale * density * -6.0f/4.0f );
}

//--------------------------------------------------------------------
//	IlluminateProjLightHair() - projected light with shadow
//--------------------------------------------------------------------
void IlluminateProjLightHair(in float3 Ps, in LightInfo light, in ProjLightInfo projLight, 
							 in bool bHasProjTex, in Texture2D ProjTextureMap,
							 in float densityScale, in float depthBias,
							 out IncidentLight OUT)
{
	OUT.L = projLight.Pos.xyz - Ps;
	if (light.Pos.w == 0) // directional projected
		OUT.L = light.ConeInfo.xyz;

	OUT.Cld = OUT.Cls = float3(0,0,0);
	float4 projTexCoord = mul(projLight.Matrix, float4(Ps,1));

	float depth = dot( g_LightViewPlane, float4( Ps, 1));
	if( depth > g_ZNear )	//only in front of near plane
	{
		float4 tex_col = 1;
		if( bHasProjTex )
		{
			tex_col = ProjTextureMap.SampleLevel(projSampler, projTexCoord.xy/projTexCoord.w, 0);
		}

		float2 uv = projTexCoord.xy/projTexCoord.w;

		if (projLight.InnerAngle <= projLight.OuterAngle)
		{
			float p = EllipticalPenumbra(projLight.InnerAngle, 
				projLight.OuterAngle, projLight.Aspect, uv);
			tex_col *= p;
		}
		else
		{
			uv = step(0, uv) * step(uv, 1);
			tex_col *= uv.x * uv.y;
		}

		float d = length(OUT.L);
		float atten = 1 / (light.Falloff.x + light.Falloff.y * d + light.Falloff.z * d * d);

		float3 shadowCoeff = 1;

		if( g_HasOpacityShadowTexture3D )
		{
			shadowCoeff = SampleOpacityShadowTexture3D( projLight, depth, projTexCoord, densityScale, depthBias );
		}
		else if( g_bHasOSMMulti32 )
		{
			shadowCoeff = SampleOpacityShadowMap32( projLight, depth, projTexCoord, densityScale, depthBias );
		}
		else if( g_bHasOSMMulti )
		{
			shadowCoeff = SampleOpacityShadowMap16( projLight, depth, projTexCoord, densityScale, depthBias );
		}
		else if( g_bHasOSM )
		{
			shadowCoeff = SampleOpacityShadowMap4( projLight, depth, projTexCoord, densityScale, depthBias );
		}

		// multiply texture color with shadow color
		float3 modifier = projLight.ShadowColor.rgb * atten * tex_col.rgb;

		OUT.Cld = lerp( modifier, atten * tex_col.rgb * light.Diffuse.rgb, shadowCoeff.rgb);
		OUT.Cls = lerp( modifier, atten * tex_col.rgb * light.Specular.rgb, shadowCoeff.rgb);
	}
}

///--------------------Tessellation functions--------------------------------------
/*
SamplerState hairDataSampler
{
	AddressU = WRAP;		//wrap to allow u to index the proper column
	AddressV = CLAMP;
	FILTER = MIN_MAG_MIP_POINT;
};

float4 SampleHairData( int index )
{
	float2 coord;
	coord.x = index * g_HairDataTextureSize.z;
	coord.y = floor( coord.x+(g_HairDataTextureSize.z*0.5) ) * g_HairDataTextureSize.w;
	coord += g_HairDataTextureSize.zw*0.5;//shift by half texel

	return hairDataTexture.SampleLevel( hairDataSampler, coord, 0 );
}
*/
void CatmullRomSpline( in float3 P0, in float3 P1, in float3 P2, in float3 P3, in float s, out float3 Pos, out float3 Tan )
{
	float s2 = s*s;
	float s3 = s2*s;

	Pos = (-0.5*s3 + s2 - 0.5*s)*P0 + (1.5*s3 - 2.5*s2 + 1)*P1 + (-1.5*s3 + 2*s2 + 0.5*s)*P2 + (0.5*s3 - 0.5*s2)*P3;
	Tan = (-1.5*s2 + 2*s - 0.5)*P0 + (4.5*s2 - 5*s)*P1 + (-4.5*s2 + 4*s + 0.5)*P2 + (1.5*s2 - s)*P3;
}

/*
float4 GetHairQuadVertexLinear( hairVertexTess In )
{
	// Prepare interpolation factors: u*v, (1-u)*v, (1-u)*(1-v), u*(1-v)
	float4 fInterpolationFactors = In.vCartesianCoordinates.xzzx * In.vCartesianCoordinates.yyww;

	//quadlinear interpolate
	return In.Position0 * fInterpolationFactors.x + In.Position1 * fInterpolationFactors.y + In.Position2 * fInterpolationFactors.z + In.Position3 * fInterpolationFactors.w;
}
*/
//--------------------------------------------------------------------------------------
// Calculate parametric coordinates between 0..1 for patch evalution	
//
// Because the tessellation HW implementation uses parametric coordinates between 0 and 0.5 (instead of 0 to 1)
// the order of the superprimitives is switched when parametric coordinates cross the patch's centre.
// For this reason we *must* recalculate our own patch parametric coordinates UVs using cartesian interpolation
// before being able to use them in patch evaluation.

// The patch UVs are defined as such for the four vertices making up the superprimitive:
//
// (0,0)       (1,0)
//      V0---V3
//      |    |
//      |    |
//      V1---V2
// (0,1)       (1,1)
//
// vInputWeights is the vertex input declared as BLENDWEIGHT0 (the input coordinates from the HW tessellation unit)
//--------------------------------------------------------------------------------------
/*
float4 CalculateQuadParametricCoordinates(float4 vInputWeights, uint4 vIndices)
{
	float4 vParametricCoordinates;
	const float2 f2_table[4] = { float2(0,0), float2(0,1), float2(1,1), float2(1,0) };

	// Patch vertex index is the index number modulo 4 i.e. 0, 1, 2 or 3.
	int4 vIndicesModulo4 = vIndices % 4;

	// Base weights to interpolate depend on index value (hence use of a small lookup table)

	float2 W0 = f2_table[vIndicesModulo4.x];
	float2 W1 = f2_table[vIndicesModulo4.y];
	float2 W2 = f2_table[vIndicesModulo4.z];
	float2 W3 = f2_table[vIndicesModulo4.w];

	float2 BaseWeight_V0;
	float2 BaseWeight_V1;
	float2 BaseWeight_V2;
	float2 BaseWeight_V3;
	BaseWeight_V0 = W3;	//works for even
	BaseWeight_V1 = W0;
	BaseWeight_V2 = W1;
	BaseWeight_V3 = W2;

	// Use Cartesian interpolation to calculate our parametric coordinates for patch evaluation
	float2 UV1 = (BaseWeight_V0 * vInputWeights.x + BaseWeight_V1 * vInputWeights.z);
	float2 UV2 = (BaseWeight_V3 * vInputWeights.x + BaseWeight_V2 * vInputWeights.z);
	vParametricCoordinates.xy =	UV1 * vInputWeights.y + UV2 * vInputWeights.w;

	// Convenience variables: z and w contain (1-u) and (1-v) respectively
	vParametricCoordinates.zw = 1.0 - vParametricCoordinates.xy;

	return vParametricCoordinates;
}

void TessellateHair(hairVertexTess In, out hairVertex Out )
{
	Out.Color = float4( 1, 1, 1, 1 );
	Out.Material = float4( 1, 1, 1, 1 );
	Out.Interp = 1;
	float4 P = GetHairQuadVertexLinear( In );

	//lookup control points
	uint ID = In.vIndices.x;
	uint ID2 = (uint)((int)ID-2);
	uint Quad;
	float4 UV;
	if( g_bDrawOddTessellated )
	{
		Quad = ((ID2/4)*2)+1;
	}
	else
	{
		Quad = (ID/4)*2;
	}
	UV = CalculateQuadParametricCoordinates( In.vCartesianCoordinates, In.vIndices );

	int Strand = Quad / g_nStrandCPs;
	int Segment = modulo( Quad, g_nStrandCPs );
	bool Degenerate = (Segment == (g_nStrandCPs-1));

	int4 ControlPoints = clamp( int4( Segment-1, Segment, Segment+1, Segment+2 ), 0, g_nStrandCPs-1 );
	int StrandBase = Strand * (3+g_nStrandCPs);
	ControlPoints += StrandBase + 3;

	float4 Root = SampleHairData( StrandBase );
	float4 Tip = SampleHairData( StrandBase+1 );
	Out.Material = SampleHairData( StrandBase+2 );
	float3 P0 = SampleHairData( ControlPoints.x ).xyz;
	float3 P1 = SampleHairData( ControlPoints.y ).xyz;
	float3 P2 = SampleHairData( ControlPoints.z ).xyz;
	float3 P3 = SampleHairData( ControlPoints.w ).xyz;

	float StrandInterp = abs(P.w);
	float interp = saturate(StrandInterp);	//make sure we read from position (Tessellation BUG!)

	float4 Blend = lerp( Root, Tip, interp );

	float radius = Degenerate ? 0.0f : Blend.w;	//collapse the degenerate quads
	float Offset = ((UV.y * 2) - 1) * radius;

	Out.Interp.x = interp;
	Out.Interp.y = radius;

	float3 Position, Tangent;
	//use 1-U for odd since winding order is reversed
	CatmullRomSpline( P0, P1, P2, P3, g_bDrawOddTessellated ? UV.z : UV.x, Position, Out.Tangent );

	Out.Position = float4( Position, Offset );

	Out.Color.rgb = Blend.rgb;
}
*/

//---Returns the material that matches the specified Patch or Vertex ID-----
hairMaterial GetMaterial( in uint PatchID )
{
	uint curStrand = PatchID / g_nStrandCPs;
	return g_HairMaterials[ curStrand ];
}

//  Calculates the position and tangent of the provided control points of the PatchID at the UV
//  U is the segment interpolant, V is the Clump instance interpolant
//  returns the parametric value from 0 - 1 over the hair strand
float InterpolateHairStrand( in const OutputPatch<hairOutput, 4> CP, in uint PatchID, in float2 UV,
							out float3 WPos, out float3 WTan )
{
	CatmullRomSpline( CP[0].Position.xyz, CP[1].Position.xyz, CP[2].Position.xyz, CP[3].Position.xyz, UV.x, WPos, WTan );

	hairMaterial Mtl = GetMaterial( PatchID );
	ClumpDisplace( UV.y, Mtl.m_SurfaceNormal, WPos );

	uint curSegment = PatchID % g_nStrandCPs;
	return (curSegment + UV.x) / (float)(g_nStrandCPs-1);
}

//--------------------------------------------------------------------------------------
// Hull shader
//--------------------------------------------------------------------------------------
struct HS_HAIR_CONSTANT_DATA_OUTPUT
{
	float    Edges[2]         : SV_TessFactor;
};

HS_HAIR_CONSTANT_DATA_OUTPUT Hair_Constants_HS( uint PatchID : SV_PrimitiveID )
{
	HS_HAIR_CONSTANT_DATA_OUTPUT output = (HS_HAIR_CONSTANT_DATA_OUTPUT)0;

	// Assign tessellation levels
	ProcessIsolineTessFactors( g_HairTessellationValue.y, g_HairTessellationValue.x, output.Edges[0], output.Edges[1] );

	if( (PatchID % g_nStrandCPs) >= (uint)(g_nStrandCPs-1) ) output.Edges[0] = -1;	//cull out degenerate segments

	return output;
}

[domain("isoline")]
[partitioning("fractional_odd")]
[outputtopology("line")]
[outputcontrolpoints(4)]
[patchconstantfunc("Hair_Constants_HS")]
hairOutput Hair_HS( uint uCPID : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID )
{
	hairOutput    output = (hairOutput)0;

	uint nStrands = g_nStrandCPs-1;
	uint curSegment = PatchID % g_nStrandCPs;
	uint curStrand = PatchID / g_nStrandCPs;

	//calculate hair vertex indices
	uint strandBaseID = curStrand * g_nStrandCPs;
	uint controlPointBase = strandBaseID + curSegment;

	//	uint ID = clamp( controlPointBase-1, strandBaseID, strandBaseID + nStrands );
	//	output.Position = mul( g_world, float4(g_HairGeometry[ ID ], 0 ));

	switch( uCPID )
	{
	case 0:
		{
			uint ID = max( strandBaseID, controlPointBase-1 );
			if( controlPointBase == 0 ) ID = 0;
			output.Position = mul( g_world, float4(g_HairGeometry[ ID ], 1 ));
			break;
		}
	case 1:
		{
			uint ID = controlPointBase;
			output.Position = mul( g_world, float4(g_HairGeometry[ ID ], 1 ));
			break;
		}
	case 2:
		{
			uint ID = controlPointBase+1;
			output.Position = mul( g_world, float4(g_HairGeometry[ ID ], 1 ));
			break;
		}
	case 3:
		{
			uint ID = min( strandBaseID + nStrands, controlPointBase+2 );
			output.Position = mul( g_world, float4(g_HairGeometry[ ID ], 1 ));
			break;
		}
	}

	return output;
}
