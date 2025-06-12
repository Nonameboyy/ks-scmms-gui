/**
 * @file bridge.h
 * @brief js返回消息处理头文件
 * @author zhenggongping <zhenggongping@kylinos.com.cn>
 * @copyright (c) 2020~2022 KylinSec Co., Ltd. All rights reserved.
 */

#ifndef BRIDGE_H
#define BRIDGE_H

#include <QObject>

class Bridge : public QObject
{
    Q_OBJECT
public:
    explicit Bridge(QObject *parent = NULL);
    ~Bridge();

signals:
    void sendLoginResSig(QString msg);

public slots:
    void jsCallSlot(QString msg);
};

#endif // BRIDGE_H

