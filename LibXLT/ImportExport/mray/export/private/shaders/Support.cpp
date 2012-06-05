#include "shader.h"
#include "geoshader.h"
#include "Arithmetic.h"
#include "mi/math.h"
#include <string>

//--------------------------------------------------------------------
// For debugging
//--------------------------------------------------------------------
void Print( const char * i_Str )
{
	printf("%s\n\n", i_Str);
}
void Print( const char * i_Str , const miScalar& i_Val )
{
	printf("%s = %f\n\n", i_Str, i_Val);
}
void Print( const char * i_Str , const int& i_Val )
{
	printf("%s = %d\n\n", i_Str, i_Val);
}
void Print( const char * i_Str , const miColor& i_Col )
{
	printf("%s = ( %f, %f, %f, %f )\n\n", i_Str, i_Col.r, i_Col.g, i_Col.b, i_Col.a);
}
void Print( const char * i_Str , const miVector& i_Vec )
{
	printf("%s = < %f, %f, %f >\n\n", i_Str, i_Vec.x, i_Vec.y, i_Vec.z);
}
void Print( const char * i_Str , const miMatrix& i_Mat )
{
	printf("%s = [\n", i_Str);
	for ( int i = 0 ; i < 16 ; i+=4 )
	{
		printf("%f, ", i_Mat[i]);
		printf("%f, ", i_Mat[i+1]);
		printf("%f, ", i_Mat[i+2]);
		printf("%f, ", i_Mat[i+3]);
		printf("\n");
	}
	printf("]\n\n");
}

//--------------------------------------------------------------------
// NonZeroVal()
//--------------------------------------------------------------------
float NonZeroVal(float i_Val)
{
	if (i_Val == 0) return NON_ZERO;
	else return i_Val;
}

//--------------------------------------------------------------------
// clamp()
//--------------------------------------------------------------------
float clamp(const float& x, const float& a, const float& b)
{
	if ( x < a ) return a;
	if ( x > b ) return b;
	return x;
}

//--------------------------------------------------------------------
// length()
//--------------------------------------------------------------------
float length( const miVector& a )
{
	return sqrt( a.x*a.x + a.y*a.y + a.z*a.z );
}

//--------------------------------------------------------------------
// GetTransformedUVs()
//--------------------------------------------------------------------
miVector GetTransformedUVs( miState * state,
						    const miScalar& u_scale,
							const miScalar& v_scale,
							const miScalar& u_offset,
							const miScalar& v_offset,
							const miScalar& uv_rotation )
{
	miVector sstt = state->tex_list[0];

	float c = cos(uv_rotation);
	float s = sin(uv_rotation);
	// scale matrix * rot matrix * trans matrix.
	float u = (sstt.x * c * u_scale)	+ ((1-sstt.y) * (-s) * u_scale)	+ u_offset;
	float v = (sstt.x * s * v_scale)	+ ((1-sstt.y) * c * v_scale)	+ v_offset;
	sstt.x = fmod(u, 1.0f);
	sstt.y = fmod(1.0f-v, 1.0f);

//
//	sstt.x = fmod( (float)((sstt.x * u_scale) + u_offset), 1.0f);
//	sstt.y = fmod( (float)((sstt.y * v_scale) + v_offset), 1.0f);
//	miMatrix uvRotationMat;
//	mi_matrix_rotate_axis(uvRotationMat,&make_vector(0,0,1),-uv_rotation);
//	mi_vector_transform( &sstt, &sstt, uvRotationMat );
	return sstt;
}

//--------------------------------------------------------------------
// step()
//--------------------------------------------------------------------
float step( const float& a, const float& x)
{
	return mi::math::step(a, x);
}

//--------------------------------------------------------------------
// step()
//--------------------------------------------------------------------
miColor step( const miColor& a, const miColor& b)
{
	return make_color( mi::math::step(a.r, b.r),
					   mi::math::step(a.g, b.g),
					   mi::math::step(a.b, b.b),
					   mi::math::step(a.a, b.a));
}

//--------------------------------------------------------------------
// step()
//--------------------------------------------------------------------
miColor step( const miColor& a, float x )
{
    return make_color( mi::math::step(a.r, x),
					   mi::math::step(a.g, x),
                       mi::math::step(a.b, x),
					   mi::math::step(a.a, x));
}

//--------------------------------------------------------------------
// step()
//--------------------------------------------------------------------
miVector step( const miVector& a, float x )
{
    return make_vector( mi::math::step(a.x, x),
					    mi::math::step(a.y, x),
                        mi::math::step(a.z, x));
}

//--------------------------------------------------------------------
// step()
//--------------------------------------------------------------------
miVector step( float x, const miVector& a )
{
    return make_vector( mi::math::step(x, a.x),
					    mi::math::step(x, a.y),
                        mi::math::step(x, a.z));
}

//--------------------------------------------------------------------
// smoothstep()
//--------------------------------------------------------------------
float smoothstep( const float& a, const float& b, const float& c)
{
	return mi::math::smoothstep(a, b, c);
}

//--------------------------------------------------------------------
/// Returns 0 if \p c is less than \p a and 1 if \p c is greater than \p b
/// in an elementwise fashion.
/// A smooth curve is applied in-between so that the return values vary
/// continuously from 0 to 1 as elements in \p c vary from \p a to \p b.
//--------------------------------------------------------------------
miColor smoothstep( const miColor& a, const miColor& b, const miColor& c)
{
	return make_color( mi::math::smoothstep(a.r, b.r, c.r),
                  mi::math::smoothstep(a.g, b.g, c.g),
                  mi::math::smoothstep(a.b, b.b, c.b),
                  mi::math::smoothstep(a.a, b.a, c.a));
}

