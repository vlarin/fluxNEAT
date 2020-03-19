//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_MEDIATOR_VALUE_H
#define FLUXNEAT_MEDIATOR_VALUE_H

#include <flux/prereq.h>

namespace flux
{
    class FLUX_API MediatorId
    {
    private:
        std::string id;
    public:
        MediatorId() = default;

        explicit MediatorId(std::string id);

        std::string GetId() const { return id; }

        bool operator<(const MediatorId &other) const { return id < other.id; }
    };

    class FLUX_API MediatorValue
    {
    private:
        MediatorId mediator;
        float_fl value;
    public:
        MediatorValue() = default;
        explicit MediatorValue(MediatorId input);
        MediatorValue(MediatorId input, float_fl value);

        MediatorId GetMediator() const { return mediator; }
        float_fl GetValue() const { return value; }

        void Apply(float_fl delta);
        void Merge(const MediatorValue &other);
    };
}

#endif //FLUXNEAT_MEDIATOR_VALUE_H
