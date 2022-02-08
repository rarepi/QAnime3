#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SeriesSelection.h"

class SeriesSelection : public QDialog
{
    Q_OBJECT

public:
    SeriesSelection(QWidget *parent = Q_NULLPTR);

private slots:
    void listSearchResults();
    void buttonSelectToggle();
    void select();

private:
    Ui::SeriesSelection ui;
};
