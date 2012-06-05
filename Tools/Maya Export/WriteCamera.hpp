/*****************************************************************************
**  WriteCamera.hpp
**
**     Command to write lod versions of.mx file from selected subdivision 
**	surfaces.  
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef WriteCamera_HPP
#error WriteCamera.hpp multiply included
#endif
#define WriteCamera_HPP

#ifndef BAKECOMMAND_HPP
#include <BakeCommand.hpp>
#endif

class WriteCamera: public BakeCommand
{
public:
	WriteCamera() ;
	virtual	~WriteCamera() {}
	static void*	creator();
	virtual MStatus	doIt( const MArgList& );
	MStatus safeDoIt(const MArgList &args);
private:
	virtual MStatus	ParseArgs( const MArgList& );
	MString	filePath;
	bool	bUseMayaAnimCurves;

};

