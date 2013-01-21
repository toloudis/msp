#pragma once

#include "shdrPS.hpp"

#include "Area18/ogl/oglTexture2d.h"

class oglContext;
class g3dPointLight;
class maFloatRGBA;
class maVector4d;

struct cbLambert
{
	float g_Color[4];
	float g_EyePos[4];
	bool g_HasDiffuseTexture;
	float pad[3];
};
struct cbLight 
{
	float g_Pos[4];
	float g_Diffuse[4];
	float g_Specular[4];
};

class shdrLambertParams : public shdrParams
{
public:
	void SetColor(const maFloatRGBA& i_Color);
	void SetEyePos(const maVector4d& i_E);
	void SetTexture(oglTexture2dHandle iTex);
	void SetLight(g3dPointLight* iLight);

	cbLambert mLambert;
	oglTexture2dHandle mDiffuseTexture;
	cbLight mLight;
};

class shdrLambertPS : public shdrPS
{
public:
	shdrLambertPS(oglContext* i_pDevice);
	virtual ~shdrLambertPS(void);

	void BindConstants(shdrParams* iParams);

private:
	GLuint mCB;
	GLuint mDiffuseSampler;
	GLuint mCBLight;
};

