/****************************************************************************\
**  sgpuFileWriterLifeTimeKeeper.hpp
**
**      sgpuFileWriterLifeTimeKeeper.hpp defines private implementation for sgpuMaterial.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_FILEWRITERLIFETIMEKEEPER_HPP
#define SGPU_FILEWRITERLIFETIMEKEEPER_HPP

class gfFileBin;
class sgpuBaseScene;
class chBinWriter;
class sgpuString;

		//A wrapper for governing the
		//life time semantics of opening
		//and closing of the exported file
		struct sgpuFileWriterLifeTimeKeeper
		{
			sgpuFileWriterLifeTimeKeeper( sgpuBaseScene &i_Scene, const sgpuString &i_ExportFilename );
			~sgpuFileWriterLifeTimeKeeper();
			void Cleanup();
			sgpuBaseScene *m_pScene;
			gfFileBin *m_pFile;
			chBinWriter *m_pWriter;
		};
#endif // #ifndef SGPU_FILEWRITERLIFETIMEKEEPER_HPP
