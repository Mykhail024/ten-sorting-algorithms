#include <QVBoxLayout>
#include <QLineEdit>
#include <QIntValidator>
#include <QLabel>
#include <random>
#include <tuple>

#include "AbstractSourceModel.h"
#include "RandomSourceModel.h"

static int *_start = new int(0);
static int *_end = new int(0);
static int *_step = new int(1);
static int *_min_num = new int(0);
static int *_max_num = new int(0);
static bool *_is_valid = new bool(false);

class RandomSourceWidget : public QWidget
{
    Q_OBJECT
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

            m_layout->addWidget(new QLabel(tr("Numbers on first array"), this));
            m_layout->addWidget(m_start);
            m_layout->addWidget(new QLabel(tr("Numbers on last array"), this));
            m_layout->addWidget(m_end);
            m_layout->addWidget(new QLabel(tr("Min number in array"), this));
            m_layout->addWidget(m_min_num);
            m_layout->addWidget(new QLabel(tr("Max number in array"), this));
            m_layout->addWidget(m_max_num);
            m_layout->addWidget(new QLabel(tr("Step"), this));
            m_layout->addWidget(m_step);

            m_layout->setContentsMargins(0, 0, 0, 0);
            m_layout->setSpacing(0);

            connect(m_start, &QLineEdit::textChanged, this, [&](const QString &text){
                        if(!text.isEmpty()) {
                            bool v = false;
                            int val = text.toInt(&v);
                            if(v) {
                                *_start = val;
                                *_is_valid = true;
                                return;
                            }
                        }
                        *_is_valid = false;
                    });
            connect(m_end, &QLineEdit::textChanged, this, [&](const QString &text){
                        if(!text.isEmpty()) {
                            bool v = false;
                            int val = text.toInt(&v);
                            if(v) {
                                *_end = val;
                                *_is_valid = true;
                                return;
                            }
                        }
                        *_is_valid = false;
                    });
            connect(m_step, &QLineEdit::textChanged, this, [&](const QString &text){
                        if(!text.isEmpty()) {
                            bool v = false;
                            int val = text.toInt(&v);
                            if(v) {
                                *_step = val;
                                *_is_valid = true;
                                return;
                            }
                        }
                        *_is_valid = false;
                    });
            connect(m_min_num, &QLineEdit::textChanged, this, [&](const QString &text){
                        if(!text.isEmpty()) {
                            bool v = false;
                            int val = text.toInt(&v);
                            if(v) {
                                *_min_num = val;
                                *_is_valid = true;
                                return;
                            } 
                        }
                        *_is_valid = false;
                    });
            connect(m_max_num, &QLineEdit::textChanged, this, [&](const QString &text){
                        if(!text.isEmpty()) {
                            bool v = false;
                            int val = text.toInt(&v);
                            if(v) {
                                *_max_num = val;
                                *_is_valid = true;
                                return;
                            }
                        }
                        *_is_valid = false;
                    });
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
         *  @return [start, end, step, min_num, max_num]
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
    : AbstractSourceModel(parent)
{
    
}

QWidget* RandomSourceModel::widget() const
{
    return new RandomSourceWidget();
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

std::vector<std::vector<int>> RandomSourceModel::arrays() const
{
    return m_vectors;
}

bool RandomSourceModel::commit()
{
    if(!_is_valid) {
        Log_Warning("RandomSourceModel: info not valid");
        return false;
    } 
    emit commitStarted();

    const int numVectors = ((*_end - *_start) / *_step) + 1;

    m_keys.clear();
    m_vectors.clear();
    m_vectors.resize(numVectors);

    std::default_random_engine generator(time(0));

    for(int i = 0; i < numVectors; ++i) {
        int currentSize = (*_start) + i * (*_step);
        m_vectors[i].resize(currentSize);
        m_keys.push_back(currentSize);
        std::uniform_int_distribution<int> distribution(*_min_num, *_max_num);
        for(int j = 0; j < currentSize; ++j) {
            m_vectors[i][j] = distribution(generator);
        }
    }

    emit commitFinished();
    return true;
}


bool RandomSourceModel::infoIsValid() const
{
    return _is_valid;
}


QString RandomSourceModel::name() const
{
    return tr("Random generated");
}

#include "RandomSourceModel.moc"
