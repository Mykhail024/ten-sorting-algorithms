#pragma once

#include <QThread>

#include "algorithmThread.h"

class Visual;

class BubleAlgorithmThread : public AlgorithmThread
{
    Q_OBJECT
    public:
        BubleAlgorithmThread(Visual *parent = nullptr);

    protected:
        virtual void run() override;
};
