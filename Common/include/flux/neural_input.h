#ifndef FLUXNEAT_NEURAL_INPUT_H
#define FLUXNEAT_NEURAL_INPUT_H

#include "prereq.h"

namespace flux {

    class EXPORTED NeuralInputId {
    private:
        std::string id;
    public:
        explicit NeuralInputId(std::string id);

        std::string GetId() { return id; }
    };

    class EXPORTED NeuralInput {
    private:
        NeuralInputId input;
        float_fl value;
    public:
        explicit NeuralInput(NeuralInputId input);

        NeuralInputId GetInput() { return input; }
        float_fl GetValue() { return value; }

        void Reset();
        void Apply(float_fl signal);
    };
};
#endif //FLUXNEAT_NEURAL_INPUT_H
