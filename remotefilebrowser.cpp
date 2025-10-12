#include "remotefilebrowser.h"
#include "ui_remotefilebrowser.h"

RemoteFileBrowser::RemoteFileBrowser(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RemoteFileBrowser)
{
    ui->setupUi(this);
}

RemoteFileBrowser::~RemoteFileBrowser()
{
    delete ui;
}
