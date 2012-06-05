#define		PI			3.141592653589f
#define		PI_Times_2	PI * 2.0f
#define		PI_Times_4	PI * 4.0f
#define		PI_Div_2	PI / 2.0f
#define		PI_Inverse	1.0f / PI
#define		RadToAngle	180.0f / PI
#define		AngleToRad	PI / 180.0f
#define		NON_ZERO	0.00001f

/* Simple ambient occlusion parameter struct */
struct mib_amb_occlusion_p {
	int	    samples;
	miColor     bright;
	miColor     dark;
	miScalar    spread;
	miScalar    max_distance;
	miBoolean   reflective;
	int	    return_type;
	miBoolean   occlusion_in_alpha;
        /* Version 2 parameters */
        miScalar    falloff;
        int         id_includeexclude;
        int         id_nonself;
};
typedef struct miao_trace_info_t {
    miBoolean compatible;
    int id_inclexcl;
    int id_nonself;
} miao_trace_info;

// Colors
inline miColor operator+( const miColor& a, const miColor& b)
{
	miColor val;
	val.r = a.r + b.r;
	val.g = a.g + b.g;
	val.b = a.b + b.b;
	val.a = a.a + b.a;
    return val;
}
inline miColor operator-(const miColor& a, const miColor& b)
{
	miColor val;
	val.r = a.r - b.r;
	val.g = a.g - b.g;
	val.b = a.b - b.b;
	val.a = a.a - b.a;
    return val;
}

inline void operator+=(miColor &a, const miColor& b)
{
	a.r += b.r;
	a.g += b.g;
	a.b += b.b;
	a.a += b.a;
}

inline miColor operator*(const miColor& a, const miColor& b)
{
	miColor val;
	val.r = b.r * a.r;
	val.g = b.g * a.g;
	val.b = b.b * a.b;
	val.a = b.a * a.a;
    return val;
}

inline miColor operator*(const miScalar& a, const miColor& b)
{
	miColor val;
	val.r = b.r * a;
	val.g = b.g * a;
	val.b = b.b * a;
	val.a = b.a * a;
    return val;
}

inline miColor operator*(const miColor& a, const miScalar& b)
{
	miColor val;
	val.r = a.r * b;
	val.g = a.g * b;
	val.b = a.b * b;
	val.a = a.a * b;
    return val;
}

inline void operator*=(miColor &a, const miScalar& b)
{
	a.r *= b;
	a.g *= b;
	a.b *= b;
	a.a *= b;
}

inline void operator*=(miColor &a, const miColor& b)
{
	a.r *= b.r;
	a.g *= b.g;
	a.b *= b.b;
	a.a *= b.a;
}

inline miColor operator/(const miColor& a, const miColor& b)
{
	miColor val;
	val.r = b.r / a.r;
	val.g = b.g / a.g;
	val.b = b.b / a.b;
	val.a = b.a / a.a;
    return val;
}

inline miColor operator/(const miScalar& a, const miColor& b)
{
	miColor val;
	val.r = b.r / a;
	val.g = b.g / a;
	val.b = b.b / a;
	val.a = b.a / a;
    return val;
}

inline miColor operator/(const miColor& a, const miScalar& b)
{
	miColor val;
	val.r = a.r / b;
	val.g = a.g / b;
	val.b = a.b / b;
	val.a = a.a / b;
    return val;
}

inline void operator/=(miColor &a, const miScalar& b)
{
	a.r /= b;
	a.g /= b;
	a.b /= b;
	a.a /= b;
}
inline miColor make_color(const miScalar& a)
{
	miColor val;
	val.r = a;
	val.g = a;
	val.b = a;
	val.a = a;
	return val;
}
inline miColor make_color(const miScalar& a, const miScalar& b, const miScalar& c, const miScalar& d)
{
	miColor val;
	val.r = a;
	val.g = b;
	val.b = c;
	val.a = d;
	return val;
}


// Vectors
inline miVector make_vector(const miScalar& a)
{
	miVector val;
	val.x = a;
	val.y = a;
	val.z = a;
	return val;
}
inline miVector make_vector(const miScalar& a, const miScalar& b, const miScalar& c)
{
	miVector val;
	val.x = a;
	val.y = b;
	val.z = c;
	return val;
}
inline miVector operator+(const miVector& a, const miVector& b)
{
	miVector val;
	mi_vector_add(&val,&a,&b);
    return val;
}
inline miVector operator+(const miVector& a, const miScalar& b)
{
	miVector val;
	val.x = a.x + b;
	val.y = a.y + b;
	val.z = a.z + b;
    return val;
}
inline miVector operator-(const miVector& a, const miVector& b)
{
	miVector val;
	mi_vector_sub(&val,&a,&b);
    return val;
}
inline miVector operator-(const miVector& a, const miScalar& b)
{
	miVector val;
	val.x = a.x - b;
	val.y = a.y - b;
	val.z = a.z - b;
    return val;
}
inline miVector operator*(const miVector& a, const miScalar& b)
{
	miVector val = a;
	mi_vector_mul(&val,b);
    return val;
}
inline miVector operator*(const miScalar& a, const miVector& b)
{
	miVector val = b;
	mi_vector_mul(&val,a);
    return val;
}
inline miVector operator*(const miVector& a, const miVector& b)
{
	miVector val;
	val.x = a.x * b.x;
	val.y = a.y * b.y;
	val.z = a.z * b.z;
    return val;
}
inline miVector operator/(const miVector& a, const miScalar& b)
{
	miVector val = a;
	mi_vector_div(&val,b);
    return val;
}
inline miVector operator/(const miScalar& a, const miVector& b)
{
	miVector val = b;
	mi_vector_div(&val,a);
    return val;
}
inline miVector operator/(const miVector& a, const miVector& b)
{
	miVector val;
	val.x = a.x / b.x;
	val.y = a.y / b.y;
	val.z = a.z / b.z;
    return val;
}
