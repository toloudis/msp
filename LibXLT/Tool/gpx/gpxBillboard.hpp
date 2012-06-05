/*****************************************************************************
**	gpxBillboard.hpp
**
**	This class is a thread-safe proxy for a api3dBillboard.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_BILLBOARD_HPP
#error gpxBillboard.hpp multiply included
#endif
#define GPX_BILLBOARD_HPP

#ifndef GPX_SCENEOBJECT_HPP
#include "Tool/gpx/gpxSceneObject.hpp"
#endif 

#ifndef API3D_BILLBOARD_HPP
#include "Tool/api3d/api3dBillboard.hpp"
#endif 


//============================================================================
//============================================================================
class gpxBillboard : public gpxSceneObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to object it will control
	//--------------------------------------------------------------------
	explicit gpxBillboard(api3dBillboard &i_Object);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxBillboard();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the character when "Update()" is called.
	//--------------------------------------------------------------------
	void SetTexture(matTexture *i_pTexture);
	void SetAdditiveMaterial(bool i_bAdditive);
	void SetOrientToCamera(bool i_bOrient);
	void SetSnapToCamera(bool i_bSnap);
	void SetDistToCamera(float i_Value);
	void SetCamera(shared_ptr<camCamera> i_Camera);
	void SetBrightness(float i_Brightness);

	void SetCKActive(bool i_bIsActive);
	void SetCKColor(const maFloatRGBA &i_Color);
	void SetCKTolerance(float i_Value);
	void SetCKRemoveSpill(bool i_bIsActive);
	void SetCKSpillType(int i_Type);
	void SetCKSpillBias(float i_Value);
	void SetCKEdgeBlur(int i_Width);

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	api3dBillboard &m_Billboard;

#if USE_PROXIES
	matTexture *m_pTexture;
	bool m_bAdditiveMaterial;
	float m_Brightness;
	bool m_bOrientToCamera;
	bool m_bSnapToCamera;
	float m_DistToCamera;

	bool m_bCKActive;
	maFloatRGBA m_CKColor;
	float m_CKTolerance;
	bool m_bCKRemoveSpill;
	int m_CKSpillType;
	float m_CKSpillBias;
	int m_CKEdgeBlur;
	shared_ptr<camCamera> m_Camera;
#endif
};
