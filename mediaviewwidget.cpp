#include "mediaviewwidget.h"
#include "ui_mediaviewwidget.h"

MediaViewWidget::MediaViewWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MediaViewWidget)
{
    ui->setupUi(this);
}

MediaViewWidget::~MediaViewWidget()
{
    delete ui;
}
