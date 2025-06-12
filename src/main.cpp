/**
 * @file main.cpp
 * @brief 程序主入口
 * @author zhenggongping <zhenggongping@kylinos.com.cn>
 * @copyright (c) 2020~2022 KylinSec Co., Ltd. All rights reserved.
 */

#include "widget.h"
#include <QApplication>
#include <QCoreApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QCoreApplication::setOrganizationName("kylinsec");
    QCoreApplication::setApplicationName("ks-scmms-gui");
    QSettings settings;
    settings.setDefaultFormat(QSettings::IniFormat);
    QApplication a(argc, argv);
    Widget w;
    w.show();

    return a.exec();
}


