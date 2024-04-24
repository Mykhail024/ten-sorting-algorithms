#include "visual.h"

#include "bubleAlgorithmThread.h"

BubleAlgorithmThread::BubleAlgorithmThread(Visual *parent) : AlgorithmThread(parent)
{
}

void BubleAlgorithmThread::run()
{
    emit onStart();
    auto &_vec = vector();
    auto &_pos = position();
    const size_t size = _vec.size();

    if(_vec.size() > 1) {
        if(_pos.size() != 2) {
            _pos.resize(2);
        }
        for(size_t i = 0; i < size-1; ++i) {
            _pos[0] = i;
            bool swapped = false;
            for(size_t j = 0; j < size - i - 1; ++j) {
                if(_vec[j] > _vec[j+1]) {
                    std::swap(_vec[j], _vec[j+1]);
                    swapped = true;
                }
                _pos[1] = j+1;
                updateWithDelay();
            }
            if(!swapped) break;
        }
        _pos.clear();
        onUpdate();
    }

    emit onFinish();
}

