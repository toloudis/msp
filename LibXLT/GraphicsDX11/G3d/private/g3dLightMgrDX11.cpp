/****************************************************************************\
**  g3dLightMgrDX11.cpp
**
**	The g3dLightMgrDX11 component contains the Windows (Direct3D)
**	implementation of the g3dLightMgr.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/g2d/g2dResetHandler.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dJitterSettings.hpp"
#include "Graphics/g3d/g3dPointLight.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dSpotLight.hpp"
#include "GraphicsDX11/g3d/g3dHelpersWin.hpp"

#include <algorithm>
#include <functional>
#include <vector>


//------------------------------------------------------------------------
//	anonymous namespace for local data/functions
//------------------------------------------------------------------------
namespace
{

	//------------------------------------------------------------------------
	//  film_light_struct
	//------------------------------------------------------------------------
	//not used
//	void film_light_struct( const g3dLight* i_Light, g2dD3D11LIGHT& o_LightStruct, float int_mod = 1.0f, float pass = -1.0f)
}

//------------------------------------------------------------------------
//	Don't call Init() and CleanUp() yourself they are called
//	by the package Init and Cleanup.
//------------------------------------------------------------------------
g3dLightMgrDX11::g3dLightMgrDX11()
:	m_nLightsEnabled(0), 
	m_pHeadlight(NULL),
	m_bHeadlightEnabled(false)
{
	m_Ambient.Set(1.0f, 1.0f, 1.0f, 1.0f);
//	g2dResetHandler::AddResetHandler(new g3dLightResetter(m_LightMap, m_Ambient));

	m_pHeadlight = new g3dDirectionalLight();
	m_pHeadlight->Disable();
	m_pHeadlight->SetSpecularEnabled(false);
	m_HeadlightList.push_back(m_pHeadlight); // dummy light list for when headlight is enabled

}
g3dLightMgrDX11::~g3dLightMgrDX11()
{
	delete m_pHeadlight;
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
//static
g3dLightMgrDX11* g3dLightMgrDX11::Implementation()
{
	return dynamic_cast<g3dLightMgrDX11*>(g3dLightMgr::Implementation());
}

//------------------------------------------------------------------------
//	CreatePointLight creates a point light object.  This object can
//	be manipulated by the client (including enabling/disabling) to change
//	the properties of the light.  A point light has a isotropic influence
//	in a limited area.
//------------------------------------------------------------------------
g3dPointLight* g3dLightMgrDX11::CreatePointLight()
{
	g3dPointLight* ret_val = new g3dPointLight;

	m_Lights.push_back(ret_val);
	m_Enabled[ret_val] = false;

	return ret_val;
}

//------------------------------------------------------------------------
//	CreateDirectionalLight creates a directional light object.  A
//	directional light has no range limit and always radiates in one
//	direction.
//------------------------------------------------------------------------
g3dDirectionalLight* g3dLightMgrDX11::CreateDirectionalLight()
{
	g3dDirectionalLight* ret_val = new g3dDirectionalLight;

	m_Lights.push_back(ret_val);
	m_Enabled[ret_val] = false;

	return ret_val;
}

//------------------------------------------------------------------------
//	CreateSpotLight creates a spot light object.  This object can
//	be manipulated by the client (including enabling/disabling) to change
//	the properties of the light.
//------------------------------------------------------------------------
g3dSpotLight* g3dLightMgrDX11::CreateSpotLight()
{
	g3dSpotLight* ret_val = new g3dSpotLight;

	m_Lights.push_back(ret_val);
	m_Enabled[ret_val] = false;

	return ret_val;
}


//------------------------------------------------------------------------
//	CreateProjectedLight creates a projected light object.  This object 
//	projects a texture along a direction like a slide projector.
//------------------------------------------------------------------------
g3dProjectedLight* g3dLightMgrDX11::CreateProjectedLight()
{
	g3dProjectedLight* ret_val = new g3dProjectedLight();

	m_Lights.push_back(ret_val);
	m_Enabled[ret_val] = false;

	return ret_val;
}

//------------------------------------------------------------------------
//	DestroyLight should be called to destroy any light object (no matter
//	which function created it).
//------------------------------------------------------------------------
void g3dLightMgrDX11::DestroyLight( g3dLight* i_pLight )
{
	m_Enabled.erase( i_pLight );

	bool found = envSTLHelpers::RemoveOneValue(m_Lights, i_pLight);

#ifdef _DEBUG
	//	if for DEBUGGING only!
	if (!found)
	{
		int x = 100;
	}
#endif
	DBG_ASSERT( found, "Light not found" );

	delete i_pLight;
}

//------------------------------------------------------------------------
//	SetAmbient sets the global ambient light color for the scene.
//------------------------------------------------------------------------
void g3dLightMgrDX11::SetAmbient(const maFloatRGBA& i_Color)
{
	m_Ambient = i_Color;
}


//------------------------------------------------------------------------
//	For some passes, it is desirable to turn off the ambient.
//  This allows the renderer to have control over the current
//	ambient setting without altering the ambient color itself.
//------------------------------------------------------------------------
void g3dLightMgrDX11::EnableAmbient(bool i_bEnable)
{
}


//------------------------------------------------------------------------
//	GetMaxLights returns the maximum number of lights that can be
//	displayed by the hardware.  If more lights than this number are
//	enabled, they will probably not affect the scene.
//------------------------------------------------------------------------
int g3dLightMgrDX11::GetMaxLights()
{
	return 1000;
}


//------------------------------------------------------------------------
//	SetLight
//------------------------------------------------------------------------
void g3dLightMgrDX11::SetLight( g3dLight* i_pLight )
{
}

//------------------------------------------------------------------------
//	SetLightJittered
//------------------------------------------------------------------------
void g3dLightMgrDX11::SetLightJittered( g3dLight* i_pLight, int i_Pass )
{
	int passes = g3dJitterSettings::GetNumJitterPasses();
	float intensity_mod = 1.0f / float(passes);
}

//------------------------------------------------------------------------
//	EnableLight
//------------------------------------------------------------------------
void g3dLightMgrDX11::EnableLight( g3dLight* i_pLight )
{
	if (!m_Enabled[i_pLight])
	{
		++m_nLightsEnabled;
		m_Enabled[i_pLight] = true;
	}
}

//------------------------------------------------------------------------
//	DisableLight
//------------------------------------------------------------------------
void g3dLightMgrDX11::DisableLight( g3dLight* i_pLight )
{
	if (m_Enabled[i_pLight])
	{
		--m_nLightsEnabled;
		m_Enabled[i_pLight] = false;
	}
}

//------------------------------------------------------------------------
//	DisableAllLights - disables all d3d lights and sets the ambient to black
//------------------------------------------------------------------------
void g3dLightMgrDX11::DisableAllLights()
{
	std::vector<g3dLight*>::iterator it = m_Lights.begin(), end = m_Lights.end();
	for ( ; it != end; ++it )
	{
		DisableLight( *it );
	}

	EnableAmbient(false);
}

//------------------------------------------------------------------------
// Returns direction of light from light that has biggest
// influence on the given point.  This could be used to
// determine direction of shadows or bump mapping.
//------------------------------------------------------------------------
maVector3d g3dLightMgrDX11::GetPrimaryLightDirection(const maPoint3d &i_Pos)
{
	maVector3d dir(0,0,0);

	maFloatRGBA intensity;
	g3dPointLight *point_light;
	g3dSpotLight *spot_light;
	g3dDirectionalLight *dir_light;
	float contrib, dist, dist_sq, f0, f1, f2, atten;
	float best_contrib = 0;
	g3dLight* light;
	for (int it = 0; it < m_Lights.size(); ++it )
	{
		light = m_Lights[it];
		intensity =  light->GetScaledIntensity();
		dir_light = dynamic_cast<g3dDirectionalLight*>(light);
		point_light = dynamic_cast<g3dPointLight*>(light);
		spot_light = dynamic_cast<g3dSpotLight*>(light);
		if (point_light)
		{
			dist_sq = (i_Pos - point_light->GetPosition()).LengthSqr();
			dist = sqrtf(dist_sq);
			if (dist > point_light->GetRange())
				continue;

			if (spot_light)
			{
				dir = i_Pos - spot_light->GetPosition();
				dir.Normalize();
				float angle_between = fabs(dir * spot_light->GetDirection());
				if ( angle_between > spot_light->GetOuterAngle() )
					continue;
			}

			f0 = point_light->GetFalloff0();
			f1 = point_light->GetFalloff1();
			f2 = point_light->GetFalloff2();

			atten = 1.0f / (f0 + f1 * dist * f2 * dist_sq);
			intensity *= atten;
		}
		else if (!dir_light)
			continue; // only point, spot, or directional lights

		contrib = intensity.GetRed() + intensity.GetGreen() + intensity.GetBlue();

		if (contrib > best_contrib)
		{
			best_contrib = contrib;
			if (spot_light)
			{
				// already calculated the dir
			}
			else if (point_light)
			{
				dir = i_Pos - point_light->GetPosition();
				dir.Normalize();
			}
			else if (dir_light)
			{
				dir = dir_light->GetDirection();
			}
		}
	}

	return dir;
}

//------------------------------------------------------------------------
// Gets list of all lights
//------------------------------------------------------------------------
//void g3dLightMgrDX11::GetLights(std::vector<g3dLight*> &o_Lights)
//{
//	o_Lights = m_Lights;
//}
const std::vector<g3dLight*>& g3dLightMgrDX11::GetLights()
{
	if (m_bHeadlightEnabled)
		return m_HeadlightList;
	else
		return m_Lights;
}


//------------------------------------------------------------------------
// Gets list of all lights that are currently enabled
//------------------------------------------------------------------------
void g3dLightMgrDX11::GetEnabledLights(std::vector<g3dLight*> &o_Lights)
{
	o_Lights.clear();
	
	// Enabling the headlight turns off all other lights
	if (m_bHeadlightEnabled)
	{
		o_Lights.push_back(m_pHeadlight);
	}
	else
	{
		for (int i = 0; i < m_Lights.size(); i++)
		{
			//if (m_Enabled[m_Lights[i]])
			if (m_Lights[i]->IsEnabled())
			{
				o_Lights.push_back(m_Lights[i]);
			}
		}
	}
}

//------------------------------------------------------------------------
//	EnableOnlyNonShadowLights causes only non-shadow lights and ambient
//	light to be D3D-enabled.
//------------------------------------------------------------------------
void g3dLightMgrDX11::EnableOnlyNonShadowLights()
{
	for (int i = 0; i < m_Lights.size(); i++)
	{
		g3dLight* pLight = m_Lights[i];

		if( pLight->GetCastsShadow() )
		{
			DisableLight( pLight );
		}
		else if( pLight->IsEnabled() )
		{
			EnableLight( pLight );
		}
	}

	//	set ambient to full value
	SetAmbient( m_Ambient );
}

//------------------------------------------------------------------------
//	CountNumShadowLights returns the number of enabled shadow lights in
//	the scene.
//------------------------------------------------------------------------
int g3dLightMgrDX11::CountNumShadowLights()
{
	if (m_bHeadlightEnabled)
		return 0;

	int count = 0;
	for (int i = 0; i < m_Lights.size(); i++)
	{
		if( m_Lights[i]->IsEnabled() && m_Lights[i]->GetCastsShadow() )
		{
			count++;
		}
	}

	return count;
}

//------------------------------------------------------------------------
//	EnableOnlyShadowLight causes only the shadow light with the given
//	number to be enabled (ambient is disabled too).  The light is
//	returned also.
//------------------------------------------------------------------------
g3dLight* g3dLightMgrDX11::EnableOnlyShadowLight( int i_Num )
{
	int count = 0;

	g3dLight* ret_val = NULL;
	for (int i = 0; i < m_Lights.size(); i++)
	{
		g3dLight* pLight = m_Lights[i];

		bool shadow = pLight->GetCastsShadow() && pLight->IsEnabled();

		if( shadow && ( count == i_Num ) )
		{
			EnableLight( pLight );
			ret_val = pLight;
			break;
		}
		else
		{
			DisableLight( pLight );
		}

		if( shadow )
		{
			++count;
		}
	}
	return ret_val;
}

//------------------------------------------------------------------------
// returns  global ambient light color for the scene.
//------------------------------------------------------------------------
const maFloatRGBA& g3dLightMgrDX11::GetAmbient()
{
	return m_Ambient;
}

//------------------------------------------------------------------------
// Enable or disable a directional light that faces in the direction 
// of the camera view. Enabling the headlight also turns off all 
// other lights.
//------------------------------------------------------------------------
void g3dLightMgrDX11::EnableHeadlight(bool i_bEnable)
{
	m_bHeadlightEnabled = i_bEnable;
	m_pHeadlight->SetEnable(i_bEnable);
}
bool g3dLightMgrDX11::IsHeadlightEnabled() const
{
	return m_bHeadlightEnabled;
}

//------------------------------------------------------------------------
// Sets direction of the headlight - the directional light that
//	faces in the direction of the camera view.
//------------------------------------------------------------------------
void g3dLightMgrDX11::SetHeadlightDirection( const maVector3d& i_LightDir )
{
	m_pHeadlight->SetDirection( i_LightDir );
}


