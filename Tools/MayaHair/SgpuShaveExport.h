#include <iostream>
#include <vector>
#include <maya/MPxCommand.h>
#include <maya/MString.h>
#include <maya/MSyntax.h>
#include <maya/MTypes.h>


#include <shaveAPI.h>
#include <shaveItHair.h>

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
struct mdlHairInfo;
struct ShaveExportArgs;

class sgpuShaveExport : public MPxCommand
{

public:
	static MSyntax createSyntax();
	sgpuShaveExport();
	virtual      ~sgpuShaveExport();	
	MStatus		doIt(const MArgList&);
	bool         isUndoable() const;
	static void* creator();

protected:
	//parse the arguments into a 'ShaveExportArgs' structure
	MStatus		parseArgs( const MArgList& argList, ShaveExportArgs &exportArgs );
	//		Export selected hair shape nodes to a .txt file or to a .gxb file
	MStatus		doItCore_Geom( const ShaveExportArgs &exportArgs );
	//		Export seleted hair vertices into a compressed vertex animation
	MStatus		doItCore_Anim( const ShaveExportArgs &exportArgs );
	
	//		Export shaveAPI::HairInfo to a txt file
	void         exportHairInfo(
		shaveAPI::HairInfo* i_pHairInfo, 
		bool i_bInstances,
		std::ostream &io_os
		) const;
	//		Export shaveAPI::HairInfo to a mdlHairInfo structure
	void exportHairInfo(
		const shaveAPI::HairInfo* i_HairInfo,
		bool i_bInstances,
		shared_ptr< mdlHairInfo > &o_HairInfo
		)const;

private:
	static const char* m_HelpFlag;
	static const char* m_HelpFlagLong;
	static const char* m_HelpText;
	static const char *m_OutFilename;
	static const char *m_OutFilenameLong;
	static const char *m_ModelIntent;
	static const char *m_AnimIntent;
	static const char *m_StartFrame;	
	static const char *m_EndFrame;
	static const char *m_ModelIntentLong;
	static const char *m_AnimIntentLong;
	static const char *m_StartFrameLong;
	static const char *m_EndFrameLong;
	static const char *m_Compress;
	static const char *m_CompressLong;
	static const char *m_Tolerance;
	static const char *m_ToleranceLong;
};