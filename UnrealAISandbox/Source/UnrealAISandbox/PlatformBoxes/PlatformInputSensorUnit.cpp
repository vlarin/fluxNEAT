//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#include "PlatformInputSensorUnit.h"

#include <PlatformBoxes/PlatformBoxesContext.h>
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"

using namespace flux;

const NeuralNodeId BoxHId("BoxH");
const NeuralNodeId BoxVId("BoxV");
const NeuralNodeId SpeedHId("SpeedH");
const NeuralNodeId SpeedVId("SpeedV");
const NeuralNodeId DistId("Dist");

const std::set<NeuralNodeId> GInputIds { BoxHId, BoxVId, SpeedHId, SpeedVId, DistId };

PlatformInputSensorUnit::PlatformInputSensorUnit(const std::string& id, const std::shared_ptr<flux::IContext>& context)
	: IRawSensorUnit(id, context) {}

std::set<NeuralNodeId> PlatformInputSensorUnit::GetInputIds() const
{
	return GInputIds;
}

std::vector<NeuralNode> PlatformInputSensorUnit::Fetch() const
{
	auto environment = std::static_pointer_cast<PlatformBoxesContext>(GetContext());
	auto target = environment->Sandbox().GetFallingCubeLocation();
	auto centroid = environment->Sandbox().GetCentroidLocation();

	DrawDebugSphere(environment->Sandbox().GetWorld(), target, 3, 32, FColor(255, 0, 0));
	DrawDebugSphere(environment->Sandbox().GetWorld(), centroid, 3, 32, FColor(0, 0, 255));
	
	auto speed = environment->Sandbox().GetPlatformSpeed();
	target.Z = 0;
	centroid.Z = 0;
	
	auto dir = (target - centroid);
	dir.Normalize(1);

	auto dist = (target - centroid).Size() / 20.0;
	if (dist > 0.1)
	{
		dir *= dist;
	}
	else
	{
		dir *= 0.1;
	}
	
	auto boxH = FMath::Clamp<float>(dir.X, -5, 5);
	auto boxV = FMath::Clamp<float>(dir.Y, -5, 5);
	
	return std::vector<NeuralNode> {
		NeuralNode(BoxHId, boxH),
		NeuralNode(BoxVId, boxV),
		NeuralNode(SpeedHId, speed.X / 100.0),
		NeuralNode(SpeedVId, speed.Y / 100.0),
		NeuralNode(DistId, dist)
	};
}

std::shared_ptr<IContextUnit> PlatformInputSensorUnit::Clone(std::shared_ptr<IContext> context) const
{
	return CloneToContext<PlatformInputSensorUnit>(context);
}
