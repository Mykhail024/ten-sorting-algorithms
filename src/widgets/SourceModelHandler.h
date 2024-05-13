#pragma once

#include <QObject>
#include <QMutex>

class AbstractSourceModel;

class SourceModelHandler : public QObject
{
    Q_OBJECT
    public:
        size_t count() const;
        AbstractSourceModel* model(const size_t &index) const;

        bool registerModel(AbstractSourceModel *model);
        bool unregisterModel(const size_t &index);

    signals:
        void onModelRegistered(AbstractSourceModel *model);
        void onModelUnregistered(const size_t &index);

    private:
        friend SourceModelHandler& sourceModelHandler();
        SourceModelHandler(QObject *parent = nullptr);

        std::vector<AbstractSourceModel*> m_models;
};

SourceModelHandler& sourceModelHandler();
