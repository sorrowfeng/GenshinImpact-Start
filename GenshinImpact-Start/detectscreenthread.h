#ifndef DETECTSCREENTHREAD_H
#define DETECTSCREENTHREAD_H

#include <QThread>
#include <QObject>
#include <QPixmap>

class DetectScreenThread : public QThread
{
    Q_OBJECT
public:
    explicit DetectScreenThread(QObject *parent = nullptr);
    void ExitThread();
    void PauseThread();
    void ContinueThread();


    // QThread interface
protected:
    virtual void run() override;

private:
    void DetectScreen();
    bool IsWhitePixelPercentage(const QPixmap &pixmap, float percentage);

signals:
    void screenItemDetected();


private:
    bool is_exit_ = false;
    bool is_stop_ = false;
};

#endif // DETECTSCREENTHREAD_H
