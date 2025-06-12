/**
 * @file webpage.cpp
 * @brief 忽略https证书错误
 * @author zhenggongping <zhenggongping@kylinos.com.cn>
 * @copyright (c) 2020~2022 KylinSec Co., Ltd. All rights reserved.
 */

#include "webpage.h"

WebPage::WebPage()
{
}

bool WebPage::certificateError(const QWebEngineCertificateError &error)
{
    Q_UNUSED(error);
    return true;
}
