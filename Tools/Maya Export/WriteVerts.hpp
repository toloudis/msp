/*****************************************************************************
**  WriteVerts.hpp
**
**     Command to write pure vertex animation in world space to .gxb 
**	and .gab files.
**
**	Extra Large Technology
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef WRITEVERTS_HPP
#error WriteVerts.hpp multiply included
#endif
#define WRITEVERTS_HPP

#ifndef BAKECOMMAND_HPP
#include <BakeCommand.hpp>
#endif

class WriteVerts: public BakeCommand
{
public:
	WriteVerts();
	virtual	~WriteVerts() {}
	static void*	creator();
	virtual MStatus	doIt( const MArgList& );
	MStatus safeDoIt(const MArgList &args);
private:
	virtual MStatus	ParseArgs( const MArgList& );
	//MStatus doSubAnim();

	MString	filePath;
	bool	bDoGeom;
	bool	bDoAnim;
	bool	bForceAllSubdivs;
	bool	bStatic;
	bool	bConfirmFlags;
	bool	bSuggest;
	bool	bMergeMaterials;
	bool	bUseCompressStream;
	float   m_Tolerance;
};

