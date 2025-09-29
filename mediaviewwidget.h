#ifndef MEDIAVIEWWIDGET_H
#define MEDIAVIEWWIDGET_H

#include <QWidget>

namespace Ui {
class MediaViewWidget;
}

class MediaViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MediaViewWidget(QWidget *parent = nullptr);
    ~MediaViewWidget();

private:
    Ui::MediaViewWidget *ui;
};

#endif // MEDIAVIEWWIDGET_H
