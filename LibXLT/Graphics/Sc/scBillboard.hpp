/*****************************************************************************
**	scBillboard.hpp
**
**		scBillboard represents a two poly model with a texture that faces
**	the camera
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SC_BILLBOARD_HPP
#error scBillboard.hpp multiply included
#endif
#define SC_BILLBOARD_HPP

#ifndef EFF_PHONGDATA_HPP
#include "Graphics/eff/effPhongData.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef SC_OBJECT_HPP
#include "Graphics/sc/scObject.hpp"
#endif


//====================================================================
//	Forward References
//====================================================================
class g3dFragment;
class camCamera;


//====================================================================
//====================================================================
class scBillboard : public scObject
{
	public:
		//--------------------------------------------------------------------
		// Constructor
		//--------------------------------------------------------------------
		scBillboard();

		//--------------------------------------------------------------------
		// Destructor
		//--------------------------------------------------------------------
		virtual ~scBillboard();

		//--------------------------------------------------------------------
		//	SetCameraPosition - updates all billboards before rendering
		//--------------------------------------------------------------------
		static void SetCameraPosition( const camCamera &i_Camera );

		//--------------------------------------------------------------------
		//	OrientBillboard - turns this billboard to face 
		//		this camera position
		//--------------------------------------------------------------------
		void OrientBillboard( const maPoint3d &i_CameraPos );

		//--------------------------------------------------------------------
		//	OrientBillboard - turns this billboard to face 
		//		this camera position
		//--------------------------------------------------------------------
		void SnapBillboard();

		//--------------------------------------------------------------------
		//	GetMaterial returns the material of the object
		//--------------------------------------------------------------------
		inline matMaterial& GetMaterial();

		//--------------------------------------------------------------------
		// If OrientToCamera is true (the default) the billboard will
		// rotate to face the camera.
		//--------------------------------------------------------------------
		inline bool GetOrientToCamera() const;
		void SetOrientToCamera(bool i_bOrient);

		//--------------------------------------------------------------------
		// If SnapToCamera is true the billboard will snap to camera view
		// and scale to the camera aspect
		//--------------------------------------------------------------------
		inline bool GetSnapToCamera() const;
		void SetSnapToCamera(bool i_bSnap);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline float GetBrightness() const;
		void SetBrightness(float i_Brightness);

		//--------------------------------------------------------------------
		// Set/Get billboard distance to camera
		//--------------------------------------------------------------------
		inline float GetDistToCamera() const;
		void SetDistToCamera(float i_Dist);

		//--------------------------------------------------------------------
		// Set attached camera
		//--------------------------------------------------------------------
		void SetCamera(shared_ptr<camCamera> i_Cam);

	private:
		g3dFragment* m_pFragment;
		matMaterial* m_pMaterial;
		bool m_bOrientToCamera;
		bool m_bSnapToCamera;
		float m_DistToCamera;

		shared_ptr<camCamera>	m_Camera;
};

//--------------------------------------------------------------------
//	GetMaterial returns the material of the object
//--------------------------------------------------------------------
inline matMaterial& scBillboard::GetMaterial()
{
	return *m_pMaterial;
}

//--------------------------------------------------------------------
// If OrientToCamera is true (the default) the billboard will
// rotate to face the camera.
//--------------------------------------------------------------------
inline bool scBillboard::GetOrientToCamera() const
{
	return m_bOrientToCamera;
}

//--------------------------------------------------------------------
// If SnapToCamera is true the billboard will snap to camera view
// and scale to the camera aspect
//--------------------------------------------------------------------
inline bool scBillboard::GetSnapToCamera() const
{
	return m_bSnapToCamera;
}

//--------------------------------------------------------------------
// Set/Get billboard distance to camera
//--------------------------------------------------------------------
inline float scBillboard::GetDistToCamera() const
{
	return m_DistToCamera;
}
