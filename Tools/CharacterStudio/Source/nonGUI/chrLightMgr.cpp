/*****************************************************************************
**  chrLightMgr.hpp
**
**		Handles manipulation of a few lights for MatStudio
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "nonGUI/chrLightMgr.hpp"

#include "Tool/api3d/api3dLightMgr.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dProjectedLightWrapper.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dPointLight.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/mat/matTextureMgr.hpp"


//====================================================================
//====================================================================
namespace chrLightMgr
{

//====================================================================
// Local variables and functions
//====================================================================
namespace
{
	maPoint3d l_Center(0,0,0);
	g3dDirectionalLight *l_pDirLight = NULL; 
	g3dPointLight *l_pPointLight1 = NULL; 
	api3dObject *l_pPointObj1 = NULL;
	g3dProjectedLight *l_pProjectedLight = NULL; 
	api3dProjectedLightWrapper *l_pPrjLightWrapper = NULL;
	api3dObject *l_pProjObj = NULL;

	void vector_to_pitchyaw(const maVector3d &i_Dir, float &o_Pitch, float &o_Yaw)
	{
		o_Yaw = float(::atan2f(i_Dir.m_X, i_Dir.m_Z));
		float zx_len = sqrtf( i_Dir.m_Z * i_Dir.m_Z + i_Dir.m_X * i_Dir.m_X );
		o_Pitch = float(::atan2f(i_Dir.m_Y, zx_len));

		o_Yaw *= maConstants::c_fRadToAngle;
		o_Pitch *= maConstants::c_fRadToAngle;
	}

	maVector3d pitchyaw_to_dir(float i_Pitch, float i_Yaw)
	{
		i_Yaw *= maConstants::c_fAngleToRad;
		i_Pitch *= maConstants::c_fAngleToRad;

		float x = float(sin(i_Yaw) * cos(i_Pitch));
		float y = float(sin(i_Pitch));
		float z = float(cos(i_Yaw) * cos(i_Pitch));
		return maVector3d(x,y,z);
	}

}	// end of namespace

	//============================================================================
	//	Initialize()
	//============================================================================
	void Initialize()
	{
		l_pDirLight = api3dLightMgr::CreateDirectionalLight();
		l_pDirLight->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
		//l_pDirLight->SetIntensity(maFloatRGBA(0.65f, 0.65f, 0.6f, 1.0f));
		l_pDirLight->SetIntensity(maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f));
		l_pDirLight->Disable(); // off by default
		l_pDirLight->SetCastsShadow(true);

		l_pPointLight1 = api3dLightMgr::CreatePointLight();
		l_pPointLight1->SetPosition(maVector3d(1.2f, -1.0f, -1.0f));
		//l_pPointLight1->SetIntensity(maFloatRGBA(0.65f, 0.6f, 0.6f, 1.0f));
		l_pPointLight1->SetIntensity(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
		l_pPointLight1->Enable();
		l_pPointLight1->SetCastsShadow(true);

		l_pPointObj1 = api3dShape::CreateSphere(l_pPointLight1->GetIntensity(), 0.25f, 6, 6);
		api3dScene::AddObject(l_pPointObj1);

		l_pProjectedLight = api3dLightMgr::CreateProjectedLight();
		l_pProjectedLight->SetPosition(maVector3d(1.2f, -1.0f, -1.0f));
		l_pProjectedLight->SetIntensity(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
		l_pProjectedLight->Disable(); // off by default
		l_pProjectedLight->SetCastsShadow(true);

		l_pPrjLightWrapper = new api3dProjectedLightWrapper(*l_pProjectedLight);
		fsLocator tex_loc = gfPaths::GetPath(gfPaths::e_ExePath);
		tex_loc.Push("projection.bmp");
		if (fsFileUtil::FileExists(tex_loc))
		{
			matTexture *pTexture = matTextureMgr::LoadTexture(tex_loc);
			l_pPrjLightWrapper->SetTexture(pTexture); // ownership of texture passes to wrapper
		}
		l_pPrjLightWrapper->OrientCamera();

		l_pProjObj = api3dShape::CreateCone(l_pProjectedLight->GetIntensity(), 0.25f, 0.25f, 6);
		api3dScene::AddObject(l_pProjObj);
		l_pProjObj->SetRenderable(false);

	}

	//============================================================================
	//	DeInitialize()
	//============================================================================
	void DeInitialize()
	{
		api3dLightMgr::DestroyLight(l_pDirLight);
		l_pDirLight = NULL;
		api3dLightMgr::DestroyLight(l_pPointLight1);
		l_pPointLight1 = NULL;
		api3dLightMgr::DestroyLight(l_pProjectedLight);
		l_pProjectedLight = NULL;
		delete l_pPrjLightWrapper;
		l_pPrjLightWrapper = NULL;

		api3dScene::RemoveObject(l_pPointObj1);
		delete l_pPointObj1;
		l_pPointObj1 = NULL;

		api3dScene::RemoveObject(l_pProjObj);
		delete l_pProjObj;
		l_pProjObj = NULL;
	}


	//============================================================================
	//	Center - set center of lighting. point lights will rotate around this
	//============================================================================
	void SetCenter(const maAxisBox& i_Box)
	{
		l_Center = i_Box.GetCenter();

		maPoint3d pos = (i_Box.GetBoxPoint(0) - l_Center) * 1.25f + l_Center;
		l_pPointLight1->SetPosition(pos);
		l_pPointObj1->SetPosition(pos);

		l_pProjectedLight->SetPosition(pos);
		maVector3d dir = l_Center - pos;
		l_pProjectedLight->SetTarget(pos + dir);

		l_pProjObj->SetPosition(pos);
		maRotation rot;
		rot.SetValue(maVector3d(0,1,0), dir);
		l_pProjObj->SetOrientation(rot);
		l_pPrjLightWrapper->OrientCamera();
	}

	//============================================================================
	// DirectionalLight properties
	//============================================================================
	maFloatRGBA	GetDirLightColor()
	{
		return l_pDirLight->GetIntensity();
	}
	void SetDirLightColor(const maFloatRGBA& i_Color)
	{
		l_pDirLight->SetIntensity(i_Color);
	}

	void GetDirLightDirection(float &o_Pitch, float &o_Yaw)
	{
		vector_to_pitchyaw(l_pDirLight->GetDirection(), o_Pitch, o_Yaw);
	}
	void SetDirLightDirection(float i_Pitch, float i_Yaw)
	{
		l_pDirLight->SetDirection(pitchyaw_to_dir(i_Pitch, i_Yaw));
	}

	bool GetDirLightCastShadow()
	{
		return l_pDirLight->GetCastsShadow();
	}
	void SetDirLightCastShadow(bool i_bCastShadow)
	{
		l_pDirLight->SetCastsShadow(i_bCastShadow);
	}

	bool IsEnabledDirLight()
	{
		return l_pDirLight->IsEnabled();
	}
	void EnableDirLight(bool i_bEnabled)
	{
		if (i_bEnabled) l_pDirLight->Enable();
		else l_pDirLight->Disable();
	}

	//============================================================================
	// PointLight1 properties
	//============================================================================
	maFloatRGBA	GetPointLight1Color()
	{
		return l_pPointLight1->GetIntensity();
	}
	void SetPointLight1Color(const maFloatRGBA& i_Color)
	{
		l_pPointLight1->SetIntensity(i_Color);
		l_pPointObj1->SetColor(i_Color);
	}

	void GetPointLight1Position(float &o_Pitch, float &o_Yaw, float &o_Radius)
	{
		maVector3d dir = l_pPointLight1->GetPosition() - l_Center;
		o_Radius = dir.Length();
		vector_to_pitchyaw(dir, o_Pitch, o_Yaw);
	}
	void SetPointLight1Position(float i_Pitch, float i_Yaw, float i_Radius)
	{
		maVector3d dir = pitchyaw_to_dir(i_Pitch, i_Yaw);
		maPoint3d pos = l_Center + dir * i_Radius;
		l_pPointLight1->SetPosition(pos);
		l_pPointObj1->SetPosition(pos);
	}

	bool GetPointLight1CastShadow()
	{
		return l_pPointLight1->GetCastsShadow();
	}
	void SetPointLight1CastShadow(bool i_bCastShadow)
	{
		l_pPointLight1->SetCastsShadow(i_bCastShadow);
	}

	bool IsEnabledPointLight1()
	{
		return l_pPointLight1->IsEnabled();
	}
	void EnablePointLight1(bool i_bEnabled)
	{
		if (i_bEnabled) l_pPointLight1->Enable();
		else l_pPointLight1->Disable();

		l_pPointObj1->SetRenderable(i_bEnabled);
	}


	//============================================================================
	// ProjectedLight properties
	//============================================================================
	maFloatRGBA	GetProjectedLightColor()
	{
		return l_pProjectedLight->GetIntensity();
	}
	void SetProjectedLightColor(const maFloatRGBA& i_Color)
	{
		l_pProjectedLight->SetIntensity(i_Color);
		l_pProjObj->SetColor(i_Color);
	}

	void GetProjectedLightPosition(float &o_Pitch, float &o_Yaw, float &o_Radius)
	{
		maVector3d dir = l_pProjectedLight->GetPosition() - l_Center;
		o_Radius = dir.Length();
		vector_to_pitchyaw(dir, o_Pitch, o_Yaw);
	}
	void SetProjectedLightPosition(float i_Pitch, float i_Yaw, float i_Radius)
	{
		maVector3d dir = pitchyaw_to_dir(i_Pitch, i_Yaw);
		maPoint3d pos = l_Center + dir * i_Radius;
		l_pProjectedLight->SetPosition(pos);
		l_pProjectedLight->SetTarget(pos-dir);
		l_pPrjLightWrapper->OrientCamera();

		l_pProjObj->SetPosition(pos);
		maRotation rot;
		rot.SetValue(maVector3d(0,1,0), dir);
		l_pProjObj->SetOrientation(rot);
	}

	bool IsEnabledProjectedLight()
	{
		return l_pProjectedLight->IsEnabled();
	}
	void EnableProjectedLight(bool i_bEnabled)
	{
		if (i_bEnabled) l_pProjectedLight->Enable();
		else l_pProjectedLight->Disable();

		l_pProjObj->SetRenderable(i_bEnabled);
	}

}