#include "SeriesSelection.h"

SeriesSelection::SeriesSelection(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);


    ui.tableResults->setEnabled(false);
    ui.buttonSelect->setEnabled(false);
    ui.tableResults->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui.tableResults->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui.tableResults->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui.tableResults->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

    connect(ui.tableResults, &QTableWidget::itemSelectionChanged, this, &SeriesSelection::buttonSelectToggle);
    connect(ui.buttonSelect, &QPushButton::clicked, this, &SeriesSelection::select);
}

void SeriesSelection::listSearchResults() {
    return;
}
void SeriesSelection::buttonSelectToggle() {
    return;
}
void SeriesSelection::select() {
    return;
}