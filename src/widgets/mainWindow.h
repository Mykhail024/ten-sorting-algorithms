#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QProgressBar>
#include <QThread>
#include <QCheckBox>
#include <QTableView>
#include <QTableWidget>
#include <QItemDelegate>
#include <memory>

#include "testThread.h"

class QCustomPlot;
class VisualWidget;
class QCPLegend;
class QCPAbstractLegendItem;

class TimeColDelegate : public QItemDelegate 
{
    Q_OBJECT
    public:
        explicit TimeColDelegate(QObject *parent = nullptr);
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

};

class MainWindow : public QWidget
{
    Q_OBJECT
    public:
        MainWindow();
        ~MainWindow();

        static const int EXIT_CODE_REBOOT = -123456789;

    public slots:
        void test();
        void testEnd(std::vector<testResult> results);
        void validateInt(const QString &text);

    private slots:
        void onGraphLegendClicked(QCPLegend *legend, QCPAbstractLegendItem *legendItem, QMouseEvent *event);
        void onSourceChanged(const int &source);
        void onAlgorithmEnable(const int &state);

    private:
        friend TestThread;

        void setControlsEnabled(const bool &val);
        void setRandomSourceControlsVisible(const bool &val);
        void setTextFileSourceControlsVisible(const bool &val);

        struct InputWitchLabel {
            QLineEdit *lineEdit = nullptr;
            QLabel *label = nullptr;

            void setEnabled(const bool &val)
            {
                lineEdit->setEnabled(val);
                label->setEnabled(val);
            }

            void setVisible(const bool &val)
            {
                lineEdit->setVisible(val);
                label->setVisible(val);
            }

            void addToLayout(QLayout *layout)
            {
                layout->addWidget(label);
                layout->addWidget(lineEdit);
            }
            
            void setValidator(QValidator *validator) 
            {
                lineEdit->setValidator(validator);
            }

            InputWitchLabel(QString text, QWidget *parent)
                : lineEdit(new QLineEdit(parent))
                , label(new QLabel(text, parent))
            {}

            ~InputWitchLabel()
            {
                delete lineEdit;
                delete label;
            }
        };

        QComboBox *m_dataSource;

        QHBoxLayout *m_main;
        QVBoxLayout *m_left;
        QVBoxLayout *m_right;

        std::unique_ptr<InputWitchLabel> m_iterationsCount;
        std::unique_ptr<InputWitchLabel> m_startIt;
        std::unique_ptr<InputWitchLabel> m_endIt;
        std::unique_ptr<InputWitchLabel> m_stepIt;
        std::unique_ptr<InputWitchLabel> m_min;
        std::unique_ptr<InputWitchLabel> m_max;

        QProgressBar *m_progressBar;
        QPushButton *m_testButton;

        QCustomPlot *m_SwapsPlot = nullptr;
        QCustomPlot *m_TimesPlot = nullptr;
        QTabWidget *m_tabMain;
        QTabWidget *m_tabGraphs;
        QTabWidget *m_tabTables;

        QVector<QTableView*> m_tables;
        TimeColDelegate *m_timeColDelegate = nullptr;

        TestThread *m_testThread;

        QMap<QString, QCheckBox*> m_checkboxes;

        QPushButton *m_settingsBtn;
};
