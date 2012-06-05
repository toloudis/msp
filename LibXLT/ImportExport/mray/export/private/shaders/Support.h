#include "shader.h"
#include "Arithmetic.h"
#include <string>

//--------------------------------------------------------------------
// For debugging
//--------------------------------------------------------------------
void Print( const char * i_Str );
void Print( const char * i_Str , const miScalar& i_Val );
void Print( const char * i_Str , const int& i_Val );
void Print( const char * i_Str , const miColor& i_Col );
void Print( const char * i_Str , const miVector& i_Vec );
void Print( const char * i_Str , const miMatrix& i_Mat );

struct sgpuLightData
{
	miBoolean AffectsDiffuse;
	miBoolean AffectsSpecular;
};
#define SGPU_LIGHT_DATA_NAME "LtData"

//--------------------------------------------------------------------
// NonZeroVal()
//--------------------------------------------------------------------
float NonZeroVal(float i_Val);

//--------------------------------------------------------------------
// clamp()
//--------------------------------------------------------------------
float clamp(const float& x, const float& a, const float& b);

//--------------------------------------------------------------------
// length()
//--------------------------------------------------------------------
float length( const miVector& a );

//--------------------------------------------------------------------
// GetTransformedUVs()
//--------------------------------------------------------------------
miVector GetTransformedUVs( miState * state,
						    const miScalar& u_scale,
							const miScalar& v_scale,
							const miScalar& u_offset,
							const miScalar& v_offset,
							const miScalar& uv_rotation );
//--------------------------------------------------------------------
// step()
//--------------------------------------------------------------------
float step( const float& a, const float& x);

//--------------------------------------------------------------------
// step()
//--------------------------------------------------------------------
miColor step( const miColor& a, const miColor& b);

//--------------------------------------------------------------------
// step()
//--------------------------------------------------------------------
miColor step( const miColor& a, float x);


//--------------------------------------------------------------------
// step()
//--------------------------------------------------------------------
miVector step( const miVector& a, float x );

//--------------------------------------------------------------------
// step()
//--------------------------------------------------------------------
miVector step( float x, const miVector& a );

//--------------------------------------------------------------------
// smoothstep()
//--------------------------------------------------------------------
float smoothstep( const float& a, const float& b, const float& c);

//--------------------------------------------------------------------
/// Returns 0 if \p c is less than \p a and 1 if \p c is greater than \p b
/// in an elementwise fashion.
/// A smooth curve is applied in-between so that the return values vary
/// continuously from 0 to 1 as elements in \p c vary from \p a to \p b.
//--------------------------------------------------------------------
miColor smoothstep( const miColor& a, const miColor& b, const miColor& c);

//--------------------------------------------------------------------
/// Returns 0 if \p c is less than \p a and 1 if \p c is greater than \p b
/// in an elementwise fashion.
/// A smooth curve is applied in-between so that the return values vary
/// continuously from 0 to 1 as \p x varies from \p a to \p b.
//--------------------------------------------------------------------
miColor smoothstep( const miColor& a, const miColor& b, float x);

//--------------------------------------------------------------------
// lerp()
//--------------------------------------------------------------------
float lerp( float x, float y, float s );

//--------------------------------------------------------------------
/// Returns the elementwise linear interpolation between \p c1 and \c c2,
/// i.e., it returns <tt>(1-t) * c1 + t * c2</tt>.
//--------------------------------------------------------------------
miColor lerp(
    const miColor& c1,  ///< one color
    const miColor& c2,  ///< second color
    const miColor& t);   ///< interpolation parameter in [0,1]

//--------------------------------------------------------------------
/// Returns the linear interpolation between \p c1 and \c c2,
/// i.e., it returns <tt>(1-t) * c1 + t * c2</tt>.
//--------------------------------------------------------------------
miColor lerp(
    const miColor& c1,  ///< one color
    const miColor& c2,  ///< second color
    float      t);   ///< interpolation parameter in [0,1]

//--------------------------------------------------------------------
/// Returns the linear interpolation between \p v1 and \c v2,
/// i.e., it returns <tt>(1-t) * v1 + t * v2</tt>.
//--------------------------------------------------------------------
miVector lerp(
    const miVector& v1,  ///< one vector
    const miVector& v2,  ///< second vector
    float      t);   ///< interpolation parameter in [0,1]

