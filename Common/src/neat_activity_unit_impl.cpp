#include "include/neat_activity_unit_impl.h"

void flux::NeatActivityUnit::NeatActivityUnitImpl::UpdateScheme(std::istream &istream)
{
    //TODO: catch exceptions..
    boost::archive::binary_iarchive ar(istream);
    ar >> _genome;
}

arma::vec flux::NeatActivityUnit::NeatActivityUnitImpl::Activate(const arma::vec &inputs) const
{
    return _genome.Evaluate(inputs);
}

void flux::NeatActivityUnit::NeatActivityUnitImpl::CopySchemeFrom(const NeatActivityUnitImpl &other)
{
    _genome = other._genome;
}
