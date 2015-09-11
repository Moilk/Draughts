#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include<QDialog>
#include "ui_optionsdialog.h"

class OptionsDialog:public QDialog,public Ui::OptionsDialog
{
    Q_OBJECT
public:
    OptionsDialog(QWidget *parent=0);
};

#endif // OPTIONSDIALOG_H
