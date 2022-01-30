#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QAnime3.h"
#include "SeriesSelection.h"

class QAnime3 : public QMainWindow
{
    Q_OBJECT

public:
    QAnime3(QWidget *parent = Q_NULLPTR);

private slots:
    void scanForRenames();
    void openSeriesSelection();
    void openPatternSelector();
    void rename();

private:
    Ui::QAnime3Class ui;
};
