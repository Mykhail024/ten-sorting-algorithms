#pragma once

#include <QWidget>
#include <QThread>

class Visual;

class AlgorithmThread : public QThread
{
    Q_OBJECT
    public:
        AlgorithmThread(Visual *parent = nullptr);

    protected:
        virtual void run() override;

    signals:
        void onUpdate();
        void onFinish();
        void onStart();

    private:
        Visual *m_vis;
};

class Visual : public QWidget
{
    Q_OBJECT
    public:
        Visual(QWidget *parent = nullptr);
        void setVector(std::vector<int> vec);
        void setActiveColor(const QColor &color);
        void setColor(const QColor &color);
        void start();
        void setDelay(const int &delay);
        std::vector<int> getResult() const;

    signals:
        void onFinish();

    private:
        void paintEvent(QPaintEvent *e) override;

    private:
        friend AlgorithmThread;
        AlgorithmThread *m_thread;
        QColor m_color, m_active_color;
        int m_min, m_max;
        std::vector<int> m_vec;
        QVector<int> m_pos;
        int m_delay = 10;
};
