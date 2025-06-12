/**
 * @file widget.cpp
 * @brief gui主窗口
 * @author zhenggongping <zhenggongping@kylinos.com.cn>
 * @copyright (c) 2020~2022 KylinSec Co., Ltd. All rights reserved.
 */

#include "widget.h"
#include "ui_widget.h"
#include "webpage.h"
#include "bridge.h"
#include "mousedeal.h"

#include <QMouseEvent>
#include <QWebEngineSettings>
#include <QNetworkProxyFactory>
#include <QDesktopWidget>
#include <QWebEngineCookieStore>
#include <QTimer>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QSettings>
#include <QFileInfo>
#include <QTextStream>

#ifdef HIGH_VERSION
#include <QWebEngineProfile>
#else
#include <QWebEngineDownloadItem>
#endif

Widget::Widget(QWidget *parent) : QWidget(parent),
                                  ui(new Ui::Widget),
                                  m_bDebug(false),
                                  m_bFirstLogin(false),
                                  m_bShowWidget(false),
                                  m_bRememberPW(false),
                                  m_bLoginFailCount(0)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/logo.png"));
    setWindowTitle("麒麟信安集中运维管控系统");
    m_maskWidget = new MaskWidget(this);
    m_maskWidget->setFixedSize(this->size()); // 设置窗口大小
    this->stackUnder(qobject_cast<QWidget *>(m_maskWidget));
    setFontPageUi();
#ifdef CUSTOMIZE_TITLE_BAR
    setTitleBar();
#endif
    setWebview();
    m_pLoginTimer = new QTimer(this);
    connect(m_pLoginTimer, SIGNAL(timeout()), this, SLOT(handleLoginFailTimeout()));
	
	if(getLoginFailed()){
		QDateTime parsedTime = QDateTime::fromString(m_LoginLockTime, "yyyy-MM-dd HH:mm:ss");
		if(parsedTime.isValid()){
			QDateTime current = QDateTime::currentDateTime(); //当前时间
			qint64 secondsDiff = current.secsTo(parsedTime);  // 目标时间与当前时间的秒差
			qDebug() << "the secondsDiff is :" << secondsDiff;
			if((secondsDiff < 3 * 60 && secondsDiff > 0) && m_bLoginFailCount >= 3) {
				lockLoginUI(3 * 60 * 1000 - secondsDiff * 1000);
			}else if(m_bLoginFailCount >= 3 && (secondsDiff <= 0)){
				m_bLoginFailCount = 0;
				rmTmpFile();
			}

			//获取登录失败次数
			if(m_bLoginFailCount >= 3 && !parsedTime.isValid()){
				lockLoginUI(3 * 60 * 1000);//锁定
			}
		}
	}
    QStringList args = qApp->arguments();
    if (args.size() > 1)
    {
        if (args[1].toStdString() == "DBG")
        {
            printf("[%s][%d] open debug\n", __func__, __LINE__);
            m_bDebug = true;
        }
    }
}

Widget::~Widget()
{
    if (m_pTimer)
    {
        delete m_pTimer;
        m_pTimer = NULL;
    }
    if (m_pLoginTimer)
    {
        delete m_pLoginTimer;
        m_pLoginTimer = NULL;
    }
    if (m_pBridge)
    {
        delete m_pBridge;
        m_pBridge = NULL;
    }
    if (m_pWebChannel)
    {
        delete m_pWebChannel;
        m_pWebChannel = NULL;
    }
    if (m_pWebView)
    {
        delete m_pWebView;
        m_pWebView = NULL;
    }
    if (m_pWebPage)
    {
        delete m_pWebPage;
        m_pWebPage = NULL;
    }
    if (m_pTitleBar)
    {
        delete m_pTitleBar;
        m_pTitleBar = NULL;
    }

    if (m_maskWidget)
    {
        delete m_maskWidget;
        m_maskWidget = NULL;
    }
    delete ui;
}

void Widget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawPixmap(rect(), QPixmap(":/images/BG.png"), QRect());
}

#ifdef CUSTOMIZE_TITLE_BAR
void Widget::mousePressEvent(QMouseEvent *event)
{
    MouseDeal::pressMouse(event, this);
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    MouseDeal::releaseMouse(event, this);
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    MouseDeal::moveMouse(event, this);
}

void Widget::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_F5)
    {
        printf("[%s][%d][F5]\n", __func__, __LINE__);
        m_pWebView->reload();
    }
