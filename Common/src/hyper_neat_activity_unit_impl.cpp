#include "include/hyper_neat_activity_unit_impl.h"

void flux::HyperNeatActivityUnit::HyperNeatActivityUnitImpl::UpdateScheme(std::istream &istream)
{
    //TODO: catch exceptions..
    boost::archive::text_iarchive ar(istream);
    ar >> _genome;
}

arma::vec flux::HyperNeatActivityUnit::HyperNeatActivityUnitImpl::Activate(const arma::vec &inputs) const
{
    return _genome.Evaluate(inputs);
}

void flux::HyperNeatActivityUnit::HyperNeatActivityUnitImpl::CopySchemeFrom(const HyperNeatActivityUnitImpl &other)
{
    _genome = other._genome;
}
