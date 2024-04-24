#pragma once

#include <QWidget>
#include <QThread>

class AlgorithmThread;

class Visual : public QWidget
{
    Q_OBJECT
    public:
        Visual(QWidget *parent = nullptr);
        void setVector(std::vector<int> vec);
        void setActiveColor(const QColor &color);
        void setColor(const QColor &color);
        bool start();
        void setDelay(const int &delay);
        void setThread(AlgorithmThread *thread);
        AlgorithmThread* thread();
        std::vector<int> getResult() const;

    signals:
        void onFinish();

    private:
        void paintEvent(QPaintEvent *e) override;

    private:
        friend AlgorithmThread;
        AlgorithmThread *m_thread = nullptr;
        QColor m_color, m_active_color;
        int m_min, m_max;
        std::vector<int> m_vec;
        QVector<int> m_pos;
        int m_delay = 10;
};