#ifdef HIDE_FUNCTION
    if (m_bDebug)
    {
        if (e->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) && e->key() == Qt::Key_F12)
        {
            printf("[%s][%d][F12]\n", __func__, __LINE__);
            m_pTitleBar->sendDebug();
        }
    }
#endif

    if (Qt::Key_Return == e->key())
    {
        if (ui->m_pIp->hasFocus())
            ui->m_pUser->setFocus();
        else if (ui->m_pUser->hasFocus())
            ui->m_pPasswd->setFocus();
        else if (ui->m_pPasswd->hasFocus())
        {
            ui->m_pLogin->click();
        }
    }
    else if (Qt::Key_Up == e->key())
    {
        if (ui->m_pPasswd->hasFocus())
            ui->m_pUser->setFocus();
        else if (ui->m_pUser->hasFocus())
            ui->m_pIp->setFocus();
    }
    else if (Qt::Key_Down == e->key())
    {
        if (ui->m_pIp->hasFocus())
            ui->m_pUser->setFocus();
        else if (ui->m_pUser->hasFocus())
            ui->m_pPasswd->setFocus();
    }
}

void Widget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    QDesktopWidget *pDesktopWidget = QApplication::desktop();
    QRect screenRect = pDesktopWidget->availableGeometry();
    if (pDesktopWidget->screenCount() > 1)
    {
        int prim_screen = pDesktopWidget->screenNumber(this);
        screenRect = pDesktopWidget->screenGeometry(prim_screen);
    }

    qreal w = width() / (qreal)screenRect.width();

    if (w > 5)
        w = 5;
    else if (w < 0.25)
        w = 0.25;
    else if (w < 0.75)
        w = w + 0.1;

    m_pWebView->setZoomFactor(w);

    if (m_maskWidget != nullptr)
    {
        m_maskWidget->setAutoFillBackground(true);
        QPalette pal = m_maskWidget->palette();
        pal.setColor(QPalette::Background, QColor(0x00, 0x00, 0x00, 0x20));
        m_maskWidget->setPalette(pal);
        m_maskWidget->setFixedSize(this->size());
    }
}

void Widget::setTitleBar()
{
    this->setMouseTracking(true);
    ui->m_pLogin->setCursor(Qt::ArrowCursor);
    ui->m_pRememberPW->setCursor(Qt::ArrowCursor);
    ui->pageLogin->setCursor(Qt::IBeamCursor);
    ui->pageFrame->setCursor(Qt::ArrowCursor);
    ui->m_pCopyR->setCursor(Qt::IBeamCursor);
    QWidget::setCursor(Qt::IBeamCursor);
    setWindowFlags(Qt::FramelessWindowHint);
    m_pTitleBar = new TitleBar(this);
    m_pTitleBar->move(0, 0);
    connect(m_pTitleBar, SIGNAL(sendMinSig()), this, SLOT(clickMinSlot()));
    connect(m_pTitleBar, SIGNAL(sendMaxSig()), this, SLOT(clickMaxSlot()));
    connect(m_pTitleBar, SIGNAL(sendRestoreSig()), this, SLOT(clickRestoreSlot()));
    connect(m_pTitleBar, SIGNAL(sendCloseSig()), this, SLOT(clickCloseSlot()));
}
#endif

void Widget::setFontPageUi()
{
    ui->stackedWidget->setCurrentWidget(ui->pageLogin);
    ui->m_pIcon->setPixmap(QPixmap(":/images/scmms.png"));
    ui->m_pIcon->setScaledContents(true);
    ui->m_pIconIp->setPixmap(QPixmap(":/images/IP.png"));
    ui->m_pIconUser->setPixmap(QPixmap(":/images/user.png"));
    ui->m_pIconPasswd->setPixmap(QPixmap(":/images/passwd.png"));
    ui->m_pLogin->setObjectName("loginBtn");
    ui->m_pLogin->setStyleSheet("QPushButton#loginBtn{background-image: url(:/images/login_btn.png);border:none;} QPushButton#loginBtn:focus{outline:none;} "
                                "QPushButton#loginBtn:pressed{margin-top:3px;}");

    ui->m_pIp->setPlaceholderText("请输入服务器IP");
    ui->m_pUser->setPlaceholderText("请输入用户名");
    ui->m_pPasswd->setPlaceholderText("请输入用户密码");
    ui->m_pPasswd->setEchoMode(QLineEdit::Password);

    connect(ui->m_pRememberPW, SIGNAL(clicked(bool)), this, SLOT(checkRemberPW(bool)));
    connect(ui->m_pLogin, SIGNAL(clicked()), this, SLOT(clickConnectSlot()));
    connect(ui->m_pIp, SIGNAL(textEdited(QString)), this, SLOT(dealPasswd(QString)));
    connect(ui->m_pUser, SIGNAL(textEdited(QString)), this, SLOT(dealPasswd(QString)));
}

