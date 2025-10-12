#ifndef REMOTEFILEBROWSER_H
#define REMOTEFILEBROWSER_H

#include <QWidget>

namespace Ui {
class RemoteFileBrowser;
}

class RemoteFileBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit RemoteFileBrowser(QWidget *parent = nullptr);
    ~RemoteFileBrowser();

private:
    Ui::RemoteFileBrowser *ui;
};

#endif // REMOTEFILEBROWSER_H