//--------------------------------------------------------------------
/// Returns 0 if \p c is less than \p a and 1 if \p c is greater than \p b
/// in an elementwise fashion.
/// A smooth curve is applied in-between so that the return values vary
/// continuously from 0 to 1 as \p x varies from \p a to \p b.
//--------------------------------------------------------------------
miColor smoothstep( const miColor& a, const miColor& b, float x)
{
    return make_color( mi::math::smoothstep(a.r, b.r, x),
                  mi::math::smoothstep(a.g, b.g, x),
                  mi::math::smoothstep(a.b, b.b, x),
                  mi::math::smoothstep(a.a, b.a, x));
}

//--------------------------------------------------------------------
// lerp()
//--------------------------------------------------------------------
float lerp( float x, float y, float s )
{
	return x + s*(y-x);
}

//--------------------------------------------------------------------
/// Returns the elementwise linear interpolation between \p c1 and \c c2,
/// i.e., it returns <tt>(1-t) * c1 + t * c2</tt>.
//--------------------------------------------------------------------
miColor lerp(
    const miColor& c1,  ///< one color
    const miColor& c2,  ///< second color
    const miColor& t)   ///< interpolation parameter in [0,1]
{
    return make_color( mi::math::lerp( c1.r, c2.r, t.r),
                  mi::math::lerp( c1.g, c2.g, t.g),
                  mi::math::lerp( c1.b, c2.b, t.b),
                  mi::math::lerp( c1.a, c2.a, t.a));
}

//--------------------------------------------------------------------
/// Returns the linear interpolation between \p c1 and \c c2,
/// i.e., it returns <tt>(1-t) * c1 + t * c2</tt>.
//--------------------------------------------------------------------
miColor lerp(
    const miColor& c1,  ///< one color
    const miColor& c2,  ///< second color
    float      t)   ///< interpolation parameter in [0,1]
{
    // equivalent to: return c1 * (Float32(1)-t) + c2 * t;
    return make_color( mi::math::lerp( c1.r, c2.r, t),
                  mi::math::lerp( c1.g, c2.g, t),
                  mi::math::lerp( c1.b, c2.b, t),
                  mi::math::lerp( c1.a, c2.a, t));
}

//--------------------------------------------------------------------
/// Returns the linear interpolation between \p v1 and \c v2,
/// i.e., it returns <tt>(1-t) * v1 + t * v2</tt>.
//--------------------------------------------------------------------
miVector lerp(
    const miVector& v1,  ///< one vector
    const miVector& v2,  ///< second vector
    float      t)   ///< interpolation parameter in [0,1]
{
	// equivalent to: return c1 * (Float32(1)-t) + c2 * t;
	return make_vector( mi::math::lerp( v1.x, v2.x, t),
		mi::math::lerp( v1.y, v2.y, t),
		mi::math::lerp( v1.z, v2.z, t));
}

//--------------------------------------------------------------------
// dot()
//--------------------------------------------------------------------
float dot(const miVector& a, const miVector& b)
{
	return mi_vector_dot(&a,&b);
}

//--------------------------------------------------------------------
// expand()
//--------------------------------------------------------------------
miVector expand(const miVector& v)
{
	return 2.0*(v-0.5);
}

//--------------------------------------------------------------------
// saturate()
//--------------------------------------------------------------------
float saturate(const float& in)
{
	float retVal = in;
	if ( in < 0 ) retVal = 0;
	if ( in > 1 ) retVal = 1;	
	return retVal;
}

//--------------------------------------------------------------------
// saturate()
//--------------------------------------------------------------------
miColor saturate(const miColor& in)
{
	miColor out;
	out.r = saturate(in.r);
	out.g = saturate(in.g);
	out.b = saturate(in.b);
	out.a = saturate(in.a);
	
	return out;
}

//--------------------------------------------------------------------
// saturate()
//--------------------------------------------------------------------
miVector saturate(const miVector& in)
{
	miVector out;
	out.x = saturate(in.x);
	out.y = saturate(in.y);
	out.z = saturate(in.z);
	
	return out;
}

//--------------------------------------------------------------------
// max()
//--------------------------------------------------------------------
float max(const float& a, const float& b)
{
	if ( b > a ) return b;
	else return a;
}

//--------------------------------------------------------------------
// normalize()
//--------------------------------------------------------------------
miVector normalize(const miVector& in)
{	
	miVector out = in;
	mi_vector_normalize(&out);
	return out;
}

//--------------------------------------------------------------------
// fastFresnel()
//--------------------------------------------------------------------
float fastFresnel(const float& NdotE, const float& R0, const float& power)
{
	float result = R0 + (1.0f-R0)*pow((float)abs(1.0f - saturate(NdotE)), (float)power);
	return result;
} 

//--------------------------------------------------------------------
// fresnel()
//--------------------------------------------------------------------
float fresnel(const float& NdotE, const float& eta)
{
	float R0 = (1.0f-eta)*(1.0f-eta)/((1.0f+eta)*(1.0f+eta));
	return fastFresnel(NdotE, R0, 5.0);
}

//--------------------------------------------------------------------
// PhongDiffuse()
//--------------------------------------------------------------------
miColor PhongDiffuse(const miVector& normal, const miVector& lightDir, const miColor& lDiffColor, const miColor& sDiffColor)
{
	float cosine = saturate(dot(normal,lightDir));
	return cosine * (lDiffColor * sDiffColor);
}

