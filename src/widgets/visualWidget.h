#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>

class Visual;
class AlgorithmThread;

class VisualWidget : public QWidget
{
    Q_OBJECT
    public:
        VisualWidget(QWidget *parent = nullptr);

    private slots:
        void enableControls(const bool &enable);

    private:

        AlgorithmThread* algorithmFromString(const QString &name);

        Visual *m_visual;
        QComboBox *m_algorithm;
        QVBoxLayout *m_layout;
        QLineEdit *m_delay;
        QLineEdit *m_count;
        QPushButton *m_start;
};
