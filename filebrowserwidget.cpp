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
    ui->treeView->setSortingEnabled(true);
    ui->treeView->sortByColumn(0, Qt::SortOrder::AscendingOrder);
}

FileBrowserWidget::~FileBrowserWidget()
{
    delete ui;
}

//TODO: It does not show any of the media files when you are in the root that only has playable files.
//		i.e: In the album folder without any sub-folders
void FileBrowserWidget::changeRoot()
{
    if (!model) {return;}

    QString dir = QFileDialog::getExistingDirectory(this, "Select Root Directory");
    if (!dir.isEmpty()) {
        // setRootPath() both configures the model AND returns a valid QModelIndex
        QModelIndex rootIndex = model->setRootPath(dir);

        // rootIndex is guaranteed to belong to 'model'
        ui->treeView->setRootIndex(rootIndex);

        emit folderSelected(dir);
    }
}

void FileBrowserWidget::on_treeView_doubleClicked(const QModelIndex &index)
{
    handleChangeSelect(index);
}

void FileBrowserWidget::on_treeView_expanded(const QModelIndex &index)
{
    handleChangeSelect(index);
}


void FileBrowserWidget::on_treeView_clicked(const QModelIndex &index)
{
    handleChangeSelect(index);
}


void FileBrowserWidget::handleChangeSelect(const QModelIndex &index)
{
    QFileSystemModel *model = qobject_cast<QFileSystemModel*>(ui->treeView->model());
    if (model)
    {
        QString folderPath = model->filePath(index);

        if (QFileInfo(folderPath).isDir())
        {
            emit folderSelected(folderPath);
        }
    }
}