//--------------------------------------------------------------------
// PhongSpecular()
//--------------------------------------------------------------------
miColor PhongSpecular(const miVector& normal, const miVector& lightDir, const miVector& eyeDir, 
					  const miColor& lSpecColor, const miColor& sSpecColor, const float& sSpecPower)
{
	// eyeDir is passed as dir FROM shade point TO eye
	// lightDir is passed as dir FROM shade point TO light
	float specComp = saturate(dot(normal,normalize(lightDir + eyeDir)));
	float doSpec = (dot(normal,lightDir)>0.0f) ? 1.0f : 0.0f;
    specComp = doSpec * (pow(specComp, max(0.001f, sSpecPower)));
	return specComp * (lSpecColor * sSpecColor);
}

//--------------------------------------------------------------------
// BlinnSpecular()
//--------------------------------------------------------------------
miColor BlinnSpecular(miState* state, const miVector& normal, const miVector& lightDir, const miVector& eyeDir, 
	const miColor& lSpecColor, const miColor& sSpecColor, float eccentricity, float IOR,
	float specularBias, float specularPower)
{
	// eyeDir is passed as dir FROM shade point TO eye
	// lightDir is passed as dir FROM shade point TO light

	miVector H = normalize(lightDir + eyeDir);
	// NdotH = cos a
	float NdotH = dot(normal, H);
	float NdotE = dot(normal, eyeDir);
	float EdotH = dot(eyeDir, H);
	float NdotL = dot(normal, lightDir);

	if (EdotH == 0) EdotH = NON_ZERO;
	if (NdotE == 0) NdotE = NON_ZERO;
	
	float Gb = 2 * NdotH * NdotE / EdotH;
	float Gc = 2 * NdotH * NdotL / EdotH; 
	float G = mi::math::min(Gb, Gc);
	G = mi::math::min(1.0f, G);
	
	// eccentricity varies from 0 to 1. 
	// D3 = [ c^2 / (1 + cos^2(a)(c^2 - 1)) ]^2
	float c2 = eccentricity*eccentricity;
	float D3 = ( c2 / ( 1 + (NdotH*NdotH*(c2-1)) ) );
	float D = D3*D3;
	
	// eta = ni/nt. assume air-material is the interface
	float F = fresnel(NdotE, 1.0f/(IOR+NON_ZERO)); 

	float specComp = D * G * F / NdotE;
	specComp = max(specComp, 0);
	return (specComp * specularPower) * (lSpecColor * sSpecColor);
}

//--------------------------------------------------------------------
// rotateAboutY()
//--------------------------------------------------------------------
miVector rotateAboutY(const miVector& vec, const float& angle)
{
	if (angle == 0)
		return vec;
	else
	{
		float s = sin(angle*AngleToRad);
		float c = cos(angle*AngleToRad);
		miVector out;
		out.x = vec.z*s + vec.x*c;
		out.y = vec.y;
		out.z = vec.z*c - vec.x*s;
		return out;
	}
}

//--------------------------------------------------------------------
// CartesianToPolar()
//--------------------------------------------------------------------
miVector CartesianToPolar( const miVector& vec )
{
	miVector out;
	miVector nvec = normalize( vec );
	out.x = (atan2( nvec.z, nvec.x )+PI)*(1/(2*PI));
	out.y = acos( nvec.y )*(1/PI);
	out.z = 1;
	return out;
}

//--------------------------------------------------------------------
// SampleTexture()
//--------------------------------------------------------------------
miColor SampleTexture( miState* i_State, const miTag& i_TextureTag, miVector * i_Coords, bool i_bIsFilter = true)
{
	miColor o_Color = make_color(0);
	miVector n = i_State->normal;
	mi_vector_to_world(i_State, &n, &n);
	n = normalize(n);
	miVector e = i_State->dir * -1;
	mi_vector_to_world(i_State, &e, &e);
	e = normalize(e);

	if (i_TextureTag)
	{
		float angle_test = fabs(dot(n, e));
		
		miVector p[3], t[3];
		miMatrix ST;
		miScalar disc_r = 0.5f;
		miTexfilter ell_opt;
		if (i_State->reflection_level == 0 &&
			mi_texture_filter_project(p, t, i_State, disc_r, 0) && 
			i_bIsFilter)
		{
			/*t[0].x -= (miScalar)floor(t[0].x);
			t[1].x -= (miScalar)floor(t[1].x);
			t[2].x -= (miScalar)floor(t[2].x);
			t[0].y -= (miScalar)floor(t[0].y);
			t[1].y -= (miScalar)floor(t[1].y);
			t[2].y -= (miScalar)floor(t[2].y);*/

//			t[0].y = 1.0f - t[0].y;
//			t[1].y = 1.0f - t[1].y;
//			t[2].y = 1.0f - t[2].y;

			/*mi_call_shader_x((miColor*)&t[0], miSHADER_TEXTURE,
			i_State, *i_Remap, &t[0]);
			mi_call_shader_x((miColor*)&t[1], miSHADER_TEXTURE,
			i_State, *i_Remap, &t[1]);
			mi_call_shader_x((miColor*)&t[2], miSHADER_TEXTURE,
			i_State, *i_Remap, &t[2]);*/
			if (mi_texture_filter_transform(ST, p, t))
			{
				ell_opt.eccmax = max(80.0f * angle_test, 1.0f);
				ell_opt.max_minor = max(8.0f * angle_test, 1.0f);
				ell_opt.bilinear = true;
				//ell_opt.circle_radius = saturate(1.0f - angle_test) * 10.0f + 0.1f;
				ell_opt.circle_radius = 0.8f;
				ST[2*4+0] = i_Coords->x;
				ST[2*4+1] = i_Coords->y;

				if (mi_lookup_filter_color_texture(&o_Color, i_State,
					i_TextureTag, &ell_opt, ST))
				{
					return o_Color;
				}
			}

		}
		
		miBoolean textureOk = mi_lookup_color_texture( &o_Color,
													   i_State,
													   i_TextureTag,
													   i_Coords);
	}
	
	return o_Color;
}

