//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "APlatformAndBoxesSandbox.h"

#include <flux/common.h>

/**
 * Defines sandbox context which allows to fetch actual external inputs, get fitness data and apply outputs
 */
class UNREALAISANDBOX_API PlatformBoxesContext final : public flux::IContext
{
public:
	PlatformBoxesContext(APlatformAndBoxesSandbox *sandbox, uint16 index);
	virtual ~PlatformBoxesContext() = default;

	std::string GetName() const override;
	uint16 GetIndex() const { return _index; }
	APlatformAndBoxesSandbox& Sandbox() const { return *_sandbox; }
private:
	uint16 _index;
	APlatformAndBoxesSandbox* _sandbox;
};
