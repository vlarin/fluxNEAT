//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_NEURAL_INPUT_H
#define FLUXNEAT_NEURAL_INPUT_H

#include <flux/prereq.h>

namespace flux {

    class FLUX_API NeuralInputId
    {
    private:
        std::string id;
    public:
        NeuralInputId() = default;
        explicit NeuralInputId(std::string id);

        std::string GetId() const { return id; }

        bool operator <(const NeuralInputId &other) const { return id < other.id; }
    };

    class FLUX_API NeuralInput
    {
    private:
        NeuralInputId input;
        float_fl value;
    public:
        NeuralInput() = default;
        explicit NeuralInput(NeuralInputId input);
        NeuralInput(NeuralInputId input, float_fl value);

        NeuralInputId GetInputId() const { return input; }
        float_fl GetValue() const { return value; }

        void Reset();
        void Apply(float_fl signal);
    };
};
#endif //FLUXNEAT_NEURAL_INPUT_H