//--------------------------------------------------------------------
// Tex2d()
//--------------------------------------------------------------------
miColor Tex2d( miState* i_State, const miTag& i_TextureTag, miVector sstt, bool i_bIsFilter = true)
{
	miColor color = make_color(1);
	if ( i_TextureTag )
	{
		color = SampleTexture(i_State,i_TextureTag,&sstt,i_bIsFilter);
	}
	return color;
}

//--------------------------------------------------------------------
// Tex2dCombine()
//--------------------------------------------------------------------
miColor Tex2dCombine( miState* i_State, const miTag& i_TextureTag, const miColor& i_Color, miVector sstt)
{
	miColor color = i_Color;
	if ( i_TextureTag )
	{
		color *= SampleTexture(i_State,i_TextureTag,&sstt);
	}
	return color;
}

//--------------------------------------------------------------------
// Tex2DNormal()
//--------------------------------------------------------------------
miVector Tex2dNormal(miState* i_State, const miTag& i_TextureTag, miVector sstt)
{
	miVector bumpNormal = make_vector(0,0,1);
	if ( i_TextureTag )
	{
		miColor tex = Tex2d(i_State, i_TextureTag, sstt);
		bumpNormal = normalize( expand( make_vector(tex.r,tex.g,tex.b) ) );
	}
	return bumpNormal;
}

//--------------------------------------------------------------------
// Tex2dReplace()
//--------------------------------------------------------------------
miColor Tex2dReplace( miState* i_State, const miTag& i_TextureTag, const miColor& i_Color, miVector sstt)
{
	miColor color = i_Color;
	if ( i_TextureTag )
	{
		color = SampleTexture(i_State,i_TextureTag,&sstt);
	}
	return color;
}

//--------------------------------------------------------------------
// SampleEnvironmentLOD()
//--------------------------------------------------------------------
miColor SampleEnvironmentLOD(miState* state, const miVector& worldEyeDir, const miVector& worldNormal,
							const miTag& CubeMap, const float& angle, const float& lod)
{
	// do world space reflection
	float nDotV = dot(worldEyeDir, worldNormal);
	miVector reflVect = 2.0f * nDotV * worldNormal - worldEyeDir;
	miVector rotateVec = rotateAboutY(reflVect, angle-180); // 180 degree HACK
	miVector polar = CartesianToPolar(rotateVec);
	return SampleTexture( state, CubeMap, &polar, false );
}

//--------------------------------------------------------------------
// SampleEnvironment()
//--------------------------------------------------------------------
miColor SampleEnvironment(miState* state, const miVector& worldEyeDir, const miVector& worldNormal,
						  const miTag& CubeMap, const float& angle)
{
	// do world space reflection
	float nDotV = dot(worldEyeDir, worldNormal);
	miVector reflVect = 2.0f * nDotV * worldNormal - worldEyeDir;
	miVector rotateVec = rotateAboutY(reflVect, angle-180); // 180 degree HACK
	miVector polar = CartesianToPolar(rotateVec);
	return SampleTexture( state, CubeMap, &polar, false );
}

//--------------------------------------------------------------------
// SampleEnvDiffuse()
//--------------------------------------------------------------------
miColor SampleEnvDiffuse(miState* state, const miVector& worldNormal, const miTag& CubeMap, const float& angle)
{
	miVector norm = rotateAboutY(worldNormal, angle);
	miVector polar = CartesianToPolar(norm);
	polar.y = 1-polar.y; // 1- flip HACK
	return SampleTexture( state, CubeMap, &polar, false );
}

//--------------------------------------------------------------------
// faceforward()
//--------------------------------------------------------------------
miVector faceforward( const miVector& N, const miVector& I, const miVector& Ng ) 
{
	miVector Nres;
	if (dot(N,Ng) > 0) {
		if (dot(I,Ng) > 0) 	Nres = N*-1;
		else  		Nres = N;
	} else {
		if (dot(I,Ng) > 0) 	Nres = N;
		else  		Nres = N*-1;
	}
	return Nres;
}

//--------------------------------------------------------------------
// reflect()
//--------------------------------------------------------------------
miVector reflect(const miVector& I , const miVector& N) 
{
	return I-2*dot(N,I)*N;
}

