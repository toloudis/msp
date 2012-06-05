/*****************************************************************************
**	api3dBillboard.hpp
**
**	Derived class, implements api3dObject with a textured polygon that
**	always faces the camera.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef API3D_BILLBOARD_HPP
#error api3dBillboard.hpp multiply included
#endif
#define API3D_BILLBOARD_HPP

#ifndef API3D_OBJECTSINGLE_HPP
#include "Tool/api3d/api3dObjectSingle.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif

//============================================================================
//============================================================================
class scBillboard;
class matTexture;
class camCamera;

//============================================================================
//============================================================================
class api3dBillboard : public api3dObjectSingle
{
public:
	//--------------------------------------------------------------------
	// constructor. The billboard adds a reference to the texture
	// and releases in in the destructor
	//--------------------------------------------------------------------
	api3dBillboard(matTexture *i_pTexture);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~api3dBillboard();

	//--------------------------------------------------------------------
	// Position
	//--------------------------------------------------------------------
	virtual maPoint3d  GetPosition() const;
	virtual void  SetPosition(const maPoint3d &i_Position);

	//--------------------------------------------------------------------
	// Orientation
	//--------------------------------------------------------------------
	virtual maRotation GetOrientation() const;
	virtual void  SetOrientation(const maRotation &i_Rotation);

	//--------------------------------------------------------------------
	// Scale
	//--------------------------------------------------------------------
	virtual maVector3d GetScale() const;
	virtual void  SetScale(const maVector3d& i_Scale);
	virtual void  SetUniformScale(const float i_fScale);

	//--------------------------------------------------------------------
	//	Renderable sets whether the object is visible
	//--------------------------------------------------------------------
	void SetRenderable(bool i_Renderable);
	bool GetRenderable() const;

	//--------------------------------------------------------------------
	//	ActiveInRenderLayer sets whether the object is visible
	//	in render layer
	//--------------------------------------------------------------------
	void SetActiveInRenderLayer(bool i_bRenderable);
	bool GetActiveInRenderLayer() const;

	//--------------------------------------------------------------------
	//	ActiveInSceneMgr sets whether the object is visible
	//	in scene manager
	//--------------------------------------------------------------------
	void SetActiveInSceneMgr(bool i_bRenderable);
	bool GetActiveInSceneMgr() const;

	//--------------------------------------------------------------------
	// If OrientToCamera is true (the default) the billboard will
	// rotate to face the camera.
	//--------------------------------------------------------------------
	bool GetOrientToCamera() const;
	void SetOrientToCamera(bool i_bOrient);

	//--------------------------------------------------------------------
	// If SnapToCamera is true the billboard will snap to camera view
	// and scale to the camera aspect
	//--------------------------------------------------------------------
	bool GetSnapToCamera() const;
	void SetSnapToCamera(bool i_bSnap);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetBrightness(float i_Brightness);

	//--------------------------------------------------------------------
	// Set/Get billboard distance to camera
	//--------------------------------------------------------------------
	float GetDistToCamera() const;
	void SetDistToCamera(float i_Dist);

	//--------------------------------------------------------------------
	// Set camera to attach on
	//--------------------------------------------------------------------
	void SetCamera(shared_ptr<camCamera> i_Camera);

	//--------------------------------------------------------------------
	// Set/Get additive flag on billboard's material
	//--------------------------------------------------------------------
	bool GetAdditiveMaterial() const;
	void SetAdditiveMaterial(bool i_bAdditive);

	//--------------------------------------------------------------------
	// Set/Get chroma key End for background removal
	//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	//	GetWorldBox returns the bounding box
	//--------------------------------------------------------------------
	const maAxisBox& GetWorldBox() const;

	//--------------------------------------------------------------------
	// Set color (emissive, not diffuse) for fragments
	//--------------------------------------------------------------------
	void SetColor(const maFloatRGBA &i_Color);

	//--------------------------------------------------------------------
	// Set is chroma key active
	//--------------------------------------------------------------------
	void SetCKActive(bool i_bIsActive);

	//--------------------------------------------------------------------
	// Set chroma key color
	//--------------------------------------------------------------------
	void SetCKColor(const maFloatRGBA &i_Color);

	//--------------------------------------------------------------------
	// Set chroma key tolerance
	//--------------------------------------------------------------------
	void SetCKTolerance(float i_Value);

	//--------------------------------------------------------------------
	// Set is removing spill 
	//--------------------------------------------------------------------
	void SetCKRemoveSpill(bool i_bIsActive);

	//--------------------------------------------------------------------
	// Set removing spill type
	//--------------------------------------------------------------------
	void SetCKSpillType(int i_Type);

	//--------------------------------------------------------------------
	// Set removing spill bias
	//--------------------------------------------------------------------
	void SetCKSpillBias(float i_Value);

	//--------------------------------------------------------------------
	// Set chroma key edge blurring width
	//--------------------------------------------------------------------
	void SetCKEdgeBlur(int i_Width);

	//--------------------------------------------------------------------
	// Set texture for billboard. The billboard will increment the
	// reference on the texture and will release the old texture.
	//--------------------------------------------------------------------
	void SetTexture(matTexture *i_pTexture);

	//--------------------------------------------------------------------
	// Return pointer to texture for billboard. 
	//--------------------------------------------------------------------
	matTexture* GetTexture() const;

	//--------------------------------------------------------------------
	//	Get reference for given name.  The returned pointer is owned
	//	by this object.  The object should be retained by the caller
	//	to avoid repeated string searches.
	//--------------------------------------------------------------------
	virtual api3dReference* GetReference(const char* i_Name);

	//--------------------------------------------------------------------
	// Get list of references
	//--------------------------------------------------------------------
	virtual void GetReferenceList(std::vector<std::string> &o_List);

	//----------------------------------------------------------------------------
	//	get a pointer to the scene object
	//----------------------------------------------------------------------------
	virtual const scObject* GetObject() const;
	virtual scObject* Object();

private:
	scBillboard *m_pObject;
	matTexture *m_pTexture;
};