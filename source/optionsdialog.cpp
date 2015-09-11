#include <QtWidgets>
#include "optionsdialog.h"

OptionsDialog::OptionsDialog(QWidget *parent):QDialog(parent)
{
    setupUi(this);
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    colorcomboBox->setMinimumSize(levelcomboBox->sizeHint());
}
