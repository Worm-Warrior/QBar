#include "serverinputbox.h"
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>

serverinputbox::serverinputbox(QWidget *parent)
    : QDialog{parent}
{
    url_edit = new QLineEdit(this);
    username_edit = new QLineEdit(this);
    password_edit = new QLineEdit(this);
    warning = new QLabel(this);

    warning->setText("WARNING: This information is saved in a plain text file");

    auto* layout = new QFormLayout(this);
    layout->addRow("Server URL:", url_edit);
    layout->addRow("Username:", username_edit);
    layout->addRow("Password (PLAIN TEXT):", password_edit);
    layout->addRow(warning);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel,
        this);

    connect(buttons, &QDialogButtonBox::accepted,
            this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected,
            this, &QDialog::reject);

    layout->addWidget(buttons);
}

QString serverinputbox::url() const {return url_edit->text();}
QString serverinputbox::username() const {return username_edit->text();}
QString serverinputbox::password() const {return password_edit->text();}
