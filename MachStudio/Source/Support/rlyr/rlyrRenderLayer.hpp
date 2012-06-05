/*****************************************************************************
**	rlyrRenderLayer.hpp
**
**	Keeps track of objects that can be grouped based on environment maps
**
**	By definition, an object can only belong to one environment. 
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef RLYR_RENDERLAYER_HPP
#error rlyrRenderLayer.hpp multiply included
#endif
#define RLYR_RENDERLAYER_HPP


#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include <map>

//----------------------------------------------------------------------------
// Forward References
//----------------------------------------------------------------------------
class rlyrRenderCam;
class rlyrObject;
class rlyrPassesData;
class rlyrPassesObject;
class captRenderOutputObject;
class captRenderOutputData;
class rprfPrefsObject;
class rprfPrefsData;
class pfxData;
class pfxPostEffectObject;

//----------------------------------------------------------------------------
// RenderData will hold the per-frame rendered file data for each render layer
// It will hold file location and render time.
//----------------------------------------------------------------------------
struct RenderData
{
	fsLocator m_RenderLocation;
	int m_RenderTime;
};

//----------------------------------------------------------------------------
// ObjectState class - holds information on an object and its fragments
//----------------------------------------------------------------------------
class ObjectState
{
public:
	ObjectState(){}
	bool m_bObjectActive;
	std::map<int, bool> m_FragmentState;
};

//----------------------------------------------------------------------------
// Render Layer class
//----------------------------------------------------------------------------
class rlyrRenderLayer
{
public:
	
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	rlyrRenderLayer(const nameString& i_Name, bool i_IsMasterLayer);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	rlyrRenderLayer(const nameString& i_Name, const rlyrRenderLayer& i_CopyLayer);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~rlyrRenderLayer();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetName(const nameString& i_Name);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	nameString GetName();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetParentCam(rlyrRenderCam* i_ParentCam);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	rlyrRenderCam* GetParentCam();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void GetObjectVisiblity(std::map<nameString, bool>& o_Objects);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	std::map<rlyrObject*, ObjectState*> GetObjectMap();
	
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void AddObject(rlyrObject* i_Object);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void RemoveObject(rlyrObject* i_Object);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetObjectState(rlyrObject* i_Object, bool i_Active);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	bool GetObjectState(rlyrObject* i_Object);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetFragmentState(rlyrObject* i_Object, int i_FragmentIndex, bool i_Active);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	bool GetFragmentState(rlyrObject* i_Object, int i_FragmentIndex);
	
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetFragmentActive( rlyrObject* i_Object, int i_FragmentIndex, bool i_bActive );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	bool GetFragmentActive( rlyrObject* i_Object, int i_FragmentIndex );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	bool GetActiveState();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetActiveState( bool i_IsActive );
	
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ApplyPrefs();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	const g3dPrefs::g3dRenderPrefs& rlyrRenderLayer::GetActualData();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	rprfPrefsObject* GetRenderPrefs();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetRenderPrefs(rprfPrefsData& i_NewPrefs);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	captRenderOutputObject* GetCaptureOptions();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetCaptureOptions(captRenderOutputData& i_NewOptions);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	rlyrPassesObject* GetRenderPasses();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetRenderPasses(rlyrPassesData& i_NewPasses);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	pfxPostEffectObject* GetPostEffect();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetPostEffect(pfxData& i_NewPfx);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void AddToRenderDataList(fsLocator& i_RenderLocation, int i_RenderTime);
	void AddToRenderDataList(RenderData& i_RenderData);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ClearRenderDataList();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	std::vector<RenderData>& GetRenderDataList();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void rlyrRenderLayer::UpdateRenderPassesVisbility();

private:
	nameString m_Name;
	//use a map to keep track of objects and their active state in the layer
	std::map<rlyrObject*, ObjectState*> m_ObjectStatus;
	std::vector<RenderData> m_RenderDataList;
	captRenderOutputObject* m_CaptureOptions;
	rprfPrefsObject* m_RenderPrefs;
	rlyrPassesObject* m_RenderPasses;
	pfxPostEffectObject* m_RenderPfx;
	rlyrRenderCam* m_ParentCam;
	bool m_bIsActive;
};


