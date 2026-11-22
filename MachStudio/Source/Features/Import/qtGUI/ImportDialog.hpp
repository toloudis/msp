/*****************************************************************************
**	ImportDialog.hpp
**
**	Dialog for importing parts of another scene, written in Qt: a .mab's
**	chunks and items, or the object tree of a file another format's
**	importer reads (iscnSceneImport)
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef IMPORTDIALOG_HPP
#error ImportDialog.hpp multiply included
#endif
#define IMPORTDIALOG_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif
#ifndef IMPORT_DATA_HPP
#include "Features/Import/ImportData.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif
#ifndef ISCN_SCENEDESC_HPP
#include "ImportExport/iscn/iscnSceneDesc.hpp"
#endif

#include <map>
#include <vector>

//============================================================================
//============================================================================
class docDocument;
class QCheckBox;
class QTreeWidget;
class QTreeWidgetItem;
class tqcFilePicker;

#ifdef USE_QT

//----------------------------------------------------------------------------
// Class ImportDialog
//----------------------------------------------------------------------------
class ImportDialog : public QDialog
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		ImportDialog( QWidget* parent, ImportData& i_Data);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~ImportDialog();

	private:
		//--------------------------------------------------------------------
		// private functions
		//--------------------------------------------------------------------
		void load_document(const fsLocator& i_ImportLoc);
		void fillintree();
		void update_ImportDataList();
		void clear_file();
		void load_scene(const fsLocator& i_ImportLoc);
		void fillintree_scene();
		void add_scene_items(QTreeWidgetItem* i_pParentItem, int i_Parent);
		std::vector<bool> chosen_scene_objects() const;

		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		void filePicker_FileChanged();
		void buttonImport_Click();

		tqcFilePicker* m_filePicker1;
		QCheckBox* m_checkBox_AllowDupes;
		QTreeWidget* m_treeCtrl_Import;

		ImportData& m_Data;
		shared_ptr<docDocument> m_pDoc;	// a .mab being imported
		iscnSceneDesc m_Scene;			// or the objects of a file iscnSceneImport reads
};

#endif // USE_QT
