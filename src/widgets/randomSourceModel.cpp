#include <QVBoxLayout>
#include <QLineEdit>
#include <QIntValidator>
#include <QLabel>
#include <random>
#include <tuple>

#include "randomSourceModel.h"

class RandomSourceWidget : public QWidget
{
    public:
        RandomSourceWidget()
            : QWidget()
            , m_layout(new QVBoxLayout(this))
            , m_start(new QLineEdit(this))
            , m_end(new QLineEdit(this))
            , m_step(new QLineEdit(this))
            , m_min_num(new QLineEdit(this))
            , m_max_num(new QLineEdit(this))
            , m_validator(new QIntValidator(0, INT_MAX, this))
        {
            m_start->setValidator(m_validator);
            m_end->setValidator(m_validator);
            m_step->setValidator(m_validator);
            m_min_num->setValidator(m_validator);
            m_max_num->setValidator(m_validator);

            m_layout->addWidget(new QLabel("Numbers on first array", this));
            m_layout->addWidget(m_start);
            m_layout->addWidget(new QLabel("Numbers on last array", this));
            m_layout->addWidget(m_end);
            m_layout->addWidget(new QLabel("Min number in array", this));
            m_layout->addWidget(m_min_num);
            m_layout->addWidget(new QLabel("Max number in array", this));
            m_layout->addWidget(m_max_num);
            m_layout->addWidget(new QLabel("Step", this));
            m_layout->addWidget(m_step);

            m_layout->setContentsMargins(0, 0, 0, 0);
            m_layout->setSpacing(0);
        }

        bool validateInformation()
        {
            if(m_start->text().isEmpty()   ||
               m_end->text().isEmpty()     ||
               m_step->text().isEmpty()    ||
               m_min_num->text().isEmpty() ||
               m_max_num->text().isEmpty()) return false;

            bool ok = false;

            m_start->text().toInt(&ok);
            if(!ok) return false;

            m_end->text().toInt(&ok);
            if(!ok) return false;

            m_step->text().toInt(&ok);
            if(!ok) return false;

            m_min_num->text().toInt(&ok);
            if(!ok) return false;

            m_max_num->text().toInt(&ok);
            if(!ok) return false;

            return true;
        }

        /**
         *  @return (start, end, step, min\_num, max\_num)
         */
        const std::tuple<int, int, int, int, int> getInfo()
        {
            return std::make_tuple(m_start->text().toInt(),
                                   m_end->text().toInt(),
                                   m_step->text().toInt(),
                                   m_min_num->text().toInt(),
                                   m_max_num->text().toInt());
        }

    private:
        QVBoxLayout *m_layout;
        QLineEdit *m_start;
        QLineEdit *m_end;
        QLineEdit *m_step;
        QLineEdit *m_min_num;
        QLineEdit *m_max_num;

        QIntValidator *m_validator;
};

RandomSourceModel::RandomSourceModel(QObject *parent)
    : SourceModel(parent)
    , m_widget(new RandomSourceWidget())
{
    
}

RandomSourceModel::~RandomSourceModel()
{
    delete m_widget;
}

const QWidget* RandomSourceModel::widget() const
{
    return m_widget;
}

const QVector<double> RandomSourceModel::keys() const
{
    return m_keys;
}

const size_t RandomSourceModel::count() const
{
    return m_vectors.size();
}

std::vector<int> RandomSourceModel::array(const size_t &index) const
{
    return m_vectors[index];
}

void RandomSourceModel::commit()
{
    if(!m_widget->validateInformation()) return;

    emit commitStarted();

    auto [start, end, step, m_min, m_max] = m_widget->getInfo();

    const int numVectors = ((end - start) / step) + 1;

    m_vectors.clear();
    m_vectors.resize(numVectors);

    std::default_random_engine generator(time(0));
    std::uniform_int_distribution<int> distribution(m_min, m_max);

    for(int i = 0; i < numVectors; ++i) {
        int currentSize = start + i * step;
        m_vectors[i].resize(currentSize);
        for(int j = 0; j < currentSize; ++j) {
            m_vectors[i][j] = distribution(generator);
        }
    }

    emit commitFinished();
}
