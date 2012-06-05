/*****************************************************************************
**  AO.sl
**
**      AO shader
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support.hsl"

//--------------------------------------------------------------------
// AO()
//--------------------------------------------------------------------
surface 
AO(				
    string normalMap = "";
	float normalMapScale = 0;
	float g_DisplacementEnabled = 0;
	float g_DisplacementNormalScale = 0;
	float UScale = 0;
	float VScale = 0;
	float UTrans = 0;
	float VTrans = 0;
	float UVAngle = 0;
	float bRenderAO = 1;
	float AOSamples = 256;
	float AOMaxDist = 1000;
	float AOMaxVariation = 0.1;
	float AOConeAngle = 90;
	color AOColor = 0;
	float AORadiusNear = 10;
	float AORadiusFar = 10;
	float AOAngleBias = 10;
	float AOAttenuation = 10;
	float AOContrast = 10;
	float AOBlurWidth = 10;
	float AOBlurSharpness = 10;
	float AOOverscanPixels = 10;
	float camNearClip = 1;
	float camFarClip = 10000;
	string transparencyMap = "";
	float objVisible = 1;
	float g_transparency = 1;
	float bRenderTransparent = 1;
	output varying color outGI = color(0,0,0);
	)
{

	matrix uvTransform = MakeUVTransform( UScale, VScale, UTrans, VTrans, UVAngle );

	point st_point = transform( uvTransform, point(s,t,0) );
	float ss = xcomp(st_point);
	float tt = ycomp(st_point);

	ss = ss - floor(ss);
	tt = tt - floor(tt);

	normal Nf = GetBumpNormal(normalMap, normalMapScale, ss, tt);

	color C = 1;

	CalculateGIAO(C, P, Nf, camNearClip, camFarClip, 
				  0, 0, 0, 0, 0,
				  color(0), 0, 0, 0, 0, 0,
				  AOSamples, AOMaxVariation, AOMaxDist, AOConeAngle, bRenderAO,
				  AOColor, AORadiusNear, AORadiusFar, AOAngleBias, AOAttenuation, AOContrast,
				  outGI
				  );
		
	float doTransparency = ( g_transparency<1 && bRenderTransparent==0 ) ? 0 : 1;
		
	Oi = Tex2DCombineOpacity( transparencyMap , color(g_transparency,g_transparency,g_transparency), ss, tt ) * doTransparency * objVisible;

	Ci = Oi * Cs * C;

}







