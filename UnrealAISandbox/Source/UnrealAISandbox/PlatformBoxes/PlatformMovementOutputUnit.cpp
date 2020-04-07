//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#include "PlatformMovementOutputUnit.h"

#include <PlatformBoxes/PlatformBoxesContext.h>

using namespace flux;

const NeuralNodeId BoxHUpId("BoxH+");
const NeuralNodeId BoxHDownId("BoxH-");
const NeuralNodeId BoxVUpId("BoxV+");
const NeuralNodeId BoxVDownId("BoxV-");

const std::set<NeuralNodeId> GOutputIds{ BoxHUpId, BoxHDownId, BoxVUpId, BoxVDownId };

PlatformMovementOutputUnit::PlatformMovementOutputUnit(const std::string& id, const std::shared_ptr<flux::IContext>& context)
	: IOutputUnit(id, context) {}

void PlatformMovementOutputUnit::Apply(const std::map<flux::NeuralNodeId, flux::NeuralNode> &outputs) const
{
    float dx = outputs.find(BoxHUpId)->second.GetValue() - outputs.find(BoxHDownId)->second.GetValue();
	float dy = outputs.find(BoxVUpId)->second.GetValue() - outputs.find(BoxVDownId)->second.GetValue();

	std::static_pointer_cast<PlatformBoxesContext>(GetContext())->Sandbox().ApplyPlatformMovement(dx, dy);
}

std::shared_ptr<IContextUnit> PlatformMovementOutputUnit::Clone(std::shared_ptr<flux::IContext> context) const
{
	return CloneToContext<PlatformMovementOutputUnit>(context);
}
