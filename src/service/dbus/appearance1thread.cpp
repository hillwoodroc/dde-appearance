// SPDX-FileCopyrightText: 2021 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "appearance1thread.h"
#include "appearanceproperty.h"
#include "impl/appearancemanager.h"
#include "modules/common/commondefine.h"
#include "modules/dconfig/dconfigsettings.h"

#include <QMutexLocker>

#include <DConfig>
#include <QThread>

// AppearanceManager是实际功能实现
// Appearance1Thread在线程中处理，并处理DBus返回及属性设置
// 属性获取不能在线程中，需先初始化
Appearance1Thread::Appearance1Thread()
    : QObject(nullptr)
    , property(new AppearanceProperty)
    , thread(new QThread(this))
{
    // 属性初始化
    DTK_CORE_NAMESPACE::DConfig::setAppId(APPEARANCEAPPID);
    DTK_CORE_NAMESPACE::DConfig settingDconfig(APPEARANCESCHEMA);
    property->globalTheme.init(settingDconfig.value(GSKEYGLOBALTHEME).toString());
    property->gtkTheme.init(settingDconfig.value(GSKEYGTKTHEME).toString());
    property->iconTheme.init(settingDconfig.value(GSKEYICONTHEM).toString());
    property->cursorTheme.init(settingDconfig.value(GSKEYCURSORTHEME).toString());
    property->standardFont.init(settingDconfig.value(GSKEYFONTSTANDARD).toString());
    property->monospaceFont.init(settingDconfig.value(GSKEYFONTMONOSPACE).toString());
    property->dtkSizeMode.init(settingDconfig.value(DDTKSIZEMODE).toInt());
    property->fontSize.init(settingDconfig.value(GSKEYFONTSIZE).toDouble());
    property->opacity.init(settingDconfig.value(GSKEYOPACITY).toDouble());
    property->wallpaperSlideShow.init(settingDconfig.value(GSKEYWALLPAPERSLIDESHOW).toString());
    property->wallpaperURls.init(settingDconfig.value(GSKEYWALLPAPERURIS).toString());
    QString qtActivecolor =  DconfigSettings::ConfigValue(DDEDAEMONAPPID,XSETTINGSNAME,GSKEYQTACTIVECOLOR,"").toString();
    QString qtDarkActivecolor =  DconfigSettings::ConfigValue(DDEDAEMONAPPID,XSETTINGSNAME,GSKEYQTACTIVECOLOR_DARK,"").toString();
    int radius = DconfigSettings::ConfigValue(DDEDAEMONAPPID,XSETTINGSNAME,DCKEYDTKWINDOWRADIUS,"").toInt();
    property->windowRadius.init(radius);
    QString activeColor = settingDconfig.value(GSKEYGLOBALTHEME).toString().endsWith("dark") ? qtDarkActivecolor : qtActivecolor;
    property->qtActiveColor.init(AppearanceManager::qtActiveColorToHexColor(activeColor));
    property->background.init("file:////usr/share/backgrounds/default_background.jpg");
    init();
    // 线程
    moveToThread(thread);
    thread->start();
}

Appearance1Thread::~Appearance1Thread()
{
    thread->quit();
    thread->wait();
    appearanceManager.reset();
    delete property;
}

QString Appearance1Thread::background() const
{
    return property->background;
}

QString Appearance1Thread::cursorTheme() const
{
    return property->cursorTheme;
}

double Appearance1Thread::fontSize() const
{
    return property->fontSize;
}

void Appearance1Thread::setFontSize(double value)
{
    QMutexLocker locker(&mutex);
    appearanceManager->setFontSize(value);
}

QString Appearance1Thread::globalTheme() const
{
    return property->globalTheme;
}

QString Appearance1Thread::gtkTheme() const
{
    return property->gtkTheme;
}

QString Appearance1Thread::iconTheme() const
{
    return property->iconTheme;
}

QString Appearance1Thread::monospaceFont() const
{
    return property->monospaceFont;
}

double Appearance1Thread::opacity() const
{
    return property->opacity;
}

