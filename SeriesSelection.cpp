#include "SeriesSelection.h"

SeriesSelection::SeriesSelection(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);


    ui.tableResults->setEnabled(false);
    ui.buttonConfirm->setEnabled(false);
    ui.tableResults->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui.tableResults->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui.tableResults->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui.tableResults->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

    connect(ui.inputName, &QLineEdit::returnPressed, this, &SeriesSelection::searchSeries);
    connect(ui.tableResults, &QTableWidget::itemSelectionChanged, this, &SeriesSelection::confirmButtonToggle);
    connect(ui.radioAbsolute, &QRadioButton::toggled, this, &SeriesSelection::inputSeasonSync);
    connect(ui.radioAbsolute, &QRadioButton::toggled, this, &SeriesSelection::confirmButtonToggle);
    connect(ui.inputSeason, &QLineEdit::textChanged, this, &SeriesSelection::confirmButtonToggle);
    connect(ui.buttonConfirm, &QPushButton::clicked, this, &SeriesSelection::confirm);
}

void SeriesSelection::searchSeries() {
    return;
}
void SeriesSelection::listSearchResults() {
    return;
}
void SeriesSelection::confirmButtonToggle() {
    return;
}
void SeriesSelection::inputSeasonSync() {
    return;
}
void SeriesSelection::confirm() {
    return;
}