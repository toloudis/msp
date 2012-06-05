/*****************************************************************************
**  WriteCharacter.hpp
**
**     Command to write .chx file of a character controlled by
**	a jointed skeleton, morph targets and cluster groups.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef WRITECHARACTER_HPP
#error WriteCharacter.hpp multiply included
#endif
#define WRITECHARACTER_HPP

#include <maya/MPxCommand.h>

class WriteCharacter: public MPxCommand
{
public:
	WriteCharacter();
	virtual	~WriteCharacter() {}
	static void*	creator();
	virtual MStatus	doIt( const MArgList& );
	virtual MStatus	debugIt( const MArgList& );
private:
	virtual MStatus	ParseArgs( const MArgList& );
	MStatus WriteCharacter::doSubAnim();

	MString	filePath;
	bool	bDoGeom;
	bool	bDoAnim;
	bool	bDoSubAnim;
	bool	shareMaterials;
	bool	bDoPose;
	bool	bDoDeltas;
	bool	bForceAllSubdivs;
	bool	bUseMayaBaking;
};

