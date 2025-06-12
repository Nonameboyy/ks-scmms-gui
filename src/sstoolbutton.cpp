/**
 * @file sstoolbutton.cpp
 * @brief QToolButton处理
 * @author zhenggongping <zhenggongping@kylinos.com.cn>
 * @copyright (c) 2020~2022 KylinSec Co., Ltd. All rights reserved.
 */

#include "sstoolbutton.h"
#include <QPixmap>

SSToolButton::SSToolButton(QWidget *parent) : QToolButton(parent)
{
}

SSToolButton::~SSToolButton()
{
}

void SSToolButton::setBorderImage(const QString &filePath, const int &pixEach)
{
    QPixmap image(filePath);
    QString cropNum = "";
    int left = 0, right = 0, top = 0, bottom = 0;
    int times = image.width() / pixEach;

    for (int i = 0; i < times; ++i)
    {
        right = image.width() - pixEach * (i + 1);
        left = pixEach * i;
        if (i > 0)
            cropNum.append(':');
        cropNum = cropNum + QString("%1 %2 %3 %4").arg(top).arg(right).arg(bottom).arg(left);
    }

    setSSText(cropNum, filePath);
}

void SSToolButton::setBorderImage(const QString filePathN, const QString filePathS, const QString filePathP, const int pixEach)
{
    QVector<QString> vFilePath;
    vFilePath.push_back(filePathN);
    vFilePath.push_back(filePathS);
    vFilePath.push_back(filePathP);

    QVector<QString> vCropNum;
    for (int f = 0; f < 3; ++f)
    {
        QPixmap image(vFilePath[f]);
        QString cropNum = "";
        int left = 0, right = 0, top = 0, bottom = 0;
        int times = image.width() / pixEach;

        for (int i = 0; i < times; ++i)
        {
            right = image.width() - pixEach * (i + 1);
            left = pixEach * i;
            if (i > 0)
                cropNum.append(':');
            cropNum = cropNum + QString("%1 %2 %3 %4").arg(top).arg(right).arg(bottom).arg(left);
        }
        vCropNum.push_back(cropNum);
    }

    setSSText(vCropNum, vFilePath);
}

void SSToolButton::setSSText(QString cropNum, QString NormalPicPath)
{
    QString norStr("QToolButton{border-image:url(%1) %2;}");
    QString disableStr("QToolButton:disabled{border-image:url(%1) %2;}");
    int index = 0;
    QString style = norStr.arg(NormalPicPath, cropNum.section(":", index + 2, index + 2))+
                        disableStr.arg(NormalPicPath, cropNum.section(":", index + 3, index + 3));

    this->setStyleSheet(style);
}

void SSToolButton::setSSText(QVector<QString> vCropNum, QVector<QString> vPicPath)
{
    QString norStr("QToolButton{border-image:url(%1) %2;}");
    QString hoverStr("QToolButton:hover{border-image:url(%1) %2;}");
    QString pressStr("QToolButton:pressed{border-image:url(%1) %2; padding: 1px; margin: 1px;}");
    QString disableStr("QToolButton:disabled{border-image:url(%1) %2;}");
    int index = 0;
    QString style = norStr.arg(vPicPath[0], vCropNum[0].section(":", index + 2, index + 2)) +
                        disableStr.arg(vPicPath[0], vCropNum[0].section(":", index + 3, index + 3)) +
                        hoverStr.arg(vPicPath[1], vCropNum[1].section(":", index + 1, index + 1)) +
                        pressStr.arg(vPicPath[2], vCropNum[2].section(":", index + 2, index + 2));

    this->setStyleSheet(style);
}
