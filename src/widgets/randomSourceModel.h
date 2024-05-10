#pragma once

#include <QObject>
#include <QWidget>
#include <QVector>
#include <vector>

#include "sourceModel.h"

class RandomSourceWidget;

class RandomSourceModel : public SourceModel
{
    public:
        RandomSourceModel(QObject *parent = nullptr);
        ~RandomSourceModel();
        const QWidget* widget() const override;
        const QVector<double> keys() const override;
        const size_t count() const override;
        std::vector<int> array(const size_t &index) const override;
        void commit() override;

    private:
        std::vector<std::vector<int>> m_vectors;
        QVector<double> m_keys;

        RandomSourceWidget *m_widget;
};
