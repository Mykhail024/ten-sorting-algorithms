#pragma once

#include <QObject>
#include <QWidget>
#include <QVector>
#include <vector>

class SourceModel : public QObject
{
    Q_OBJECT
    public:
        SourceModel(QObject *parent = nullptr)
        {
            connect(this, &SourceModel::commitStarted, [&]{ m_commit_in_proccess = true; });
            connect(this, &SourceModel::commitFinished, [&]{ m_commit_in_proccess = false; });
        }
        ~SourceModel(){}
        virtual const QWidget* widget() const = 0;
        virtual const QVector<double> keys() const = 0;
        virtual const size_t count() const = 0;
        virtual std::vector<int> array(const size_t &index) const = 0;
        virtual void commit() = 0;

        const bool commitInProccess() const { return m_commit_in_proccess; }
    signals:
        void commitStarted();
        void commitFinished();

    private:
        bool m_commit_in_proccess = false;
};
