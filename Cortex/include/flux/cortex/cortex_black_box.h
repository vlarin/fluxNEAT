//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_CORTEX_BLACK_BOX_H
#define FLUXNEAT_CORTEX_BLACK_BOX_H

#include <flux/prereq.h>
#include <flux/context_unit.h>
#include <flux/neural_node.h>
#include <flux/augmented_sensor_unit.h>
#include <flux/activity_unit.h>
#include <flux/black_box.h>

#include <flux/feedback/feedback_unit.h>

#include <flux/cortex/cortex_transition.h>
#include <flux/cortex/cortex_layer.h>
#include <flux/cortex/cortex_decision_layer.h>

namespace flux {

    /**
     * Represents AI Cortex (Azu Generation) that fetches inputs transforms them through hidden core that selects
     * current activity and extracts them to the outputs TODO: serialization support
     */
    class FLUX_API CortexBlackBox : public IBlackBox
    {
    public:
        CortexBlackBox(const std::string &id, std::shared_ptr<IContext> context);

        void AddRawInput(std::shared_ptr<IRawSensorUnit> input) override;
        void AddAugmentedInput(std::shared_ptr<IAugmentedSensorUnit> input) override;
        void AddActivity(std::shared_ptr<IActivityUnit> activity) override;
        void AddFeedback(std::shared_ptr<IFeedbackUnit> feedback);
        void AddOutput(std::shared_ptr<IOutputUnit> output) override;
        void AddBuiltinTransition(const CortexTransition &transition);

        const NeuralNode &GetInputOf(std::string id) const override;
        const NeuralNode &GetOutputOf(std::string id) const override;
        const MediatorValue &GetMediatorOf(std::string id) const;

        void Step() override;
        void InjectMediatorValue(MediatorValue mediatorValue);

        void SetActivityBound(std::string activityId) override;
        void ResetActivityBound() override;

        std::shared_ptr<IContextUnit> Clone(std::shared_ptr<IContext> context) const override;

        void UpdateChildScheme(std::string childId, std::istream &istream) override;

    private:
        std::vector<std::shared_ptr<IRawSensorUnit>> _rawInputs;
        std::vector<std::shared_ptr<IAugmentedSensorUnit>> _augmentedInputs;
        std::vector<std::shared_ptr<IOutputUnit>> _outputs;
        std::vector<std::shared_ptr<IFeedbackUnit>> _feedbacks;

        std::map<std::string, std::shared_ptr<IActivityUnit>> _activityUnits;

        std::map<NeuralNodeId, NeuralNode> _sensors;
        std::map<NeuralNodeId, NeuralNode> _responses;
        std::map<MediatorId, MediatorValue> _mediators;

        CortexLayer _cortexLayer;
        CortexDecisionLayer _cortexDecisionLayer;

        void FetchFeedback(const std::map<NeuralNodeId, NeuralNode> &context);
    };
}

#endif //FLUXNEAT_CORTEX_BLACK_BOX_H
