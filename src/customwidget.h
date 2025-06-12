/**
 * @file customwidget.h
 * @brief 自定义标题栏处理头文件
 * @author zhenggongping <zhenggongping@kylinos.com.cn>
 * @copyright (c) 2020~2022 KylinSec Co., Ltd. All rights reserved.
 */

#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <QWidget>
#include <QWebEngineView>
#include <QHBoxLayout>

namespace Ui {
class customwidget;
}

class CustomWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CustomWidget(QWidget *parent = NULL);
    ~CustomWidget();
    void showInspector();
    void hideInspector();

private:
    Ui::customwidget *ui;
    QWebEngineView *m_webCustom;
    QHBoxLayout *m_layout;
};

#endif // CUSTOMWIDGET_H