void Appearance1Thread::setOpacity(double value)
{
    QMutexLocker locker(&mutex);
    appearanceManager->setOpacity(value);
}

QString Appearance1Thread::qtActiveColor() const
{
    return property->qtActiveColor;
}

void Appearance1Thread::setQtActiveColor(const QString &value)
{
    QMutexLocker locker(&mutex);
    appearanceManager->setQtActiveColor(value);
}

QString Appearance1Thread::standardFont() const
{
    return property->standardFont;
}

QString Appearance1Thread::wallpaperSlideShow() const
{
    return property->wallpaperSlideShow;
}

void Appearance1Thread::setWallpaperSlideShow(const QString &value)
{
    QMutexLocker locker(&mutex);
    appearanceManager->setWallpaperSlideShow(value);
}

QString Appearance1Thread::wallpaperURls() const
{
    return property->wallpaperURls;
}

int Appearance1Thread::dtkSizeMode() const
{
    return property->dtkSizeMode;
}

void Appearance1Thread::setDTKSizeMode(int value)
{
    QMutexLocker locker(&mutex);
    appearanceManager->doSetDTKSizeMode(value);
}

int Appearance1Thread::qtScrollBarPolicy() const
{
    return property->qtScrollBarPolicy;
}

void Appearance1Thread::setQtScrollBarPolicy(int value)
{
    QMutexLocker locker(&mutex);
    appearanceManager->doSetQtScrollBarPolicy(value);
}

int Appearance1Thread::windowRadius() const
{
    return property->windowRadius;
}

void Appearance1Thread::setWindowRadius(int value)
{
    QMutexLocker locker(&mutex);
    appearanceManager->setWindowRadius(value);
}

void Appearance1Thread::Delete(const QString &ty, const QString &name, const QDBusMessage &message)
{
    Q_UNUSED(message);
    QMutexLocker locker(&mutex);
    appearanceManager->deleteThermByType(ty, name);
}

QString Appearance1Thread::GetCurrentWorkspaceBackground(const QDBusMessage &message)
{
    Q_UNUSED(message);
    QMutexLocker locker(&mutex);
    return appearanceManager->doGetCurrentWorkspaceBackground();
}

QString Appearance1Thread::GetCurrentWorkspaceBackgroundForMonitor(const QString &strMonitorName, const QDBusMessage &message)
{
    Q_UNUSED(message);
    QMutexLocker locker(&mutex);
    return appearanceManager->doGetCurrentWorkspaceBackgroundForMonitor(strMonitorName);
}

double Appearance1Thread::GetScaleFactor(const QDBusMessage &message)
{
    Q_UNUSED(message);
    QMutexLocker locker(&mutex);
    return appearanceManager->getScaleFactor();
}

ScaleFactors Appearance1Thread::GetScreenScaleFactors(const QDBusMessage &message)
{
    Q_UNUSED(message);
    QMutexLocker locker(&mutex);
    return appearanceManager->getScreenScaleFactors();
}

QString Appearance1Thread::GetActiveColors(const QDBusMessage &message)
{
    Q_UNUSED(message);
    QMutexLocker locker(&mutex);
    return appearanceManager->getActiveColors();
}

QString Appearance1Thread::GetWallpaperSlideShow(const QString &monitorName, const QDBusMessage &message)
{
    Q_UNUSED(message);
    QMutexLocker locker(&mutex);
    return appearanceManager->doGetWallpaperSlideShow(monitorName);
}

QString Appearance1Thread::GetWorkspaceBackgroundForMonitor(const int &index, const QString &strMonitorName, const QDBusMessage &message)
{
    Q_UNUSED(message);
    QMutexLocker locker(&mutex);
    return appearanceManager->doGetWorkspaceBackgroundForMonitor(index, strMonitorName);
}

QString Appearance1Thread::List(const QString &ty, const QDBusMessage &message)
{
    QMutexLocker locker(&mutex);
    APPEARANCEDBUS.send(message.createReply(appearanceManager->doList(ty.toLower())));
    return QString();
}

