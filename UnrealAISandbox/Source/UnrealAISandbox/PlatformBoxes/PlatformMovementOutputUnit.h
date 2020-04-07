//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#pragma once

#include <flux/common.h>

extern const flux::NeuralNodeId BoxHUpId;
extern const flux::NeuralNodeId BoxHDownId;
extern const flux::NeuralNodeId BoxVUpId;
extern const flux::NeuralNodeId BoxVDownId;

extern const std::set<flux::NeuralNodeId> GOutputIds;

class PlatformMovementOutputUnit : public flux::IOutputUnit
{
public:
	PlatformMovementOutputUnit(const std::string& id, const std::shared_ptr<flux::IContext>& context);

	void Apply(const std::vector<flux::NeuralNode>& outputs) const override;

	std::shared_ptr<IContextUnit> Clone(std::shared_ptr<flux::IContext> context) const override;
};

