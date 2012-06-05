/*****************************************************************************
**  WriteParticles.hpp
**
**     Command to write vertex animation of particles.   
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef WRITEPARTICLES_HPP
#error WriteParticles.hpp multiply included
#endif
#define WRITEPARTICLES_HPP

#ifndef BAKECOMMAND_HPP
#include <BakeCommand.hpp>
#endif

class MDagPath;

class WriteParticles: public BakeCommand
{
public:
					WriteParticles();
	virtual			~WriteParticles();
	static void*	creator();
	virtual MStatus	doIt( const MArgList& );
	MStatus safeDoIt(const MArgList &args);

private:
	MStatus			parseArgs(const MArgList& args);
	MStatus			doScan();
	void			printTransformData(const MDagPath& dagPath, bool quiet);
	bool	bDoGeom;
	bool	bDoAnim;
	bool	bConfirmFlags;
	bool	bSuggest;
	MString	filePath;
	int		end_frame;
	int		begin_frame;
	int		frame_step;
};
