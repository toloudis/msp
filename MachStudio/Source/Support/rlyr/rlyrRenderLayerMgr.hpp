/*****************************************************************************
**	rlyrRenderLayerMgr.hpp
**
**	Keeps track of objects that can be grouped based on Render layers
**
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef RLYR_RENDERLAYERMGR_HPP
#error rlyrRenderLayerMgr.hpp multiply included
#endif
#define RLYR_RENDERLAYERMGR_HPP

#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif

#ifndef RLYR_LAYERSDATA_HPP
#include "Support/rlyr/data/rlyrLayersData.hpp"
#endif

#ifndef CAPT_RENDEROUTPUTDATA_HPP
#include "Support/capt/captRenderOutputData.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//	Forward References
//============================================================================
class api3dObjectSingle;
class rlyrPassesObject;
class rlyrRenderLayer;
class rlyrRenderLayerInterest;
class rlyrRenderCam;
class nameObject;
class nameString;
class rprfPrefsObject;
class captRenderOutputObject;
class pfxPostEffectObject;
struct RenderData;

//============================================================================
//============================================================================
namespace rlyrRenderLayerMgr
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Initialize();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeInitialize();

	//------------------------------------------------------------------------
	//	Create the master layer
	//------------------------------------------------------------------------
	void CreateMasterLayer();
	void CreateMasterLayer(rlyrRenderCam* i_Cam);

	//------------------------------------------------------------------------
	//	Update the layers with the information from the document chunk
	//------------------------------------------------------------------------
	void Update();

	//------------------------------------------------------------------------
	//	Prepare the render layers and the manager for the render process
	//------------------------------------------------------------------------
	void PrepareForRender();

	//------------------------------------------------------------------------
	//	restore any changed global values
	//------------------------------------------------------------------------
	void FinalizeRender();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void AddToLayerRenderDataList(const nameString& i_LayerName, 
								  fsLocator& i_RenderLocation, int i_RenderTime);
	void AddToLayerRenderDataList(const nameString& i_LayerName, RenderData& i_RenderData);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ClearAllRenderDataList();
	void ClearLayerRenderDataList(const nameString& i_LayerName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void GetLayerRenderDataList(const nameString& i_LayerName, std::vector<RenderData>& o_DataList);

	//------------------------------------------------------------------------
	//set the layer data gathered from the document chunk
	//------------------------------------------------------------------------
	void SetData(rlyrLayersData &i_LayerData);

	//------------------------------------------------------------------------
	//return the layer data for the document chunk
	//------------------------------------------------------------------------
	void GetData(rlyrLayersData &o_LayerData);

	//------------------------------------------------------------------------
	//	merge a set of layer data from the chunk with our current layer data
	//
	//	If i_bRenameDupes is true, the duplicate entry will be renamed
	//	and merged.  If it is false, the duplicate will NOT be merged.
	//--------------------------------------------------------------------
	void MergeData(rlyrLayersData &i_LayerData, bool i_bRenameDupes = false);

	//------------------------------------------------------------------------
	//  Add named camera as a render cam for the render layer tree
	//------------------------------------------------------------------------
	void  AddRenderCam(nameObject* i_pNameObj);

	//------------------------------------------------------------------------
	//	Remove render camera from manager (removing all render layers belonging
	//		to this camera)
	//------------------------------------------------------------------------
	void RemoveRenderCam(nameObject* i_pNameObj);

	//------------------------------------------------------------------------
	// return the index of the camera with the given name
	//------------------------------------------------------------------------
	//int GetCameraIndexByName( const nameString& i_CameraName );

	//------------------------------------------------------------------------
	// return the index of the layer at the camera with the given index
	//------------------------------------------------------------------------
	int GetLayerIndexByName( const nameString& i_LayerName );

	//------------------------------------------------------------------------
	// return the number of cameras in the manager
	//------------------------------------------------------------------------
	int GetNumCameras();
	
	//------------------------------------------------------------------------
	// return the number of cameras in the manager
	//------------------------------------------------------------------------
	int GetNumRenderLayers();

	//------------------------------------------------------------------------
	// return the number of active cameras in the manager
	//------------------------------------------------------------------------
	int GetNumActiveCameras();

	//------------------------------------------------------------------------
	// return the next active camera index
	//------------------------------------------------------------------------
	//int GetNextCameraIndex(int i_CurrentCamIndex);

	//------------------------------------------------------------------------
	// return the next active layer index, for a specified camera
	//------------------------------------------------------------------------
	int GetNextLayerIndex(int i_CurrentLayerIndex);

	//------------------------------------------------------------------------
	// return whether or not there is at least 1 active render layer
	//------------------------------------------------------------------------
	bool CanRender();

	//------------------------------------------------------------------------
	//  Add a new layer to the named camera
	//------------------------------------------------------------------------
	const nameString AddRenderLayer(bool i_IsMasterLayer = false);

	//------------------------------------------------------------------------
	//  Add a new hidden layer
	//	A hidden render layer is not shown in the render options. It will
	//	only be updated when an object is added or removed from the scene
	//	Return Null if a name is already found in the layer list
	//	
	//	Note: a hidden layer can be accessed by any function in 
	//	rlyrRenderLayerMgr that uses layer name as index.
	//------------------------------------------------------------------------
	rlyrRenderLayer* AddHiddenRenderLayer(const nameString& i_HiddenLayerName);

	//------------------------------------------------------------------------
	// make a duplicate of a current render layer of a camera
	//------------------------------------------------------------------------
	const nameString CopyRenderLayer(const nameString& i_CopyLayer);

	//------------------------------------------------------------------------
	// return the name of the layer at the index
	//------------------------------------------------------------------------
	const nameString GetLayerName( int i_LayerIndex );

	//------------------------------------------------------------------------
	//  Add named objects for the render layers pool
	//------------------------------------------------------------------------
	void  AddObject(nameObject* i_pNameObj, 
				    api3dObjectSingle* i_pObject);

	//------------------------------------------------------------------------
	//	Remove object from manager (removing from all render layers)
	//------------------------------------------------------------------------
	void RemoveObject(nameObject* i_pNameObj, 
				      api3dObjectSingle* i_pObject);

	//------------------------------------------------------------------------
	//	Before rendering, get the original editor visibilty levels for each
	//	object in the scene
	//------------------------------------------------------------------------
	void GetObjectsEditorVisibility();

	//------------------------------------------------------------------------
	//	Get visibility for a single object
	//------------------------------------------------------------------------
	bool GetSingleObjectVisibility(nameString i_Name);

	//------------------------------------------------------------------------
	//  Set the editor visibility to match the visible prefs for
	//  the given layer.
	//------------------------------------------------------------------------
	void SetupLayerObjectsVisible(int i_RenderLayerIndex);

	//------------------------------------------------------------------------
	//  Set the editor visibility to match the visible prefs for
	//  the given layer name.
	//------------------------------------------------------------------------
	void SetupLayerObjectsVisible(const nameString& i_RenderLayerName);

	//------------------------------------------------------------------------
	// Return true if we have hidden some objects from a call to 
	//	SetObjectsEditorVisibility. 
	//------------------------------------------------------------------------
	bool HaveHiddenObjects();

	//------------------------------------------------------------------------
	//	after and during rendering, set the editor visibility for each object
	//	based on the given std::map pairing
	//------------------------------------------------------------------------
	void SetObjectsEditorVisibility(const std::map<nameString, bool>& i_ObjectStatus);	

	//------------------------------------------------------------------------
	//	after and during rendering, set the rendering flags for the nodes of each object
	//------------------------------------------------------------------------
	void SetFragmentVisibility(rlyrRenderLayer* i_pLayer);

	//------------------------------------------------------------------------
	//	Set fragment visibility for an object without referencing
	//	a named render layer. Used in lighting isolation.
	//------------------------------------------------------------------------
	void SetFragmentVisibility(const nameString &i_ObjectName,
							   const std::vector<int>& i_LitFragmentIndices);

	//------------------------------------------------------------------------
	//	after capture restore the editor to the default state
	//------------------------------------------------------------------------
	void RestoreEditorVisibility();

	//------------------------------------------------------------------------
	//	after capture restore the fragments to their visible state
	//------------------------------------------------------------------------
	void RestoreFragmentVisibilty();

	//------------------------------------------------------------------------
	//	after capture restore the visibility to ture
	//------------------------------------------------------------------------
	void RestoreActiveInRenderLayer();

	//------------------------------------------------------------------------
	// Get the visibility flags of the object from each layer
	//------------------------------------------------------------------------
	void GetVisibilityForObject(const nameString& i_ObjectName, std::vector<nameString>& o_LayerNames);

	//------------------------------------------------------------------------
	// Set the visibility flags of the object for each layer
	//------------------------------------------------------------------------
	void SetVisibilityForObject(const nameString& i_ObjectName, std::vector<nameString> i_LayerNames);

	//------------------------------------------------------------------------
	// Get the number of nodes for the given object
	//------------------------------------------------------------------------
	int GetObjectNumNodes(const nameString& i_ObjectName);

	//------------------------------------------------------------------------
	//	Change assignments from OldNameObj to NewNameObj (both of which
	//	should be registered with the manager at the point of calling 
	//	this function). This is used when reloading or replacing 
	//	geometry in a system.
	//------------------------------------------------------------------------
	void ReplaceObject(nameObject* i_pOldNameObj, 
					   nameObject* i_pNewNameObj);

	//------------------------------------------------------------------------
	//	Notify the environment manager that the name of this object has changed
	//------------------------------------------------------------------------
	void ObjectRenamed(nameObject* i_pNameObj);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetLayerObjectState(const nameString& i_LayerName, const nameString& i_ObjectName, bool i_Active);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool GetLayerObjectState(const nameString& i_LayerName, const nameString& i_ObjectName);

	//------------------------------------------------------------------------
	// Get the names of the fragments belonging to the object
	//------------------------------------------------------------------------
	void GetFragmentNodeNames( const nameString& i_ObjectName, std::vector<std::string>& o_NodeNames );

	//------------------------------------------------------------------------
	// Get the names of the fragments belonging to the object
	//------------------------------------------------------------------------
	void GetRenderedFragmentIndices( const nameString& i_ObjectName, std::vector<int>& o_FragmentIndices );

	//------------------------------------------------------------------------
	// Set the render state of the indexed fragment for the given object
	//------------------------------------------------------------------------
	void SetLayerFragmentState(const nameString& i_LayerName, const nameString& i_ObjectName, int i_FragmentIndex, bool i_Active);

	//------------------------------------------------------------------------
	// Get the render state of the indexed fragment for the given object
	//------------------------------------------------------------------------
	bool GetLayerFragmentState(const nameString& i_LayerName, const nameString& i_ObjectName, int i_FragmentIndex);

	//------------------------------------------------------------------------
	//	Allow the manager to set the active state of a particular layer
	//------------------------------------------------------------------------
	void SetLayerActive(const nameString& i_LayerName, bool i_IsActive);

	//------------------------------------------------------------------------
	//	Allow the manager to set the active state of all layers belonging to the camera
	//------------------------------------------------------------------------
	void SetAllLayersActive(bool i_IsActive);

	//------------------------------------------------------------------------
	//	Allow the manager to get the active state of a particular layer
	//------------------------------------------------------------------------
	bool GetLayerActive(const nameString& i_LayerName);

	//------------------------------------------------------------------------
	//	Change the name of a layer - returns true on success
	//------------------------------------------------------------------------
	bool ChangeLayerName(const nameString& i_OldName, const nameString& i_NewName);

//------------------------------------------------------------------------
// The user interface can then manipulate the groupings with the
// following functions by using the names of the lights and objects
//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	// Returns true if non-empty and unique name for a environment.
	//------------------------------------------------------------------------
	bool IsValidRenderLayerName(const std::string &i_Name);

	//------------------------------------------------------------------------
	//	Create a render layer
	//------------------------------------------------------------------------
	void CreateRenderLayer(rlyrRenderCam* i_Cam);

	//------------------------------------------------------------------------
	//	Destroy named render layers,
	//------------------------------------------------------------------------
	void  DeleteRenderLayer(const nameString& i_RemovedLayer);

//------------------------------------------------------------------------
// These functions get the current state of the environment groupings
// in order to be displayed to the user.
//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	//int GetNumRenderCams();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	//int GetNumRenderLayersFromCam();
	
	//------------------------------------------------------------------------
	//  Get names of render layers
	//------------------------------------------------------------------------
	void GetRenderLayerNames(std::vector<nameString> &o_Names);

	//------------------------------------------------------------------------
	//  Get names of objects that are enabled in the render layer. 
	//------------------------------------------------------------------------
	void GetActiveObjects(const nameString& i_RenderLayerName, 
						 std::vector<nameString> &o_ObjectNames);

	//------------------------------------------------------------------------
	//  Get list of all names of objects registered in the manager
	//------------------------------------------------------------------------
	void GetAllObjects(std::vector<nameString> &o_ObjectNames);

	//------------------------------------------------------------------------
	//  Get names of cameras
	//------------------------------------------------------------------------
	//void GetRenderCamNames(std::vector<nameString> &o_Names);

	//------------------------------------------------------------------------
	//  Get the render prefs of a layer
	//------------------------------------------------------------------------
	rprfPrefsObject* GetLayerRenderPrefs(const nameString& i_RenderLayer);

	//------------------------------------------------------------------------
	//  Get the render passes of a layer
	//------------------------------------------------------------------------
	rlyrPassesObject* GetLayerRenderPasses(const nameString& i_RenderLayer);

	//------------------------------------------------------------------------
	//  Get the post effect object of a layer
	//------------------------------------------------------------------------
	pfxPostEffectObject* GetLayerRenderPfx(const nameString& i_RenderLayer);

	//------------------------------------------------------------------------
	//  Apply the prefs of the layer to the scene
	//------------------------------------------------------------------------
	void ApplyLayerPrefs(int i_RenderLayerIndex);

	//------------------------------------------------------------------------
	//  Apply the prefs of the layer to the scene by given layer name
	//------------------------------------------------------------------------
	void ApplyLayerPrefs(nameString i_RenderLayerName);

	//------------------------------------------------------------------------
	//  return the g3dPrefs of the layer to the scene
	//------------------------------------------------------------------------
	const g3dPrefs::g3dRenderPrefs& GetLayerActualData(int i_RenderLayerIndex);

	//------------------------------------------------------------------------
	//  Set the render prefs of a layer
	//------------------------------------------------------------------------
	void SetLayerRenderPrefs(const nameString& i_RenderLayer, rprfPrefsData& i_NewRenderPrefs);

	//------------------------------------------------------------------------
	//  Set the render passes of a layer
	//------------------------------------------------------------------------
	void SetLayerRenderPasses( const nameString& i_RenderLayer, rlyrPassesData& i_NewRenderPasses);

	//------------------------------------------------------------------------
	//  Set the pfx data of a layer
	//------------------------------------------------------------------------
	void SetLayerPfxData( const nameString& i_RenderLayer, pfxData& i_pfxData);

	//------------------------------------------------------------------------
	//  Get the capture options of a layer
	//------------------------------------------------------------------------
	captRenderOutputObject* GetLayerCaptureOptions(const nameString& i_RenderLayer);

	//------------------------------------------------------------------------
	//  set the capture options of a layer
	//------------------------------------------------------------------------
	void SetLayerCaptureOptions(const nameString& i_RenderLayer, captRenderOutputData& i_NewCaptureData);

	//------------------------------------------------------------------------
	//  apply the individual capture options of the layer to the 
	//	global options
	//------------------------------------------------------------------------
	void ApplyLayerCaptureOptions(int i_RenderLayerIndex);

	//------------------------------------------------------------------------
	//  setup controls for the layers capture dialog
	//------------------------------------------------------------------------
	void SetupLayerCaptureControls(const nameString& i_RenderLayer);

	//------------------------------------------------------------------------
	//  Get the list of render cams
	//------------------------------------------------------------------------
	std::vector<rlyrRenderCam*> GetRenderCams();

	//------------------------------------------------------------------------
	//  Get list of all names of render layers registered in the manager
	//------------------------------------------------------------------------
	std::vector<rlyrRenderLayer*> GetRenderLayers();

//------------------------------------------------------------------------
//	data changed interest functions
//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	RegisterRenderLayerInterest() - add a Render layer interest 
	//------------------------------------------------------------------------
	void RegisterRenderLayerInterest( rlyrRenderLayerInterest* i_pInterest );

	//------------------------------------------------------------------------
	//	UnRegisterRenderLayerInterest() - remove a render layer interest 
	//
	//	Note: this will NOT delete the render layer interest.  It is up to the
	//	registerer.
	//------------------------------------------------------------------------
	void UnRegisterRenderLayerInterest( rlyrRenderLayerInterest* i_pInterest );

}	// end of namespace