//--------------------------------------------------------------------
// dot()
//--------------------------------------------------------------------
float dot(const miVector& a, const miVector& b);

//--------------------------------------------------------------------
// expand()
//--------------------------------------------------------------------
miVector expand(const miVector& v);

//--------------------------------------------------------------------
// saturate()
//--------------------------------------------------------------------
float saturate(const float& in);

//--------------------------------------------------------------------
// saturate()
//--------------------------------------------------------------------
miColor saturate(const miColor& in);

//--------------------------------------------------------------------
// saturate()
//--------------------------------------------------------------------
miVector saturate(const miVector& in);

//--------------------------------------------------------------------
// max()
//--------------------------------------------------------------------
float max(const float& a, const float& b);

//--------------------------------------------------------------------
// normalize()
//--------------------------------------------------------------------
miVector normalize(const miVector& in);

//--------------------------------------------------------------------
// fastFresnel()
//--------------------------------------------------------------------
float fastFresnel(const float& NdotE, const float& R0, const float& power);

//--------------------------------------------------------------------
// fresnel()
//--------------------------------------------------------------------
float fresnel(const float& NdotE, const float& eta);

//--------------------------------------------------------------------
// PhongDiffuse()
//--------------------------------------------------------------------
miColor PhongDiffuse(const miVector& normal, const miVector& lightDir, const miColor& lDiffColor, const miColor& sDiffColor);

//--------------------------------------------------------------------
// PhongSpecular()
//--------------------------------------------------------------------
miColor PhongSpecular(const miVector& normal, const miVector& lightDir, const miVector& eyeDir, 
					  const miColor& lSpecColor, const miColor& sSpecColor, const float& sSpecPower);

//--------------------------------------------------------------------
// BlinnSpecular()
//--------------------------------------------------------------------
miColor BlinnSpecular(miState* state, const miVector& normal, const miVector& lightDir, const miVector& eyeDir, 
	const miColor& lSpecColor, const miColor& sSpecColor, float eccentricity, float IOR,
	float specularBias, float specularPower);

//--------------------------------------------------------------------
// rotateAboutY()
//--------------------------------------------------------------------
miVector rotateAboutY(const miVector& vec, const float& angle);

//--------------------------------------------------------------------
// CartesianToPolar()
//--------------------------------------------------------------------
miVector CartesianToPolar( const miVector& vec );

//--------------------------------------------------------------------
// SampleTexture()
//--------------------------------------------------------------------
miColor SampleTexture( miState* i_State, const miTag& i_TextureTag, miVector * i_Coords, bool i_bIsFilter = true );

//--------------------------------------------------------------------
// Tex2d()
//--------------------------------------------------------------------
miColor Tex2d( miState* i_State, const miTag& i_TextureTag, miVector sstt, bool i_bIsFilter = true);

//--------------------------------------------------------------------
// Tex2dCombine()
//--------------------------------------------------------------------
miColor Tex2dCombine( miState* i_State, const miTag& i_TextureTag, const miColor& i_Color, miVector sstt);

//--------------------------------------------------------------------
// Tex2DNormal()
//--------------------------------------------------------------------
miVector Tex2dNormal(miState* i_State, const miTag& i_TextureTag, miVector sstt);

//--------------------------------------------------------------------
// Tex2dReplace()
//--------------------------------------------------------------------
miColor Tex2dReplace( miState* i_State, const miTag& i_TextureTag, const miColor& i_Color, miVector sstt);

//--------------------------------------------------------------------
// SampleEnvironmentLOD()
//--------------------------------------------------------------------
miColor SampleEnvironmentLOD(miState* state, const miVector& worldEyeDir, const miVector& worldNormal,
							const miTag& CubeMap, const float& angle, const float& lod);

//--------------------------------------------------------------------
// SampleEnvironment()
//--------------------------------------------------------------------
miColor SampleEnvironment(miState* state, const miVector& worldEyeDir, const miVector& worldNormal,
						  const miTag& CubeMap, const float& angle);

//--------------------------------------------------------------------
// SampleEnvDiffuse()
//--------------------------------------------------------------------
miColor SampleEnvDiffuse(miState* state, const miVector& worldNormal, const miTag& CubeMap, const float& angle);

