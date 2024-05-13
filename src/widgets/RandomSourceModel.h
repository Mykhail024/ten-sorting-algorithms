#pragma once

#include <QObject>
#include <QWidget>
#include <QVector>

#include <vector>

#include "AbstractSourceModel.h"

class RandomSourceWidget;

class RandomSourceModel : public AbstractSourceModel
{
    Q_OBJECT
    public:
        RandomSourceModel(QObject *parent = nullptr);
        QWidget* widget() const override;
        const QVector<double> keys() const override;
        const size_t count() const override;
        std::vector<int> array(const size_t &index) const override;
        std::vector<std::vector<int>> arrays() const override;
        QString name() const override;
        bool infoIsValid() const override;
        bool commit() override;

    private:
        friend RandomSourceWidget;

        std::vector<std::vector<int>> m_vectors;
        QVector<double> m_keys;
};
