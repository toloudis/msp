/*****************************************************************************
**	cptrRenderBakeDataUtil.hpp
**
**		API for accessing the capture bake data
**		
**		Note: bake data contains a renderlayer plus a bakeData. Renderlayer
**		is used to maintain the objects that need to be rendered, and bake 
**		data is for bake preference
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDERBAKEDATAUTIL_HPP
#error cptrRenderBakeDataUtil.hpp multiply included
#endif
#define CPTR_RENDERBAKEDATAUTIL_HPP

#ifndef CPTR_RENDERBAKEDATA_HPP
#include "Features/Capture/cptrRenderBakeData.hpp"
#endif

#ifndef RLYR_PASSESOBJECT_HPP
#include "Support/rlyr/rlyrPassesObject.hpp"
#endif

class nameString;
class rlyrRenderLayer;
//class rlyrPassesObject;

//============================================================================
//============================================================================
namespace cptrRenderBakeDataUtil
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Init();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Cleanup();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReadData( const fsLocator& i_ConfigFile,
					cptrRenderBakeData& o_Data );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void WriteData( const fsLocator& i_ConfigFile,
					const cptrRenderBakeData& i_Data );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateData(cptrRenderBakeData& i_NewData);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cptrRenderBakeData& Data();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	rlyrRenderLayer* GetBakeLayer();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	nameString GetBakeLayerName();

	//------------------------------------------------------------------------
	//	ApplyBakeDataOnRenderLayer copy bake data into renderlayer pref
	//	data.
	//------------------------------------------------------------------------
	void ApplyBakeDataOnRenderLayer();

	//------------------------------------------------------------------------
	//	ApplyBakeDataOnRenderPasses copy bake data into renderlayer passes
	//	data.
	//------------------------------------------------------------------------
	void ApplyBakeDataOnRenderPasses();

	//------------------------------------------------------------------------
	//	AdjustRenderPref adjust renderpasstype in render pref
	//------------------------------------------------------------------------
	void AdjustRenderPref(rlyrPassesObject::ePassType type);

}	// end of namespace
