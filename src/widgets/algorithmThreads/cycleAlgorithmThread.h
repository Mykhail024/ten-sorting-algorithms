#pragma once

#include <QThread>

#include "algorithmThread.h"

class Visual;

class CycleAlgorithmThread : public AlgorithmThread
{
    Q_OBJECT
    public:
        CycleAlgorithmThread(Visual *parent = nullptr);

    protected:
        virtual void run() override;
};
