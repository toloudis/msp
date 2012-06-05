/*****************************************************************************
**  matMaterial.hpp
**
**      The matMaterial represents all intensive visual properties of a
**	surface.  It includes information about the ambient, diffuse, and
**	specular colors, and the texture maps applied to the surface.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mat/matMaterial.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/mat/matMatAnim.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

#include <algorithm>
#include <functional>


//============================================================================
//============================================================================
namespace
{
//matMaterial l_DefaultMaterial;

bool test_has_transformation(const maVector2d& i_Translation, const maVector2d& i_Scale, float i_Rotation)
{
	if( !(i_Translation == maVector2d(0, 0)) ) return true;
	if( !(i_Scale == maVector2d(1, 1)) ) return true;
	if( i_Rotation != 0.0f ) return true;
	return false;
}

}

//--------------------------------------------------------------------
//	Default constructor
//--------------------------------------------------------------------
matMaterial::matMaterial()
:	m_bHasGlow(false),
	m_bHasOutline(false),
	m_bHasReflection(false),
	m_bHasDisplacement(false),
	m_bHasStrandHair(false),
	m_bHasRendermanOverride(false),
	m_PaintOverlay(NULL),
	m_bBelongsToLightShaft(false)
{
	m_Flags.m_bHasSpecular = true;
	m_Flags.m_bAdditive = false;

	// Set up base material layer
	shared_ptr<matMaterialLayer> base_layer(new matMaterialLayer());
	m_MaterialLayers.push_back( base_layer);

	m_Flags.m_bForceTransparent = false;
	this->test_transparency();
}

//--------------------------------------------------------------------
//	convenience constructor that will allow you to set the initial color
//	values
//--------------------------------------------------------------------
matMaterial::matMaterial(const maFloatRGBA& i_Diffuse,
						 const maFloatRGBA& i_Ambient,
						 const maFloatRGBA& i_Emissive)
:	m_bHasGlow(false),
	m_bHasOutline(false),
	m_bHasReflection(false),
	m_bHasDisplacement(false),
	m_bHasRendermanOverride(false),
	m_bHasStrandHair(false),
	m_PaintOverlay(NULL),
	m_bBelongsToLightShaft(false)
{
	m_Flags.m_bHasSpecular = false;
	m_Flags.m_bAdditive = false;

	// Set up base material layer
	shared_ptr<matMaterialLayer> base_layer(new matMaterialLayer(i_Diffuse, i_Ambient, i_Emissive));
	m_MaterialLayers.push_back( base_layer);

	m_Flags.m_bForceTransparent = false;
	this->test_transparency();
}

//--------------------------------------------------------------------
//	convenience constructor that will allow you to set the shader
//  and its data
//--------------------------------------------------------------------
matMaterial::matMaterial(const std::string& i_EffectID,
			effShaderData* i_ShaderData /* = NULL */)
:	m_bHasGlow(false),
	m_bHasOutline(false),
	m_bHasReflection(false),
	m_bHasDisplacement(false),
	m_bHasRendermanOverride(false),
	m_bHasStrandHair(false),
	m_PaintOverlay(NULL),
	m_bBelongsToLightShaft(false)
{
	m_Flags.m_bHasSpecular = true;
	m_Flags.m_bAdditive = false;

	// Set up base material layer
	shared_ptr<matMaterialLayer> base_layer(new matMaterialLayer(i_EffectID, i_ShaderData));
	m_MaterialLayers.push_back( base_layer);

	m_Flags.m_bForceTransparent = false;
	this->test_transparency();
}
matMaterial::matMaterial(const fsLocator& i_EffectID)
:	m_bHasGlow(false),
	m_bHasReflection(false),
	m_bHasStrandHair(false),
	m_PaintOverlay(NULL),
	m_bBelongsToLightShaft(false)
{
	m_Flags.m_bHasSpecular = true;
	m_Flags.m_bAdditive = false;

	// Set up base material layer
	shared_ptr<matMaterialLayer> base_layer(new matMaterialLayer(i_EffectID));
	m_MaterialLayers.push_back( base_layer);

	m_Flags.m_bForceTransparent = false;
	this->test_transparency();
}


