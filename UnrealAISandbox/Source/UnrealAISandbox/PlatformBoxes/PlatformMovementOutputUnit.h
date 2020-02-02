//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#pragma once

#include <flux/common.h>

extern const flux::NeuralOutputId BoxHUpId;
extern const flux::NeuralOutputId BoxHDownId;
extern const flux::NeuralOutputId BoxVUpId;
extern const flux::NeuralOutputId BoxVDownId;

extern const std::set<flux::NeuralOutputId> GOutputIds;

class PlatformMovementOutputUnit : public flux::IOutputUnit
{
public:
	PlatformMovementOutputUnit(const std::string& id, const std::shared_ptr<flux::IContext>& context);

	void Apply(const std::vector<flux::NeuralOutput>& outputs) const override;

	std::shared_ptr<IContextUnit> Clone(std::shared_ptr<flux::IContext> context) const override;
};

