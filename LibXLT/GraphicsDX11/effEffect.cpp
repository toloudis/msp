#include "effEffect.h"

#include "G2d/g2dDX11GlobalWin.hpp"
#include <fstream>

std::wstring shaderDir(L"D:\\Projects\\SourceCode\\LibXLT\\GraphicsDX11\\Eff\\private\\Shaders\\");

static char* genericNames[] = {
	"VS_Default", "VS_Tess", "Tangent_HS", "Tangent_DS"
};

HRESULT CompileShaderFromFile(std::wstring file, const char* entryPoint, const char* target, ID3DBlob** ppBlobOut)
{
	const D3D_SHADER_MACRO defines[] =
	{
		NULL, NULL
	};
	ID3DBlob* pErrorBlob = NULL;
	HRESULT hr = D3DCompileFromFile(
		file.c_str(),
		defines,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint,
		target,
		0,
		0,
		ppBlobOut,
		&pErrorBlob
		);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
		}
		return hr;
	}
	if (pErrorBlob)
	{
		pErrorBlob->Release();
	}
	return hr;
}

ID3D11VertexShader* CompileVS(const std::wstring& file, const std::string& name, std::string target = "vs_5_0")
{
	ID3D11VertexShader* sh = NULL;
	ID3DBlob* pBlob = NULL;
	HRESULT hr = CompileShaderFromFile(file, name.c_str(), target.c_str(), &pBlob);
	if (hr == S_OK) {
		hr = g2dDX11Global::g_pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &sh);
	}
	if (pBlob) {
		pBlob->Release();
	}
	return sh;
}
ID3D11PixelShader* CompilePS(const std::wstring& file, const std::string& name, std::string target = "ps_5_0")
{
	ID3D11PixelShader* sh = NULL;
	ID3DBlob* pBlob = NULL;
	HRESULT hr = CompileShaderFromFile(file, name.c_str(), target.c_str(), &pBlob);
	if (hr == S_OK) {
		hr = g2dDX11Global::g_pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &sh);
	}
	if (pBlob) {
		pBlob->Release();
	}
	return sh;
}
ID3D11HullShader* CompileHS(const std::wstring& file, const std::string& name, std::string target = "hs_5_0")
{
	ID3D11HullShader* sh = NULL;
	ID3DBlob* pBlob = NULL;
	HRESULT hr = CompileShaderFromFile(file, name.c_str(), target.c_str(), &pBlob);
	if (hr == S_OK) {
		hr = g2dDX11Global::g_pDevice->CreateHullShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &sh);
	}
	if (pBlob) {
		pBlob->Release();
	}
	return sh;
}
ID3D11DomainShader* CompileDS(const std::wstring& file, const std::string& name, std::string target = "ds_5_0")
{
	ID3D11DomainShader* sh = NULL;
	ID3DBlob* pBlob = NULL;
	HRESULT hr = CompileShaderFromFile(file, name.c_str(), target.c_str(), &pBlob);
	if (hr == S_OK) {
		hr = g2dDX11Global::g_pDevice->CreateDomainShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &sh);
	}
	if (pBlob) {
		pBlob->Release();
	}
	return sh;
}

bool technique::Compile(std::wstring file, char* psEntryPoint)
{
	ID3DBlob* pShaderBlob = NULL;

	ID3DBlob* pBlob = NULL;

	ID3D11VertexShader* vsDefault;
	ID3D11VertexShader* vsTess;
	ID3D11PixelShader* ps;
	ID3D11HullShader* hs;
	ID3D11DomainShader* ds;

	vsDefault = CompileVS(file, "VS_Default");
	ps = CompilePS(file, psEntryPoint);

	addPass(pass(vsDefault, ps, NULL, NULL));

	vsTess = CompileVS(file, "VS_Tess");
	hs = CompileHS(file, "Tangent_HS");
	ds = CompileDS(file, "Tangent_DS");

	addPass(pass(vsTess, ps, hs, ds));

	return true;
}

void technique::addPass(const pass& p) 
{
	mPasses.push_back(p);
}

bool technique::CompilePass(std::wstring file, const char* vss, const char* pss, const char* hss /*= NULL*/, const char* dss /*= NULL*/)
{
	ID3D11VertexShader* vs;
	ID3D11PixelShader* ps;
	ID3D11HullShader* hs;
	ID3D11DomainShader* ds;

	vs = CompileVS(file, vss);
	ps = CompilePS(file, pss);
	if (hss) {
		hs = CompileHS(file, hss);
	}
	if (dss) {
		ds = CompileDS(file, dss);
	}

	addPass(pass(vs, ps, hs, ds));

	return true;
}

effEffect::effEffect()
{
}

bool effMaterialEffect::CompileFromSourceFile(std::wstring hlslFile)
{
	// for each technique:
	// 2 passes, default and tess
	// in each pass, have VS_
	char* psNames[4] = {
		"defaultPS",
		"ambientPS",
		"ambientPS",
		"singleLightPS"
	};

	mSourceFile = hlslFile;
	
	bool ok = false;
	ok = mDefault.Compile(hlslFile, psNames[0]);
	ok = mAmbient.Compile(hlslFile, psNames[1]);
	ok = mEnvironment.Compile(hlslFile, psNames[2]);
	ok = mSingleLight.Compile(hlslFile, psNames[3]);

	return ok;
}

bool effBakeEffect::CompileFromSourceFile(std::wstring hlslFile)
{
	// for each technique:
	// 2 passes, default and tess
	// in each pass, have VS_
	char* psNames[4] = {
		"defaultPS",
		"ambientPS",
		"ambientPS",
		"singleLightPS"
	};

	mSourceFile = hlslFile;

	bool ok = false;

	ok = mDefault.CompilePass(hlslFile, "VS_Default", "PS_Default");
	ok = mDefault.CompilePass(hlslFile, "VS_Tess", "PS_Tess", "HS_Tess", "DS_Tess");
	ok = mEnvironment.CompilePass(hlslFile, "VS_Default", "PS_Default");
	ok = mEnvironment.CompilePass(hlslFile, "VS_Tess", "PS_Tess", "HS_Tess", "DS_Tess");

	return ok;
}

effEffect::~effEffect()
{
}
