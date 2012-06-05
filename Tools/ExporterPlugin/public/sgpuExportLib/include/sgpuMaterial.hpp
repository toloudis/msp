/****************************************************************************\
**  sgpuMaterial.hpp
**
**  sgpuMaterial.hpp defines class for a simple phong material
**	that can be attached to meshes.  The same material can be applied to 
**	more than one mesh.
**
**	Please note that sgpuMaterial is just handle to an internal material
**	representation that we have.
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_MATERIAL_HPP
#define SGPU_MATERIAL_HPP

#include "sgpuExportLib.hpp"
#include "sgpuVector.hpp"
#include "sgpuString.hpp"

//============================================================================
//============================================================================
struct sgpuMaterialImpl;
class sgpuMesh;
class sgpuModelExportScene;
class sgpuSubdiv;
class sgpuConstructor;
class sgpuSubdivConstructor;
class sgpuMeshConstructor;
//============================================================================
//============================================================================
class SGPUEXPORTLIB_API sgpuMaterial
{
public:
	sgpuMaterial( const sgpuMaterial &i_Other);
	~sgpuMaterial();
	sgpuMaterial& operator=( const sgpuMaterial &i_Other );

	//========================================================================
	// Get the name of this material
	//========================================================================
	sgpuString GetName() const;
	//========================================================================
	// Set phong diffuse color. Color values should be in the range 0-1
	//========================================================================
	void SetDiffuseColor(float i_Red, float i_Green, float i_Blue);	
	//========================================================================
	// Get phong diffuse color as an sgpuVector
	// The 0, 1 and 2 elements of the vector will be the r,g and b values
	// in the range 0-1.
	//========================================================================
	sgpuVector3 GetDiffuseColor() const;

	//========================================================================
	// Set transparency of material, in range from 0-1. 
	// 1 is opaque and 0 is fully transparent.
	//========================================================================
	void SetOpacity(float i_Opacity);
	
	//========================================================================
	// Get the opacity of the material.
	//1 is opaque and 0 is fully transparent
	//========================================================================
	float GetOpacity() const;

	//========================================================================
	// Set phong specular color. Color values should be in the range 0-1
	//========================================================================
	void SetSpecularColor(float i_Red, float i_Green, float i_Blue);
	//========================================================================
	// Get phong speular color as an sgpuVector
	// The 0, 1 and 2 elements of the vector will be the r,g and b values
	// in the range 0-1.
	//========================================================================
	sgpuVector3 GetSpecularColor() const;
	//========================================================================
	// Set ambient color. Color values should be in the range 0-1
	//========================================================================
	void SetAmbientColor(float i_Red, float i_Green, float i_Blue);
	//========================================================================
	// Get ambient color as an sgpuVector
	// The 0, 1 and 2 elements of the vector will be the r,g and b values
	// in the range 0-1.
	//========================================================================
	sgpuVector3 GetAmbientColor() const;
	//========================================================================
	// Set phong specular exponent
	//========================================================================
	void SetShininess(float i_Power);
	//========================================================================
	// Get phong specular exponent
	//========================================================================
	float GetShininess() const;
	//========================================================================
	// Set filename for diffuse texture layer
	// If i_TextureFileName is internationalized,
	// please  convert that to a utf-8 sequence and
	// pass it as an std::string
	//========================================================================
	void SetDiffuseTexture( const sgpuString& i_TextureFileName );
	//========================================================================
	// Get filename for diffuse texture layer
	// returns empty string if no diffusetexture
	//========================================================================	
	const sgpuString GetDiffuseTexture() const;
	//returns true if the internal material structure pointed to by
	//this is the same as the other.
	bool operator== ( const sgpuMaterial &i_Other ) const;

	friend sgpuModelExportScene;
	friend sgpuMesh;
	friend sgpuSubdiv;
	friend sgpuConstructor;
	friend sgpuSubdivConstructor;
	friend sgpuMeshConstructor;
public:
	//obsolete functions
#if defined( SGPU_SUPPORT_1200)
	void SetDiffuseTexture(const char* i_TextureFilename);
	void SetDiffuseTexture(const wchar_t* i_TextureFilename);
#endif
private:
	sgpuMaterial();
	sgpuMaterialImpl *m_pImpl;
};

#endif // #ifndef SGPU_MATERIAL_HPP
