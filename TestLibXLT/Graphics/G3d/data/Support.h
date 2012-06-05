/*****************************************************************************
**  Support.h
**
**      Support functions for .fx functions
**
**	Gigawatt Studios
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

/*********** support data ******/

struct LightInfo
{
	float4 Pos;
	float4 Diffuse;
	float4 Specular;
	float3 Falloff;
};

/*********** support functions ******/

float attenuation(float3 Pw,		// Position of vertex in world coords
				  LightInfo light)
{
	float atten = 1;
	if (light.Pos.w > 0.5) // only point lights (w == 1)
	{
		float d = distance(Pw, light.Pos.xyz);
		atten = 1 / (light.Falloff.x + light.Falloff.y * d + light.Falloff.z * d * d);
	}
	return atten;
}

void diffuse_contrib(LightInfo lightInfo,
						float3 Pw,			// Position of vertex in world coords
						float3 Nn,			// Normalize vertex normal
						float3 V,			// Eye position, world - vertex position
						float shininess,	// Specular power
						
						out float4 diffContrib,
						out float4 specContrib)
{    
	float atten = attenuation(Pw, lightInfo);
    float3 Ln = normalize(lightInfo.Pos - mul(Pw, lightInfo.Pos.w));
    float ldn = dot(Ln,Nn);
    float diffComp = max(0,ldn) * atten;
    
    diffContrib = diffComp * lightInfo.Diffuse;
    diffContrib.w = 1.0;
	
	float3 H = normalize(Ln + V);
	float specComp = pow(max(dot(Nn, H), 0), shininess);
	if (diffComp <= 0) specComp = 0;
	specContrib = specComp * lightInfo.Specular;
    specContrib.w = 0.0;
	//specContrib = float4(0,0,0, 1);
	
    //return float4(diffComp,diffComp,diffComp,1);
}   
    
float3 compress(float3 v)
{
	return 0.5 * v + 0.5.xxx;
}
float3 expand(float4 v)
{
	float4 v4 = 2.0*(v-0.5);
	return v4.xyz;
}
float3 expand(float3 v)
{
	return 2.0*(v-0.5);
}

/***************************** eof ***/
