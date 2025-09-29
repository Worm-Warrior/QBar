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
    void on_chooseRoot_clicked();

private:
    Ui::FileBrowserWidget *ui;
    QFileSystemModel *model = nullptr;
};

#endif // FILEBROWSERWIDGET_H
