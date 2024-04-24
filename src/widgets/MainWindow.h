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
        void testEnd(testResults results);
        void validateInt(const QString &text);

    private slots:
        void onGraphLegendClicked(QCPLegend *legend, QCPAbstractLegendItem *legendItem, QMouseEvent *event);
        void onSourceChanged(const int &source);

    private:
        friend TestThread;

        void setControlsEnabled(const bool &val);

        QComboBox *m_dataSource;

        QHBoxLayout *m_main;
        QVBoxLayout *m_left;
        QVBoxLayout *m_right;

        QPushButton *m_testButton;
        QLineEdit *m_iterationsCount;
        QLineEdit *m_startIt;
        QLineEdit *m_endIt;
        QLineEdit *m_stepIt;
        QLineEdit *m_min;
        QLineEdit *m_max;
        QProgressBar *m_progressBar;

        VisualWidget *m_vis;
        QPushButton *m_visBtn;

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
