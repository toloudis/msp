/*****************************************************************************
**	fioUtils.hpp
**
**		handles helper functions to separate app functionality from the UI.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef FIO_UTILS_HPP
#error fioUtils.hpp multiply defined
#endif
#define FIO_UTILS_HPP


//============================================================================
// Forward References
//============================================================================
class fsLocator;


//============================================================================
//============================================================================
namespace fioUtils
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void CopyFile( const fsLocator& i_From, const fsLocator& i_To );

	//------------------------------------------------------------------------
	///	Write an XML file that stores an absolute location
	//------------------------------------------------------------------------
	void WriteLocationXML(const fsLocator& i_XMLFile, const fsLocator& i_DataPathToSave);
	
	//------------------------------------------------------------------------
	///	Read an XML file that stores an absolute location
	//------------------------------------------------------------------------
	void ReadLocationXML(const fsLocator& i_XMLFile, fsLocator& o_DataPathSaved);

}	//end namespace fioUtils