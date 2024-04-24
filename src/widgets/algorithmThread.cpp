#include "visual.h"

#include "algorithmThread.h"

AlgorithmThread::AlgorithmThread(Visual *parent) : QThread(parent), m_vis(parent)
{
}

void AlgorithmThread::updateWithDelay()
{
    emit onUpdate();
    QThread::msleep(m_vis->m_delay);
}

Visual* AlgorithmThread::visual()
{
    return m_vis;
}

int& AlgorithmThread::delay()
{
    return m_vis->m_delay;
}
QVector<int>& AlgorithmThread::position()
{
    return m_vis->m_pos;
}
std::vector<int>& AlgorithmThread::vector()
{
    return m_vis->m_vec;
}

const int& AlgorithmThread::delay() const
{
    return m_vis->m_delay;
}
const QVector<int>& AlgorithmThread::position() const
{
    return m_vis->m_pos;
}
const std::vector<int>& AlgorithmThread::vector() const
{
    return m_vis->m_vec;
}
