/*****************************************************************************
**	cmraLensTable.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Data/cmraLensTable.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "Core/Ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"

namespace cmraLensTable
{
	enum FilmGates
	{
		e_User = 0,
		e_16mm = 1,
		e_Super16 = 2,
		e_35Acad = 3,
		e_35TVProj = 4,
		e_35FullAperture = 5,
		e_35_185Proj = 6,
		e_35Anamorphic = 7,
		e_70Projection = 8,
		e_VistaVision = 9,
		e_IMax = 10
	};

	const float c_InchesToMM = 25.4f;
	const float c_MMToInches = 1.0f / 25.4f;

	// In Maya and our engine, the field of view only affects the width, so
	// we only need to use the 36mm part of the film dimension
	// The values donot have any relation to the number on the FilmDimension
	// For e.g 35mm doesnot necessarily mean a 35mm diagonal. The values are taken from Maya
	const float c_16mmTheatricalFilmDimension = 0.404f;
	const float c_Super16mmFilmDimension = 0.493f; 
	const float c_35mmAcademyFilmDimension = 0.864f;
	const float c_35mmTvProjectionFilmDimension = 0.816f;
	const float c_35mmFullApertureFilmDimension = 0.980f;
	const float c_35mm185ProjectionFilmDimension = 0.825f;
	const float c_35mmAnamorphicFilmDimension = 0.864f;
	const float c_70mmProjectionFilmDimension = 2.066f;
	const float c_VistaVisionFilmDimension = 1.485f;
	const float c_IMaxFilmDimension = 2.772f;


	//--------------------------------------------------------------------
	// Add the types of flim gates possible to this enumeration.
	//--------------------------------------------------------------------
	void SetUpFilmGateEnum(prtyEnum &o_FilmGate)
	{
		o_FilmGate.SetEnumTag(e_User, "User");
		o_FilmGate.SetEnumTag(e_16mm, "16mm");
		o_FilmGate.SetEnumTag(e_Super16, "Super 16mm");
		o_FilmGate.SetEnumTag(e_35Acad, "35mm Academy");
		o_FilmGate.SetEnumTag(e_35TVProj, "35mm TV Projection");
		o_FilmGate.SetEnumTag(e_35FullAperture, "35mm Full Aperture");
		o_FilmGate.SetEnumTag(e_35_185Proj, "35mm 1.85 Projection");
		o_FilmGate.SetEnumTag(e_35Anamorphic, "35mm Anamorphic");
		o_FilmGate.SetEnumTag(e_70Projection, "70mm Projection");
		o_FilmGate.SetEnumTag(e_VistaVision, "Vista Vision");
		o_FilmGate.SetEnumTag(e_IMax, "IMax");


	}

	//--------------------------------------------------------------------
	// Return true if this enumeration value is the "User" type.
	// If it is the user type, then do not use the table for the
	// camera properties.
	//--------------------------------------------------------------------
	bool IsUserType(int i_FilmGateEnumValue)
	{
		return (i_FilmGateEnumValue == e_User);
	}
	int GetUserType()
	{
		return e_User;
	}
	
	//--------------------------------------------------------------------
	// Returns horizontal aperture in inches for the flim gate given
	//--------------------------------------------------------------------
	float GetHorizontalAperture(int i_FilmGateEnumValue)
	{
		DBG_ASSERT(i_FilmGateEnumValue!=e_User, "Cannot user lens table for user types.");
		switch (i_FilmGateEnumValue)
		{
		default:
		case e_16mm:
			return c_16mmTheatricalFilmDimension;
		case e_Super16:
			return c_Super16mmFilmDimension;
		case e_35Acad:
			return c_35mmAcademyFilmDimension;
		case e_35TVProj:
			return c_35mmTvProjectionFilmDimension;
		case e_35FullAperture:
			return c_35mmFullApertureFilmDimension;
		case e_35_185Proj:
			return c_35mm185ProjectionFilmDimension;
		case e_35Anamorphic:
			return c_35mmAnamorphicFilmDimension;
		case e_70Projection:
			return c_70mmProjectionFilmDimension;
		case e_VistaVision:
			return c_VistaVisionFilmDimension;
		case e_IMax:
			return c_IMaxFilmDimension;
		}
	}
	int GetEnumerationForAperture(float i_HorizontalAperture)
	{
		if (fabsf(i_HorizontalAperture - c_16mmTheatricalFilmDimension) < maConstants::c_fEpsilon)
			return e_16mm;
		if (fabsf(i_HorizontalAperture - c_Super16mmFilmDimension) < maConstants::c_fEpsilon)
			return e_Super16;
		if (fabsf(i_HorizontalAperture - c_35mmAcademyFilmDimension) < maConstants::c_fEpsilon)
			return e_35Acad;
		if (fabsf(i_HorizontalAperture - c_35mmTvProjectionFilmDimension) < maConstants::c_fEpsilon)
			return e_35TVProj;
		if (fabsf(i_HorizontalAperture - c_35mmFullApertureFilmDimension) < maConstants::c_fEpsilon)
			return e_35FullAperture;
		if (fabsf(i_HorizontalAperture - c_35mm185ProjectionFilmDimension) < maConstants::c_fEpsilon)
			return e_35_185Proj;
		if (fabsf(i_HorizontalAperture - c_35mmAnamorphicFilmDimension) < maConstants::c_fEpsilon)
			return e_35Anamorphic;
		if (fabsf(i_HorizontalAperture - c_70mmProjectionFilmDimension) < maConstants::c_fEpsilon)
			return e_70Projection;
		if (fabsf(i_HorizontalAperture - c_VistaVisionFilmDimension) < maConstants::c_fEpsilon)
			return e_VistaVision;
		if (fabsf(i_HorizontalAperture - c_IMaxFilmDimension) < maConstants::c_fEpsilon)
			return e_IMax;

		else
			return e_User;
	}

} // end of namespace


