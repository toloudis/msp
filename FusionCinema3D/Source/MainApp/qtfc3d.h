#ifndef QTFC3D_H
#define QTFC3D_H

#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QMessageBox>
#include <QCloseEvent>
#include "GeneratedFiles/ui_qtfc3d.h"

class QtFC3D : public QMainWindow
{
	Q_OBJECT

public:
	QtFC3D(QWidget *parent = 0);
	~QtFC3D();

	HWND GetWinId();
	void closeEvent(QCloseEvent*);
	void SetWinId(HWND i_winId);

	HWND winid;
private:
	Ui::QtFC3DClass ui;

	public slots:
				void setIndex1();
				void setIndex2();
				void setIndex3();
	
};

#endif // QTFC3D_H
