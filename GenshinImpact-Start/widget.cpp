#include "widget.h"
#include "./ui_widget.h"

#include <QAudioOutput>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QStandardPaths>
#include <QSettings>
#include "videoplayer.h"
#include "detectscreenthread.h"


#define AUTO_RUN_KEY	"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"




Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon("qrc:/resource/icon.png"));
    this->setWindowTitle("原神启动！");

    // 界面始终显示在最顶层，设置属性 Qt::WindowStaysOnTopHint;
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setWindowState(Qt::WindowActive | Qt::WindowFullScreen);

    // 设置开机自启
    SetAutoRun(true);

    LoadSettings();
    if (local_video_path_.isEmpty()) {
        local_video_path_ = "qrc:/resource/start.mp4";
    }


    video_player_ = new VideoPlayer(this);

    video_player_->setUrl(QUrl::fromLocalFile(local_video_path_));

    connect(video_player_, &VideoPlayer::playEnded, this, &Widget::PlayEnd);

    ui->layoutVideo->addWidget(video_player_);



    detect_thread_ = new DetectScreenThread(this);
    connect(detect_thread_, &DetectScreenThread::screenItemDetected, this, &Widget::PlayStart);

    detect_thread_->start();
}

Widget::~Widget()
{
    delete ui;
    if (detect_thread_->isRunning()) {
        detect_thread_->ExitThread();
        detect_thread_->quit();
        detect_thread_->wait();
    }
}

void Widget::closeEvent(QCloseEvent *event)
{
    event->ignore();
}

QString Widget::GetDocumentPathDir() const
{
    auto placeholder =
            QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)
            .first();
    QString path = placeholder + "/" + QApplication::applicationName();
    QDir dir(path);
    if (!dir.exists()) {
        //只创建一级子目录，即必须保证上级目录存在
        dir.mkdir(path);
    }
    return path;
}

//设置/取消自启动
//isStart: true(开机启动)    false(开机不启动)
void Widget::SetAutoRun(bool isStart)
{
    QString application_name = QApplication::applicationName();//获取应用名称
    QSettings *settings = new QSettings(AUTO_RUN_KEY, QSettings::NativeFormat);//创建QSetting, 需要添加QSetting头文件
    if(isStart)
    {
        QString application_path = QApplication::applicationFilePath();//找到应用的目录
        settings->setValue(application_name, application_path.replace("/", "\\"));//写入注册表
    }
    else
    {
        settings->remove(application_name);		//从注册表中删除
    }
}



void Widget::CreateNewSettings(const QString& path)
{
    QSettings set(path, QSettings::IniFormat);

    set.beginGroup("configuration");
    set.setValue("video_path", "");
    set.endGroup();
}

void Widget::LoadSettings()
{
    QString path = GetDocumentPathDir() + "/settings.ini";
    if (!QFile::exists(path))
        CreateNewSettings(path);


    QSettings set(path, QSettings::IniFormat);

    set.beginGroup("configuration");

    local_video_path_ = set.value("video_path", "").toString();
    if (local_video_path_.contains("\\")) local_video_path_.replace("\\", "/");
    if (!QFile::exists(local_video_path_)) local_video_path_.clear();

    set.endGroup();
}

void Widget::PlayMedia()
{
    this->showFullScreen();
    video_player_->play();
}

void Widget::PlayStart()
{
    if (detect_thread_ != nullptr)
        detect_thread_->PauseThread();

    PlayMedia();
}

void Widget::PlayEnd()
{
    this->hide();
    video_player_->stop();

    if (detect_thread_ != nullptr)
        detect_thread_->ContinueThread();

}

