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
#include <QScrollArea>
#include <QButtonGroup>
#include <QGroupBox>

#include "AbstractTestModel.h"

class GraphView;
class QCPLegend;
class QCPAbstractLegendItem;
class TableView;
class AbstractSourceModel;
class StandardTestModel;

class MainWindow : public QWidget
{
    Q_OBJECT
    public:
        static const int EXIT_CODE_REBOOT = -123456789;

        MainWindow();
        ~MainWindow();

    public slots:
        void startTest();

    private slots:
        void sourceChanged(const int &source);
        void checkboxChecked(QAbstractButton *button, bool checked); 

        void testFinished(const std::vector<AbstractTestModel::_result> &results);

        void loadSources();
        void createCheckBoxes();

    private:
        void setControlsEnabled(const bool &val);

        StandardTestModel *m_test_model;

        QVBoxLayout *m_sourceContainer;
        QGridLayout *m_groupBoxLayout;
        QGroupBox *m_groupBox;
        QButtonGroup *m_btnGroup;
        QScrollArea *m_scroll;

        QComboBox *m_dataSource;

        QHBoxLayout *m_main;
        QVBoxLayout *m_left;
        QVBoxLayout *m_right;

        QLineEdit *m_iterationsCount;

        QProgressBar *m_progressBar;
        QPushButton *m_testButton;

        GraphView *m_SwapsPlot = nullptr;
        GraphView *m_TimesPlot = nullptr;

        QTabWidget *m_tabMain;

        QTabWidget *m_tabGraphs;
        QTabWidget *m_tabTables;

        QVector<TableView*> m_tables;

        QPushButton *m_settingsBtn;
};
