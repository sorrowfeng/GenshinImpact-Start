#include "widget.h"
#include "./ui_widget.h"

#include <QAudioOutput>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QStandardPaths>
#include "videoplayer.h"
#include "detectscreenthread.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon("qrc:/resource/icon.png"));

    // 由于存在类似QQ这样界面始终显示在最顶层，设置属性 Qt::WindowStaysOnTopHint;
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setWindowState(Qt::WindowActive | Qt::WindowFullScreen);
//    this->setAttribute(Qt::WA_TranslucentBackground);

    video_player_ = new VideoPlayer(this);

    video_player_->setUrl(QUrl::fromLocalFile("qrc:/resource/start.mp4"));

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