void Widget::setWebview()
{
#ifdef HIDE_FUNCTION
    qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9988");
#endif
    m_pWebView = new QWebEngineView(this);
    QHBoxLayout *layout = new QHBoxLayout(ui->frame);
    layout->setMargin(0);
    ui->frame->setLayout(layout);
    layout->addWidget(m_pWebView);
    m_pWebView->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    m_pWebView->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    m_pWebView->setAttribute(Qt::WA_DeleteOnClose);
    m_pWebPage = new WebPage();
    m_pWebView->setPage(m_pWebPage);
    m_pWebView->setContextMenuPolicy(Qt::NoContextMenu);

    m_pWebChannel = new QWebChannel(this);
    m_pBridge = new Bridge;
    m_pWebChannel->registerObject("QtWebBridge", (QObject *)m_pBridge);
    m_pWebView->page()->setWebChannel(m_pWebChannel);
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    ui->stackedWidget->setCurrentWidget(ui->pageLogin);

    m_pTimer = new QTimer(this);
    connect(m_pWebView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinishedSlot(bool)));
    connect(m_pWebView, SIGNAL(urlChanged(QUrl)), this, SLOT(openUrlSlot(QUrl)));
    connect(m_pWebView, SIGNAL(iconUrlChanged(QUrl)), this, SLOT(iconurlChangedSlot(QUrl)));
    connect(m_pBridge, SIGNAL(sendLoginResSig(QString)), this, SLOT(loginDealSlot(QString)));
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(handleLoginTimeout()));
    QWebEngineProfile *pProfile = m_pWebView->page()->profile();
    connect(pProfile, SIGNAL(downloadRequested(QWebEngineDownloadItem *)), this, SLOT(webDownloadSlot(QWebEngineDownloadItem *)));
    m_pWebView->show();
}

void xorData(QByteArray &data)
{
    QByteArray m1 = "08e3f0ffdd1995a218d174e67a9d288e";
    QByteArray m2 = "8f2f9227d39d7291f2153cc01e9be799";
    for (int i = 0; i < data.size(); ++i)
    {
        int i1 = i % m1.size();
        int i2 = i % m2.size();
        data[i] = data[i] ^ m1[i1];
        data[i] = data[i] ^ m2[i2];
    }
}

void Widget::writeConfig()
{
    QString suffix = ui->m_pIp->text() + ui->m_pUser->text();
    QSettings settings;
    QString pwk = suffix + "/passswd";
    QString chkk = suffix + "/remember";
    QByteArray pwBArr = ui->m_pPasswd->text().toLatin1();
    xorData(pwBArr);
    QString pw = pwBArr.toBase64();
    settings.setValue(pwk, pw);
    settings.setValue(chkk, true);
    settings.sync();
}

void Widget::readConfig()
{
    QString suffix = ui->m_pIp->text() + ui->m_pUser->text();
    QSettings settings;
    QString pwk = suffix + "/passswd";
    QString pw = settings.value(pwk).toString();
    if (!pw.isEmpty())
    {
        QByteArray pwBArr = pw.toLatin1();
        pw = QByteArray::fromBase64(pwBArr);
        pwBArr = pw.toLatin1();
        xorData(pwBArr);
        ui->m_pPasswd->setText(pwBArr);
        ui->m_pRememberPW->setChecked(true);
        m_bRememberPW = true;
    }
}

void Widget::removeConfig()
{
    QString suffix = ui->m_pIp->text() + ui->m_pUser->text();
    QSettings settings;
    settings.remove(suffix);
    QStringList keys = settings.allKeys();
}

