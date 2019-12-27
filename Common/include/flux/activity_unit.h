#ifndef FLUXNEAT_ACTIVITY_UNIT_H
#define FLUXNEAT_ACTIVITY_UNIT_H

#include <set>

#include "prereq.h"
#include "neural_input.h"
#include "neural_output.h"

namespace flux {

    class EXPORTED IActivityUnit {
    public:
        virtual std::set<flux::NeuralInputId> GetInputIds() = 0;
        virtual std::set<flux::NeuralOutputId> GetOutputIds() = 0;

        virtual std::vector<NeuralOutput> Activate(std::vector<NeuralInput> inputs) = 0;
    };
}

#endif //FLUXNEAT_ACTIVITY_UNIT_H
