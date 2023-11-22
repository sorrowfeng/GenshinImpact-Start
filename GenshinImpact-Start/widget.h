#ifndef WIDGET_H
#define WIDGET_H

#include <QMediaPlayer>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
class VideoPlayer;
class DetectScreenThread;
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void PlayMedia();


    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event) override;

protected:
    void PlayStart();
    void PlayEnd();

private:
    Ui::Widget *ui;

    VideoPlayer *video_player_ = nullptr;
    DetectScreenThread *detect_thread_ = nullptr;
};
#endif // WIDGET_H
