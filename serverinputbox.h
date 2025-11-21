#ifndef SERVERINPUTBOX_H
#define SERVERINPUTBOX_H

#include <QDialog>
#include <qlabel.h>

class QLineEdit;

class serverinputbox : public QDialog
{
    Q_OBJECT
public:
    explicit serverinputbox(QWidget *parent = nullptr);

    QString url() const;
    QString username() const;
    QString password() const;

private:
    QLineEdit *url_edit;
    QLineEdit *username_edit;
    QLineEdit *password_edit;
    QLabel *warning;

signals:
};

#endif // SERVERINPUTBOX_H
