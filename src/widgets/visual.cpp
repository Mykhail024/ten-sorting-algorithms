#include <QThread>
#include <QPainter>
#include <algorithm>

#include "visual.h"


AlgorithmThread::AlgorithmThread(Visual *parent) : QThread(parent), m_vis(parent)
{
}

void AlgorithmThread::run()
{
    emit onStart();
    auto &vec = m_vis->m_vec;
    auto &pos = m_vis->m_pos;
    auto &delay = m_vis->m_delay;
    const size_t size = vec.size();

    if(vec.size() > 1) {
        if(pos.size() != 2) {
            pos.resize(2);
        }
        for(size_t i = 0; i < size-1; ++i) {
            pos[0] = i;
            bool swapped = false;
            for(size_t j = 0; j < size - i - 1; ++j) {
                pos[1] = j;
                emit onUpdate();
                if(vec[j] > vec[j+1]) {
                    std::swap(vec[j], vec[j+1]);
                    swapped = true;
                }
                QThread::msleep(delay);
            }
            if(!swapped) break;
        }
        pos.clear();
        emit onUpdate();
    }

    emit onFinish();
}

Visual::Visual(QWidget *parent)
    : QWidget(parent)
    , m_thread(new AlgorithmThread(this))
    , m_color("#242424")
    , m_active_color("#00f1f4")
    , m_pos({-1, -1})
{
    connect(m_thread, &AlgorithmThread::onUpdate, this, [&]{
                update();
            });
    connect(m_thread, &AlgorithmThread::onFinish, this, [&]{
                emit onFinish();
            });
}

void Visual::setDelay(const int &delay)
{
    m_delay = delay;
}

void Visual::setVector(std::vector<int> vec)
{
    m_vec = vec;
    auto [min, max] = std::minmax_element(m_vec.begin(), m_vec.end());
    m_min = *min;
    m_max = *max;
}

void Visual::start()
{
    m_thread->start();
}

std::vector<int> Visual::getResult() const
{
    return m_vec;
}

void Visual::setActiveColor(const QColor &color)
{
    m_active_color = color;
}

void Visual::setColor(const QColor &color)
{
    m_color = color;
}

void Visual::paintEvent(QPaintEvent *e)
{
    if(m_vec.size() == 0) return;

    const int margins = 8;
    const int bottom = height() - margins;
    const int top = margins;
    const int left = margins;
    const int right = width() - margins;
    const int spacing = width() * 0.003;
    const size_t count = m_vec.size();
    const int diff = m_max - m_min;
    const int rectWidth = (right - left - (count - 1) * spacing) / count;

    QPainter painter(this);

    int normalized, x;
    for(size_t i = 0; i < count; ++i) {
        normalized = (m_vec[i] - (m_min-1)) * (bottom - top) / diff;
        x = left + i * (rectWidth + spacing);

        painter.fillRect(x, qMax(bottom - normalized, top), rectWidth, qMin(normalized, bottom - top), m_pos.contains(i) ? m_active_color : m_color);
    }
}
