/**
 * @file mousedeal.cpp
 * @brief 标题栏鼠标处理
 * @author zhenggongping <zhenggongping@kylinos.com.cn>
 * @copyright (c) 2020~2022 KylinSec Co., Ltd. All rights reserved.
 */

#include "mousedeal.h"

#include <QMouseEvent>
#include <QApplication>

#define CORNER_LENGTH 5

enum MousePosition
{
    UPPER_LEFT_CORNER = 11,
    UPPER_MIDDLE_CORNER = 12,
    UPPER_RIGHT_CORNER = 13,
    MIDDLE_LEFT_CORNER = 21,
    MIDDLE_AREA = 22,
    MIDDLE_RIGHT_CORNER = 23,
    BOTTOM_LEFT_CORNER = 31,
    BOTTOM_MIDDLE_CORNER = 32,
    BOTTOM_RIGHT_CORNER = 33
};


MouseDeal::MouseDeal(QWidget *parent) : QWidget(parent)
  , m_bMousePressed(false), m_mousePos(0), m_lastPoint(0, 0)
{
    Q_UNUSED(parent);
}

MouseDeal::~MouseDeal()
{
}

MouseDeal &MouseDeal::Instance()
{
    static MouseDeal s_mouse;
    return s_mouse;
}

void MouseDeal::_pressMouse(QMouseEvent *event, QWidget *Widget)
{
    if (event->button() == Qt::LeftButton)
    {
        m_bMousePressed = true;
        m_lastPoint = event->globalPos();
        m_mousePos = countMouseRanks(event->pos(), countMouseRow(event->pos(), Widget), Widget);
        event->ignore();
    }
}

void MouseDeal::_releaseMouse(QMouseEvent *event, QWidget *Widget)
{
    Q_UNUSED(Widget);
    if (m_bMousePressed)
        m_bMousePressed = false;
    QApplication::restoreOverrideCursor();
    event->ignore();
}

void MouseDeal::_doubleClickMouse(QMouseEvent *event, QWidget *Widget)
{
    if (event->button() == Qt::LeftButton)
    {
        if (Widget->windowState() != Qt::WindowFullScreen)
            Widget->setWindowState(Qt::WindowFullScreen);
        else
            Widget->setWindowState(Qt::WindowNoState);
    }
    event->ignore();
}

void MouseDeal::_moveMouse(QMouseEvent *event, QWidget *Widget)
{
    int mousePos = countMouseRanks(event->pos(), countMouseRow(event->pos(), Widget), Widget);
    setCursorType(mousePos, Widget);
    if (m_bMousePressed)
    {
        QPoint point = event->globalPos() - m_lastPoint;

        if (m_mousePos == MIDDLE_AREA)
        {
            point = point + Widget->pos();
            Widget->move(point);
        }
        else
        {
            QRect rect = Widget->geometry();
            switch (m_mousePos)
            {
                case UPPER_LEFT_CORNER:  rect.setTopLeft(rect.topLeft() + point); break;
                case UPPER_MIDDLE_CORNER: rect.setTop(rect.top() + point.y()); break;
                case UPPER_RIGHT_CORNER: rect.setTopRight(rect.topRight() + point); break;
                case MIDDLE_LEFT_CORNER: rect.setLeft(rect.left() + point.x()); break;
                case MIDDLE_RIGHT_CORNER: rect.setRight(rect.right() + point.x()); break;
                case BOTTOM_LEFT_CORNER: rect.setBottomLeft(rect.bottomLeft() + point); break;
                case BOTTOM_MIDDLE_CORNER: rect.setBottom(rect.bottom() + point.y()); break;
                case BOTTOM_RIGHT_CORNER: rect.setBottomRight(rect.bottomRight() + point); break;
            }
            Widget->setGeometry(rect);
        }
        m_lastPoint = event->globalPos();
    }
    event->ignore();
}

void MouseDeal::_titleSetCursorType(QMouseEvent *event, QWidget *Widget)
{
    int mousePos = countMouseRanks(event->pos(), countMouseRow(event->pos(), Widget), Widget);
    Qt::CursorShape cursor = Qt::ArrowCursor;

    switch(mousePos)
    {
        case UPPER_LEFT_CORNER:
            cursor = Qt::SizeFDiagCursor; break;
        case UPPER_RIGHT_CORNER:
            cursor = Qt::SizeBDiagCursor; break;
        case UPPER_MIDDLE_CORNER:
            cursor = Qt::SizeVerCursor; break;
        case MIDDLE_LEFT_CORNER:
        case MIDDLE_RIGHT_CORNER:
            cursor = Qt::SizeHorCursor; break;
        default:
            break;
    }

    Widget->setCursor(cursor);
}

void MouseDeal::setCursorType(int mousePos, QWidget *Widget)
{
    Qt::CursorShape cursor = Qt::ArrowCursor;

    switch(mousePos)
    {
        case UPPER_LEFT_CORNER:
        case BOTTOM_RIGHT_CORNER:
            cursor = Qt::SizeFDiagCursor; break;
        case UPPER_RIGHT_CORNER:
        case BOTTOM_LEFT_CORNER:
            cursor = Qt::SizeBDiagCursor; break;
        case MIDDLE_LEFT_CORNER:
        case MIDDLE_RIGHT_CORNER:
            cursor = Qt::SizeHorCursor; break;
        case UPPER_MIDDLE_CORNER:
        case BOTTOM_MIDDLE_CORNER:
            cursor = Qt::SizeVerCursor; break;
        case MIDDLE_AREA:
        {
            if (m_bMousePressed)
                cursor = Qt::ClosedHandCursor;
            else
                cursor = Qt::IBeamCursor;
            break;
        }
        default:
            break;
    }
    Widget->setCursor(cursor);
}

int MouseDeal::countMouseRanks(QPoint p, int row, QWidget *Widget)
{
    if (p.y() < CORNER_LENGTH)
     return 10 + row;
    else if (p.y() > Widget->height() - CORNER_LENGTH)
     return 30 + row;
    else
     return 20 + row;
}

int MouseDeal::countMouseRow(QPoint p, QWidget *Widget)
{
    int row = p.x() > (Widget->width() - CORNER_LENGTH) ? 3 : 2;
    return p.x() < CORNER_LENGTH ? 1 : row;
}