//--------------------------------------------------------------------
// refract()
//--------------------------------------------------------------------
miVector refract( const miVector& I, const miVector& N, const float& eta ) 
{
	float IdotN = dot(I,N);
	float k = 1 - eta*eta*(1 - IdotN*IdotN);
	return k < 0 ? make_vector(0) : eta*I - (eta*IdotN + sqrt(k))*N;
}
//--------------------------------------------------------------------
// MI_AUX functions from WMRS src
//--------------------------------------------------------------------
miScalar miaux_state_outgoing_ior(miState *state) 
{ 
    miScalar unassigned_ior = 0.0, default_ior = 1.0; 
    if (state != NULL && state->ior != unassigned_ior) 
	return state->ior; 
    else 
	return default_ior; 
}  
miScalar miaux_state_incoming_ior(miState *state) 
{ 
    miScalar unassigned_ior = 0.0, default_ior = 1.0; 
    if (state != NULL && state->ior_in != unassigned_ior) 
	return state->ior_in; 
    else 
	return default_ior; 
} 
miBoolean miaux_ray_is_transmissive(miState *state) 
{ 
    return state->type == miRAY_TRANSPARENT || 
	state->type == miRAY_REFRACT; 
} 
miBoolean miaux_parent_exists(miState *state) 
{  
    return state->parent != NULL; 
} 
miBoolean miaux_shaders_equal(miState *s1, miState *s2) 
{ 
    return s1->shader == s2->shader; 
}
miBoolean miaux_ray_is_entering(
    miState *state,
    miState *state_of_this_shaders_previous_transmission)
{
    miState *s;
    miBoolean ray_is_entering = miTRUE;
    state_of_this_shaders_previous_transmission = NULL;
    for (s = state; s; s = s->parent)	
	if (miaux_ray_is_transmissive(s) &&
	    miaux_parent_exists(s) &&
	    miaux_shaders_equal(s->parent, state)) {
	    ray_is_entering = !ray_is_entering;
	    if (state_of_this_shaders_previous_transmission == NULL)
		state_of_this_shaders_previous_transmission = s->parent;
	}
    return ray_is_entering;
}
void miaux_set_state_refraction_indices(miState *state, 
					miScalar material_ior) 
{ 
    miState *previous_transmission = NULL; 
    miScalar incoming_ior, outgoing_ior; 
        
    if (miaux_ray_is_entering(state, previous_transmission)) { 
	outgoing_ior = material_ior; 
	incoming_ior = miaux_state_outgoing_ior(state->parent); 
    } else {	 
	incoming_ior = material_ior; 
	outgoing_ior = miaux_state_incoming_ior(previous_transmission); 
    } 
    state->ior_in = incoming_ior; 
    state->ior = outgoing_ior; 
} 
miBoolean miaux_ray_is_entering_material(miState *state) 
{ 
    miState *s; 
    miBoolean entering = miTRUE; 
    for (s = state; s != NULL; s = s->parent) 
	if (s->material == state->material) 
	    entering = !entering; 
    return entering; 
} 

//--------------------------------------------------------------------
// GetGlossyReflection()
//--------------------------------------------------------------------
miColor GetGlossyReflection(miState *state, miScalar shiny, miUint samples)
{
    miVector reflect_dir;
	miColor result  = make_color(0);
    miColor reflect_color;
    int sample_number = 0;
    double sampled_dir[2];

    while (mi_sample(sampled_dir, &sample_number, state, 2, &samples)) {
        mi_reflection_dir_glossy_x(&reflect_dir, state, shiny, sampled_dir);
        if (!mi_trace_reflection(&reflect_color, state, &reflect_dir))
            mi_trace_environment(&reflect_color, state, &reflect_dir);

		result += reflect_color;
    }
	result *= (1.0f / samples);
	return result;
}

//--------------------------------------------------------------------
// GetGlossyRefraction()
//--------------------------------------------------------------------
miColor GetGlossyRefraction(miState *state, miScalar shiny, miUint samples, miScalar ior)
{
	miColor result = make_color(0);

    miVector refract_dir, reflect_dir;
    miColor refract_color;
    int sample_number = 0;
    double sample[2];

    miaux_set_state_refraction_indices(state, ior);

    while (mi_sample(sample, &sample_number, state, 2, &samples)) {
        if (mi_transmission_dir_glossy_x(&refract_dir, state,
                                         state->ior_in, state->ior,
                                         shiny, sample)) {
            if (mi_trace_refraction(&refract_color, state, &refract_dir))
				result += refract_color;
        }
        else {
            mi_reflection_dir(&reflect_dir, state);
            if (!mi_trace_reflection(&refract_color, state, &reflect_dir))
                mi_trace_environment(&refract_color, state, &reflect_dir);
			result += refract_color;
        }
    }
	result *= (1.0 / samples);
	return result;
}

//--------------------------------------------------------------------
// GetReflection()
//--------------------------------------------------------------------
miColor GetReflection(miState* state, miVector sstt, const miVector& bumpNormal,
					  const miVector& worldNormal, const miVector& WorldEyeDir, 
					  const float& bumpScale, const float& refrFactor, const float& reflFactor,
					  const miTag& reflectFactorSampler,
					  const float& fresnelBias, const float& fresnelPower, const float& IOR, const float& refrLOD,
					  const float& reflLOD, const bool& reflEnable, const float& reflSamples)
{
	miColor rr = make_color(0);

	if ( reflEnable )
	{

		miColor reflColorFactor = make_color(reflFactor);
		miColor refrColorFactor = make_color(refrFactor);

		miColor reflColor = make_color(0);
		miColor refrColor = make_color(0);

		miColor factorCol = Tex2dReplace(state, reflectFactorSampler, make_color(1),sstt);
		reflColorFactor *= factorCol;
		refrColorFactor *= factorCol;

		miVector n = normalize(worldNormal);
		miVector i = normalize(WorldEyeDir);
		miVector nf = faceforward(n, i, state->normal_geom);

		float fresnelRefl = fastFresnel( dot((i*-1),nf), fresnelBias, fresnelPower);
		float fresnelRefr = 1 - fresnelRefl;

		if ( reflFactor != 0 )
		{
			if ( reflLOD == 0 )
			{
				miVector reflVec = reflect(WorldEyeDir*-1, nf);
				mi_trace_reflection(&reflColor,state,&reflVec);
			}
			else
			{
				float modReflBlurFactor = clamp(-7.14286*reflLOD + 50,0,50); // approximate our blur with mray glossy blur
				reflColor = GetGlossyReflection(state,modReflBlurFactor,reflSamples);
			}
			reflColor *= fresnelRefl * reflColorFactor;
		}

		if ( refrFactor != 0 )
		{
			if ( refrLOD == 0 )
			{
				miVector direction; 
				miaux_set_state_refraction_indices(state, IOR); 	 
				mi_refraction_dir(&direction, state, state->ior_in, state->ior);			
				mi_trace_refraction(&refrColor, state, &direction);
			}
			else
			{
				float modRefrBlurFactor = clamp(4.90476*refrLOD*refrLOD - 69.7619*refrLOD + 250,0,250); // approximate our blur with mray glossy blur
				refrColor = GetGlossyRefraction(state,modRefrBlurFactor,reflSamples,IOR);
			}
			refrColor *= fresnelRefr * refrColorFactor;
		}
	
		rr = reflColor + refrColor;
	}	
	return rr;
}

