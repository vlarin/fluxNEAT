#pragma once

#include <flux/common.h>

extern const flux::NeuralNodeId BoxHId;
extern const flux::NeuralNodeId BoxVId;
extern const flux::NeuralNodeId SpeedHId;
extern const flux::NeuralNodeId SpeedVId;
extern const flux::NeuralNodeId DistId;

extern const std::set<flux::NeuralNodeId> GInputIds;

class UNREALAISANDBOX_API PlatformInputSensorUnit : public flux::IRawSensorUnit
{
public:
	PlatformInputSensorUnit(const std::string& id, const std::shared_ptr<flux::IContext>& context);

	std::set<flux::NeuralNodeId> GetInputIds() const override;
	std::vector<flux::NeuralNode> Fetch() const override;

	std::shared_ptr<IContextUnit> Clone(std::shared_ptr<flux::IContext> context) const override;
};
