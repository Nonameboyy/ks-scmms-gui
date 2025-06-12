/**
 * @file webpage.h
 * @brief 忽略https证书错误头文件
 * @author zhenggongping <zhenggongping@kylinos.com.cn>
 * @copyright (c) 2020~2022 KylinSec Co., Ltd. All rights reserved.
 */

#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWebEnginePage>

class WebPage : public QWebEnginePage
{
    Q_OBJECT

public:
    WebPage();

protected:
    bool certificateError(const QWebEngineCertificateError &error);

};


#endif // WEBPAGE_H
