#include <flux/context_unit.h>

flux::IContextUnit flux::IContextUnit::Clone(std::shared_ptr<flux::IContext> context) const
{
    flux::IContextUnit clone = *this;
    clone._context = std::move(context);
    return clone;
}