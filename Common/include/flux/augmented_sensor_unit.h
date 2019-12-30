//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_AUGMENTED_SENSOR_UNIT_H
#define FLUXNEAT_AUGMENTED_SENSOR_UNIT_H

#include <flux/prereq.h>
#include <flux/neural_input.h>

#include <set>

namespace flux {

    /**
     * Input Augmentation interface acts like an additional sensor in the cortex
     */
    class FLUX_API IAugmentedSensorUnit
    {
    public:
        virtual std::set<NeuralInputId> GetInputIds() const = 0;
        virtual std::set<NeuralInputId> GetAugmentedInputIds() const = 0;

        virtual std::vector<NeuralInput> ApplyAugmentation(const std::vector<NeuralInput> &rawInputs) const = 0;
    };

}

#endif //FLUXNEAT_AUGMENTED_SENSOR_UNIT_H
