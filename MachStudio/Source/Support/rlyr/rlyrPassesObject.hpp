/********************************************************************************************\
** rlyrPassesObject.hpp
**
**		Property object for render passes of a render layer
**
**  studio|gpu
\********************************************************************************************/
#pragma once

#ifdef RLYR_PASSESOBJECT_HPP
#error rlyrPassesObject.hpp multiply included
#endif
#define RLYR_PASSESOBJECT_HPP

#ifndef RLYR_PASSESDATA_HPP
#include "Support/rlyr/data/rlyrPassesData.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif

// fwd decl
namespace g3dPrefs
{
	struct g3dRenderPrefs;
}

class rlyrPassesObject : public prtyObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	rlyrPassesObject();
	
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	rlyrPassesObject(const rlyrPassesData& i_Data);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	enum ePassType
	{
		e_Beauty,
		e_Diffuse,
		e_Specular,
		e_AOOnly,
		e_Depth,
		e_ShadowMask,
		e_IlluminationOnly,
		e_Normals,
		e_DirtyMatte,
		e_Wireframe,
		e_Materials,
		e_ReflectionsOnly,
		e_Velocity,
		e_Bloom,
		e_Star,
		e_CameraDOF,
		e_Preview,
		e_Emissive,
		e_SpecEnv,
		e_SpecLit,
		e_DiffEnv,
		e_DiffLit,
		e_GlobalIllumination,
		e_Glow,
		e_RmanColorBleed,
		e_MrayFinalGather
	};

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void CollectPasses(std::vector<ePassType>& o_Passes);

	//--------------------------------------------------------------------
	// Modify prefs flags relevant to the given pass type
	//--------------------------------------------------------------------
	static void AdjustPrefs(ePassType i_Pass, g3dPrefs::g3dRenderPrefs& o_Prefs);
	
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	rlyrPassesData GetPassesData();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void UpdateVisibility(int i_RenderEngine);

private:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Init();

	//void AddCallbacks();
	
	//--------------------------------------------------------------------
	// Callbacks for when properties change, updates member data
	//--------------------------------------------------------------------
	//virtual void UpdateBeauty(prtyProperty *i_pProperty, bool i_bDirty);

public:
	rlyrPassesData m_Data;
};

