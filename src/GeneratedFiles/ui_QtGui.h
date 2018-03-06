/********************************************************************************
** Form generated from reading UI file 'QtGui.ui'
**
** Created by: Qt User Interface Compiler version 5.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTGUI_H
#define UI_QTGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtGui
{
public:
    QSpinBox *delaySpinBox;
    QSlider *delaySlider;
    QLabel *infoLabel;

    void setupUi(QWidget *QtGui)
    {
        if (QtGui->objectName().isEmpty())
            QtGui->setObjectName(QStringLiteral("QtGui"));
        QtGui->resize(339, 72);
        delaySpinBox = new QSpinBox(QtGui);
        delaySpinBox->setObjectName(QStringLiteral("delaySpinBox"));
        delaySpinBox->setGeometry(QRect(200, 10, 131, 22));
        delaySpinBox->setMaximum(5000);
        delaySpinBox->setSingleStep(100);
        delaySlider = new QSlider(QtGui);
        delaySlider->setObjectName(QStringLiteral("delaySlider"));
        delaySlider->setGeometry(QRect(10, 40, 321, 22));
        delaySlider->setMaximum(5000);
        delaySlider->setSingleStep(100);
        delaySlider->setPageStep(1000);
        delaySlider->setOrientation(Qt::Horizontal);
        infoLabel = new QLabel(QtGui);
        infoLabel->setObjectName(QStringLiteral("infoLabel"));
        infoLabel->setGeometry(QRect(10, 10, 171, 21));

        retranslateUi(QtGui);
        QObject::connect(delaySlider, SIGNAL(sliderMoved(int)), QtGui, SLOT(set(int)));
        QObject::connect(delaySpinBox, SIGNAL(valueChanged(int)), QtGui, SLOT(set(int)));

        QMetaObject::connectSlotsByName(QtGui);
    } // setupUi

    void retranslateUi(QWidget *QtGui)
    {
        QtGui->setWindowTitle(QApplication::translate("QtGui", "Auto Answer Blocker", Q_NULLPTR));
        infoLabel->setText(QApplication::translate("QtGui", "Detection Time in milliseconds", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class QtGui: public Ui_QtGui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTGUI_H
