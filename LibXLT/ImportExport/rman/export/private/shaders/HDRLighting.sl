/*****************************************************************************
**  HDRLighting.sl
**
**      Tonemapping imager shader
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support.hsl"

//--------------------------------------------------------------------
// RGB2Yxy()
//--------------------------------------------------------------------
vector RGB2Yxy(color colorrgb)
{

	matrix RGB2XYZ = matrix(
						0.5141364, 0.3238786,  0.16036376, 0,
						0.265068,  0.67023428, 0.06409157, 0,
						0.0241188, 0.1228178,  0.84442666, 0,
						0, 0, 0, 1);

	vector vectorrgb = vector( colorrgb[0], colorrgb[1], colorrgb[2] );
	vector XYZ = vtransform(RGB2XYZ,vectorrgb);	

	vector Yxy;
	setxcomp(Yxy,ycomp(XYZ));                            // copy luminance Y
	setycomp(Yxy,xcomp(XYZ) / (xcomp(XYZ) + ycomp(XYZ) + zcomp(XYZ)) ); // x = X / (X + Y + Z)
	setzcomp(Yxy,ycomp(XYZ) / (xcomp(XYZ) + ycomp(XYZ) + zcomp(XYZ)) ); // y = Y / (X + Y + Z)

	return Yxy;
}

//--------------------------------------------------------------------
// RGB2Yxy()
//--------------------------------------------------------------------
color Yxy2RGB(vector colorYxy)
{
	matrix XYZ2RGB = matrix(
							2.5651,	-1.1665,-0.3986, 0,
							-1.0217, 1.9777, 0.0439, 0,
							0.0753, -0.2543, 1.1892, 0,
							0, 0, 0, 1);

	vector XYZ;
	// Yxy -> XYZ conversion
	setxcomp(XYZ, xcomp(colorYxy) * ycomp(colorYxy) / zcomp(colorYxy) );					// X = Y * x / y
	setycomp(XYZ, xcomp(colorYxy) );												// copy luminance Y
	setzcomp(XYZ, xcomp(colorYxy) * (1 - ycomp(colorYxy) - zcomp(colorYxy)) / zcomp(colorYxy));	// Z = Y * (1-x-y) / y

	vector retVec = vtransform(XYZ2RGB, XYZ);

	return color(xcomp(retVec),ycomp(retVec),zcomp(retVec));
}

//--------------------------------------------------------------------
// PS_Reinhard02()
//--------------------------------------------------------------------
color PS_Reinhard02( color hdrColorSample; float avgLuminance; 
					 float exposure; float whitePoint )
{
	// map pixel to luminance space
	vector Yxy = RGB2Yxy(hdrColorSample);

	// (Lp) Map average luminance to the middlegrey zone by scaling pixel luminance
	float Lp = xcomp(Yxy) * exposure / avgLuminance;     

	// (Ld) Scale all luminance within a displayable range of 0 to 1
	setxcomp(Yxy, (Lp * (1.0 + Lp/(whitePoint * whitePoint)))/(1.0 + Lp) );

	return Yxy2RGB(Yxy);
}

//--------------------------------------------------------------------
// HDRLighting()
//--------------------------------------------------------------------
imager HDRLighting(
				   float g_bEnableToneMap = 1; 
				   float g_fixedLuminance = 1; 
				   float g_fMiddleGray = 1;
				   float g_fWhiteCutoff = 1;
				   float g_fBloomScale = 1;				   
				   float g_fStarScale = 0.5; 
				   color vBloom = 0; 
				   color vStar = 0;
				   ) 
{
	
	color toneMapped = Ci;
	
	if ( g_bEnableToneMap == 1 )
	{
		toneMapped = PS_Reinhard02( Ci, g_fixedLuminance + 0.0001, g_fMiddleGray, g_fWhiteCutoff );
	}

	Ci = toneMapped;
}
