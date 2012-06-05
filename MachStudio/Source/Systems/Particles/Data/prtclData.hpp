/********************************************************************************************\
**  prtclData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#ifdef PRTCL_DATA_HPP
#error prtclData.hpp multiply included
#endif
#define PRTCL_DATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif
//#ifndef PRTY_FILEPATH_HPP
//#include "Core/prty/prtyFilePath.hpp"
//#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_INT8_HPP
#include "Core/prty/prtyInt8.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif
#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif
#ifndef PRTY_ROTATION_HPP
#include "Core/prty/prtyRotation.hpp"
#endif


//============================================================================
//============================================================================
class prtclData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtclData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtclData(const itString& i_Filename );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtclData(const itString& i_Filename,
			  const maPoint3d& i_Position,
			  const maRotation& i_Orientation );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const prtclData& i_Item);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtclData& operator=(const prtclData& i_Data);

	//------------------------------------------------------------------------
	//	Data
	//
	//	Currently all data for all types of generators, emitters, etc are
	//	in here.  The goal is to evaluate how this system works and break
	//	it apart if necessary.
	//------------------------------------------------------------------------
	prtyBoolean		m_bEditorVisible;
	prtyName		m_Name;
	prtyFileName	m_Filename;
	prtyPoint3d		m_Position;
	prtyRotation	m_Orientation;

	prtyFloat		m_Rate;					// Base Generator
	prtyFloat		m_MaxParticles;
	prtyFloat		m_LifetimeMin;
	prtyFloat		m_LifetimeMax;

	prtyFloat		m_ScaleStart;			// Sprite Particles
	prtyFloat		m_ScaleCoefficient;
	prtyEnum		m_ScaleMode;
	prtyFloat		m_StartAngleMin;
	prtyFloat		m_StartAngleMax;
	prtyFloat		m_AngularVelocityMin;
	prtyFloat		m_AngularVelocityMax;
	prtyFloat		m_AngularAccelerationMin;
	prtyFloat		m_AngularAccelerationMax;

	prtyFloat		m_EmitterScale;			// Emitter

	prtyFloat		m_ConeAngle;		// prtConeParticleGenerator
	prtyFloat		m_MinSpeed;
	prtyFloat		m_MaxSpeed;
	prtyFloat		m_AccelerationX;
	prtyFloat		m_AccelerationY;
	prtyFloat		m_AccelerationZ;

	prtyFloat		m_MinEmitSpeed;		// prtSpiralParticleGenerator
	prtyFloat		m_MaxEmitSpeed;
	prtyFloat		m_EmitDirectionX;
	prtyFloat		m_EmitDirectionY;
	prtyFloat		m_EmitDirectionZ;
	prtyFloat		m_MinRotStartAngle;
	prtyFloat		m_MaxRotStartAngle;
	prtyFloat		m_MinRotAngularVel;
	prtyFloat		m_MaxRotAngularVel;
	prtyFloat		m_PreSimTime;
	prtyFloat		m_RotRadius;
	prtyFloat		m_RotRadiusScaleRate;
	//prtyFloat		m_AccelerationX;
	//prtyFloat		m_AccelerationY;
	//prtyFloat		m_AccelerationZ;

	prtyFloat		m_TextureAlphaStart;		// Texture
	prtyFloat		m_TextureAlphaMiddle;
	prtyFloat		m_TextureAlphaEnd;
	prtyFloat		m_TextureAlphaMiddlePercentStart;
	prtyFloat		m_TextureAlphaMiddlePercentEnd;
	prtyFileName	m_TextureFilename;
	prtyInt8		m_TextureRows;
	prtyInt8		m_TextureCols;
	prtyBoolean		m_bTextureLooping;
	prtyBoolean		m_bTextureReverse;
	prtyFloat		m_TextureRate;
	prtyEnum		m_TextureUVAMode;

	prtyBoolean		m_bRenderStreaks;
	prtyFloat		m_StreakLength;
	prtyFloat		m_StreakTaper;
	prtyFloat		m_StreakFade;

	prtyBoolean		m_bShowInCubeReflections;
	prtyBoolean		m_bShowInPlanarReflections;
	prtyBoolean		m_bCastShadows;
	prtyBoolean		m_bUseDitheredShadows;
	prtyFloat		m_ShadowDitherBias;
	prtyBoolean		m_bAdditive;
};