#ifdef CUSTOMIZE_TITLE_BAR
#ifdef HIGH_VERSION
void Widget::clearCache()
{
    QWebEngineProfile *pProfile = m_pWebView->page()->profile();
    pProfile->clearHttpCache();
    pProfile->clearAllVisitedLinks();
    pProfile->cookieStore()->deleteAllCookies();
    pProfile->cookieStore()->deleteSessionCookies();

    QString path = pProfile->cachePath();
    printf("[%s][%d] path[%s]\n", __func__, __LINE__, path.toStdString().data());
}
#endif
#endif

/*
 * 检查参数有效性
 */
bool Widget::checkLoginParam()
{
    ui->m_pTip->setText("");
    clearLoginBtn();
    if (m_pTimer->isActive())
    {
        m_pTimer->stop();
    }

    QString url = ui->m_pIp->text();
    if (url.isEmpty())
    {
        ui->m_pTip->setText("服务器地址不能为空");
        return false;
    }

    if (ui->m_pUser->text().isEmpty())
    {
        ui->m_pTip->setText("用户名不能为空");
        return false;
    }

    if (ui->m_pPasswd->text().isEmpty())
    {
        ui->m_pTip->setText("密码不能为空");
        return false;
    }

    QRegExp ipRxp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    if (!ipRxp.exactMatch(url))
    {
        ui->m_pTip->setText("服务器地址格式错误");
        return false;
    }

    if (ui->m_pPasswd->text().length() < 8 || ui->m_pPasswd->text().length() > 20)
    {
        ui->m_pTip->setText("密码长度不能少于8位或多于20位");
        return false;
    }

    return true;
}

/*
 * 设置为登录状态，有加载动画等
 */
void Widget::setLoginBtn()
{
    ui->m_pLogin->setCursor(Qt::WaitCursor);
    ui->m_pRememberPW->setCursor(Qt::WaitCursor);
    ui->pageLogin->setCursor(Qt::WaitCursor);
    QWidget::setCursor(Qt::WaitCursor);
}

/*
 * 设置为非登录装填，去掉加载动画等
 */
void Widget::clearLoginBtn()
{
    ui->m_pLogin->clearFocus();
    ui->m_pLogin->setCursor(Qt::ArrowCursor);
    ui->m_pRememberPW->setCursor(Qt::ArrowCursor);
    ui->pageLogin->setCursor(Qt::ArrowCursor);
    QWidget::setCursor(Qt::ArrowCursor);
}

/*
 * 设置和取消节目遮挡
 */
void Widget::setBusy(bool status)
{
    printf("[%s]:%d, status:%d\n", __func__, __LINE__, status);
    m_maskWidget->setMaskVisible(status);
}

/*
 * 是否保持密码
 */
void Widget::checkRemberPW(bool status)
{
    m_bRememberPW = status;
}

/*
 * 启动socat代理进程
 */
unsigned short Widget::startSocat()
{
    if (m_pSocat == NULL)
    {
        printf("[%s]:%d, try create QProcess object.\n", __func__, __LINE__);
        m_pSocat = new QProcess(this);
    }

    if (m_pSocat == NULL)
    {
        printf("[%s]:%d, create QProcess object failed.\n", __func__, __LINE__);
        return 0;
    }

    if (m_pSocat->state() == QProcess::Running)
    {
        m_pSocat->kill();
        if (m_pSocat->waitForFinished(2000))
        {
            printf("[%s]:%d, prev socat process finished gracefully.\n", __func__, __LINE__);
        }
        else
        {
            printf("[%s]:%d, terminate prev socat process.\n", __func__, __LINE__);
            m_pSocat->terminate();
        }
    }

    QStringList envVars;
    envVars << "LD_LIBRARY_PATH=/usr/local/scmms/gui/lib64:$LD_LIBRARY_PATH";
    m_pSocat->setEnvironment(envVars);

    unsigned short listenport = 19089;
    while (listenport < 19099)
    {
        QString cmd = QString("/usr/local/scmms/gui/bin/socat -o TCP-LISTEN:%1,reuseaddr,fork TCP:%2:9089").arg(listenport).arg(ui->m_pIp->text());
        m_pSocat->start(cmd);
        if (!m_pSocat->waitForStarted())
        {
            printf("[%s]:%d create socat process failed, cmd:%s\n", __func__, __LINE__, cmd.toStdString().data());
            listenport++;
            continue;
        }
        printf("[%s]:%d create socat process succeed, cmd:%s\n", __func__, __LINE__, cmd.toStdString().data());
        return listenport;
    };

    return 0;
}

