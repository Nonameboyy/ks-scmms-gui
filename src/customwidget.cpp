/**
 * @file customwidget.cpp
 * @brief 自定义标题栏处理
 * @author zhenggongping <zhenggongping@kylinos.com.cn>
 * @copyright (c) 2020~2022 KylinSec Co., Ltd. All rights reserved.
 */

#include "customwidget.h"
#include "ui_customwidget.h"

CustomWidget::CustomWidget(QWidget *parent) :
    ui(new Ui::customwidget)
{
    Q_UNUSED(parent);
    ui->setupUi(this);
    m_webCustom = new QWebEngineView(this);
    m_layout = new QHBoxLayout(ui->Inspector);
    ui->Inspector->setLayout(m_layout);
    m_layout->addWidget(m_webCustom);
}

CustomWidget::~CustomWidget()
{
    if (m_layout)
    {
        delete m_layout;
        m_layout = NULL;
    }
    if (m_webCustom)
    {
        delete m_webCustom;
        m_webCustom = NULL;
    }
    delete ui;
}

void CustomWidget::showInspector()
{
    printf("[%s][%d]\n", __func__, __LINE__);
    m_webCustom->load(QUrl("http://localhost:9988"));
    show();
}

void CustomWidget::hideInspector()
{
    hide();
}
