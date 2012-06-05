/****************************************************************************\
**	TestMainWindow.cpp
**
**	Tests custom controls for Qt
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "TestMainWindow.hpp"

#include "ControlsTestDialogUtil.hpp"

#include "Core/it/itString.hpp"
#include "Core/prty/prtyPropertyUIInfoContainer.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
//#include "Core/prty/prtyBoolean.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "ToolUIQt/pqt/pqtFormControlBuilder.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"

#include <QtGui/QtGui>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TestMainWindow::TestMainWindow()
{
	QWidget* centralwidget = new QWidget(this);
	centralwidget->setObjectName(QString::fromUtf8("MainWindow"));
	setCentralWidget(centralwidget);

	createActions();
	createMenus();
	createToolBars();
	createStatusBar();

	readSettings();

//	connect(textEdit->document(), SIGNAL(contentsChanged()),
//			this, SLOT(documentWasModified()));

	setCurrentFile("");
	setUnifiedTitleAndToolBarOnMac(true);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestMainWindow::closeEvent(QCloseEvent *event)
{
	if (maybeSave())
	{
		writeSettings();
		event->accept();
	} else {
		event->ignore();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestMainWindow::newFile()
{
	if (maybeSave()) 
	{
//		textEdit->clear();
		setCurrentFile("");
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestMainWindow::open()
{
	if (maybeSave()) 
	{
		QString fileName = QFileDialog::getOpenFileName(this);
		if (!fileName.isEmpty())
			loadFile(fileName);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool TestMainWindow::save()
{
	if (curFile.isEmpty()) 
	{
		return saveAs();
	} else {
		return saveFile(curFile);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool TestMainWindow::saveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this);
	if (fileName.isEmpty())
		return false;

	return saveFile(fileName);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestMainWindow::about()
{
	QMessageBox::about(this, tr("About Application"),
			tr("The <b>Application</b> example demonstrates how to "
			   "write modern GUI applications using Qt, with a menu bar, "
			   "toolbars, and a status bar."));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestMainWindow::documentWasModified()
{
//	setWindowModified(textEdit->document()->isModified());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestMainWindow::PopUpMessageBox()
{
	int result = guiMessageBox::Show( itString(L"This is a message box"), itString(L"Important MessageBox"), guiMessageBox::e_OKOnly );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestMainWindow::CreateControls()
{
	ControlsTestDialogUtil::Show();
/*
	prtyPropertyUIInfoContainer properties;

	//
	prtyBoolean cb_one;
	prtyBoolean cb_two;
	prtyCheckBoxUIInfo cbinfo_one(&cb_one);
	prtyCheckBoxUIInfo cbinfo_two(&cb_two);

	//	add to the property list
	properties.Add(&cbinfo_one);
	properties.Add(&cbinfo_two);

	//	build the form
	pqtFormControlBuilder::BuildForm( this, std::string("Properties"), properties );
*/
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestMainWindow::createActions()
{
	newAct = new QAction(QIcon("./images/new.png"), tr("&New"), this);
	newAct->setShortcuts(QKeySequence::New);
	newAct->setStatusTip(tr("Create a new file"));
	connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

	openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("Open an existing file"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
	saveAct->setShortcuts(QKeySequence::Save);
	saveAct->setStatusTip(tr("Save the document to disk"));
	connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

	saveAsAct = new QAction(tr("Save &As..."), this);
	saveAsAct->setShortcuts(QKeySequence::SaveAs);
	saveAsAct->setStatusTip(tr("Save the document under a new name"));
	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcuts(QKeySequence::Quit);
	exitAct->setStatusTip(tr("Exit the application"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	messageBoxAct = new QAction(tr("Message Box"), this);
	//messageBoxAct->setShortcuts(QKeySequence::Quit);
	messageBoxAct->setStatusTip(tr("Show a Message Box"));
	connect(messageBoxAct, SIGNAL(triggered()), this, SLOT(PopUpMessageBox()));

	CreateControlsAct = new QAction(tr("Create Controls"), this);
	//CreateControlsAct->setShortcuts(QKeySequence::Quit);
	CreateControlsAct->setStatusTip(tr("Create some controls"));
	connect(CreateControlsAct, SIGNAL(triggered()), this, SLOT(CreateControls()));

	//cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
	//cutAct->setShortcuts(QKeySequence::Cut);
	//cutAct->setStatusTip(tr("Cut the current selection's contents to the "
	//						"clipboard"));
	//connect(cutAct, SIGNAL(triggered()), textEdit, SLOT(cut()));

	//copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
	//copyAct->setShortcuts(QKeySequence::Copy);
	//copyAct->setStatusTip(tr("Copy the current selection's contents to the "
	//						 "clipboard"));
	//connect(copyAct, SIGNAL(triggered()), textEdit, SLOT(copy()));

	//pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
	//pasteAct->setShortcuts(QKeySequence::Paste);
	//pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
	//						  "selection"));
	//connect(pasteAct, SIGNAL(triggered()), textEdit, SLOT(paste()));

	aboutAct = new QAction(tr("&About"), this);
	aboutAct->setStatusTip(tr("Show the application's About box"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutQtAct = new QAction(tr("About &Qt"), this);
	aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

//	cutAct->setEnabled(false);
//	copyAct->setEnabled(false);
//	connect(textEdit, SIGNAL(copyAvailable(bool)),
//			cutAct, SLOT(setEnabled(bool)));
//	connect(textEdit, SIGNAL(copyAvailable(bool)),
//			copyAct, SLOT(setEnabled(bool)));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestMainWindow::createMenus()
{
	tqtSystem::g_pMainMenu = menuBar();

	fileMenu = menuBar()->addMenu(tr("&File"));

	fileMenu->addAction(newAct);
	fileMenu->addAction(openAct);
	fileMenu->addAction(saveAct);
	fileMenu->addAction(saveAsAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	editMenu = menuBar()->addMenu(tr("&Edit"));
//	editMenu->addAction(cutAct);
//	editMenu->addAction(copyAct);
//	editMenu->addAction(pasteAct);

	testMenu = menuBar()->addMenu(tr("&Test"));
	testMenu->addAction(messageBoxAct);
	testMenu->addAction(CreateControlsAct);

	menuBar()->addSeparator();

	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestMainWindow::createToolBars()
{
	fileToolBar = addToolBar(tr("File"));
	fileToolBar->addAction(newAct);
	fileToolBar->addAction(openAct);
	fileToolBar->addAction(saveAct);

	editToolBar = addToolBar(tr("Edit"));
//	editToolBar->addAction(cutAct);
//	editToolBar->addAction(copyAct);
//	editToolBar->addAction(pasteAct);

	testToolBar = addToolBar(tr("Test"));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestMainWindow::createStatusBar()
{
	tqtSystem::g_pStatusBar = statusBar();

	statusBar()->showMessage(tr("Ready"));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestMainWindow::readSettings()
{
	QSettings settings("Trolltech", "Application Example");
	QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
	QSize size = settings.value("size", QSize(400, 400)).toSize();
	resize(size);
	move(pos);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestMainWindow::writeSettings()
{
	QSettings settings("Trolltech", "Application Example");
	settings.setValue("pos", pos());
	settings.setValue("size", size());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool TestMainWindow::maybeSave()
{
	//if (textEdit->document()->isModified()) 
	//{
	//	QMessageBox::StandardButton ret;
	//	ret = QMessageBox::warning(this, tr("Application"),
	//				 tr("The document has been modified.\n"
	//					"Do you want to save your changes?"),
	//				 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	//	if (ret == QMessageBox::Save)
	//		return save();
	//	else if (ret == QMessageBox::Cancel)
	//		return false;
	//}
	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestMainWindow::loadFile(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("Application"),
							 tr("Cannot read file %1:\n%2.")
							 .arg(fileName)
							 .arg(file.errorString()));
		return;
	}

	QTextStream in(&file);
#ifndef QT_NO_CURSOR
	QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
//	textEdit->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
	QApplication::restoreOverrideCursor();
#endif

	setCurrentFile(fileName);
	statusBar()->showMessage(tr("File loaded"), 2000);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool TestMainWindow::saveFile(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("Application"),
							 tr("Cannot write file %1:\n%2.")
							 .arg(fileName)
							 .arg(file.errorString()));
		return false;
	}

	QTextStream out(&file);
#ifndef QT_NO_CURSOR
	QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
//	out << textEdit->toPlainText();
#ifndef QT_NO_CURSOR
	QApplication::restoreOverrideCursor();
#endif

	setCurrentFile(fileName);
	statusBar()->showMessage(tr("File saved"), 2000);
	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestMainWindow::setCurrentFile(const QString &fileName)
{
	curFile = fileName;
//	textEdit->document()->setModified(false);
	setWindowModified(false);

	QString shownName = curFile;
	if (curFile.isEmpty())
		shownName = "untitled.txt";
	setWindowFilePath(shownName);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
QString TestMainWindow::strippedName(const QString &fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}