/*
 * QT登录页面，点击登录按钮时的响应函数
 */
void Widget::clickConnectSlot()
{
    // 检查参数有效性
    if (!checkLoginParam())
    {
        ui->m_pPasswd->setText("");
        // ui->m_pPin->setText("");
        return;
    }

    // task 51886, donot use http in connection of server and client
    unsigned short listenport = startSocat();
    if (!listenport)
    {
        ui->m_pTip->setText("启动代理服务失败");
        return;
    }

    setBusy(true);
    QString url = QString("http://127.0.0.1:%1").arg(listenport);
    m_qstrUrl = url;
    printf("[%s][%d] url[%s]\n", __func__, __LINE__, url.toStdString().data());
    setLoginBtn();
    m_pTimer->start(30 * 1000); // 登录计时器

    m_bFirstLogin = true;
    m_bShowWidget = false;
    // 打开前端的登录页面
    m_pWebView->stop();
    m_pWebView->load(url);
}

/**
 * 登录失败3次的时候锁定登录界面
 */
void Widget::lockLoginUI(int locktimesec)
{
	ui->m_pTip->setText("登录失败次数过多,输入框将锁定");
	ui->m_pIp->setEnabled(false);
	ui->m_pUser->setEnabled(false);
	ui->m_pPasswd->setEnabled(false);
	ui->m_pLogin->setEnabled(false);
	if(!m_pLoginTimer->isActive()){
		m_pLoginTimer->start(locktimesec); 
	}
}

/**
 * @brief 解除界面锁定 
 */
void Widget::unlockLoginUI()
{
	ui->m_pIp->setEnabled(true);
	ui->m_pUser->setEnabled(true);
	ui->m_pPasswd->setEnabled(true);
	ui->m_pLogin->setEnabled(true);
	m_pLoginTimer->stop(); //关闭计时器
	m_bLoginFailCount = 0; //重置登录次数

	rmTmpFile();
}

/**
 * @brief 删除tmp 零时文件
 */
void Widget::rmTmpFile()
{
	QString filePath = "/tmp/.LoginFailLock";
	if (QFileInfo(filePath).isFile()){
		if (QFile::remove(filePath)) {
			printf("[%s]:%d, remove tmp file success\n", __func__, __LINE__);
		}else{
			printf("[%s]:%d, remove tmp file failed\n", __func__, __LINE__);
		}
	}
}

/**
 * @brief 登录失败时的处理函数
 */
void Widget::setLoginFailed()
{
	m_bLoginFailCount++;

	//创建临时文件
	QString filePath = "/tmp/.LoginFailLock";
	QFile file(filePath);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		//设置文件权限为777 防止切换用户后， 无法删除/tmp/.LoginFailLock
		//QFileInfo fileInfo(filePath);
		bool success = file.setPermissions(
			QFile::ReadUser | QFile::WriteUser | QFile::ExeUser |  // 用户权限 rwx
			QFile::ReadGroup | QFile::WriteGroup | QFile::ExeGroup | // 组权限 rwx
			QFile::ReadOther | QFile::WriteOther | QFile::ExeOther   // 其他权限 rwx
		);
		if (success) {
			qDebug() << "权限设置成功";
		} else {
			qDebug() << "权限设置失败";
		}
		QString str_formatted = QString("LoginFailCount=%1").arg(m_bLoginFailCount);
		QTextStream out(&file);
		out << str_formatted << "\n";
		out.flush();

		file.close();
		printf("[%s][%d] 文件创建成功\n", __func__, __LINE__);
	} else {
		qCritical() << "创建失败:" << file.errorString();
	}

		if(m_bLoginFailCount >= 3){
			lockLoginUI(3 * 60 * 1000);
			QDateTime now = QDateTime::currentDateTime(); //当前时间
			QDateTime future = now.addSecs(3 * 60);
			QString future_str = future.toString("yyyy-MM-dd HH:mm:ss");
			QFile file(filePath);
			if (file.open(QIODevice::Append | QIODevice::Text)){
				QTextStream out(&file);
				QString str_formatted = QString("LoginLockTime=%1").arg(future_str);
				out << str_formatted << "\n";
				out.flush();
				file.close();
				printf("[%s][%d] 文件追加写入成功\n", __func__, __LINE__);
			}
		}
}

/**
 * @brief 从文件读取锁定信息
 */
