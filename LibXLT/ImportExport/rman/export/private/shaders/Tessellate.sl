/*****************************************************************************
**  Tessellate.sl
**
**      MSP Displacement
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support.hsl"

//--------------------------------------------------------------------
// PolygonDisplaceFix()
//--------------------------------------------------------------------
normal PolygonDisplaceFix( varying normal norm; varying float amount)
{
	extern point P;
	extern normal Ng;
	varying normal Ndiff;

	Ndiff = normalize(norm) - normalize(Ng);
	P += amount * normalize(norm);
	return normalize(calculatenormal(P)) + Ndiff;
}

//--------------------------------------------------------------------
// Displace()
//--------------------------------------------------------------------
normal Displace(vector dir; string space; float amp;)
{
    float spacescale = length(vtransform(space, dir));
    vector Ndisp = dir * (amp / max(spacescale,1e-6));
    P += Ndisp;
    return normalize( calculatenormal(P) );
}

//--------------------------------------------------------------------
// Tessellate()
//--------------------------------------------------------------------
displacement Tessellate(	
	float g_DisplacementEnabled = 0;
	string displacementMap = "";
	float g_displacementScale = 0;
	float g_displacementBias = 0;
	float g_displacementBlur = 0;
	float g_ObjectUVScale = 1;
	float UScale = 1;
	float VScale = 1;
	float UTrans = 0;
	float VTrans = 0;
	float UVAngle = 0;
	)
{
    /* Only displace if a filename is provided */
    if (displacementMap != "" && g_DisplacementEnabled == 1) 
	{
		matrix uvTransform = MakeUVTransform( UScale, VScale, UTrans, VTrans, UVAngle );
		point st_point = transform( uvTransform, point(s,t,0) );
		float ss = xcomp(st_point);
		float tt = ycomp(st_point);
		ss = ss - floor(ss);
		tt = tt - floor(tt);

		float lod = g_displacementBlur/25; // fudge factor of 25

        /* Amplitude is channel 0 of the texture, indexed by s,t. */
        float amp = (ClampedFloatTexture(displacementMap,ss,tt,0.75,0,lod) + g_displacementBias) * g_displacementScale;

        /* Displace inward parallel to the surface normal, 
         * Km*amp units measured in dispspace coordinates.
         */
        N = Displace(normalize(N), "shader", amp );
    }
}
