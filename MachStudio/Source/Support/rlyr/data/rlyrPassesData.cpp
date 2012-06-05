/********************************************************************************************\
**  rlyrLayersData.cpp
**
**		See .hpp for details
**
**  studio|gpu
\********************************************************************************************/
#include "Support/rlyr/data/rlyrPassesData.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
rlyrPassesData::rlyrPassesData()
:	m_Beauty("Beauty", true),
	m_AOOnly("Ambient Occlusion"),
	m_Depth("Depth"),
	m_ShadowMask("Shadow Mask"),
	m_IlluminationOnly("Illumination"),
	m_Normals("Normals"),
	m_DirtyMatte("Dirty Matte"),
	m_Wireframe("Wireframe"),
	m_Materials("Materials"),
	m_ReflectionsOnly("Reflections"),
	m_Velocity("Velocity"),
	m_Diffuse("Diffuse"),
	m_Specular("Specular"),
	m_Bloom("Bloom"),
	m_Star("Star"),
	m_CameraDOF("Camera DOF"),
	m_Preview("Preview"),
	m_Emissive("Emissive"),
	m_SpecEnv("Specular Environment"),
	m_SpecLit("Specular Lights"),
	m_DiffEnv("Diffuse Environment"),
	m_DiffLit("Diffuse Lights"),
	m_GI("Global Illumination"),
	m_Glow("Glow"),
	m_RmanColorBleed("Color Bleeding"),
	m_MrayFinalGather("Final Gather")
{
}
