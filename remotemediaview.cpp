#include "remotemediaview.h"
#include "ui_remotemediaview.h"

RemoteMediaView::RemoteMediaView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RemoteMediaView)
{
    ui->setupUi(this);

    setupHeaderCols();
}

RemoteMediaView::~RemoteMediaView()
{
    delete ui;
}

void RemoteMediaView::setupHeaderCols() {
    ui->mediaView->setColumnCount(COL_COUNT);
}
