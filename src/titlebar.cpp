/**
 * @file titlebar.cpp
 * @brief 标题栏
 * @author zhenggongping <zhenggongping@kylinos.com.cn>
 * @copyright (c) 2020~2022 KylinSec Co., Ltd. All rights reserved.
 */

#include "titlebar.h"
#include "customwidget.h"
#include "mousedeal.h"
#include <QMouseEvent>
#include <QPainterPath>

#ifdef HIGH_VERSION
#include <QtGui>
#endif

#define TITLE_BUTTON_HEIGHT 16
#define TITLE_BUTTON_WIDTH 16
#define TITLE_HEIGHT 30

TitleBar::TitleBar(QWidget *parent) : QWidget(parent), m_isPressed(false)
{
    initControl();
    initConnections();
    setTitleInfo();
    initOption();
    initInspector();
}

TitleBar::~TitleBar()
{
    if (m_CstmW)
    {
        delete m_CstmW;
        m_CstmW = NULL;
    }
    if (m_layout)
    {
        delete m_layout;
        m_layout = NULL;
    }
}

void TitleBar::setRestoreInfo(const QPoint point, const QSize size)
{
   m_restorePos = point;
   m_restoreSize = size;
}

void TitleBar::getRestoreInfo(QPoint& point, QSize& size)
{
    point = m_restorePos;
    size = m_restoreSize;
}

void TitleBar::sendDebug()
{
    ShowSpector();
}

void TitleBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPainterPath pathBack;
    pathBack.setFillRule(Qt::WindingFill);
    pathBack.addRoundedRect(QRect(0, 0, this->width(), this->height()), 3, 3);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(pathBack, QBrush(QColor(45, 45, 45)));

    int BorderWidth = 0;
    if (this->width() != (this->parentWidget()->width() - BorderWidth))
    {
        this->setFixedWidth(this->parentWidget()->width() - BorderWidth);
    }

    QWidget::paintEvent(event);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (m_pMax->isVisible())
        sendMax();
    else
        sendRestore();

    MouseDeal::doubleClickMouse(event, this);
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    if (m_pMax->isVisible())
    {
        m_isPressed = true;
        m_startMovePos = event->globalPos();
    }
    MouseDeal::pressMouse(event, this);
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isPressed)
    {
        QPoint movePoint = event->globalPos() - m_startMovePos;
        QPoint widgetPos = this->parentWidget()->pos();
        m_startMovePos = event->globalPos();
        this->parentWidget()->move(widgetPos.x() + movePoint.x(), widgetPos.y() + movePoint.y());
    }

    MouseDeal::titleSetCursorType(event, this);
    return QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    m_isPressed = false;
    MouseDeal::releaseMouse(event, this);
}

void TitleBar::keyPressEvent(QKeyEvent *e)
{
    if (e->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) && e->key() == Qt::Key_F12)
    {
        printf("[%s][%d]\n", __func__, __LINE__);
        ShowSpector();
    }
}

void TitleBar::initControl()
{
    this->setMouseTracking(true);
    QWidget::setCursor(Qt::IBeamCursor);

    m_pIcon = new SSToolButton(this);
    m_pTitleContent = new QLabel;
    m_pMin = new SSToolButton(this);
    m_pMax = new SSToolButton(this);
    m_pRestore = new SSToolButton(this);
    m_pClose = new SSToolButton(this);

   // m_pMin->setIconSize(QSize(16, 16));
    m_pIcon->setFixedSize(QSize(TITLE_BUTTON_WIDTH, TITLE_BUTTON_WIDTH));
    m_pMin->setFixedSize(QSize(TITLE_BUTTON_WIDTH, TITLE_BUTTON_HEIGHT));
    m_pMax->setFixedSize(QSize(TITLE_BUTTON_WIDTH, TITLE_BUTTON_HEIGHT));
    m_pRestore->setFixedSize(QSize(TITLE_BUTTON_WIDTH, TITLE_BUTTON_HEIGHT));
    m_pClose->setFixedSize(QSize(TITLE_BUTTON_WIDTH, TITLE_BUTTON_HEIGHT));

    m_pMin->setToolTip(QStringLiteral("最小化"));
    m_pMax->setToolTip(QStringLiteral("最大化"));
    m_pRestore->setToolTip(QStringLiteral("还原"));
    m_pClose->setToolTip(QStringLiteral("关闭"));

    m_layout = new QHBoxLayout(this);
    m_layout->addWidget(m_pIcon);
    m_layout->addWidget(m_pTitleContent);
    m_layout->addWidget(m_pMin);
    m_layout->addWidget(m_pMax);
    m_layout->addWidget(m_pRestore);
    m_layout->addWidget(m_pClose);
    m_layout->setMargin(3);
    m_layout->setSpacing(2);

    m_pRestore->setVisible(false);
    m_pTitleContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    this->setFixedHeight(TITLE_HEIGHT);

    m_pIcon->setCursor(Qt::ArrowCursor);
    m_pTitleContent->setCursor(Qt::ArrowCursor);
    m_pMin->setCursor(Qt::ArrowCursor);
    m_pMax->setCursor(Qt::ArrowCursor);
    m_pRestore->setCursor(Qt::ArrowCursor);
    m_pClose->setCursor(Qt::ArrowCursor);
}

void TitleBar::initConnections()
{
    connect(m_pMin, SIGNAL(clicked()), this, SLOT(sendMin()));
    connect(m_pMax, SIGNAL(clicked()), this, SLOT(sendMax()));
    connect(m_pRestore, SIGNAL(clicked()), this, SLOT(sendRestore()));
    connect(m_pClose, SIGNAL(clicked()), this, SLOT(sendClose()));
}

void TitleBar::setTitleInfo()
{
    m_pIcon->setBorderImage(":/images/logo.png", 16);
    m_pTitleContent ->setStyleSheet("color:white;");
    QFont font;
    font.setPointSize(8);
    m_pTitleContent ->setFont(font);
    m_pTitleContent->setText(QStringLiteral("麒麟信安集中运维管控系统"));
    m_pMin->setBorderImage(":/images/min_n.png", ":/images/min_s.png", ":/images/min_pre.png", 16);
    m_pMax->setBorderImage(":/images/max_n.png", ":/images/max_s.png", ":/images/max_pre.png", 16);
    m_pRestore->setBorderImage(":/images/windows_n.png", ":/images/windows_s.png", ":/images/windows_pre.png", 16);
    m_pClose->setBorderImage(":/images/close_n.png", ":/images/close_s.png", ":/images/close_pre.png", 16);
}

void TitleBar::initOption()
{
}

void TitleBar::initInspector()
{
    m_CstmW = new CustomWidget();
    m_CstmW->setWindowModality(Qt::NonModal);
    m_CstmW->setWindowTitle(tr("Inspector"));
    m_CstmW->setWindowIcon(QIcon(":images/logo.png"));
}

void TitleBar::sendMin()
{
    emit sendMinSig();
}

void TitleBar::sendMax()
{
    m_pMax->setVisible(false);
    m_pRestore->setVisible(true);
    emit sendMaxSig();
}

void TitleBar::sendRestore()
{
    m_pRestore->setVisible(false);
    m_pMax->setVisible(true);
    emit sendRestoreSig();
}

void TitleBar::sendClose()
{
    emit sendCloseSig();
}

void TitleBar::ShowSpector()
{
    m_CstmW->showInspector();
}
