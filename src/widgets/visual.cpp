#include <QThread>
#include <QPainter>
#include <algorithm>

#include "algorithmThread.h"

#include "visual.h"

Visual::Visual(QWidget *parent)
    : QWidget(parent)
    , m_color("#202020")
    , m_active_color("#00f1f4")
    , m_pos({-1, -1})
{
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

bool Visual::start()
{
    if(m_thread == nullptr) {
        qDebug() << "Null Thread!";
        return false;
    }
    m_thread->start();
    return true;
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


    QPainter painter(this);

    QFontMetrics fm(painter.font());
    QSize maxTextSize = fm.size(Qt::TextSingleLine, QString::number(m_max));

    const int margins = 8;
    const int top = margins;
    const int left = margins;
    const int right = width() - margins;
    const int spacing = width() * 0.003;
    const size_t count = m_vec.size();
    const int diff = m_max - m_min;
    const int rectWidth = (right - left - (count - 1) * spacing) / count;
    const int bottom_text =  height() - margins;
    const int bottom = maxTextSize.height() <= rectWidth ? bottom_text - maxTextSize.width() : bottom_text;


    int normalized, x;
    for(size_t i = 0; i < count; ++i) {
        normalized = (m_vec[i] - (m_min-(m_max*0.01))) * (bottom - top) / diff;
        x = left + i * (rectWidth + spacing);

        painter.fillRect(x, qMax(bottom - normalized, top), rectWidth, qMin(normalized, bottom - top), m_pos.contains(i) ? m_active_color : m_color);

        if(maxTextSize.height() <= rectWidth) {
            painter.save();

            painter.translate(x + static_cast<qreal>(rectWidth) / 2, bottom_text + margins);
            painter.rotate(-90);

            painter.drawText(0, 0, QString::number(m_vec[i]));

            painter.restore();
        }
    }
}

void Visual::setThread(AlgorithmThread *thread)
{
    if(thread == nullptr) {
        qDebug() << "Null Thread!";
    } else {
        if(m_thread != nullptr) {
            if(m_thread->isRunning()) {
                m_thread->terminate();
            }
            delete m_thread;
        }
        m_thread = thread;
        connect(m_thread, &AlgorithmThread::onUpdate, this, [&]{
                update();
                });
        connect(m_thread, &AlgorithmThread::onFinish, this, [&]{
                emit onFinish();
                });
    }
}

AlgorithmThread* Visual::thread()
{
    return m_thread;
}
