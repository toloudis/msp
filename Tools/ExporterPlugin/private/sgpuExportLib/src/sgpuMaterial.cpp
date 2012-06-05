/****************************************************************************\
**  sgpuMaterial.cpp
**
**      sgpuMaterial.hpp defines the sgpuMaterial class
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuMaterial.hpp"
#include "sgpuMaterialImpl.hpp"
#include "sgpuUtilsImpl.hpp"
#include "sgpuException.hpp"
#include "sgpuStringImpl.hpp"

#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Env/envString.hpp"
//========================================================================
// sgpuMaterial:: constructor and dstructors
//========================================================================

sgpuMaterial::sgpuMaterial():m_pImpl(new sgpuMaterialImpl()){}
sgpuMaterial::~sgpuMaterial()
{
	delete m_pImpl;
}
sgpuMaterial::sgpuMaterial( const sgpuMaterial &i_Other ):
m_pImpl( new sgpuMaterialImpl(*i_Other.m_pImpl) )
{}

sgpuMaterial &sgpuMaterial::operator=(const sgpuMaterial &i_CopyFrom)
{
	if (&i_CopyFrom != this) 									\
	{ 															\
		delete m_pImpl; 										\
		m_pImpl = new sgpuMaterialImpl(*i_CopyFrom.m_pImpl);	\
	} 															\
	return *this; 												\
}

sgpuString sgpuMaterial::GetName() const 
{
	return sgpuString( m_pImpl->m_Name.c_str() );
}

//========================================================================
// Set phong diffuse color. Color values should be in the range 0-1
//========================================================================
void sgpuMaterial::SetDiffuseColor(float i_Red, float i_Green, float i_Blue)
{
	NO_IMPL_EXCEPTION( m_pImpl->m_PhongData, "sgpuMaterial" )
	m_pImpl->m_PhongData->m_ColorDiffuse.SetRed( i_Red );
	m_pImpl->m_PhongData->m_ColorDiffuse.SetGreen( i_Green );
	m_pImpl->m_PhongData->m_ColorDiffuse.SetBlue( i_Blue );
}
//========================================================================
// Get phong diffuse color as an sgpuVector
// The 0, 1 and 2 elements of the vector will be the r,g and b values
// in the range 0-1.
//========================================================================
sgpuVector3 sgpuMaterial::GetDiffuseColor() const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_PhongData, "sgpuMaterial" )
	maFloatRGBA::Value rVal =  m_pImpl->m_PhongData->m_ColorDiffuse.GetRed();
	maFloatRGBA::Value gVal = m_pImpl->m_PhongData->m_ColorDiffuse.GetGreen();
	maFloatRGBA::Value bVal = m_pImpl->m_PhongData->m_ColorDiffuse.GetBlue();
	return sgpuVector3( rVal, gVal, bVal );
}
//========================================================================
// Set transparency of material, in range from 0-1. 
// 1 is opaque and 0 is fully transparent.
//========================================================================
void sgpuMaterial::SetOpacity(float i_Opacity)
{
	NO_IMPL_EXCEPTION( m_pImpl->m_PhongData, "sgpuMaterial" )
	m_pImpl->m_PhongData->m_ColorDiffuse.SetAlpha( i_Opacity );
	m_pImpl->m_PhongData->m_Transparency = i_Opacity;
}

//========================================================================
// Get the opacity of the material.
//1 is opaque and 0 is fully transparent
//========================================================================
float sgpuMaterial::GetOpacity()const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_PhongData, "sgpuMaterial" )
		maFloatRGBA::Value aVal = m_pImpl->m_PhongData->m_ColorDiffuse.GetAlpha( );
	DBG_ASSERT( aVal == m_pImpl->m_PhongData->m_Transparency, "Alpha value of diffuse texture should be the same as the transparency value, context material:" << m_pImpl->m_Name );
	return m_pImpl->m_PhongData->m_Transparency;
}

//========================================================================
// Set phong specular color. Color values should be in the range 0-1
//========================================================================
void sgpuMaterial::SetSpecularColor(float i_Red, float i_Green, float i_Blue)
{
	NO_IMPL_EXCEPTION( m_pImpl->m_PhongData, "sgpuMaterial" )
	m_pImpl->m_PhongData->m_ColorSpecular.SetRed( i_Red );
	m_pImpl->m_PhongData->m_ColorSpecular.SetGreen( i_Green );
	m_pImpl->m_PhongData->m_ColorSpecular.SetBlue( i_Blue );
}

//========================================================================
// Get phong speular color as an sgpuVector
// The 0, 1 and 2 elements of the vector will be the r,g and b values
// in the range 0-1.
//========================================================================
sgpuVector3 sgpuMaterial::GetSpecularColor() const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_PhongData, "sgpuMaterial" )
	maFloatRGBA::Value rVal =  m_pImpl->m_PhongData->m_ColorSpecular.GetRed();
	maFloatRGBA::Value gVal = m_pImpl->m_PhongData->m_ColorSpecular.GetGreen();
	maFloatRGBA::Value bVal = m_pImpl->m_PhongData->m_ColorSpecular.GetBlue();
	return sgpuVector3( rVal, gVal, bVal );
}

//========================================================================
// Set phong specular color. Color values should be in the range 0-1
//========================================================================
void sgpuMaterial::SetAmbientColor(float i_Red, float i_Green, float i_Blue)
{
	NO_IMPL_EXCEPTION( m_pImpl->m_PhongData, "sgpuMaterial" )
	m_pImpl->m_PhongData->m_ColorAmbient.SetRed( i_Red );
	m_pImpl->m_PhongData->m_ColorAmbient.SetGreen( i_Green );
	m_pImpl->m_PhongData->m_ColorAmbient.SetBlue( i_Blue );
}
//========================================================================
// Get ambient color as an sgpuVector
// The 0, 1 and 2 elements of the vector will be the r,g and b values
// in the range 0-1.
//========================================================================
sgpuVector3 sgpuMaterial::GetAmbientColor() const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_PhongData, "sgpuMaterial" )
	maFloatRGBA::Value rVal =  m_pImpl->m_PhongData->m_ColorAmbient.GetRed();
	maFloatRGBA::Value gVal = m_pImpl->m_PhongData->m_ColorAmbient.GetGreen();
	maFloatRGBA::Value bVal = m_pImpl->m_PhongData->m_ColorAmbient.GetBlue();
	return sgpuVector3( rVal, gVal, bVal );
}

//========================================================================
// Set phong specular exponent
//========================================================================
void sgpuMaterial::SetShininess(float i_Power)
{
	NO_IMPL_EXCEPTION( m_pImpl->m_PhongData, "sgpuMaterial" )
	m_pImpl->m_PhongData->m_SpecularPower = i_Power;
}
//========================================================================
// Get phong specular exponent
//========================================================================
float sgpuMaterial::GetShininess() const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_PhongData, "sgpuMaterial" )
	return m_pImpl->m_PhongData->m_SpecularPower;
}

	//========================================================================
//========================================================================
// Set filename for diffuse texture layer
//========================================================================
void sgpuMaterial::SetDiffuseTexture(const sgpuString &i_TextureFilename)
{
	NO_IMPL_EXCEPTION( m_pImpl->m_PhongData, "sgpuMaterial" )
	fsLocator file_loc;
	std::wstring wTextureFilename;
	UTF8ToUnicode( i_TextureFilename.m_pImpl->m_Data.c_str() , wTextureFilename );
	itString it_filename( (itString::CharType*) wTextureFilename.c_str() );
	fsFileUtil::UnicodeStringToLocator(it_filename, file_loc);
	m_pImpl->m_PhongData->m_FullpathDiffuse = file_loc;
}

//========================================================================
// Get filename for diffuse texture layer
// returns empty string if no diffusetexture
//========================================================================	
const sgpuString sgpuMaterial::GetDiffuseTexture() const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_PhongData, "sgpuMaterial" )
	itString diffuseTexture;
	fsFileUtil::LocatorToUnicodeString( m_pImpl->m_PhongData->m_FullpathDiffuse, diffuseTexture );
	const itString::CharType* pzItDiffuseTexture = diffuseTexture.GetString();
	const wchar_t *wpzDiffuseTexture = reinterpret_cast<const wchar_t * > ( pzItDiffuseTexture );
	return sgpuString( wpzDiffuseTexture );
}

bool sgpuMaterial::operator== ( const sgpuMaterial &other ) const
{  	
	DBG_ASSERT( (m_pImpl), "Implementation is NULL!");
	DBG_ASSERT( (other.m_pImpl), "Implementation is NULL!");
	return *m_pImpl == *other.m_pImpl;
}

#if defined( SGPU_SUPPORT_1200)
void sgpuMaterial::SetDiffuseTexture(const wchar_t* i_TextureFilename)
{
	SetDiffuseTexture( sgpuString( i_TextureFilename ) );
}

void sgpuMaterial::SetDiffuseTexture(const char* i_TextureFilename)
{
	SetDiffuseTexture( sgpuString( i_TextureFilename ) );
}
#endif