bool Widget::getLoginFailed()
{
	QString filePath = "/tmp/.LoginFailLock";
	QDateTime parsedTime;
	if (QFileInfo(filePath).isFile()) {
		QFile file(filePath);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QTextStream in(&file);
			QString line;
			while (in.readLineInto(&line)) {
				QStringList fields = line.split('=', QString::SkipEmptyParts);
				if(fields[0].contains("LoginFailCount")){
					m_bLoginFailCount = fields[1].toInt();
					qDebug() << "LoginFailCount : " << m_bLoginFailCount << "\n";
				}
				if(fields[0].contains("LoginLockTime")){
					m_LoginLockTime = fields[1];
					parsedTime = QDateTime::fromString(m_LoginLockTime, "yyyy-MM-dd HH:mm:ss");
					qDebug() << "LoginLockTime :" << m_LoginLockTime;
				}
			}
			file.close();
		}
		return true;
	}else{
		return false;
	}
}

/*
 * 页面加载完成的回调响应函数
 */
void Widget::loadFinishedSlot(bool ok)
{
    if (!m_bFirstLogin || !ok)
        return;

    printf("[%s][%d] set user date to login page and click login button.\n", __func__, __LINE__);

    m_bFirstLogin = false;
    m_pWebView->page()->runJavaScript(QString("document.getElementById(\"login-user-input\").value=\"%1\";").arg(ui->m_pUser->text()));
    m_pWebView->page()->runJavaScript(QString("document.getElementById(\"login-password-input\").value=\"%1\";").arg(ui->m_pPasswd->text()));
    m_pWebView->page()->runJavaScript(QString("document.getElementById(\"login-pin-input\").value=\"%1\";").arg(""));
    m_pWebView->page()->runJavaScript(QString("document.getElementById(\"login-qt-input\").value=\"qtCallLogin\";"));
    m_pWebView->page()->runJavaScript("document.getElementById(\"login-button\").click();");
}

/*
 * 响应js发送的事件，比如前端登录页面的onload函数会发送消息过来
 */
void Widget::loginDealSlot(QString msg)
{
    printf("[%s][%d] loginDealSlot msg: %s\n", __func__, __LINE__, msg.toStdString().data());

    if (msg == "Login Page")
    {
        ui->stackedWidget->setCurrentWidget(ui->pageLogin);
    }
    else if (msg == "success")
    {
        m_bLoginFailCount = 0;
    }
    else
    {
        clearLoginBtn();
        if (m_pTimer->isActive())
        {
            m_pTimer->stop();
        }
        ui->m_pTip->setText(msg);
		setLoginFailed();
        setBusy(false);
    }
}

/*
 * 打开页面的回调响应，用来处理是否保持账号密码
 */
void Widget::openUrlSlot(QUrl url)
{
    if (m_bShowWidget)
        return;

    QString str = url.toString();
    printf("[%s]:%d m_qstrUrl:%s url:%s\n", __func__, __LINE__, m_qstrUrl.toStdString().data(), str.toStdString().data());
    if (!str.contains(m_qstrUrl))
        return;

    if (str.endsWith("/asset-management/list") || str.endsWith("/empty-page") || str.endsWith("/log-management/alarm-list"))
    {
        printf("[%s]:%d login, rememberPW[%d]\n", __func__, __LINE__, m_bRememberPW);
        if (m_pTimer->isActive())
        {
            m_pTimer->stop();
        }
	m_bLoginFailCount = 0;
	rmTmpFile();

        ui->m_pIp->setFocus();
        ui->stackedWidget->setCurrentWidget(ui->pageFrame);
        m_bShowWidget = true;
        clearLoginBtn();
        if (!m_bRememberPW)
        {
            ui->m_pTip->setText("");
            ui->m_pPasswd->setText("");
            removeConfig();
        }
        else
        {
            writeConfig();
        }
        setBusy(false);
    }
}

void Widget::handleLoginTimeout()
{
    printf("[%s][%d] login time out\n", __func__, __LINE__);
    if (m_pTimer->isActive())
    {
        m_pTimer->stop();
    }
    ui->m_pTip->setText("登录失败");
	setLoginFailed();
    clearLoginBtn();
    setBusy(false);
}

//三分钟后重新设置登录界面输入框
void Widget::handleLoginFailTimeout()
{
	unlockLoginUI();
	//删除 .LoginFailLock 锁定文件
	QString filePath = "/tmp/.LoginFailLock";
	if (QFileInfo(filePath).isFile()) {
		if (QFile::remove(filePath)) {
			qDebug() << "文件删除成功!";
		} else {
			qDebug() << "文件删除失败!";
		}
	}
}

