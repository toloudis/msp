/****************************************************************************\
**	shdwVelocityStateManager.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwVelocityStateManager.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
VelocityStateManager::VelocityStateManager()
{
	m_pOldCamera = new camCamera();
	m_pCurrCamera = new camCamera();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
VelocityStateManager::~VelocityStateManager()
{
	delete m_pOldCamera;
	m_pOldCamera = NULL;

	delete m_pCurrCamera;
	m_pCurrCamera = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
maMatrix4x4 VelocityStateManager::GetViewProjectionTransform_Old()
{
	maMatrix4x4 viewMatrix;
	maMatrix4x4 viewProjectionMatrix;
	m_pOldCamera->GetCameraMatrix(viewMatrix);
	m_pOldCamera->GetProjectionMatrix(viewProjectionMatrix);
	
	if ( !g3dSingleLightRendering::GetDoCubeReflectionGen() )
		viewProjectionMatrix.ScaleBy(-1.0f, 1.0f, 1.0f);

	return viewMatrix * viewProjectionMatrix;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
maMatrix4x4 VelocityStateManager::GetViewTransform_Old()
{
	maMatrix4x4 viewMatrix;
	m_pOldCamera->GetCameraMatrix(viewMatrix);
	return viewMatrix;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void VelocityStateManager::SetOldCamera(const camCamera& i_Copy)
{
	delete m_pOldCamera;
	m_pOldCamera = new camCamera(i_Copy);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
camCamera* VelocityStateManager::GetOldCamera()
{
	return m_pOldCamera;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void VelocityStateManager::SetCurrCamera(const camCamera& i_Copy)
{
	delete m_pCurrCamera;
	m_pCurrCamera = new camCamera(i_Copy);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
camCamera* VelocityStateManager::GetCurrCamera()
{
	return m_pCurrCamera;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void VelocityStateManager::InspectCams()
{
	maMatrix4x4 v_curr;
	maMatrix4x4 vp_curr;

	maMatrix4x4 v_old;
	maMatrix4x4 vp_old;

	m_pOldCamera->GetCameraMatrix(v_old);
	m_pOldCamera->GetProjectionMatrix(vp_old);

	m_pCurrCamera->GetCameraMatrix(v_curr);
	m_pCurrCamera->GetProjectionMatrix(vp_curr);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void VelocityStateManager::InsertPrevWorldTransform(std::string i_Id, maMatrix4x4 i_Mat)
{
	m_PrevWorldTransforms.insert(std::pair<std::string,maMatrix4x4>(i_Id , i_Mat));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
maMatrix4x4 VelocityStateManager::GetPrevWorldTransform(std::string i_Id)
{
	return m_PrevWorldTransforms[i_Id];
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void VelocityStateManager::ClearPrevWorldTransforms()
{
	m_PrevWorldTransforms.clear();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void VelocityStateManager::InsertCurrWorldTransform(std::string i_Id, maMatrix4x4 i_Mat)
{
	m_CurrWorldTransforms[i_Id] = i_Mat;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
maMatrix4x4 VelocityStateManager::GetCurrWorldTransform(std::string i_Id)
{
	return m_CurrWorldTransforms[i_Id];
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void VelocityStateManager::ClearCurrWorldTransforms()
{
	m_CurrWorldTransforms.clear();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void VelocityStateManager::UpdatePrevTransforms()
{
	m_PrevWorldTransforms.clear();
	m_PrevWorldTransforms = m_CurrWorldTransforms;
}