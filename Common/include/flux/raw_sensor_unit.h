//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_RAW_SENSOR_UNIT_H
#define FLUXNEAT_RAW_SENSOR_UNIT_H

#include <flux/prereq.h>
#include <flux/neural_node.h>
#include <flux/context_unit.h>

#include <set>
#include <utility>

namespace flux {

    /**
     * Raw Input Unit interface acts like a base sensor in the blackbox
     */
    class FLUX_API IRawSensorUnit : public IContextUnit
    {
    protected:
        IRawSensorUnit(std::string id, std::shared_ptr<IContext> context) : IContextUnit(std::move(id), std::move(context)) {}
    public:
        virtual std::vector<NeuralNodeId> GetInputIds() const = 0;

        virtual std::vector<NeuralNode> Fetch() const = 0;
    };
}

#endif //FLUXNEAT_RAW_SENSOR_UNIT_H
