//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_HNEAT_ACTIVITY_UNIT_IMPL_H
#define FLUXNEAT_HNEAT_ACTIVITY_UNIT_IMPL_H

#include <mlpack/core.hpp>
#include <mlpack/methods/hyperneat/hyperneat.hpp>

#include <flux/prereq.h>
#include <flux/neat/hyper_neat_activity_unit.h>

namespace flux {

    class HyperNeatActivityUnit::HyperNeatActivityUnitImpl
    {
    public:
        void CopySchemeFrom(const HyperNeatActivityUnitImpl &other);
        void UpdateScheme(std::istream &istream);
        arma::vec Activate(const arma::vec &inputs) const;

    private:
        mutable mlpack::hyperneat::Genome<> _genome;
    };
}


#endif //FLUXNEAT_NEAT_ACTIVITY_UNIT_IMPL_H
