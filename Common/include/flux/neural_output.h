//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_NEURAL_OUTPUT_H
#define FLUXNEAT_NEURAL_OUTPUT_H

#include <flux/prereq.h>

namespace flux {

    class FLUX_API NeuralOutputId
    {
    private:
        std::string id;
    public:
        NeuralOutputId() = default;
        explicit NeuralOutputId(std::string id);

        std::string GetId() const { return id; }

        bool operator <(const NeuralOutputId &other) const { return id < other.id; }
    };

    class FLUX_API NeuralOutput
    {
    private:
        NeuralOutputId output;
        float_fl value;
    public:
        NeuralOutput() = default;
        explicit NeuralOutput(NeuralOutputId output);
        NeuralOutput(NeuralOutputId output, float_fl value);

        NeuralOutputId GetOutput() const { return output; }
        float_fl GetValue() const { return value; }

        void Reset();
        void Apply(float_fl signal);
    };

} // namespace flux
#endif //FLUXNEAT_NEURAL_OUTPUT_H
