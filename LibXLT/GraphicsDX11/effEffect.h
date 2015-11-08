#pragma once

#include "g2d/g2dDX11Types.hpp"

#include <string>
#include <vector>

struct pass {
	pass()
		: mVS(NULL), mHS(NULL), mPS(NULL), mDS(NULL)
	{
	}
	pass(ID3D11VertexShader* vs, ID3D11PixelShader* ps, ID3D11HullShader* hs, ID3D11DomainShader* ds)
		: mVS(vs), mHS(hs), mPS(ps), mDS(ds)
	{
	}
	ID3D11VertexShader* mVS;
	ID3D11PixelShader* mPS;
	// no geometry shader in this spec.
	ID3D11HullShader* mHS;
	ID3D11DomainShader* mDS;
};

struct technique {
	void addPass(const pass& p);

	std::vector<pass> mPasses;

	bool CompilePass(std::wstring file, const char* vss, const char* pss, const char* hss = NULL, const char* dss = NULL);

	bool Compile(std::wstring file, char* psEntryPoints);
	// default has no hull or domain shader.
	//pass mDefault;
	//pass mTess;
};

class effEffect
{
public:
	effEffect();

	virtual bool CompileFromSourceFile(std::wstring hlslFile) { return false; }

	virtual ~effEffect();

	std::wstring mSourceFile;
};

class effMaterialEffect : public effEffect
{
	virtual bool CompileFromSourceFile(std::wstring hlslFile);

	technique mDefault;
	technique mAmbient;
	technique mEnvironment;
	technique mSingleLight;

};

class effBakeEffect : public effEffect
{
	virtual bool CompileFromSourceFile(std::wstring hlslFile);

	technique mDefault;
	technique mEnvironment;
};
