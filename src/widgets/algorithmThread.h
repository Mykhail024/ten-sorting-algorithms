#pragma once

#include <QThread>

class Visual;

class AlgorithmThread : public QThread
{
    Q_OBJECT
    public:
        AlgorithmThread(Visual *parent);
        Visual* visual();
        const int& delay() const;
        const QVector<int>& position() const;
        const std::vector<int>& vector() const; 

        void updateWithDelay();

    protected:
        int& delay();
        QVector<int>& position();
        std::vector<int>& vector(); 
        virtual void run() override = 0;

    signals:
        void onUpdate();
        void onFinish();
        void onStart();

    private:
        Visual *m_vis;
};
