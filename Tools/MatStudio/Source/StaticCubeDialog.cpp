#include "StdAfx.h"
#include "StaticCubeDialog.h"

#include "mtrLevel.hpp"
#include "mtrStaticCubeSaver.hpp"

#include "fsLocator.hpp"
#include "tmaManagedStringUtils.hpp"
#include "muiFileDialogUtils.hpp"

using namespace MatStudio;

namespace
{
	const char* c_Filter = "Static Cubemap Files (*.scm)|*.scm|All files (*.*)|*.*";
}

System::Void StaticCubeDialog::butLoad_Click(System::Object *  sender, System::EventArgs *  e)
{
	fsLocator texture_dir = mtrLevel::GetTextureDir();
	fsLocator locator;
	std::string strings[6];
	if (muiFileDialogUtils::GetOpenFileName(c_Filter, texture_dir, locator))
	{
		if (mtrStaticCubeSaver::Read(locator, strings))
		{
			this->fileChooser1->Fullpath = strings[0].c_str();
			this->fileChooser2->Fullpath = strings[1].c_str();
			this->fileChooser3->Fullpath = strings[2].c_str();
			this->fileChooser4->Fullpath = strings[3].c_str();
			this->fileChooser5->Fullpath = strings[4].c_str();
			this->fileChooser6->Fullpath = strings[5].c_str();
		}
	}		
}

System::Void StaticCubeDialog::butSave_Click(System::Object *  sender, System::EventArgs *  e)
{
	std::string strings[6];

	tmaManagedStringUtils::ManagedStringToStdString(fileChooser1->Filename, strings[0]);
	tmaManagedStringUtils::ManagedStringToStdString(fileChooser2->Filename, strings[1]);
	tmaManagedStringUtils::ManagedStringToStdString(fileChooser3->Filename, strings[2]);
	tmaManagedStringUtils::ManagedStringToStdString(fileChooser4->Filename, strings[3]);
	tmaManagedStringUtils::ManagedStringToStdString(fileChooser5->Filename, strings[4]);
	tmaManagedStringUtils::ManagedStringToStdString(fileChooser6->Filename, strings[5]);
	
	// Make sure we have 6 textures
	if (strings[0].empty() || strings[1].empty() || strings[2].empty() || 
		strings[3].empty() || strings[4].empty() || strings[5].empty())
	{
		MessageBox::Show("Static Cube Map needs 6 textures", "Error");
		return;
	}

	fsLocator locator;
	if (muiFileDialogUtils::GetSaveFileName(c_Filter, locator))
	{
		mtrStaticCubeSaver::Write(locator, strings);
	}		
}