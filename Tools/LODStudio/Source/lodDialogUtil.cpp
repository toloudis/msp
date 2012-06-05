/*****************************************************************************
**	lodDialogUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "StdAfx.h"
#include "lodDialogUtil.hpp"
#include "lodOperations.hpp"

#include "MainForm.h"
#include "LODDialog.h"

//#include "lodLevel.hpp"

#include "dbgLog.hpp"


using namespace LODStudio;


namespace lodDialogUtil
{
	namespace
	{
		//lodMaterialTemplate l_LODData;

		void setup_LOD(int i_Index)
		{
			lodOperations::SetSelectedMaterialIndex(i_Index);

			//l_LODData = lodLevel::GetMaterialData(i_Index);

			if (LODDialog::FormInstance)
				LODDialog::FormInstance->UpdateData();
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{
	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		LODDialog::FormInstance = 0;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ShowLODDialog( lod3dData& i_Data )
	{
		if (!LODDialog::FormInstance)
		{
			LODDialog::FormInstance = new LODDialog( i_Data );
		}

		LODDialog::FormInstance->Show();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void HideLODDialog()
	{
		if (LODDialog::FormInstance)
		{
			LODDialog::FormInstance->Hide();
			CleanUp();
		}
	}

}	// end of namespace
