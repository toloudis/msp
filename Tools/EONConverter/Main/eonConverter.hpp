/*****************************************************************************
**	eonConverter.hpp
**
**		Reads in instructions from a conversion file and then converts
**	geometry from MachStudio to EONReality format, assigning materials
**	based on the conversion file.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef EON_CONVERTER_HPP
#error eonConverter.hpp multiply included
#endif
#define EON_CONVERTER_HPP

//============================================================================
//	forward references
//============================================================================
class fsLocator;

//============================================================================
//============================================================================
namespace eonConverter
{
	//------------------------------------------------------------------------
	//  DoConversion() - read instructions from conversion file and
	//		do conversion of file formats.
	//------------------------------------------------------------------------
	void  DoConversion(const fsLocator& i_ConvFile);
}
