/*****************************************************************************
**  GI.sl
**
**      GI shader
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support.hsl"

//--------------------------------------------------------------------
// GI()
//--------------------------------------------------------------------
surface 
GI(    
    string normalMap = "";
	float normalMapScale = 0;
	float g_DisplacementEnabled = 0;
	float g_DisplacementNormalScale = 0;
	float UScale = 0;
	float VScale = 0;
	float UTrans = 0;
	float VTrans = 0;
	float UVAngle = 0;	
	float bRenderGI = 1;
	float GISamples = 256;
	float GIMaxDist = 1000;
	float GIMaxVariation = 0.1;
	float GIConeAngle = 90;
	color GIColor = 0;
	float GIRadiusNear = 10;
	float GIRadiusFar = 10;
	float GIAngleBias = 10;
	float GIAttenuation = 10;
	float GIContrast = 10;
	float GIBlurWidth = 10;
	float GIBlurSharpness = 10;
	float GIOverscanPixels = 10;
	float camNearClip = 1;
	float camFarClip = 10000;
	string transparencyMap = "";
	float objVisible = 1;
	float g_transparency = 1;
	float bRenderTransparent = 1;
	)
{
	matrix uvTransform = MakeUVTransform( UScale, VScale, UTrans, VTrans, UVAngle );

	point st_point = transform( uvTransform, point(s,t,0) );
	float ss = xcomp(st_point);
	float tt = ycomp(st_point);

	ss = ss - floor(ss);
	tt = tt - floor(tt);

	normal Nf = GetBumpNormal(normalMap, normalMapScale, ss, tt);

	color C = 0;

	C += CalculateGI(P, Nf, GISamples, GIMaxVariation, GIMaxDist, GIConeAngle, bRenderGI,
					 GIColor, GIRadiusNear,	GIRadiusFar, GIAngleBias, GIAttenuation,
					 GIContrast, GIBlurWidth, GIBlurSharpness, GIOverscanPixels,
					 camNearClip, camFarClip);
		
	float doTransparency = ( g_transparency<1 && bRenderTransparent==0 ) ? 0 : 1;		
	Oi = Tex2DCombineOpacity( transparencyMap , color(g_transparency,g_transparency,g_transparency), ss, tt ) * doTransparency * objVisible;

	Ci = Oi * Cs * C;
}







