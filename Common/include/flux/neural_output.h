#ifndef FLUXNEAT_NEURAL_OUTPUT_H
#define FLUXNEAT_NEURAL_OUTPUT_H

#include "prereq.h"

namespace flux {

    class EXPORTED NeuralOutputId
    {
    private:
        std::string id;
    public:
        explicit NeuralOutputId(std::string id);

        std::string GetId() const { return id; }
    };

    class EXPORTED NeuralOutput
    {
    private:
        NeuralOutputId output;
        float_fl value;
    public:
        explicit NeuralOutput(NeuralOutputId output);

        NeuralOutputId GetOutput() const { return output; }
        float_fl GetValue() const { return value; }

        void Reset();
        void Apply(float_fl signal);
    };

} // namespace flux
#endif //FLUXNEAT_NEURAL_OUTPUT_H
