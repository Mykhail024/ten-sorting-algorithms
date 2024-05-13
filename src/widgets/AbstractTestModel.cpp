
#include "AbstractTestModel.h"

AbstractTestModel::AbstractTestModel(QObject *parent) : QThread(parent)
{
}

bool AbstractTestModel::setSourceModel(AbstractSourceModel *model)
{
    if(model == nullptr) {
        Log_Warning("Model is nullptr!");
        return false;
    }

    m_sourceModel = model;
    emit sourceModelChanged(m_sourceModel);

    return true;
}
AbstractSourceModel* AbstractTestModel::sourceModel() const { return m_sourceModel; }

void AbstractTestModel::setIterationCount(const int &count)
{
    m_iteration_count = count;
}

int AbstractTestModel::iterationCount() const
{
    return m_iteration_count;
}
