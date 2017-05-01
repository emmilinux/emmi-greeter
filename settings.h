#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>




class Cache : public QSettings
{
public:
    static const QString GREETER_DATA_DIR_PATH;
    static void prepare();

    Cache() : QSettings(GREETER_DATA_DIR_PATH + "/state", QSettings::NativeFormat) {}
    QString getLastUser() { return value("last-user").toString(); }
    void setLastUser(QString userId) { setValue("last-user", userId); }
    QString getLastSession(QString userId) { return value(userId + "/last-session").toString(); }
    void setLastSession(QString userId, QString session) { setValue(userId + "/last-session", session); }
};

#define CONFIG_FILE "/etc/lightdm/emmi-greeter.conf"
#define BACKGROUND_IMAGE_KEY "greeter-background-image"
#define USER_IMAGE_KEY "greeter-user-image"
#define LOGINFORM_OFFSETX_KEY "loginform-offset-x"
#define LOGINFORM_OFFSETY_KEY "loginform-offset-y"


class Settings : public QSettings
{
public:
    Settings() : QSettings(QString("/etc/lightdm/emmi-greeter.conf"), QSettings::NativeFormat) {}
    QString iconThemeName() { return value("greeter-icon-theme").toString(); }
    QString backgrundImagePath() { return value("greeter-background-image").toString(); }
    QString userImagePath() { return value("greeter-user-image").toString(); }
    QString offsetX() { return value("loginform-offset-x").toString(); }
    QString offsetY() { return value("loginform-offset-y").toString(); }
};




#endif // SETTINGS_H
