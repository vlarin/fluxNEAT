//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_NEAT_ENTITY_DESCRIPTOR_H
#define FLUXNEAT_NEAT_ENTITY_DESCRIPTOR_H

#include <flux/common.h>

#include <utility>

namespace flux {

    /**
     * Debug and Trace purpose NEAT entity descriptor. Contains all significant entity structural meta data
     */
    struct FLUX_API NeatEntityDescriptor
    {
    public:
        enum FLUX_API NeuronType { BIAS, INPUT, HIDDEN, OUTPUT };

        struct FLUX_API Neuron
        {
        public:
            int32_t Id;
            NeuronType Type;
            float_fl NormalizedPosition;

            Neuron() = default;
            Neuron(int32_t id, NeuronType type, float_fl normalizedPosition)
            : Id(id), Type(type), NormalizedPosition(normalizedPosition) {}
        };

        struct FLUX_API NeuronConnection
        {
        public:
            int32_t OriginId;
            int32_t DestinationId;
            float_fl Weight;
            int16_t Meta;

            NeuronConnection() = default;
            NeuronConnection(int32_t originId, int32_t destinationId, float_fl weight) : OriginId(originId),
                                                                                         DestinationId(destinationId),
                                                                                         Weight(weight),
                                                                                         Meta(0) {}

            NeuronConnection(int32_t originId, int32_t destinationId, float_fl weight, int16_t meta) : OriginId(originId),
                                                                                         DestinationId(destinationId),
                                                                                         Weight(weight),
                                                                                         Meta(meta) {}
        };

        int32_t Id;
        int32_t SpecieId;

        size_t Complexity;

        float_fl Fitness;

        size_t NeuronCount;
        size_t ConnectionsCount;

        Neuron* Neurons = nullptr;
        NeuronConnection* NeuronConnections = nullptr;

		NeatEntityDescriptor() = default;
		NeatEntityDescriptor(const NeatEntityDescriptor &other);

        NeatEntityDescriptor(int32_t id, int32_t specieId, size_t complexity, float_fl fitness,
                             size_t neuronCount, size_t connectionsCount, Neuron *neurons, NeuronConnection *neuronConnections);

        NeatEntityDescriptor &operator =(const NeatEntityDescriptor &other);
        std::string ToString();

        ~NeatEntityDescriptor();
    };
}

#endif //FLUXNEAT_NEAT_ENTITY_DESCRIPTOR_H
