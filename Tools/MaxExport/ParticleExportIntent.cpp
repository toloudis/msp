/*****************************************************************************
**  ParticleExportIntent.cpp
**
**	Exports max nodes
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "ParticleExportIntent.hpp"


#include "MaxExportUtils.hpp"


using namespace std;
using namespace stdext;

namespace MaxExp
{
	
	void ParticleExportIntent::Do()
	{	
		EXPLOG.WriteError( _T("exporting of particle not implemented") );
	}


	//=============================================================================
	// Cleanup the  book-keeping data structures
	//=============================================================================
	void ParticleExportIntent::CleanUp()
	{	
		ExportIntent::CleanUp();
	}




} //maxExp