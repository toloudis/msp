/*****************************************************************************
**  WriteModel.hpp
**
**     Command to write .gxb file of a character controlled by
**	a jointed skeleton, morph targets and cluster groups.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef WRITEMODEL_HPP
#error WriteModel.hpp multiply included
#endif
#define WRITEMODEL_HPP

#ifndef BAKECOMMAND_HPP
#include <BakeCommand.hpp>
#endif

class WriteModel: public BakeCommand
{
public:
	WriteModel();
	virtual	~WriteModel() {}
	static void*	creator();
	virtual MStatus	doIt( const MArgList& );
	MStatus	safeDoIt( const MArgList& );
private:
	virtual MStatus	ParseArgs( const MArgList& );
	//MStatus doSubAnim();

	MString	filePath;
	bool	bDoGeom;
	bool	bDoAnim;
	bool	bStatic;
	//bool	bDoSubAnim;
	bool	shareMaterials;
	bool	bDoPose;
	bool	bDoDeltas;
	bool	bForceAllSubdivs;
	bool	bUseMayaAnimCurves;
	bool	bMergeMaterials;
	bool	bConfirmFlags;
	bool	bSuggest;
	bool	bAutoDetect;
	bool	bUseCompressStream;
	float   m_Tolerance;
};

