#pragma once

#include <QObject>
#include <QMutex>

#include <tuple>

class AlgorithmHandler : public QObject
{
    Q_OBJECT
    public:
        typedef const size_t (*AlgorithmFunction)(std::vector<int>&);

        struct Algorithm {
            AlgorithmFunction func;
            QString name;
            bool operator==(const Algorithm &other)
            {
                return other.func == this->func && other.name == this->name;
            }
        };

        Algorithm algorithm(const size_t &index) const;
        size_t algorithmsCount() const;
        size_t enabledCount() const;

        bool registerAlgorithm(AlgorithmFunction func, const QString &name);
        bool unregisterAlgorithm(const size_t &index);

        std::vector<Algorithm> getEnabled() const;
        void setEnabled(const size_t &index, const bool &enabled);
        void setEnabled(const Algorithm &algorithm, const bool &enabled);
        void setEnabled(const QString &name, const bool &enabled);

        bool isEnabled(const size_t &index) const;
    signals:
        void onNewAlgorithmRegister(Algorithm algorithm);
        void onNewAlgorithmUnregister(Algorithm algorithm);
        void onAlgorithmEnable(Algorithm algorithm);
        void onAlgorithmDisable(Algorithm algorithm);

    private:
        enum ENABLED_ALGORITHM {
            ALGORITHM = 0,
            IS_ENABLED = 1
        };

        friend AlgorithmHandler& algorithmHandler();
        AlgorithmHandler();

        std::vector<std::tuple<Algorithm, bool>> m_algorithms;

        QMutex m_mutex;
};

AlgorithmHandler& algorithmHandler();
