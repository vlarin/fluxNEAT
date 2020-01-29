//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#include "PlatformBoxesContext.h"

PlatformBoxesContext::PlatformBoxesContext(APlatformAndBoxesSandbox *sandbox, uint16 index) : _index(index), _sandbox(sandbox)
{
}

std::string PlatformBoxesContext::GetName() const
{
	return "Context " + _sandbox->GetUniqueID();
}
