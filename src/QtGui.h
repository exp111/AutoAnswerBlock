#pragma once

#include <QWidget>
#include <qmainwindow.h>
#include "ui_QtGui.h"

class QtGui : public QMainWindow
{
	Q_OBJECT

public:
	QtGui(QWidget *parent = Q_NULLPTR);
	~QtGui();
public slots:
	void set(int value);
private:
	Ui::QtGui ui;
};
