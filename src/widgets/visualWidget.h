#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>

class Visual;

class VisualWidget : public QWidget
{
    Q_OBJECT
    public:
        VisualWidget(QWidget *parent = nullptr);

    private:
        Visual *m_visual;
        QVBoxLayout *m_layout;
        QLineEdit *m_delay;
        QLineEdit *m_count;
        QPushButton *m_start;
};
