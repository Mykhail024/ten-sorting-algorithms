
#include "AbstractSourceModel.h"


const bool AbstractSourceModel::isCommitInProccess() const {
    return m_commit_in_proccess;
}

AbstractSourceModel::AbstractSourceModel(QObject *parent) : QObject(parent)
{
    connect(this, &AbstractSourceModel::commitStarted, [&]{ m_commit_in_proccess = true; });
    connect(this, &AbstractSourceModel::commitFinished, [&]{ m_commit_in_proccess = false; });
}
