#include "filebrowserwidget.h"
#include "ui_filebrowserwidget.h"
#include <QFileDialog>

FileBrowserWidget::FileBrowserWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FileBrowserWidget), model(new QFileSystemModel(this))
{
    ui->setupUi(this);
    QString startDir = QString::fromUtf8("/home/harry/Music");
    QModelIndex rootIndex = model->setRootPath(startDir);
    ui->treeView->setModel(model);
    ui->treeView->setRootIndex(rootIndex);
}

FileBrowserWidget::~FileBrowserWidget()
{
    delete ui;
}

void FileBrowserWidget::on_chooseRoot_clicked()
{
    if (!model) {return;}

    QString dir = QFileDialog::getExistingDirectory(this, "Select Root Directory");
    if (!dir.isEmpty()) {
        // setRootPath() both configures the model AND returns a valid QModelIndex
        QModelIndex rootIndex = model->setRootPath(dir);

        // rootIndex is guaranteed to belong to 'model'
        ui->treeView->setRootIndex(rootIndex);
    }
}

void FileBrowserWidget::changeRoot()
{
    if (!model) {return;}

    QString dir = QFileDialog::getExistingDirectory(this, "Select Root Directory");
    if (!dir.isEmpty()) {
        // setRootPath() both configures the model AND returns a valid QModelIndex
        QModelIndex rootIndex = model->setRootPath(dir);

        // rootIndex is guaranteed to belong to 'model'
        ui->treeView->setRootIndex(rootIndex);
    }
}


