/**
 * @file mask-widget.cpp
 * @brief 登录页面弹出阻塞
 * @author zhenggongping <zhenggongping@kylinos.com.cn>
 * @copyright (c) 2020~2022 KylinSec Co., Ltd. All rights reserved.
 */

#include "mask-widget.h"
#include <QGridLayout>
#include <QMouseEvent>
#include <QMovie>
MaskWidget::MaskWidget(QWidget *parent) : QWidget(parent), m_labLoading(nullptr)
{
    initUI();
}

void MaskWidget::setMaskVisible(bool visible)
{
    printf("[%s][%d][%d]\n", __func__, __LINE__, visible);
    this->setVisible(visible);
}

bool MaskWidget::maskIsVisible()
{
    return this->isVisible();
}

MaskWidget::~MaskWidget()
{
}

void MaskWidget::initUI()
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setMargin(0);
    gridLayout->setSpacing(0);

    m_labLoading = new QLabel(this);
    m_labLoading->setFixedSize(128, 128);
    QMovie *movie;
    movie = new QMovie(":/images/loading.gif");
    m_labLoading->setMovie(movie);
    movie->start();

    gridLayout->addWidget(m_labLoading, 0, 0, Qt::AlignCenter);

    hide();
}
