#pragma once

#include <QObject>
#include <QWidget>
#include <QVector>
#include <vector>

class AbstractSourceModel : public QObject
{
    Q_OBJECT
    public:
        AbstractSourceModel(QObject *parent = nullptr);
        ~AbstractSourceModel(){}
        virtual QWidget* widget() const = 0;
        virtual const QVector<double> keys() const = 0;
        virtual const size_t count() const = 0;
        virtual std::vector<int> array(const size_t &index) const = 0;
        virtual std::vector<std::vector<int>> arrays() const = 0;
        virtual QString name() const = 0;
        virtual bool infoIsValid() const = 0;
        virtual bool commit() = 0;

        const bool isCommitInProccess() const;

    signals:
        void commitStarted();
        void commitFinished();

    private:
        bool m_commit_in_proccess = false;
};
