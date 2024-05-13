#pragma once

#include <QMutex>

#include "AbstractTestModel.h"

class StandardTestModel : public AbstractTestModel
{
    public:
        StandardTestModel(QObject *parent = nullptr);

        int progressMaxVal() const override;
        int progressVal() const override;
        QString progressText() const override;
        size_t resultsCount() const override;
        _result result(const size_t &index) const override;
        void run() override;

    public slots:
        bool startTest() override;
        bool cancelTest() override;
        void terminateTest() override;

    protected slots:
        void setProgressMaxVal(const int &val);
        void setProgressVal(const int &val);
        void setProgressText(const QString &text);

    private:
        void testAlgorithm(AlgorithmHandler::Algorithm alg);
        void addResult(_result r);

        int m_progress = 0;
        int m_progress_max = 0;
        QString m_progress_text;
        std::vector<_result> m_results;

        QMutex m_mutex;
};