//--------------------------------------------------------------------
// GetEnvironment()
//--------------------------------------------------------------------
miColor GetEnvironment(miState* state)
{
	miColor rr = make_color(0);
	
	/*miVector n = normalize(worldNormal);
	miVector i = normalize(WorldEyeDir);
	miVector nf = faceforward(n, i, state->normal_geom);

	miVector reflVec = reflect(WorldEyeDir*-1, nf);*/
	miVector reflVec = make_vector(0);
	mi_reflection_dir(&reflVec, state);
	mi_trace_environment(&rr,state,&reflVec);
	
	return rr;
}

//--------------------------------------------------------------------
// OrenNayarDiffuse()
//--------------------------------------------------------------------
float OrenNayarDiffuse(const miVector& L, const miVector& I, const miVector& N, const float& roughness)
{
	float sigmasq = roughness*roughness;
	float A = 1.0f - 0.5f * sigmasq/(sigmasq+0.33f);
	float B = 0.45f * sigmasq/(sigmasq+0.09f);
	
	float IdotN = dot(I,N);
	float LdotN = dot(L,N);
	float theta_r = acos(IdotN);
	float theta_i = acos(LdotN);
	float Bfactor = max(0, cos(theta_i-theta_r));
	if (Bfactor > 0)
	{
		float alpha = max(theta_i, theta_r);
		float beta = mi::math::min(theta_i, theta_r);
		Bfactor *= sin(alpha) * tan(beta);
	}
	return A + B * Bfactor;
}

//--------------------------------------------------------------------
// GetBumpNormal()
//--------------------------------------------------------------------
miVector GetBumpNormal(miState * state, const miTag& normalMap, const float& normalMapScale, miVector sstt)
{
	miVector outNormal = state->normal;

	if ( normalMap )
	{
		miColor mapColor = Tex2d(state, normalMap, sstt);
		miVector mapNormal = (( make_vector(mapColor.r,mapColor.g,mapColor.b) * 2 ) - 1) * make_vector(normalMapScale,normalMapScale,1);

		//miVector basisx = state->bump_x_list[0];	// object space
		//miVector basisy = state->bump_y_list[0];	// object space
		//miVector basisz = state->normal;			// internal space
		miVector basisx;
		miVector basisy;
		miVector basisz;
		mi_vector_to_world(state, &basisx, &state->bump_x_list[0]);	// object space
		mi_vector_to_world(state, &basisy, &state->bump_y_list[0]);	// object space
		mi_vector_to_world(state, &basisz, &state->normal);			// internal space
		/*basisx = normalize(basisx);
		basisy = normalize(basisy);
		basisz = normalize(basisz);*/

		miMatrix tangentMat = { basisx.x, basisx.y, basisx.z, 0 ,
								basisy.x, basisy.y, basisy.z, 0 ,
								basisz.x, basisz.y, basisz.z, 0 ,
								0       , 0       , 0       , 1 };

		mi_vector_transform( &outNormal, &mapNormal, tangentMat );
		outNormal = normalize(outNormal);
	}

	return (outNormal);	
}

//------------------------------------------------------------------------
// ContainsPoint()
//------------------------------------------------------------------------
float ContainsPoint(miVector i_Point,
	 			    float bbox_min_x, float bbox_min_y, float bbox_min_z,
				    float bbox_max_x, float bbox_max_y, float bbox_max_z )
{
	if (i_Point.x > bbox_max_x)
	{
		return 0.0f;
	}
	if (i_Point.x < bbox_min_x)
	{
		return 0.0f;
	}
	if (i_Point.y > bbox_max_y)
	{
		return 0.0f;
	}
	if (i_Point.y < bbox_min_y)
	{
		return 0.0f;
	}
	if (i_Point.z > bbox_max_z)
	{
		return 0.0f;
	}
	if (i_Point.z < bbox_min_z)
	{
		return 0.0f;
	}
	return 1.0f;
}

//------------------------------------------------------------------------
// miao_get_label()
//------------------------------------------------------------------------
static int miao_get_label(miTag instance)
{
    int result       = 0;
    miInstance* inst = NULL;
    
    if (!instance) return 0;
    inst = (miInstance *)mi_db_access(instance);
    if (inst) result = inst->label;
    mi_db_unpin(instance);
    return result;
}

