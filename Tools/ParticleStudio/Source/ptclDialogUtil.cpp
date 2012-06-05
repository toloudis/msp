/*****************************************************************************
**	ptclDialogUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "StdAfx.h"
#include "ptclDialogUtil.hpp"
#include "ptclOperations.hpp"

#include "MainForm.h"
#include "ParticleDialog.h"

//#include "ptclLevel.hpp"

#include "Core/dbg/dbgLog.hpp"


using namespace ParticleStudio;


namespace ptclDialogUtil
{
	namespace
	{
		//ptclMaterialTemplate l_ParticleData;

		void setup_particle(int i_Index)
		{
			//ptclOperations::SetSelectedMaterialIndex(i_Index);
			
			//l_ParticleData = ptclLevel::GetMaterialData(i_Index);

			if (ParticleDialog::FormInstance)
				ParticleDialog::FormInstance->UpdateData();
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
		ParticleDialog::FormInstance = nullptr;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ShowParticleDialog(bool i_bShow)
	{
		if (i_bShow)
		{
			if (!ParticleDialog::FormInstance)
			{
				ParticleDialog::FormInstance = gcnew ParticleDialog();
			}

			ParticleDialog::FormInstance->Show();
		}
		else
		{
			if (ParticleDialog::FormInstance)
			{
				ParticleDialog::FormInstance->Hide();
				CleanUp();
			}
		}
	}

}	// end of namespace
