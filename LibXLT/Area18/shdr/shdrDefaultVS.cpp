/****************************************************************************\
**	rasterizerUtil.cpp
**
**		see .hpp
**
** Area17
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Area18/shdr/shdrDefaultVS.hpp"

#include "Area18/ogl/oglDevice.hpp"
#include "Area18/ogl/oglBuffer.hpp"

#include "Core/ma/maMatrix4x4.hpp"

char* shdrDefaultVS::sourceCode = "\
#version 410\n\
\n\
// uniform inputs\n\
uniform mat4 ModelviewMatrix;\n\
uniform mat4 ProjectionMatrix;\n\
\n\
// per-vertex inputs\n\
layout(location = 0) in vec4 InPosition;\n\
//in vec3 InNormal;\n\
//in vec2 InUV;\n\
//in vec3 InBinormal;\n\
//in vec3 InBitangent;\n\
\n\
// outputs\n\
smooth out vec4 CameraPos;\n\
out gl_PerVertex\n\
{\n\
    vec4 gl_Position;\n\
};\n\
void main(void) {\n\
    CameraPos = ModelviewMatrix * InPosition;\n\
    gl_Position = ProjectionMatrix * CameraPos;\n\
}\n\
";

//------------------------------------------------------------------------
//------------------------------------------------------------------------
shdrDefaultVS::shdrDefaultVS(oglContext* i_pDevice)
:	shdrVS(i_pDevice, shdrSource, shdrDefaultVS::sourceCode)
{
	
	mModelView = glGetUniformLocation(this->GetShader(), "ModelviewMatrix");
	mProjection = glGetUniformLocation(this->GetShader(), "ProjectionMatrix");

	CHECKGLERROR();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
shdrDefaultVS::~shdrDefaultVS()
{
	glDeleteBuffers(1, &mCB);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cbVS::SetWorldTransform(const maMatrix4x4& i_Transform)
{
	memcpy(g_worldview, i_Transform.GetPtr(), 16*sizeof(float)); 
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cbVS::SetUVTransform(const maMatrix4x4& i_Transform)
{
	memcpy(g_uvTransform, i_Transform.GetPtr(), 16*sizeof(float)); 
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cbVS::SetProjectionTransform(const maMatrix4x4& i_Transform)
{
	memcpy(g_projection, i_Transform.GetPtr(), 16*sizeof(float)); 
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void shdrDefaultVS::BindConstants(shdrParams* iData)
{
	cbVS* data = (cbVS*)iData;
//	cbVS* data = dynamic_cast<cbVS*>(iData);
//	assert(data != NULL);
	glUniformMatrix4fv(mModelView, 1, GL_FALSE, data->g_worldview);
	glUniformMatrix4fv(mProjection, 1, GL_FALSE, data->g_projection);

//	D3D11_MAPPED_SUBRESOURCE mappedResource;
//	m_pDevice->m_pDeviceContext->Map( m_pCB->GetResource(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
//	memcpy( mappedResource.pData, &m_Data, sizeof(cb0) );
//	m_pDevice->m_pDeviceContext->Unmap( m_pCB->GetResource(), 0 );
//
//	ID3D11Buffer* ppCB[1] = { m_pCB->GetBuffer() };
//	m_pDevice->m_pDeviceContext->VSSetConstantBuffers( 0, 1, ppCB );
}
