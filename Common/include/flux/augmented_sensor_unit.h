#ifndef FLUXNEAT_AUGMENTED_SENSOR_UNIT_H
#define FLUXNEAT_AUGMENTED_SENSOR_UNIT_H

#include "prereq.h"
#include "neural_input.h"

#include <set>

namespace flux {

    /**
     * Input Augmentation interface acts like an additional sensor in the cortex
     */
    class EXPORTED IAugmentedSensorUnit
    {
    public:
        virtual std::set<NeuralInputId> GetInputIds() const = 0;
        virtual std::set<NeuralInputId> GetAugmentedInputIds() const = 0;

        virtual std::vector<NeuralInput> ApplyAugmentation(std::vector<NeuralInput> rawInputs) const = 0;
    };

}

#endif //FLUXNEAT_AUGMENTED_SENSOR_UNIT_H
