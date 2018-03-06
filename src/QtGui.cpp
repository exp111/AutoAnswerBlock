#include "QtGui.h"

#include "config.h"

QtGui::QtGui(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.delaySpinBox->setValue(config->delay);
	ui.delaySlider->setValue(config->delay);
}

QtGui::~QtGui()
{
}

void QtGui::set(int value)
{
	config->delay = value;
	ui.delaySpinBox->setValue(value);
	ui.delaySlider->setValue(value);
}