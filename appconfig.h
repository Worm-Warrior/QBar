#ifndef APPCONFIG_H
#define APPCONFIG_H
#include <QSettings>
#include <QString>
#include <QDebug>

class AppConfig
{
public:
    AppConfig();

    static QString serverURL()
    {
        QSettings settings("Voss Software", "QBar");
        settings.beginGroup("Server");
        QString url = settings.value("server_url").toString();
        settings.endGroup();
        return url;
    }

    static QString username()
    {
        QSettings settings("Voss Software", "QBar");
        settings.beginGroup("Server");
        QString username = settings.value("username").toString();
        settings.endGroup();
        return username;
    }

    static QString password()
    {
        QSettings settings("Voss Software", "QBar");
        settings.beginGroup("Server");
        QString password = settings.value("password").toString();
        settings.endGroup();
        return password;
    }


};

#endif // APPCONFIG_H
