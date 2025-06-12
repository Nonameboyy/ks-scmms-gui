/**
 * @file titlebar.h
 * @brief 标题栏头文件
 * @author zhenggongping <zhenggongping@kylinos.com.cn>
 * @copyright (c) 2020~2022 KylinSec Co., Ltd. All rights reserved.
 */

#ifndef TITLEBAR_H
#define TITLEBAR_H

#include "sstoolbutton.h"
#include "customwidget.h"

#include <QLabel>
#include <QPushButton>

class TitleBar : public QWidget
{
    Q_OBJECT
public:
    TitleBar(QWidget *parent);
    ~TitleBar();
    void setRestoreInfo(const QPoint point, const QSize size);
    void getRestoreInfo(QPoint& point, QSize& size);
    void sendDebug();

private:
    void paintEvent(QPaintEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *e);

    void initControl();
    void initConnections();
    void setTitleInfo();
    void initOption();
    void initInspector();

signals:
    void sendMinSig();
    void sendMaxSig();
    void sendRestoreSig();
    void sendCloseSig();

private slots:
    void sendMin();
    void sendMax();
    void sendRestore();
    void sendClose();
    void ShowSpector();

private:
    CustomWidget *m_CstmW;
    QHBoxLayout *m_layout;
    QLabel *m_pTitleContent;
    SSToolButton *m_pIcon;
    SSToolButton *m_pMin;
    SSToolButton *m_pRestore;
    SSToolButton *m_pMax;
    SSToolButton *m_pClose;
    bool m_isPressed;
    QPoint m_startMovePos;
    QPoint m_restorePos;
    QSize m_restoreSize;
};

#endif // TITLEBAR_H
