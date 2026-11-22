/****************************************************************************\
**  bumpVertexDecl.hpp
**
**      bumpVertexDecl contains declaration for bump mapping mesh type
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/bump/private/bumpVertexDecl.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "GraphicsDX11/eff/effShaderArray.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/eff/effShaderUtilWin.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"


namespace bumpVertexDecl
{

namespace
{

	bool l_bDoneInit = false;
	ID3D11InputLayout* l_BumpDecl = NULL;
	ID3D11InputLayout* l_BumpDeclSkinned = NULL;
	ID3D11InputLayout* l_BumpDeclVelocityMap = NULL;

}	// end of namespace

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Initialize()
{
	if (l_bDoneInit) return;
	l_bDoneInit = true;



//	float3 Position	: POSITION;
//	float3 Normal	: NORMAL;
//	float4 UV		: TEXCOORD0;
//	float3 T		: TANGENT;
//	float3 B		: BINORMAL;

	// optional skinning info.
//	float4 Bones	: TEXCOORD1;
//	float4 Weights	: TEXCOORD2;

	D3D11_INPUT_ELEMENT_DESC decl[] =
    {
		{"SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
/*    
	D3D11_INPUT_ELEMENT_DESC decl_skinning[] =
    {
		{"SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},

		{"TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
*/
	D3D11_INPUT_ELEMENT_DESC decl_velocityMap[] =
    {
		{"SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},

		{"POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

	// there needs to exist at least 1 compiled shader with this input layout
	const void* pShaderBytecode = NULL;
	unsigned long bytecodeLength = 0;
//	ID3D11VertexShader* pVS = NULL;
//	ID3D11PixelShader* pPS = NULL;
//	effShaderArray::GetDefaultEffect(&pShaderBytecode, &bytecodeLength, &pVS, &pPS);
	matShaderMgr::SetUseShaderArray(true);
	effShaderBaseDX11* pDefaultEffect = (effShaderBaseDX11*)matShaderMgr::GetSpecialEffect("default");//effShaderArray::GetDefaultEffect();
	DBG_ASSERT(pDefaultEffect, "effects not initialized yet.");
	fxEffectPassDesc passDesc;
	pDefaultEffect->GetFxEffect()->GetTechniqueByIndex(0)->GetPassByIndex(0)->GetDesc(&passDesc);
	pShaderBytecode = passDesc.pIAInputSignature;
	bytecodeLength = (unsigned long)passDesc.IAInputSignatureSize;
 
	HRESULT op_result;
	op_result = g2dDX11Global::g_pDevice->CreateInputLayout(decl, 5, 
		pShaderBytecode, bytecodeLength, &l_BumpDecl);
	DBG_ASSERT(SUCCEEDED(op_result), "CreateVertexDeclaration for bump mesh failed.");

	//don't create special layout for skinning until ready to implement!

//	op_result = g2dDX11Global::g_pDevice->CreateInputLayout(decl_skinning, 7, 
//		pShaderBytecode, bytecodeLength, &l_BumpDeclSkinned);
//	DBG_ASSERT(SUCCEEDED(op_result), "CreateVertexDeclaration for bump mesh failed.");

	effShaderBaseDX11* pVelocityEffect = (effShaderBaseDX11*)matShaderMgr::GetSpecialEffect("VelocityRender.fx");//effShaderArray::GetDefaultEffect();
	DBG_ASSERT(pVelocityEffect, "effects not initialized yet.");
	pVelocityEffect->GetFxEffect()->GetTechniqueByIndex(0)->GetPassByIndex(0)->GetDesc(&passDesc);
	pShaderBytecode = passDesc.pIAInputSignature;
	bytecodeLength = (unsigned long)passDesc.IAInputSignatureSize;

	op_result = g2dDX11Global::g_pDevice->CreateInputLayout(decl_velocityMap, 7,
		pShaderBytecode, bytecodeLength, &l_BumpDeclVelocityMap);
	DBG_ASSERT(SUCCEEDED(op_result), "CreateVertexDeclaration for bump mesh failed.");

//	pShader->Release();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void DeInitialize()
{
	l_bDoneInit = false;

	if (l_BumpDecl)
		l_BumpDecl->Release();
	l_BumpDecl = NULL;

	if (l_BumpDeclSkinned)
		l_BumpDeclSkinned->Release();
	l_BumpDeclSkinned = NULL;

	if (l_BumpDeclVelocityMap)
		l_BumpDeclVelocityMap->Release();
	l_BumpDeclVelocityMap = NULL;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
ID3D11InputLayout* GetBumpMeshDeclaration(bool i_bSkinned /* = false */, bool i_bVelocityMap)
{
	return i_bVelocityMap ? l_BumpDeclVelocityMap : (i_bSkinned ? l_BumpDeclSkinned : l_BumpDecl);
}

}	// end of namespace