/*
 * 网站图标变化时的回调，这里用来修改默认语言
 */
void Widget::iconurlChangedSlot(QUrl url)
{
    if (url.isEmpty())
        return;

    /*
    QString cmd = QString("jQuery(\"#login-password-input\").val(\"%1\");").arg(ui->m_pPasswd->text());
    printf("[%s][%d] run js: %s\n", __func__, __LINE__, cmd.toStdString().data());
    m_pWebView->page()->runJavaScript(cmd);

    m_pWebView->page()->runJavaScript(QString("document.cookie.replace(/(?:(?:^|.*;\\s*)CockpitLang\\s*=\\s*([^;]*).*$)|^.*$/, \"$1\");"), [this](const QVariant &v) {
        printf("[%s][%d] v:[%s][%d]m_qstrCurLang:[%s][%d]\n", __func__, __LINE__,
               v.toString().toStdString().data(), v.toString().isEmpty(), m_qstrCurLang.toStdString().data(), m_qstrCurLang.isEmpty());
        if (v.toString().isEmpty())
        {
        if (m_qstrCurLang.isEmpty())
        {
                m_qstrCurLang = "zh-cn";
        QString js = "var cookie = \"CockpitLang=zh-cn; path=/; expires=Sun, 16 Jul 3567 06:23:41 GMT\"; ";
        js += "document.cookie = cookie;";
        js += "window.localStorage.setItem(\"cockpit.lang\", \"zh-cn\");";
            m_pWebView->page()->runJavaScript(js);
        }
        }
        else
        {
            if (m_qstrCurLang.isEmpty())
            {
                m_qstrCurLang = v.toString();
            }
            else
            {
                if (m_qstrCurLang.toStdString() != v.toString().toStdString())
                {
                    printf("[%s][%d] modify language and reload\n", __func__, __LINE__);
                    m_qstrCurLang = v.toString();
                    QWebEngineProfile *pProfile = m_pWebView->page()->profile();
                    pProfile->clearHttpCache();
                    pProfile->clearAllVisitedLinks();
                    m_pWebView->reload();
                }
            }
        }

        printf("[%s][%d] end v:[%s][%d]m_qstrCurLang:[%s][%d]\n", __func__, __LINE__,
                v.toString().toStdString().data(), v.toString().isEmpty(), m_qstrCurLang.toStdString().data(), m_qstrCurLang.isEmpty());
    });
    */
}

void Widget::webDownloadSlot(QWebEngineDownloadItem *item)
{
    QFileInfo fileinfo = QFileInfo(item->path());
    QString suffix = "*." + fileinfo.suffix();
    QString pathsave = QFileDialog::getSaveFileName(this, "保存文件", item->path(), suffix);
    printf("[%s][%d][%s][%s]\n", __func__, __LINE__, item->path().toStdString().data(), pathsave.toStdString().data());

    if (pathsave.isEmpty())
    {
        printf("[%s][%d] pathsave is empty\n", __func__, __LINE__);
        return;
    }

    item->setPath(pathsave);
    item->accept();
}

void Widget::dealPasswd(QString text)
{
    Q_UNUSED(text);
    if (ui->m_pIp->text().isEmpty() || ui->m_pUser->text().isEmpty())
    {
        return;
    }

    QRegExp ipRxp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    if (!ipRxp.exactMatch(ui->m_pIp->text()))
    {
        return;
    }

    readConfig();
}

#ifdef CUSTOMIZE_TITLE_BAR
void Widget::clickMinSlot()
{
    showMinimized();
}

void Widget::clickMaxSlot()
{
    m_pTitleBar->setRestoreInfo(this->pos(), QSize(this->width(), this->height()));
    QRect rect = QApplication::desktop()->availableGeometry();
    QRect factRect = QRect(rect.x() - 3, rect.y() - 3, rect.width() + 6, rect.height() + 6);
    setGeometry(factRect);
}

void Widget::clickRestoreSlot()
{
    QPoint pos;
    QSize size;
    m_pTitleBar->getRestoreInfo(pos, size);
    this->setGeometry(QRect(pos, size));
}

void Widget::clickCloseSlot()
{
	close();
}

#endif
