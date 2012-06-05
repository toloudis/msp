/*****************************************************************************
**  mspLighting.hpp
**
**      Lighting rig for viewing the shader ball
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef MSP_LIGHTING_HPP
#error mspCommands.hpp multiply included
#endif
#define MSP_LIGHTING_HPP

//============================================================================
//============================================================================
class api3dProjectedLightWrapper;
struct g3dAmbientEnvState;
class g3dPointLight;
class g3dProjectedLight;
struct g3dRenderState;
class g3dSceneRenderer;
class matTexture;


//============================================================================
//	mspLighting
//============================================================================
class mspLighting
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	mspLighting();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~mspLighting();

	//--------------------------------------------------------------------
	// Set up projected light shadow buffers
	//--------------------------------------------------------------------
	void OrientLights();

private:
	g3dPointLight*				m_PointLight;
	g3dProjectedLight*			m_ProjectedLight1;
	api3dProjectedLightWrapper* m_ProjectedLightWrapper1;
	g3dProjectedLight*			m_ProjectedLight2;
	api3dProjectedLightWrapper* m_ProjectedLightWrapper2;
	matTexture*					m_LightTexture;
	g3dRenderState*				m_StateRoot;
	g3dAmbientEnvState*			m_AmbientEnvironment;
};


