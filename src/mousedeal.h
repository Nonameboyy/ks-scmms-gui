/**
 * @file mousedeal.h
 * @brief 标题栏鼠标处理头文件
 * @author zhenggongping <zhenggongping@kylinos.com.cn>
 * @copyright (c) 2020~2022 KylinSec Co., Ltd. All rights reserved.
 */

#ifndef MOUSEDEAL_H
#define MOUSEDEAL_H

#include <QWidget>

class MouseDeal : public QWidget
{
public:
    explicit MouseDeal(QWidget *parent = NULL);
    ~MouseDeal();
    static MouseDeal &Instance();
    static void pressMouse(QMouseEvent *event, QWidget *Widget) {Instance()._pressMouse(event, Widget);}
    static void releaseMouse(QMouseEvent *event, QWidget *Widget) {Instance()._releaseMouse(event, Widget);}
    static void doubleClickMouse(QMouseEvent *event, QWidget *Widget) {Instance()._doubleClickMouse(event, Widget);}
    static void moveMouse(QMouseEvent *event, QWidget *Widget) {Instance()._moveMouse(event, Widget);}
    static void titleSetCursorType(QMouseEvent *event, QWidget *Widget) {Instance()._titleSetCursorType(event, Widget);}

private:
    void _pressMouse(QMouseEvent *event, QWidget *Widget);
    void _releaseMouse(QMouseEvent *event, QWidget *Widget);
    void _doubleClickMouse(QMouseEvent *event, QWidget *Widget);
    void _moveMouse(QMouseEvent *event, QWidget *Widget);
    void _titleSetCursorType(QMouseEvent *event, QWidget *Widget);
    void setCursorType(int mousePos, QWidget *Widget);
    int countMouseRanks(QPoint p, int row, QWidget *Widget);
    int countMouseRow(QPoint p, QWidget *Widget);

private:
    bool m_bMousePressed;
    int m_mousePos;
    QPoint m_lastPoint;
};

#endif // MOUSEDEAL_H
