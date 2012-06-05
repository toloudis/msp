#include "StdAfx.h"
#include "MaterialDialog.h"
#include "TextureDialog.h"

#include "muiFileDialogUtils.hpp"
//#include "itStringUtil.hpp"

using namespace MatStudio;

bool DlgGetTextureFilename(std::string& fname)
{
	std::string filter = "Texture files (*.*)|*.*";
	fsLocator initial_dir = mtrLevel::GetTextureDir();
	fsLocator file_loc;
	if (muiFileDialogUtils::GetOpenFileName(filter, initial_dir, file_loc))
	{
		fname = itStringUtil::GetStdString(file_loc.GetLastName());
		return true;
	}
	return false;
}

System::Void MaterialDialog::buttonNew_Click(System::Object *  sender, System::EventArgs *  e)
{
	std::string fname;
	if (DlgGetTextureFilename(fname))
	{
		m_Data.AddSimpleTextureLayer(fname);
		SetupTextureList(m_Data);
		mtrOperations::ChangeMaterialData(m_Data);

		// select the new texture
		this->listTextures->SelectedIndex = this->listTextures->get_Items()->Count - 1;
		launch_texture_edit();
	}
}

System::Void MaterialDialog::buttonEdit_Click(System::Object *  sender, System::EventArgs *  e)
{
	launch_texture_edit();
}

System::Void MaterialDialog::buttonRemove_Click(System::Object *  sender, System::EventArgs *  e)
{
	int sel_item = this->listTextures->SelectedIndex;
	if (sel_item >= 0)
	{
		m_Data.RemoveTextureLayer(sel_item);
		SetupTextureList(m_Data);
		mtrOperations::ChangeMaterialData(m_Data);
	}
}

System::Void MaterialDialog::buttonMoveUp_Click(System::Object *  sender, System::EventArgs *  e)
{
	int sel_item = this->listTextures->SelectedIndex;
	if (sel_item > 0)// has to be second pos or lower
	{
		mtrTextureLayer layer = m_Data.GetTextureLayer(sel_item);
		m_Data.RemoveTextureLayer(sel_item);
		m_Data.InsertTextureLayer(layer, sel_item-1);
	
		SetupTextureList(m_Data);
		mtrOperations::ChangeMaterialData(m_Data);

		this->listTextures->SelectedIndex = sel_item-1;
	}
}

System::Void MaterialDialog::buttonMoveDown_Click(System::Object *  sender, System::EventArgs *  e)
{
	int sel_item = this->listTextures->SelectedIndex;
	if ((sel_item >= 0) && (sel_item < m_Data.GetNumTextureLayers() - 1))
	{
		mtrTextureLayer layer = m_Data.GetTextureLayer(sel_item);
		m_Data.RemoveTextureLayer(sel_item);
		m_Data.InsertTextureLayer(layer, sel_item+1);
	
		SetupTextureList(m_Data);
		mtrOperations::ChangeMaterialData(m_Data);

		this->listTextures->SelectedIndex = sel_item+1;
	}	
}

void MaterialDialog::launch_texture_edit()
{
	int sel_item = this->listTextures->SelectedIndex;
	if (sel_item >= 0)
	{
		TextureDialog* dialog = new TextureDialog(m_Data.GetTextureLayer(sel_item));
		if (dialog->ShowDialog() == DialogResult::OK)
		{
			SetupTextureList(m_Data);	
			mtrOperations::ChangeMaterialData(m_Data);

			this->listTextures->SelectedIndex = sel_item;
		}	
	}
}

