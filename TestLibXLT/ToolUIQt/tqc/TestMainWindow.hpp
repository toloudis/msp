/****************************************************************************\
**	TestMainWindow.hpp
**
**	Main test app window
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef TESTMAINWINDOW_HPP
#error TestMainWindow.hpp multiply included
#endif
#define TESTMAINWINDOW_HPP

#include <QtGui/QMainWindow>


//============================================================================
//	Forward References
//============================================================================
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPlainTextEdit;
QT_END_NAMESPACE


//============================================================================
//============================================================================
class TestMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	TestMainWindow();

protected:
	void closeEvent(QCloseEvent *event);

private slots:
	void newFile();
	void open();
	bool save();
	bool saveAs();
	void about();
	void documentWasModified();
	void PopUpMessageBox();
	void CreateControls();

private:
	void createActions();
	void createMenus();
	void createToolBars();
	void createStatusBar();
	void readSettings();
	void writeSettings();
	bool maybeSave();
	void loadFile(const QString &fileName);
	bool saveFile(const QString &fileName);
	void setCurrentFile(const QString &fileName);
	QString strippedName(const QString &fullFileName);

//	QPlainTextEdit *textEdit;
	QString curFile;

	QMenu *fileMenu;
	QMenu *editMenu;
	QMenu *testMenu;
	QMenu *helpMenu;
	QToolBar *fileToolBar;
	QToolBar *editToolBar;
	QToolBar *testToolBar;
	QAction *newAct;
	QAction *openAct;
	QAction *saveAct;
	QAction *saveAsAct;
	QAction *exitAct;
	//QAction *cutAct;
	//QAction *copyAct;
	//QAction *pasteAct;
	QAction *aboutAct;
	QAction *aboutQtAct;
	QAction *messageBoxAct;
	QAction *CreateControlsAct;
};

