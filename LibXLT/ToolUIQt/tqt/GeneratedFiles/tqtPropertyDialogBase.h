/********************************************************************************
** Form generated from reading UI file 'tqtPropertyDialogBase.ui'
**
** Created: Fri Oct 22 15:06:09 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef TQTPROPERTYDIALOGBASE_H
#define TQTPROPERTYDIALOGBASE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_tqtPropertyDialogBase
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *m_StaticText_Message;
    QFrame *m_panel_Properties;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *tqtPropertyDialogBase)
    {
        if (tqtPropertyDialogBase->objectName().isEmpty())
            tqtPropertyDialogBase->setObjectName(QString::fromUtf8("tqtPropertyDialogBase"));
        tqtPropertyDialogBase->resize(514, 406);
        verticalLayout = new QVBoxLayout(tqtPropertyDialogBase);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        m_StaticText_Message = new QLabel(tqtPropertyDialogBase);
        m_StaticText_Message->setObjectName(QString::fromUtf8("m_StaticText_Message"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(m_StaticText_Message->sizePolicy().hasHeightForWidth());
        m_StaticText_Message->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(m_StaticText_Message);

        m_panel_Properties = new QFrame(tqtPropertyDialogBase);
        m_panel_Properties->setObjectName(QString::fromUtf8("m_panel_Properties"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(m_panel_Properties->sizePolicy().hasHeightForWidth());
        m_panel_Properties->setSizePolicy(sizePolicy1);
        m_panel_Properties->setFrameShape(QFrame::StyledPanel);
        m_panel_Properties->setFrameShadow(QFrame::Raised);

        verticalLayout->addWidget(m_panel_Properties);

        buttonBox = new QDialogButtonBox(tqtPropertyDialogBase);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(tqtPropertyDialogBase);

        QMetaObject::connectSlotsByName(tqtPropertyDialogBase);
    } // setupUi

    void retranslateUi(QDialog *tqtPropertyDialogBase)
    {
        tqtPropertyDialogBase->setWindowTitle(QString());
        m_StaticText_Message->setText(QApplication::translate("tqtPropertyDialogBase", "TextLabel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class tqtPropertyDialogBase: public Ui_tqtPropertyDialogBase {};
} // namespace Ui

QT_END_NAMESPACE

#endif // TQTPROPERTYDIALOGBASE_H
