/*****************************************************************************
**  ForceVerts.hpp
**
**     Command to write vertex animation of meshes.  With argument "-geom"
**	it writes .vtx file and with "-anim" it writes .vta file.   
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef FORCEVERTS_HPP
#error ForceVerts.hpp multiply included
#endif
#define FORCEVERTS_HPP

#ifndef BAKECOMMAND_HPP
#include <BakeCommand.hpp>
#endif

class MDagPath;

class ForceVerts: public BakeCommand
{
public:
					ForceVerts();
	virtual			~ForceVerts();
	static void*	creator();
	virtual MStatus	doIt( const MArgList& );
	MStatus safeDoIt(const MArgList &args);

private:
	MStatus			parseArgs(const MArgList& args);
	MStatus			doScan();
	void			printTransformData(const MDagPath& dagPath, bool quiet);
	bool	bDoGeom;
	bool	bDoAnim;
	bool    bShareMaterials;
	bool	bConfirmFlags;
	bool	bSuggest;
	MString	filePath;
	int		subd_depth;
	int		end_frame;
	int		begin_frame;
	int		frame_step;
};
