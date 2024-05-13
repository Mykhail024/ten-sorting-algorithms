#include "AbstractSourceModel.h"

#include "RandomSourceModel.h"

#include "SourceModelHandler.h"

SourceModelHandler::SourceModelHandler(QObject *parent) : QObject(parent), m_models({new RandomSourceModel(this)})
{
}

size_t SourceModelHandler::count() const
{
    return m_models.size();
}

bool SourceModelHandler::registerModel(AbstractSourceModel *model)
{
    if(model == nullptr) {
        Log_Warning("Source model: " + model->name() + " is nullptr");
        return false;
    }

    m_models.push_back(model);
    emit onModelRegistered(model);

    return true;
}

bool SourceModelHandler::unregisterModel(const size_t &index)
{
    if(index > m_models.size()) {
        Log_Warning("No model on index: " + QString::number(index));
        return false;
    }

    m_models.erase(m_models.begin() + index);
    emit onModelUnregistered(index);

    return true;
}

AbstractSourceModel* SourceModelHandler::model(const size_t &index) const
{
    if(index >= m_models.size()) {
        Log_Warning("No model index: " + QString::number(index));
        return nullptr;
    }
    return m_models[index];
}

SourceModelHandler& sourceModelHandler()
{
    static SourceModelHandler INSTANCE;
    return INSTANCE;
}
