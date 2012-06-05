/****************************************************************************\
**	shdwPassMaterials.cpp
**
**		Render pass to draw depth values to target
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef SHDW_PASSMATERIALS_HPP
#error shdwPassMaterials.hpp multiply included
#endif
#define SHDW_PASSMATERIALS_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

#ifndef CAM_CAMERA_HPP
#include "Graphics/Cam/camCamera.hpp"
#endif 

class shdwPassTraversal;
struct sNodePlusState;
class matRenderTargetTexture;

#include <map>
#include <utility>
#include <list>

//#define USE_HASH_FUNC
//#ifdef USE_HASH_ONE
//#ifdef USE_HASH_TWO
//#ifdef USE_HASH_THREE
#define USE_ALTERNATING_GEN

#define MAX_HUES	256

struct HSV
{
	float hue;
	float sat;
	float val;
};

class shdwPassMaterials : public g3dRenderPass
{
private:
	//recalculates camera projection based on a number of border pixels
	void AdjustCameraOverscan();

public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwPassMaterials(g2dRenderTarget* i_pMaterialsTarget, 
					  const camCamera* i_pCamera, 
					  std::map<std::string, float>* i_ColorMap,							 
					  float * i_LastNumGenerated,
					  int * i_TotalColors,
					  matRenderTargetTexture* i_pSrcMaterials = NULL );
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwPassMaterials();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual int Render(float i_time);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetSceneInfo(shdwPassTraversal* i_traversal) {m_sceneInfo = i_traversal;}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int RenderNode(const sNodePlusState& i_Node);

	//----------------------------------------------------------------------------------------
	// This function renders a single node with the special hair shader
	//----------------------------------------------------------------------------------------
	int RenderHairNode(const sNodePlusState& i_Node);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	float GenerateHue(const std::string& i_MaterialName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	float hash_material(const std::string i_MaterialName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	float hash_material_one(const std::string i_MaterialName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	float hash_material_two(const std::string i_MaterialName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	float hash_material_three(const std::string i_MaterialName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	float GenerateNextNumAlternating(float i_min, float i_max, float i_LastNum);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void GenerateHSVStruct(int i_NumNodes);

	static void InitStates();
	static void CleanupStates();

protected:
	shdwPassTraversal* m_sceneInfo;
	const camCamera* m_pOrigCamera;
	camCamera m_Camera;

	float m_Near, m_Far;
	int m_OverscanPixels;

	std::map<std::string, float>* m_ColorMap;

	std::vector< HSV > m_HSVs;
	
	float * m_LastNumGenerated;
	int * m_TotalColors;

	int m_CurrNodeIdx;

	matRenderTargetTexture* m_pSrcMaterialsBuffer;
};
