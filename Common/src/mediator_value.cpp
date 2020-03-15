#include <flux/feedback/mediator_value.h>

#include <utility>

flux::MediatorId::MediatorId(std::string id) : id(std::move(id)) {}

flux::MediatorValue::MediatorValue(flux::MediatorId input) : mediator(std::move(input)), value(0.0) {}

flux::MediatorValue::MediatorValue(flux::MediatorId input, flux::float_fl value)
    : mediator(std::move(input)), value(value) {}

void flux::MediatorValue::Apply(flux::float_fl delta)
{
    value += delta;
}

void flux::MediatorValue::Merge(const flux::MediatorValue &other)
{
    Apply(other.GetValue());
}
