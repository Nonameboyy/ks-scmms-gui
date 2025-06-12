/**
 * @file widget.h
 * @brief gui主窗口头文件
 * @author zhenggongping <zhenggongping@kylinos.com.cn>
 * @copyright (c) 2020~2022 KylinSec Co., Ltd. All rights reserved.
 */

#ifndef WIDGET_H
#define WIDGET_H

#include "titlebar.h"
#include "bridge.h"
#include "webpage.h"
#include "mask-widget.h"
#include <QWidget>
#include <QWebEngineView>
#include <QWebChannel>
#include <QProcess>

#define CUSTOMIZE_TITLE_BAR
#define HIDE_FUNCTION

#ifndef HIGH_VERSION
#include <QWebEngineProfile>
#endif

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = NULL);
    ~Widget();

protected:
    void paintEvent(QPaintEvent *event);
#ifdef CUSTOMIZE_TITLE_BAR
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *e);
    void resizeEvent(QResizeEvent *event);
#endif

private:
#ifdef CUSTOMIZE_TITLE_BAR
    void setTitleBar();
#endif
    void setFontPageUi();
    void setWebview();
    void writeConfig();
    void readConfig();
    void removeConfig();
#ifdef HIGH_VERSION
    void clearCache();
#endif
    bool checkLoginParam();
    void setLoginBtn();
    void clearLoginBtn();
    void setBusy(bool status);
    unsigned short startSocat();
    void lockLoginUI(int locktimesec);
    void unlockLoginUI();
    void setLoginFailed(); 
    bool getLoginFailed();
    void rmTmpFile();

private slots:
    void checkRemberPW(bool status);
    void clickConnectSlot();
    void loadFinishedSlot(bool ok);
    void loginDealSlot(QString msg);
    void openUrlSlot(QUrl url);
    void handleLoginTimeout();
    void handleLoginFailTimeout();
    void iconurlChangedSlot(QUrl url);
    void webDownloadSlot(QWebEngineDownloadItem *item);
    void dealPasswd(QString text);
#ifdef CUSTOMIZE_TITLE_BAR
    void clickMinSlot();
    void clickMaxSlot();
    void clickRestoreSlot();
    void clickCloseSlot();
#endif

private:
    Ui::Widget *ui;
    bool m_bDebug;
    bool m_bFirstLogin;
    bool m_bShowWidget;
    bool m_bRememberPW;
    int m_bLoginFailCount; //记录当前客户端登录失败次数
    QString m_LoginLockTime;//锁定截至时间
    QTimer *m_pLoginTimer; //登录失败重置计时器, 三分钟后重置登录状态
    TitleBar *m_pTitleBar;
    QWebEngineView *m_pWebView;
    QWebChannel *m_pWebChannel;
    Bridge *m_pBridge;
    QTimer *m_pTimer;
    WebPage *m_pWebPage;
    QString m_qstrCurLang;
    QString m_qstrUrl;
    MaskWidget *m_maskWidget;
    // task 51886
    QProcess *m_pSocat = NULL;
};

#endif // WIDGET_H
