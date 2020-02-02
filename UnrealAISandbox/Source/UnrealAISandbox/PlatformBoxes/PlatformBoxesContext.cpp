//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#include "PlatformBoxesContext.h"

PlatformBoxesContext::PlatformBoxesContext(APlatformAndBoxesSandbox *sandbox, uint16 index) : _index(index), _sandbox(sandbox)
{
}

std::string PlatformBoxesContext::GetName() const
{
    char buff[40];
	snprintf(buff, 40, "Context %d", _sandbox->GetUniqueID());
    return std::string(buff);
}