void Appearance1Thread::Reset(const QDBusMessage &message)
{
    Q_UNUSED(message);
    QMutexLocker locker(&mutex);
    QStringList keys{GSKEYGLOBALTHEME, GSKEYGTKTHEME, GSKEYICONTHEM, GSKEYCURSORTHEME, GSKEYFONTSIZE, DDTKSIZEMODE};

    appearanceManager->doResetSettingBykeys(keys);

    appearanceManager->doResetFonts();
}

void Appearance1Thread::Set(const QString &ty, const QString &value, const QDBusMessage &message)
{
    Q_UNUSED(message);
    QMutexLocker locker(&mutex);
    appearanceManager->doSetByType(ty.toLower(), value);
}

void Appearance1Thread::SetCurrentWorkspaceBackground(const QString &uri, const QDBusMessage &message)
{
    Q_UNUSED(message);
    QMutexLocker locker(&mutex);
    appearanceManager->doSetCurrentWorkspaceBackground(uri);
}

void Appearance1Thread::SetCurrentWorkspaceBackgroundForMonitor(const QString &uri, const QString &strMonitorName, const QDBusMessage &message)
{
    Q_UNUSED(message);
    QMutexLocker locker(&mutex);
    appearanceManager->doSetCurrentWorkspaceBackgroundForMonitor(uri, strMonitorName);
}

void Appearance1Thread::SetMonitorBackground(const QString &monitorName, const QString &imageGile, const QDBusMessage &message)
{
    Q_UNUSED(message);
    QMutexLocker locker(&mutex);
    QString file = appearanceManager->doSetMonitorBackground(monitorName, imageGile);
}

void Appearance1Thread::SetScaleFactor(double scale, const QDBusMessage &message)
{
    Q_UNUSED(message);
    QMutexLocker locker(&mutex);
    appearanceManager->setScaleFactor(scale);
}

void Appearance1Thread::SetScreenScaleFactors(ScaleFactors scaleFactors, const QDBusMessage &message)
{
    Q_UNUSED(message);
    QMutexLocker locker(&mutex);
    appearanceManager->setScreenScaleFactors(scaleFactors);
}

void Appearance1Thread::SetActiveColors(const QString &activeColors, const QDBusMessage &message)
{
    Q_UNUSED(message);
    QMutexLocker locker(&mutex);
    appearanceManager->setActiveColors(activeColors);
}

void Appearance1Thread::SetWallpaperSlideShow(const QString &monitorName, const QString &slideShow, const QDBusMessage &message)
{
    Q_UNUSED(message);
    QMutexLocker locker(&mutex);
    appearanceManager->doSetWallpaperSlideShow(monitorName, slideShow);
}

void Appearance1Thread::SetWorkspaceBackgroundForMonitor(const int &index, const QString &strMonitorName, const QString &uri, const QDBusMessage &message)
{
    Q_UNUSED(message);
    QMutexLocker locker(&mutex);
    appearanceManager->doSetWorkspaceBackgroundForMonitor(index, strMonitorName, uri);
}

QString Appearance1Thread::Show(const QString &ty, const QStringList &names, const QDBusMessage &message)
{
    QMutexLocker locker(&mutex);
    QString show = appearanceManager->doShow(ty, names);
    APPEARANCEDBUS.send(message.createReply(show));
    return show;
}

QString Appearance1Thread::Thumbnail(const QString &ty, const QString &name, const QDBusMessage &message)
{
    QMutexLocker locker(&mutex);
    QString file = appearanceManager.get()->doThumbnail(ty, name);
    if (!file.isEmpty() && !QFile::exists(file)) {
        APPEARANCEDBUS.send(message.createErrorReply(QDBusError::InvalidArgs, file));
    } else {
        APPEARANCEDBUS.send(message.createReply(file));
    }
    return file;
}

void Appearance1Thread::init()
{
    qInfo()<<"init: begin";
    QMutexLocker locker(&mutex);
    appearanceManager.reset(new AppearanceManager(property, this));
    connect(appearanceManager.get(), &AppearanceManager::Changed, this, &Appearance1Thread::Changed);
    connect(appearanceManager.get(), &AppearanceManager::Refreshed, this, &Appearance1Thread::Refreshed);
    qInfo()<<"init: end";
}
