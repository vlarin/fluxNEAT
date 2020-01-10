//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_NEAT_ACTIVITY_UNIT_IMPL_H
#define FLUXNEAT_NEAT_ACTIVITY_UNIT_IMPL_H

#include <mlpack/core.hpp>
#include <mlpack/methods/neat/neat.hpp>

#include <flux/prereq.h>
#include <flux/neat/neat_activity_unit.h>

namespace flux {

    class NeatActivityUnit::NeatActivityUnitImpl
    {
    public:
        void CopySchemeFrom(const NeatActivityUnitImpl &other);
        void UpdateScheme(std::istream &istream);
        arma::vec Activate(const arma::vec &inputs) const;

    private:
        mutable mlpack::neat::Genome<> _genome;
    };
}


#endif //FLUXNEAT_NEAT_ACTIVITY_UNIT_IMPL_H
