#include "detectscreenthread.h"

#include <QGuiApplication>
#include <QScreen>
#include <QPixmap>
#include <QDebug>

DetectScreenThread::DetectScreenThread(QObject *parent)
    : QThread(parent)
{

}

void DetectScreenThread::ExitThread()
{
    is_exit_ = true;
}

void DetectScreenThread::PauseThread()
{
    is_stop_ = true;
}

void DetectScreenThread::ContinueThread()
{
    is_stop_ = false;
}

void DetectScreenThread::run()
{
    while (!is_exit_)
    {
        QThread::msleep(100);
        if (is_stop_) continue;

        DetectScreen();
    }
}

void DetectScreenThread::DetectScreen()
{
    // 获取当前屏幕
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) {
        qDebug() << "无法获取屏幕";
        return;
    }

    // 获取屏幕像素数据
    QPixmap screenImage = screen->grabWindow(0);
    if (screenImage.isNull()) {
        qDebug() << "无法获取屏幕图像";
        return;
    }

    bool can_start = IsWhitePixelPercentage(screenImage, 0.9);
    if (can_start) emit screenItemDetected();
}

bool DetectScreenThread::IsWhitePixelPercentage(const QPixmap &pixmap, float percentage)
{
    int whiteCount = 0;
    int totalCount = pixmap.width() * pixmap.height();

    for (int x = 0; x < pixmap.width(); ++x) {
        for (int y = 0; y < pixmap.height(); ++y) {
            QColor color = pixmap.toImage().pixelColor(x, y);
            if (color.red() == 255 && color.green() == 255 && color.blue() == 255) {
                whiteCount++;
            }
        }
    }

    return (whiteCount / static_cast<float>(totalCount)) >= percentage;
}
