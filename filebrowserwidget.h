#ifndef FILEBROWSERWIDGET_H
#define FILEBROWSERWIDGET_H

#include <QWidget>
#include <qfilesystemmodel.h>

namespace Ui {
class FileBrowserWidget;
}

class FileBrowserWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileBrowserWidget(QWidget *parent = nullptr);
    ~FileBrowserWidget();
    void changeRoot();
private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_treeView_expanded(const QModelIndex &index);
    void on_treeView_clicked(const QModelIndex &index);
    void handleChangeSelect(const QModelIndex &index);
signals:
    void folderSelected(const QString &folderPath);
private:
    Ui::FileBrowserWidget *ui;
    QFileSystemModel *model = nullptr;
};

#endif // FILEBROWSERWIDGET_H