//------------------------------------------------------------------------
// miao_trace_the_ray()
//------------------------------------------------------------------------
static miBoolean    miao_trace_the_ray(
        miState    *state,
        miVector   *dir,
        miVector   *point,
        miao_trace_info *ti)
{
    int label = 0, childlabel = 0;
    miVector  base_point = *point;
    miScalar  base_dist  = 0;
    miBoolean result     = miFALSE;

play_it_again:
    /* Trace the ray */
    result = mi_trace_probe(state, dir, &base_point);    
    /* If a miss, or compatible mode - then simply return */
    if (!result || ti->compatible) return result;

    childlabel = miao_get_label(state->child->instance);

    /* Test non-self-occlusion. A hit on this object id that
       originates on the same object id is considered a total 
       miss, i.e. the environment */
    if (ti->id_nonself > 0)
    { 
        int label = miao_get_label(state->instance);

        /* Compare parent AND child labels */
        if (label      == ti->id_nonself &&
            childlabel == ti->id_nonself)
            return miFALSE;
    }

    /* Test the include/exclude. However, a miss here causes
       the ray to continue to be traced, in case of a hit */

    /* Positive inclexcl == include */
    if (ti->id_inclexcl > 0 && childlabel != ti->id_inclexcl)
        result = miFALSE;

    /* Negative inclexcl == exclude */
    if (ti->id_inclexcl < 0 && childlabel == -ti->id_inclexcl)
        result = miFALSE;

    /* Was this hit rejected? */
    if (!result)
    {
        /* Remember distance traced so far */
        base_dist += state->child->dist;

        /* Advance the point */
        base_point.x  += base_dist * dir->x;
        base_point.y  += base_dist * dir->y;
        base_point.z  += base_dist * dir->z;

        /* Keep tracing ray */
        goto play_it_again;
    }

    /* Plug originals back, if necessary */
    /* state->child->dist += base_dist; */

    return result;
}

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
				 )
{
	if ( !bEnableAO )
	{
		*result = make_color(1,1,1,1);
		return miTRUE;
	}

	float r = lerp( aoRadiusNear, aoRadiusFar, ((state->point).z - aoCamNear) / (abs(aoCamFar - aoCamNear) + NON_ZERO) );

	miUint samples = aoSamples;
	miScalar clipdist = r;
	miScalar spread	= PI/2 - aoAngleBias*AngleToRad;
    miScalar falloff = 1/(aoAttenuation+NON_ZERO);
	miColor	dark   = aoColor;

	
	miColor	bright	= make_color(1,1,1,1);
	double sample[3], near_clip, far_clip;
	int	  counter  = 0;
	miBoolean ret_type  = 0;
	miBoolean reflecto  = false;
	miBoolean occ_alpha = false;
    miao_trace_info ti;
	ti.id_inclexcl   = 0;
    ti.id_nonself    = 0;

	miTag	   org_env = state->environment; /* Original environ. */

	miVector orig_normal, trace_dir;
	miScalar output      = 0.0, 
	samplesdone = 0.0;
	miScalar o_m_spread = 1.0f - spread;
	
	miColor   env_total;	/* environment Total */ 	
	miVector  norm_total;	/* Used for adding up normals */

    int       version = 1;

	/* If called as user area light source, return "no more samples" for	
	   any call beyond the first */
	if (state->type == miRAY_LIGHT && state->count > 0) 
		return (miBoolean)2;

    /* Figure out the call version */
    mi_query(miQ_DECL_VERSION, 0, state->shader->function_decl, &version); 
    if (version >= 2)
    {
        if (falloff <= 0.0) 
            falloff = 1.0;

        /* None of these options used, go into compatible mode */
        ti.compatible = (ti.id_inclexcl   == 0 && 
                         ti.id_nonself    == 0 
                         );
    }
    else
    {
        ti.compatible = miTRUE;
    }

	/* Used for adding up environment */
	env_total.r = env_total.g = env_total.b = env_total.a = 0;

	far_clip = near_clip = clipdist;

	orig_normal  = state->normal;
	norm_total   = state->normal; /* Begin by standard normal */

	/* Displacement? Shadow? Makes no sense */
	if (state->type == miRAY_DISPLACE ||
            state->type == miRAY_SHADOW) {
		result->r = result->g = result->b = result->a = 0.0;
		return (miTRUE); 
	}

	if (clipdist > 0.0) 
		mi_ray_falloff(state, &near_clip, &far_clip);

	/* Avoid recursion: If we are designated as environment shader,
	   we will be called with rays of type miRAY_ENVIRON, and if so
	   we switch the environment to the global environment */

	if (state->type == miRAY_ENVIRONMENT)
	    state->environment = state->camera->environment;

	while (mi_sample(sample, &counter, state, 2, &samples)) {
		mi_reflection_dir_diffuse_x(&trace_dir, state, sample);
		trace_dir.x = orig_normal.x*o_m_spread + trace_dir.x*spread;
		trace_dir.y = orig_normal.y*o_m_spread + trace_dir.y*spread;
		trace_dir.z = orig_normal.z*o_m_spread + trace_dir.z*spread;
	
		mi_vector_normalize(&trace_dir);

		if (reflecto) {
			miVector ref;
			miScalar nd = state->dot_nd;
			/* Calculate the reflection direction */
			state->normal = trace_dir;
			state->dot_nd = mi_vector_dot(
						&state->dir,
						&state->normal);
			/* Bugfix: mi_reflection_dir(&ref, state);
			   for some reason gives me the wrong result,
			   doing it "manually" works better */
			ref    = state->dir;
			ref.x -= state->normal.x*state->dot_nd*2.0f;
			ref.y -= state->normal.y*state->dot_nd*2.0f;
			ref.z -= state->normal.z*state->dot_nd*2.0f;
			state->normal = orig_normal;
			state->dot_nd = nd;
			trace_dir = ref;
		}

		if (mi_vector_dot(&trace_dir, &state->normal_geom) < 0.0) 
			continue;

		output	    += 1.0; /* Add one */
		samplesdone += 1.0;

		if (state->options->shadow &&
		    miao_trace_the_ray(state, &trace_dir, &state->point, &ti)) {
			/* we hit something */
    
			if (clipdist == 0.0) 
				output -= 1.0;
			else if (state->child->dist < clipdist) {
				miScalar f = pow(state->child->dist / clipdist,
                                                 (double) falloff);

				output -= (1.0 - f);

				norm_total.x += trace_dir.x * f;
				norm_total.y += trace_dir.y * f;
				norm_total.z += trace_dir.z * f;

				switch (ret_type) {
					case 1: 
					  { 
					    /* Environment sampling */
					    miColor envsample;
					    mi_trace_environment(&envsample, 
						state, &trace_dir);

					    env_total.r += envsample.r * f;
					    env_total.g += envsample.g * f;
					    env_total.b += envsample.b * f;
					  }
					  break;
					
					default: 
					  /* Most return types need no 
						special stuff */
					  break;
				}
			}
		}
		else {
			/* We hit nothing */
			norm_total.x += trace_dir.x;
			norm_total.y += trace_dir.y;
			norm_total.z += trace_dir.z;

			switch (ret_type) {
				case 1: /* Environment sampling */
					{
					   miColor envsample;

					   mi_trace_environment(&envsample, 
						   state, &trace_dir);

					   env_total.r += envsample.r;
					   env_total.g += envsample.g;
					   env_total.b += envsample.b;
					}
					break;
				default:
					/* Most return types need no 
						special treatment */
					break; 
			}
		}
	}

	if (clipdist > 0.0) 
		mi_ray_falloff(state, &near_clip, &far_clip);
    
	if (samplesdone <= 0.0) /* No samples? */
		samplesdone = 1.0;  /* 1.0 to not to break divisons below */

	miVector old_dir = state->dir;
	output /= (miScalar) samplesdone;

	if (ret_type == -1)
		norm_total = state->normal;
	else {
		mi_vector_normalize(&norm_total);

		/* If the color shaders use the normal....
			give them the bent one... */
		state->normal = norm_total;  
		state->dir    = norm_total;
	}

	*result = lerp(make_color(1), aoColor, ((1-output) * aoContrast));

	state->normal = orig_normal;
	state->dir    = old_dir;

	if (state->type == miRAY_LIGHT) {
		 /* Are we a light shader? */
		int type;
		mi_query(miQ_FUNC_CALLTYPE, state, 0, &type);

		/* Make sure we are called as light shader */
		if (type == miSHADER_LIGHT) {
			/* If so, move ourselves to above the point... */
			state->org.x = state->point.x + state->normal.x;
			state->org.y = state->point.y + state->normal.y;
			state->org.z = state->point.z + state->normal.z;
			/* ...and set dot_nd to 1.0 to illuminate fully */
			state->dot_nd = 1.0;
		}
	}
	/* Reset environment, if we changed it */
	state->environment = org_env;
	return miTRUE;
}

