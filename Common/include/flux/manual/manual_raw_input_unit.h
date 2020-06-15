//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_MANUAL_RAW_INPUT_UNIT_H
#define FLUXNEAT_MANUAL_RAW_INPUT_UNIT_H

#include <flux/prereq.h>
#include <flux/raw_sensor_unit.h>

namespace flux {

    class FLUX_API ManualRawInputSensor : public IRawSensorUnit
    {
    public:
        ManualRawInputSensor(std::string id, std::shared_ptr<IContext> context, std::vector<NeuralNodeId> inputIds, bool verbose = false);

        bool &IsAutoPlayed() { return _isAutoPlayed; }
        std::vector<NeuralNodeId> GetInputIds() const override { return _inputIds; }
        std::vector<NeuralNode> Fetch() const override;
        void Step();

        void SetInputs(const std::vector<NeuralNode> &inputs);
        void SetInputsSequence(std::vector<std::vector<NeuralNode>> inputSequence);

        std::shared_ptr<IContextUnit> Clone(std::shared_ptr<IContext> context) const override;

    private:
        bool _verbose;
        bool _isAutoPlayed;
        mutable uint8_t _currentSequenceId;
        std::vector<NeuralNodeId> _inputIds;
        std::vector<std::vector<NeuralNode>> _inputSequences;
    };
}

#endif //FLUXNEAT_MANUAL_RAW_INPUT_UNIT_H
