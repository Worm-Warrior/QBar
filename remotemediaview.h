#ifndef REMOTEMEDIAVIEW_H
#define REMOTEMEDIAVIEW_H

#include <QWidget>

namespace Ui {
class RemoteMediaView;
}

class RemoteMediaView : public QWidget
{
    Q_OBJECT

public:
    explicit RemoteMediaView(QWidget *parent = nullptr);
    ~RemoteMediaView();

private:
    Ui::RemoteMediaView *ui;
    void setupHeaderCols();

    enum ColIndex {
        COL_NAME = 0,
        COL_COUNT,
    };
};

#endif // REMOTEMEDIAVIEW_H
