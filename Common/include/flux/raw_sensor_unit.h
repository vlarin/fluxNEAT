//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_RAW_SENSOR_UNIT_H
#define FLUXNEAT_RAW_SENSOR_UNIT_H

#include "prereq.h"
#include "neural_input.h"

#include <set>

namespace flux {

    /**
     * Input Augmentation interface acts like an additional sensor in the cortex
     */
    class FLUX_API IRawSensorUnit
    {
    public:
        virtual std::set<NeuralInputId> GetInputIds() const = 0;

        virtual std::vector<NeuralInput> Fetch() const = 0;
    };

}

#endif //FLUXNEAT_RAW_SENSOR_UNIT_H
