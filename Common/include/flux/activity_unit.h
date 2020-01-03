//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_ACTIVITY_UNIT_H
#define FLUXNEAT_ACTIVITY_UNIT_H

#include <set>

#include <flux/prereq.h>
#include <flux/neural_input.h>
#include <flux/neural_output.h>
#include <flux/context_unit.h>

namespace flux {

    /**
     * Activity unit represents standard input -> output conversion unit
     */
    class FLUX_API IActivityUnit : public IContextUnit
    {
    protected:
        IActivityUnit(std::string id, std::shared_ptr<IContext> context) : IContextUnit(std::move(id), std::move(context)) {}
    public:
        virtual std::set<flux::NeuralInputId> GetInputIds() const = 0;
        virtual std::set<flux::NeuralOutputId> GetOutputIds() const = 0;

        virtual std::vector<NeuralOutput> Activate(const std::vector<NeuralInput> &inputs) const = 0;
    };
}

#endif //FLUXNEAT_ACTIVITY_UNIT_H
