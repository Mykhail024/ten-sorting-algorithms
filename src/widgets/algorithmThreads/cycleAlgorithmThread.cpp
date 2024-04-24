#include "visual.h"

#include "cycleAlgorithmThread.h"

CycleAlgorithmThread::CycleAlgorithmThread(Visual *parent) : AlgorithmThread(parent)
{
}

void CycleAlgorithmThread::run()
{
    emit onStart();
    auto &_vec = vector();
    auto &_pos = position();
    const size_t size = _vec.size();

    if(_vec.size() > 1) {
        if(_pos.size() != 2) {
            _pos.resize(2);
        }
        for(size_t i = 0; i < size; ++i) {
            int val = _vec[i];
            size_t pos = i;
            _pos[0] = pos;
            updateWithDelay();

            for(size_t j = i+1; j < _vec.size(); ++j) {
                if(_vec[j] < val) {
                    _pos[1] = ++pos;
                    updateWithDelay();
                }
            }

            if(pos == i) {
                continue;
            }

            while(val == _vec[pos]) {
                _pos[1] = ++pos;
                updateWithDelay();
            }

            std::swap(val, _vec[pos]);
            updateWithDelay();

            while(pos != i) {
                pos = i;
                _pos[1] = pos;
                updateWithDelay();
                for(size_t k = i+1; k < size; ++k) {
                    if(_vec[k] < val) {
                        _pos[1] = ++pos;
                        updateWithDelay();
                    }
                }
                while(val == _vec[pos]) {
                    _pos[1] = ++pos;
                    updateWithDelay();
                }
                std::swap(val, _vec[pos]);
                updateWithDelay();
            }
        }
        _pos.clear();
    }

    emit onFinish();
}

