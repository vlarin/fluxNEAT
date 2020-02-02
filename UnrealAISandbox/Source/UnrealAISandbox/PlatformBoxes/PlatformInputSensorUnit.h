#pragma once

#include <flux/common.h>

extern const flux::NeuralInputId BoxHId;
extern const flux::NeuralInputId BoxVId;
extern const flux::NeuralInputId SpeedHId;
extern const flux::NeuralInputId SpeedVId;
extern const flux::NeuralInputId DistId;

extern const std::set<flux::NeuralInputId> GInputIds;

class UNREALAISANDBOX_API PlatformInputSensorUnit : public flux::IRawSensorUnit
{
public:
	PlatformInputSensorUnit(const std::string& id, const std::shared_ptr<flux::IContext>& context);

	std::set<flux::NeuralInputId> GetInputIds() const override;
	std::vector<flux::NeuralInput> Fetch() const override;

	std::shared_ptr<IContextUnit> Clone(std::shared_ptr<flux::IContext> context) const override;
};
