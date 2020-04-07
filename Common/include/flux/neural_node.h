//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_NEURAL_NODE_H
#define FLUXNEAT_NEURAL_NODE_H

#include <flux/prereq.h>

namespace flux {

    class FLUX_API NeuralNodeId
    {
    private:
        std::string id;
    public:
        NeuralNodeId() = default;
        explicit NeuralNodeId(std::string id);

        std::string GetId() const { return id; }

        bool operator <(const NeuralNodeId &other) const { return id < other.id; }

        bool operator ==(const NeuralNodeId &other) const { return id == other.id; }
        bool operator !=(const NeuralNodeId &other) const { return id != other.id; }
    };

    class FLUX_API NeuralNode
    {
    private:
        NeuralNodeId input;
        float_fl value;
    public:
        NeuralNode() = default;
        explicit NeuralNode(NeuralNodeId input);
        NeuralNode(NeuralNodeId input, float_fl value);

        NeuralNodeId GetNodeId() const { return input; }
        float_fl GetValue() const { return value; }

        void Reset();
        void Apply(float_fl signal);
    };
};
#endif //FLUXNEAT_NEURAL_NODE_H
