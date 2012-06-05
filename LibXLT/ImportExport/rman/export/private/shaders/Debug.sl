/*****************************************************************************
**  Debug.sl
**
**      Depth shader
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support.hsl"

//--------------------------------------------------------------------
// Debug()
//--------------------------------------------------------------------
surface 
Debug(float IOR = 1  /* index of refraction */)
{
	normal  n = normalize(N);
	vector  i = normalize(I);
	vector  nf = faceforward(n, i);
	    
	vector refractRay = refract( i, nf , (-i.n >= 0) ? 1/IOR : IOR );
	color rr = trace(P, refractRay);	  

	Oi = Os;
	Ci = Oi * Cs * rr; // * diffusecolor;
}







