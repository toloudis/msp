//****************************************************************************
//	cptrRenderRmanLiveData.hpp
//
//	Preferences Data
//
//	StudioGPU
//	Copyright(c) 2004-7 - All Rights Reserved
//****************************************************************************
#include "Features/Capture/cptrRenderRmanLiveData.hpp"
#include "Features/ObjectManip/mnpModeObjectManip.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/Env/envThreadGroup.hpp"
#include "Core/prty/prtyUnits.hpp"
#include "Tool/icn/icnIconScale.hpp"


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cptrRenderRmanLiveData::cptrRenderRmanLiveData()
:	
	m_nRmanAArate("Rman Live Sampling Rate",4),
	m_RmanShadingRate("Rman Live Shading Rate",0.25f),
	m_bRmanCacheTextures("Rman Live Rewrite All Assets", false ),
	m_bRmanDisableWarnings("Rman Live Disable Warnings", false ),
	m_bRmanAOEnable("Rman Live Enable AO",false),
	m_RmanAOsamples("Rman Live Num AO Samples",64),
	m_RmanAOMaxVariation("Rman Live AO Quality", 50 ),
	m_bRmanReflEnable("Rman Live Reflection Enable", true ),
	m_RmanReflType("Rman Live Reflection Type", 0 ),
	m_bRmanShadowEnable("Rman Live Enable", true ),
	m_RmanShadowType("Rman Live Shadow Type", 0 ),
	m_bRmanGIEnable("Rman Live Enable",false),
	m_RmanGIsamples("Rman Live Num Samples",64),
	m_RmanGIMaxVariation("Rman Live Bleed Quality", 50 ),
	m_RmanFilterType("Rman Live Filter Type", 0 ),
	m_RmanFilterWidth("Rman Live Filter Width", 1 ),
	m_RmanNumCores("Rman Live Num Render Threads", 4 ),
	m_RmanTexMemory("Rman Live Available Texture Memory (MB)", 2048 ),
	m_RmanBucketOrder("Rman Live Bucket Order", 0 ),
	m_RmanBucketSize("Rman Live Bucket Size", 16 ),
	m_RmanRayDepth("Rman Live Ray Tracing Depth", 10 ),
	m_bRmanTonemapEnable("Rman Live Capture Tonemapped Pixels",true),
	m_RmanRenderPass("Rman Live Render Pass",0),
	m_bStartStop("Rman Live Render")
{
	m_RmanRenderPass.SetEnumTag(0, "Beauty");
	m_RmanRenderPass.SetEnumTag(1, "Diffuse");
	m_RmanRenderPass.SetEnumTag(2, "Diffuse Environment");
	m_RmanRenderPass.SetEnumTag(3, "Diffuse Lights");
	m_RmanRenderPass.SetEnumTag(4, "Specular");
	m_RmanRenderPass.SetEnumTag(5, "Specular Environment");
	m_RmanRenderPass.SetEnumTag(6, "Specular Lights");
	m_RmanRenderPass.SetEnumTag(7, "Emissive");
	m_RmanRenderPass.SetEnumTag(8, "Ambient Occlusion");
	m_RmanRenderPass.SetEnumTag(9, "Shadow Mask");
	m_RmanRenderPass.SetEnumTag(10, "Illumination");
	m_RmanRenderPass.SetEnumTag(11, "Normals");
	m_RmanRenderPass.SetEnumTag(12, "Reflections");
	m_RmanRenderPass.SetEnumTag(13, "Color Bleed");

	m_RmanReflType.SetEnumTag(0,"Reflection map based");
	m_RmanReflType.SetEnumTag(1,"Ray traced");

	m_RmanShadowType.SetEnumTag(0,"Shadow map based");
	m_RmanShadowType.SetEnumTag(1,"Ray traced");

	m_RmanFilterType.SetEnumTag(0,"Box");
	m_RmanFilterType.SetEnumTag(1,"Gaussian");
	m_RmanFilterType.SetEnumTag(2,"Mitchell");
	m_RmanFilterType.SetEnumTag(3,"Triangle");
	m_RmanFilterType.SetEnumTag(4,"Sinc");
	m_RmanFilterType.SetEnumTag(5,"Blackman-Harris");
	m_RmanFilterType.SetEnumTag(6,"Catmull-Rom");
	m_RmanFilterType.SetEnumTag(7,"Separable-Catmull-Rom");

	m_RmanBucketOrder.SetEnumTag(0,"Space Fill");
	m_RmanBucketOrder.SetEnumTag(1,"Horizontal");
	m_RmanBucketOrder.SetEnumTag(2,"Vertical");
	m_RmanBucketOrder.SetEnumTag(3,"Zigzag - X");
	m_RmanBucketOrder.SetEnumTag(4,"Zigzag - Y");
	m_RmanBucketOrder.SetEnumTag(5,"Spiral");
	m_RmanBucketOrder.SetEnumTag(6,"Random");
}

