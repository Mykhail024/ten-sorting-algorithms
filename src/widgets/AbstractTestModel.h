#pragma once

#include <QThread>

#include "AlgorithmHandler.h"

class AbstractSourceModel;

class AbstractTestModel : public QThread
{
    Q_OBJECT
    public:
        struct _result {
            QVector<double> times;
            QVector<double> swaps;
            AlgorithmHandler::Algorithm algorithm;
        };

        void run() override = 0;

        AbstractTestModel(QObject *parent = nullptr);
        virtual int progressMaxVal() const = 0;
        virtual int progressVal() const = 0;
        virtual QString progressText() const = 0;

        virtual size_t resultsCount() const = 0;
        virtual _result result(const size_t &index) const = 0;

        bool setSourceModel(AbstractSourceModel *model);
        AbstractSourceModel* sourceModel() const;

        virtual void setIterationCount(const int &count);

        virtual int iterationCount() const;

    public slots:
        virtual bool startTest() = 0;
        virtual bool cancelTest() = 0;
        virtual void terminateTest() = 0;

    signals:
        void onTestStart();
        void onTestFinished(const std::vector<_result> &results);
        void onTestCancel();
        void onTestTerminate();

        void sourceModelChanged(const AbstractSourceModel *newModel);

        void progressMaxValueUpdate(const int &max);
        void progressValueUpdate(const int &progress);
        void progressTextUpdate(const QString &text);

    private:
        using QThread::start;
        using QThread::terminate;

        AbstractSourceModel *m_sourceModel;
        int m_iteration_count;
};