//------------------------------------------------------------------------
// GetBufferIdx()
//------------------------------------------------------------------------
size_t GetBufferIdx(miState *state, const char* i_BufferName)
{
	mi::shader::Access_fb framebuffers(state->camera->buffertag);
	size_t buffer_index = 0;
	framebuffers->get_index(i_BufferName, buffer_index);
	return buffer_index;
}

//------------------------------------------------------------------------
// GetGI()
//------------------------------------------------------------------------
void GetGI( miColor *result, 
			miState *state, 
			bool	bEnableGI
			)
{
	miColor giColor = make_color(0,0,0,0);
	if ( bEnableGI )
	{
		//mi_compute_irradiance(&giColor, state);
		mi_compute_avg_radiance(&giColor, state, 'f', NULL);
		size_t buffer_index = GetBufferIdx(state,"gi_only");
		mi_fb_put(state, buffer_index, &giColor);
	}
	*result = giColor;
}


//------------------------------------------------------------------------
// PutRefl()
//------------------------------------------------------------------------
void PutRefl(miColor *result, 
			 miState *state, 
			 bool	bEnableRefl
			 )
{
	if (bEnableRefl)
	{
		size_t buffer_index = GetBufferIdx(state,"refl_only");
		mi_fb_put(state, buffer_index, result);
	}
}

//--------------------------------------------------------------------
//	attenuation()
//--------------------------------------------------------------------
float attenuation(float d,		// Position of vertex in world coords
				  float falloffX,
				  float falloffY,
				  float falloffZ,
				  float falloffW,
				  float falloffStart)
{
	float atten = 1;
	if (d > falloffStart)
	{
		atten = 1 / NonZeroVal(falloffX + 
					falloffY * (d - falloffStart) + 
					falloffZ * (d - falloffStart) * (d - falloffStart) + 
					falloffW * (d - falloffStart) * (d - falloffStart) * (d - falloffStart));
	}
	else
	{
		atten = 1 / NonZeroVal(falloffX);
	}
	return atten;
}

//------------------------------------------------------------------------
// GetScaledIBL()
//------------------------------------------------------------------------
miColor GetScaledIBL(const miColor& in)
{
	return in / PI;
}