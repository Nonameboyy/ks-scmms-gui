/**
 * @file sstoolbutton.h
 * @brief QToolButton处理头文件
 * @author zhenggongping <zhenggongping@kylinos.com.cn>
 * @copyright (c) 2020~2022 KylinSec Co., Ltd. All rights reserved.
 */

#ifndef SSTOOLBUTTON_H
#define SSTOOLBUTTON_H

#include <QToolButton>

class SSToolButton : public QToolButton
{
    Q_OBJECT
public:
    explicit SSToolButton(QWidget *parent = NULL);
    ~SSToolButton();
    void setBorderImage(const QString &filePath, const int &pixEach);
    void setBorderImage(const QString filePathN, const QString filePathS, const QString filePathP, const int pixEach);
private:
    void setSSText(QString cropNum, QString NormalPicPath);
    void setSSText(QVector<QString> vCropNum, QVector<QString> vPicPath);
};

#endif //SSTOOLBUTTON_H
