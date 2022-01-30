#include "QAnime3.h"

QAnime3::QAnime3(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    // setup default state of UI elements
    ui.buttonConfirmRename->setEnabled(false);

    ui.treeTorrents->headerItem()->setText(0, "Old Name");
    ui.treeTorrents->headerItem()->setText(1, "New Name");
    ui.treeTorrents->headerItem()->setText(2, ""); // file path
    ui.treeTorrents->header()->setStretchLastSection(false);
    ui.treeTorrents->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui.treeTorrents->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui.treeTorrents->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui.treeTorrents->header()->setDefaultAlignment(Qt::AlignCenter);

    connect(ui.buttonScan, &QPushButton::released, this, &QAnime3::scanForRenames);
    connect(ui.buttonAdd, &QPushButton::released, this, &QAnime3::openSeriesSelection);
    connect(ui.buttonEdit, &QPushButton::released, this, &QAnime3::openPatternSelector);
    connect(ui.buttonConfirmRename, &QPushButton::released, this, &QAnime3::rename);
}

void QAnime3::scanForRenames() {
    return;
}

void QAnime3::openSeriesSelection() {
    SeriesSelection* dialog = new SeriesSelection();
    dialog->show();
}

void QAnime3::openPatternSelector() {
    return;
}

void QAnime3::rename() {
    return;
}