//--------------------------------------------------------------------
//	The copy constructor does copy the material animations, if
//	any.
//--------------------------------------------------------------------
matMaterial::matMaterial(const matMaterial& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
matMaterial::~matMaterial()
{
	DestroyAllMatAnims();
	matTextureMgr::ReleaseTexture(m_PaintOverlay);
}

//--------------------------------------------------------------------
//	The operator = does copy the material animations, if any.
//--------------------------------------------------------------------
matMaterial& matMaterial::operator = (const matMaterial& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	this->m_Flags = i_CopyFrom.m_Flags;
	this->m_bHasGlow = i_CopyFrom.m_bHasGlow;
	this->m_GlowData = i_CopyFrom.m_GlowData;
	m_bHasOutline = i_CopyFrom.m_bHasOutline;
	m_OutlineData = i_CopyFrom.m_OutlineData;
	this->m_bHasReflection = i_CopyFrom.m_bHasReflection;
	this->m_ReflectionData = i_CopyFrom.m_ReflectionData;
	this->m_Name = i_CopyFrom.m_Name;
	this->m_PaintOverlay = i_CopyFrom.m_PaintOverlay;

	m_bHasDisplacement = i_CopyFrom.m_bHasDisplacement;
	m_DisplacementData = i_CopyFrom.m_DisplacementData;

	m_bHasRendermanOverride = i_CopyFrom.m_bHasRendermanOverride;
	m_RendermanOverrideData = i_CopyFrom.m_RendermanOverrideData;

	m_bHasStrandHair = i_CopyFrom.m_bHasStrandHair;
	m_StrandHairData = i_CopyFrom.m_StrandHairData;

	// Fill our material layer with clones of the layers
	// from the other class
	m_MaterialLayers.clear();
	for (int i=0; i<i_CopyFrom.m_MaterialLayers.size(); ++i)
	{
		shared_ptr<matMaterialLayer> new_layer(new matMaterialLayer(*i_CopyFrom.m_MaterialLayers[i]));
		m_MaterialLayers.push_back(new_layer);
	}

	// Copy material animations
	std::list<matMatAnim*>::const_iterator it = i_CopyFrom.m_MatAnims.begin();
	while (it != i_CopyFrom.m_MatAnims.end())
	{
		matMatAnim *mat_anim = (*it);
		DBG_ASSERT(mat_anim, "Null material anim pointer in copy");
		if (mat_anim)
			m_MatAnims.push_back(new matMatAnim(*mat_anim));
		++it;
	}

	return *this;
}

//--------------------------------------------------------------------
//	SimpleCopy() copies values from given material, but
//	does not copy the material animations, if any.
//--------------------------------------------------------------------
void matMaterial::SimpleCopy(const matMaterial& i_CopyFrom)
{
	m_Flags = i_CopyFrom.m_Flags;
	m_bHasGlow = i_CopyFrom.m_bHasGlow;
	m_GlowData = i_CopyFrom.m_GlowData;
	m_bHasOutline = i_CopyFrom.m_bHasOutline;
	m_OutlineData = i_CopyFrom.m_OutlineData;
	m_bHasReflection = i_CopyFrom.m_bHasReflection;
	m_ReflectionData = i_CopyFrom.m_ReflectionData;
	m_Name = i_CopyFrom.m_Name;

	m_bHasDisplacement = i_CopyFrom.m_bHasDisplacement;
	m_DisplacementData = i_CopyFrom.m_DisplacementData;

	m_bHasRendermanOverride = i_CopyFrom.m_bHasRendermanOverride;
	m_RendermanOverrideData = i_CopyFrom.m_RendermanOverrideData;

	m_bHasStrandHair = i_CopyFrom.m_bHasStrandHair;
	m_StrandHairData = i_CopyFrom.m_StrandHairData;

	// Fill our material layer with clones of the layers
	// from the other class
	m_MaterialLayers.clear();
	for (int i=0; i<i_CopyFrom.m_MaterialLayers.size(); ++i)
	{
		shared_ptr<matMaterialLayer> new_layer(new matMaterialLayer(*i_CopyFrom.m_MaterialLayers[i]));
		m_MaterialLayers.push_back(new_layer);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool matMaterial::operator == (const matMaterial& i_Material) const
{
	bool bEqual = 
	(m_Name == i_Material.m_Name) &&
	(m_Flags.m_bHasSpecular == i_Material.m_Flags.m_bHasSpecular) &&
	(m_Flags.m_bTransparency == i_Material.m_Flags.m_bTransparency) &&
	(m_Flags.m_bForceTransparent == i_Material.m_Flags.m_bForceTransparent) &&
	(m_bHasGlow == i_Material.m_bHasGlow) &&
	(m_GlowData == i_Material.m_GlowData) && 
	(m_bHasOutline == i_Material.m_bHasOutline) &&
	(m_OutlineData == i_Material.m_OutlineData) && 
	(m_bHasReflection == i_Material.m_bHasReflection) &&
	(m_ReflectionData == i_Material.m_ReflectionData) && 
	(m_bHasDisplacement == i_Material.m_bHasDisplacement) &&
	(m_bHasRendermanOverride == i_Material.m_bHasRendermanOverride) &&
	(m_DisplacementData == i_Material.m_DisplacementData) &&
	(m_bHasStrandHair == i_Material.m_bHasStrandHair) &&
	(m_StrandHairData == i_Material.m_StrandHairData);

	if (!bEqual) 
		return false;

	if (m_MaterialLayers.size() != i_Material.m_MaterialLayers.size())
		return false;

	for (int i=0; i<m_MaterialLayers.size(); ++i)
	{
		if (m_MaterialLayers[i] != i_Material.m_MaterialLayers[i])
			return false;
	}

	return true;
}

//--------------------------------------------------------------------
// Access to material layers
//--------------------------------------------------------------------
const shared_ptr<matMaterialLayer> matMaterial::GetMaterialLayer(int i_Index) const
{
	return m_MaterialLayers[i_Index];
}

//--------------------------------------------------------------------
// Add an extra material layer with the given material parameters
//--------------------------------------------------------------------
//void matMaterial::AddMaterialLayer(shared_ptr<effShaderParams> i_Params)
void matMaterial::AddMaterialLayer()
{
	shared_ptr<matMaterialLayer> new_layer(new matMaterialLayer());
	m_MaterialLayers.push_back( new_layer );
}

//--------------------------------------------------------------------
// Remove all material layers except the base material layer
//--------------------------------------------------------------------
void matMaterial::RemoveExtraMaterialLayers()
{
	// material layers are shared_ptrs, they clean up automatically
	m_MaterialLayers.resize(1);

	//? this->test_transparency();
}

//--------------------------------------------------------------------
//	GetHasTransparency returns true if the material
//	might be translucent or transparent (this is important for
//	sorting things).
//--------------------------------------------------------------------
 bool matMaterial::GetHasTransparency() const
{
	if (m_Flags.m_bTransparency)
		return true;

	// Check only base layer?
	if (m_MaterialLayers[0]->GetHasTransparency())
		return true;

	return false;
}

//--------------------------------------------------------------------
//	GetHasSpecular returns true if the material should be rendered
//	with a specular highlight.
//--------------------------------------------------------------------
 bool matMaterial::GetHasSpecular() const
{
	return m_Flags.m_bHasSpecular;
}


//--------------------------------------------------------------------
//	SetHasSpecular allows the user to specify if the material should
//	be rendered with a specular highlight.
//--------------------------------------------------------------------
void matMaterial::SetHasSpecular(bool i_Specular)
{
	m_Flags.m_bHasSpecular = i_Specular;
}

//--------------------------------------------------------------------
//	The paint overlay will allow the material to be painted
//--------------------------------------------------------------------
void matMaterial::SetPaintOverlay(matTexture* i_PaintOverlay)
{
	m_PaintOverlay = i_PaintOverlay;
}
matTexture* matMaterial::GetPaintOverlay() const
{
	return m_PaintOverlay;
}

//--------------------------------------------------------------------
//	ShaderEffect controls algorithm for rendering with
//		multiple passes
//--------------------------------------------------------------------
void matMaterial::SetShaderEffect(const std::string& i_EffectID, const effShaderData* i_Data /*= NULL*/) const 
{
	// Legacy support, apply this to base material layer
	m_MaterialLayers[0]->SetShaderEffect(i_EffectID, i_Data);
}

//--------------------------------------------------------------------
//	RemoveTextures removes all textures
//--------------------------------------------------------------------
void matMaterial::RemoveTextures()
{
	for (int i=0; i<m_MaterialLayers.size(); ++i)
	{
		m_MaterialLayers[i]->RemoveTextures();
	}

	m_GlowData.RemoveTextures();
	m_OutlineData.RemoveTextures();
	this->test_transparency();
}

//--------------------------------------------------------------------
//	SetForceTransparency forces the material to be considered
//	transparent.  If it is false then it is tested for transparency
//	using the normal methods.
//--------------------------------------------------------------------
void matMaterial::ForceTransparency(bool i_bTransparent)
{
	m_Flags.m_bForceTransparent = i_bTransparent;

	test_transparency();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
 std::string matMaterial::GetEffectID() const
{
	// Legacy support, return base material layer
	return m_MaterialLayers[0]->GetEffectID();
}

//----------------------------------------------------------------------------
//	SetAdditive turns on additive mode rendering.
//----------------------------------------------------------------------------
void matMaterial::SetAdditive( bool i_bAdditive )
{
	m_Flags.m_bAdditive = i_bAdditive;
}

//--------------------------------------------------------------------
//	GetDefaultMaterial returns the default material, which is used
//	by the matPrimitiveUtil.  The default material can be modified.
//--------------------------------------------------------------------
//matMaterial& matMaterial::GetDefaultMaterial()
//{
//	return l_DefaultMaterial;
//}

//--------------------------------------------------------------------
// ModifyMatAnim will return a modifiable material anim, the given
// index must be in the mat anim list or this will assert
//--------------------------------------------------------------------
matMatAnim* matMaterial::ModifyMatAnim(int i_Index)
{
	DBG_ASSERT(i_Index >= 0 && i_Index < m_MatAnims.size(), "Invalid mat anim index!");
	if (i_Index < 0 || i_Index >= m_MatAnims.size())
		return NULL;

	int i;
	std::list<matMatAnim*>::iterator it = m_MatAnims.begin();
	for (i=0; i<i_Index; ++i)
		++it;
	return *it;
}

//--------------------------------------------------------------------
//	AddMatAnim adds a material animation to the list
//--------------------------------------------------------------------
void matMaterial::AddMatAnim( matMatAnim* i_MatAnim )
{
	m_MatAnims.push_back( i_MatAnim );
}

//--------------------------------------------------------------------
//	DestroyAllMatAnims deletes all of the material animations
//--------------------------------------------------------------------
void matMaterial::DestroyAllMatAnims()
{
	envSTLHelpers::DeleteContainer( m_MatAnims );
}

//--------------------------------------------------------------------
//	Activate/DeactivateMatAnim - sets/unsets active flag
//	on material animation with given index.  If StartTime>0
//	then the start time of the material animation is reset.
//  Use -1 as Index to affect all animations.
//--------------------------------------------------------------------
void matMaterial::ActivateMatAnim( int i_Index, float i_StartTime )
{
	int num_anims = m_MatAnims.size();
	DBG_ASSERT(i_Index < num_anims, "MaterialAnimation out of range (index = " << i_Index << " - num anims = " << num_anims << ")" );
	if (i_Index >= num_anims)
		return;

	std::list<matMatAnim*>::iterator it = m_MatAnims.begin();
	std::list<matMatAnim*>::iterator end = m_MatAnims.end();
	for (int i=0; it != end; ++it, i++)
	{
		if (i_Index < 0 || i_Index == i)
		{
			(*it)->SetActive(true);
			if (i_StartTime > 0)
				(*it)->SetStartTime(i_StartTime);
		}
	}
}
void matMaterial::DeactivateMatAnim( int i_Index )
{
	int num_anims = m_MatAnims.size();
	DBG_ASSERT(i_Index < num_anims, "MaterialAnimation out of range (index = " << i_Index << " - num anims = " << num_anims << ")" );
	if (i_Index >= num_anims)
		return;

	std::list<matMatAnim*>::iterator it = m_MatAnims.begin();
	std::list<matMatAnim*>::iterator end = m_MatAnims.end();
	for (int i=0; it != end; ++it, i++)
	{
		if (i_Index < 0 || i_Index == i)
			(*it)->SetActive(false);
	}
}

//----------------------------------------------------------------------------
//	test_transparency inspects all of the texture and color information
//	and decides if the material could be transparent.
//----------------------------------------------------------------------------
void matMaterial::test_transparency()
{
	// it is transparent if it is being forced to transparent
	if ( m_Flags.m_bForceTransparent )
	{
		m_Flags.m_bTransparency = true;
		return;
	}

	// Check only base layer?
	if (m_MaterialLayers[0]->GetHasTransparency())
	{
		m_Flags.m_bTransparency = true;
		return;
	}

	//	It is probably not transparent 
	m_Flags.m_bTransparency = false;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
effShaderData* matMaterial::GetEffectData(int i_Layer) const
{
	DBG_ASSERT(i_Layer >= 0 && i_Layer < m_MaterialLayers.size(), "Material layer index out of range");
	if (i_Layer < 0 || i_Layer >= m_MaterialLayers.size())
		return NULL;
	return m_MaterialLayers[i_Layer]->GetEffectData();
}

//--------------------------------------------------------------------
// Shader Parameters - properties specific to the shader technique
//--------------------------------------------------------------------
shared_ptr<effShaderParams> matMaterial::GetShaderParams(int i_Layer) const
{
	DBG_ASSERT(i_Layer >= 0 && i_Layer < m_MaterialLayers.size(), "Material layer index out of range");
	
	return m_MaterialLayers[i_Layer]->GetShaderParams();
}
void matMaterial::SetShaderParams(shared_ptr<effShaderParams> i_Params)
{
	// Legacy support, sets base material layer
	return m_MaterialLayers[0]->SetShaderParams(i_Params);
}
void matMaterial::SetShaderParams(int i_Layer, shared_ptr<effShaderParams> i_Params)
{
	DBG_ASSERT(i_Layer >= 0 && i_Layer < m_MaterialLayers.size(), "Material layer index out of range");
	if (i_Layer < 0 || i_Layer >= m_MaterialLayers.size())
		return;
	return m_MaterialLayers[i_Layer]->SetShaderParams(i_Params);
}

//--------------------------------------------------------------------
// UVTransform
//--------------------------------------------------------------------
effUVTransform& matMaterial::UVTransform(int i_Layer) const 
{
	DBG_ASSERT(i_Layer >= 0 && i_Layer < m_MaterialLayers.size(), "Material layer index out of range");
	
	return m_MaterialLayers[i_Layer]->UVTransform();
}
const effUVTransform& matMaterial::GetUVTransform(int i_Layer) const 
{
	DBG_ASSERT(i_Layer >= 0 && i_Layer < m_MaterialLayers.size(), "Material layer index out of range");
	
	return m_MaterialLayers[i_Layer]->GetUVTransform();
}

void matMaterial::SetSolidColor(maVector3d i_Color)
{
	m_SolidColor = i_Color;
}

maVector3d matMaterial::GetSolidColor()
{
	return m_SolidColor;
}

void matMaterial::SetBelongsToLightShaft(bool i_bBelongsToLightShaft)
{
	m_bBelongsToLightShaft = i_bBelongsToLightShaft;
}

bool matMaterial::GetBelongsToLightShaft() const
{
	return m_bBelongsToLightShaft;
}