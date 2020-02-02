//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#include "PlatformMovementOutputUnit.h"

#include <PlatformBoxes/PlatformBoxesContext.h>

using namespace flux;

const NeuralOutputId BoxHUpId("BoxH+");
const NeuralOutputId BoxHDownId("BoxH-");
const NeuralOutputId BoxVUpId("BoxV+");
const NeuralOutputId BoxVDownId("BoxV-");

const std::set<NeuralOutputId> GOutputIds{ BoxHUpId, BoxHDownId, BoxVUpId, BoxVDownId };

PlatformMovementOutputUnit::PlatformMovementOutputUnit(const std::string& id, const std::shared_ptr<flux::IContext>& context)
	: IOutputUnit(id, context) {}

void PlatformMovementOutputUnit::Apply(const std::vector<flux::NeuralOutput>& outputs) const
{
	float dx = outputs[0].GetValue() - outputs[1].GetValue();
	float dy = outputs[2].GetValue() - outputs[3].GetValue();

	std::static_pointer_cast<PlatformBoxesContext>(GetContext())->Sandbox().ApplyPlatformMovement(dx, dy);
}

std::shared_ptr<IContextUnit> PlatformMovementOutputUnit::Clone(std::shared_ptr<flux::IContext> context) const
{
	return CloneToContext<PlatformMovementOutputUnit>(context);
}
