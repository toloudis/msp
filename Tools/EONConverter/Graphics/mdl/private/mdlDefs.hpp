/****************************************************************************\
**  mdlDefs.hpp
**
**      mdlDefs supplies constants for configuring parsing
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_DEFS_HPP
#error mdlDefs.hpp multiply included
#endif
#define MDL_DEFS_HPP


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace mdlDefs
{
	// Read the whole array at once. Will fail on text file formats
	// or on files with different endian.
	const bool c_ReadArraysAtOnce = true;

	//----------------------------------------------------------------------------
	//	SetVerboseMode will cause a bunch of debugging information and statistics
	//	about whatever meshes are being loaded to be written to the debug log.
	//----------------------------------------------------------------------------
	void SetVerboseMode(bool i_Verbose);
	bool GetVerboseMode();

}