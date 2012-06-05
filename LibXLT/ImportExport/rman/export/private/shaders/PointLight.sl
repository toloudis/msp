/*****************************************************************************
**  PointLight.sl
**
**      MSP Point light shader
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support.hsl"

//--------------------------------------------------------------------
// PointLight()
//--------------------------------------------------------------------
light 
PointLight(
        float intensity = 1;
        point pos = point "shader" (0,0,0);
        color lightColor = 0.5;
		float falloff_x = 0;
		float falloff_y = 0;
		float falloff_z = 0;
		float falloff_w = 0;
        float falloffStart = 0;
		output float bEnableLight = 1;
		output float bEnableDiffuse = 1;
		output float bEnableSpecular = 1;
		output float bAffectsGlow = 1;
		float shadowsOnly = 0;
        )
{
    illuminate(pos)
    {
		float d = length(L);
		float atten = attenuation( d,falloffStart,falloff_x,falloff_y,falloff_z,falloff_w);
        Cl = intensity * lightColor * atten;

		if ( shadowsOnly == 1 )
			Cl = 0;
    }
}
