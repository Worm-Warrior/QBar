#include "remotemediaview.h"
#include "ui_remotemediaview.h"

RemoteMediaView::RemoteMediaView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RemoteMediaView)
{
    ui->setupUi(this);
}

RemoteMediaView::~RemoteMediaView()
{
    delete ui;
}
