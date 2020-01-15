//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_EVALUATION_OUTPUT_UNIT_H
#define FLUXNEAT_EVALUATION_OUTPUT_UNIT_H

#include <flux/prereq.h>
#include <flux/output_unit.h>

namespace flux {

    class FLUX_API IEvaluationOutputUnit : public IOutputUnit
    {
    public:
        IEvaluationOutputUnit(std::string id, std::shared_ptr<IContext> context) : IOutputUnit(std::move(id), std::move(context)) {}

        virtual bool IsEvaluationCompleted() = 0;
        virtual float_fl GetAggregatedFitness() = 0;
        virtual void Reset() = 0;
    };
}

#endif //FLUXNEAT_EVALUATION_OUTPUT_UNIT_H
