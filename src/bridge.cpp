/**
 * @file bridge.cpp
 * @brief js返回消息处理
 * @author zhenggongping <zhenggongping@kylinos.com.cn>
 * @copyright (c) 2020~2022 KylinSec Co., Ltd. All rights reserved.
 */

#include "bridge.h"

Bridge::Bridge(QObject *parent)
{
    Q_UNUSED(parent);
}

Bridge::~Bridge()
{
}

void Bridge::jsCallSlot(QString msg)
{
    printf("[%s][%d][%s]\n", __func__, __LINE__, msg.toStdString().data());
    emit sendLoginResSig(msg);
}
