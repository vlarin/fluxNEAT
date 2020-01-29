//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#include "PlatformInputSensorUnit.h"

#include <PlatformBoxes/PlatformBoxesContext.h>
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"

using namespace flux;

const NeuralInputId BoxHId("BoxH");
const NeuralInputId BoxVId("BoxV");
const NeuralInputId SpeedHId("SpeedH");
const NeuralInputId SpeedVId("SpeedV");
const NeuralInputId DistId("Dist");

const std::set<NeuralInputId> GInputIds { BoxHId, BoxVId, SpeedHId, SpeedVId, DistId };

PlatformInputSensorUnit::PlatformInputSensorUnit(const std::string& id, const std::shared_ptr<flux::IContext>& context)
	: IRawSensorUnit(id, context) {}

std::set<NeuralInputId> PlatformInputSensorUnit::GetInputIds() const
{
	return GInputIds;
}

std::vector<NeuralInput> PlatformInputSensorUnit::Fetch() const
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
	
	auto msg = FString::Format(TEXT("T: {5} C: {6} BoxH:{0} BoxV:{1} SpH:{2} SpV:{3} Dist:{4}"),
		{
			boxH,
			boxV,
			speed.X / 100.0, speed.Y / 100.0, dist, 0, 0// target.ToCompactString(), centroid.ToCompactString()
		});
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, msg);
	
	return std::vector<NeuralInput> {
		NeuralInput(BoxHId, boxH),
		NeuralInput(BoxVId, boxV),
		NeuralInput(SpeedHId, speed.X / 100.0),
		NeuralInput(SpeedVId, speed.Y / 100.0),
		NeuralInput(DistId, dist)
	};
}

std::shared_ptr<IContextUnit> PlatformInputSensorUnit::Clone(std::shared_ptr<IContext> context) const
{
	return CloneToContext<PlatformInputSensorUnit>(context);
}
