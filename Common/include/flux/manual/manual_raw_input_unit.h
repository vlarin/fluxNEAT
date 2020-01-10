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
        ManualRawInputSensor(std::string id, std::shared_ptr<IContext> context, std::set<NeuralInputId> inputIds, bool verbose = false);

        std::set<NeuralInputId> GetInputIds() const override { return _inputIds; }
        std::vector<NeuralInput> Fetch() const override;

        void SetInputs(const std::vector<NeuralInput> &inputs);
        void SetInputsSequence(std::vector<std::vector<NeuralInput>> inputSequence);

    private:
        bool _verbose;
        mutable uint8_t _currentSequenceId;
        std::set<NeuralInputId> _inputIds;
        std::vector<std::vector<NeuralInput>> _inputSequences;
    };
}

#endif //FLUXNEAT_MANUAL_RAW_INPUT_UNIT_H
