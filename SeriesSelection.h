#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SeriesSelection.h"

class SeriesSelection : public QDialog
{
    Q_OBJECT

public:
    SeriesSelection(QWidget *parent = Q_NULLPTR);

private slots:
    void searchSeries();
    void listSearchResults();
    void confirmButtonToggle();
    void inputSeasonSync();
    void confirm();

private:
    Ui::SeriesSelection ui;
};