//--------------------------------------------------------------------
// faceforward()
//--------------------------------------------------------------------
miVector faceforward( const miVector& N, const miVector& I, const miVector& Ng );

//--------------------------------------------------------------------
// reflect()
//--------------------------------------------------------------------
miVector reflect(const miVector& I , const miVector& N);

//--------------------------------------------------------------------
// refract()
//--------------------------------------------------------------------
miVector refract( const miVector& I, const miVector& N, const float& eta );

//--------------------------------------------------------------------
// GetGlossyReflection()
//--------------------------------------------------------------------
miColor GetGlossyReflection(miState *state, miScalar shiny, miUint samples);

//--------------------------------------------------------------------
// GetGlossyRefraction()
//--------------------------------------------------------------------
miColor GetGlossyRefraction(miState *state, miScalar shiny, miUint samples, miScalar ior);

//--------------------------------------------------------------------
// GetReflection()
//--------------------------------------------------------------------
miColor GetReflection(miState* state, miVector sstt, const miVector& bumpNormal,
					  const miVector& worldNormal, const miVector& WorldEyeDir, 
					  const float& bumpScale, const float& refrFactor, const float& reflFactor,
					  const miTag& reflectFactorSampler,
					  const float& fresnelBias, const float& fresnelPower, const float& IOR, const float& refrLOD,
					  const float& reflLOD, const bool& reflEnable, const float& reflSamples);

//--------------------------------------------------------------------
// GetEnvironment()
//--------------------------------------------------------------------
miColor GetEnvironment(miState* state);

//--------------------------------------------------------------------
// OrenNayarDiffuse()
//--------------------------------------------------------------------
float OrenNayarDiffuse(const miVector& L, const miVector& I, const miVector& N, const float& roughness);

//--------------------------------------------------------------------
// GetBumpNormal()
//--------------------------------------------------------------------
miVector GetBumpNormal(miState * state, const miTag& normalMap, const float& normalMapScale, miVector sstt);


//------------------------------------------------------------------------
// ContainsPoint()
//------------------------------------------------------------------------
float ContainsPoint(miVector i_Point,
	 			    float bbox_min_x, float bbox_min_y, float bbox_min_z,
				    float bbox_max_x, float bbox_max_y, float bbox_max_z );

//------------------------------------------------------------------------
// miao_get_label()
//------------------------------------------------------------------------
static int miao_get_label(miTag instance);

//------------------------------------------------------------------------
// miao_trace_the_ray()
//------------------------------------------------------------------------
static miBoolean    miao_trace_the_ray(
        miState    *state,
        miVector   *dir,
        miVector   *point,
        miao_trace_info *ti);

//------------------------------------------------------------------------
// GetAO()
//------------------------------------------------------------------------
miBoolean GetAO( miColor *result, 
				 miState *state, 
				 bool		bEnableAO, 
				 miColor	aoColor,
				 miScalar	aoRadiusNear,
				 miScalar	aoRadiusFar,
				 miScalar	aoAngleBias,
				 miScalar	aoAttenuation,
				 miScalar	aoContrast,
				 miScalar	aoCamNear,
				 miScalar	aoCamFar,
				 miScalar	aoSamples
				 );

//------------------------------------------------------------------------
// GetGI()
//------------------------------------------------------------------------
void GetGI( miColor *result, 
		    miState *state, 
		    bool	bEnableGI
			);

//--------------------------------------------------------------------
//	attenuation()
//--------------------------------------------------------------------
float attenuation(float d,		// Position of vertex in world coords
				  float falloffX,
				  float falloffY,
				  float falloffZ,
				  float falloffW,
				  float falloffStart);

//------------------------------------------------------------------------
// GetBufferIdx()
//------------------------------------------------------------------------
size_t GetBufferIdx(miState *state, const char* i_BufferName);

//------------------------------------------------------------------------
// PutRefl()
//------------------------------------------------------------------------
void PutRefl(miColor *result, 
			 miState *state, 
			 bool	bEnableRefl
			 );

//------------------------------------------------------------------------
// GetScaledIBL()
//------------------------------------------------------------------------
miColor GetScaledIBL(const miColor& in);