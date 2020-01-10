//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_SINGLE_ACTIVITY_BLACK_BOX_H
#define FLUXNEAT_SINGLE_ACTIVITY_BLACK_BOX_H

#include <flux/prereq.h>
#include <flux/black_box.h>

namespace flux {

    /**
     * Represents raw black box that fetches inputs transforms them through single activity unit
     * and extracts them to the outputs
     */
    class FLUX_API SingleActivityBlackBox : public IBlackBox
    {
    public:
        SingleActivityBlackBox(const std::string &id, std::shared_ptr<IContext> context);

        void AddRawInput(std::shared_ptr<IRawSensorUnit> input) override;
        void AddAugmentedInput(std::shared_ptr<IAugmentedSensorUnit> input) override;
        void AddActivity(std::shared_ptr<IActivityUnit> activity) override;
        void AddOutput(std::shared_ptr<IOutputUnit> output) override;

        void Step() override;

        void UpdateChildScheme(std::string childId, std::istream &istream) override;

    private:
        std::vector<std::shared_ptr<IRawSensorUnit>> _rawInputs;
        std::vector<std::shared_ptr<IAugmentedSensorUnit>> _augmentedInputs;
        std::vector<std::shared_ptr<IOutputUnit>> _outputs;

        std::shared_ptr<IActivityUnit> _activityUnit;
    };
}

#endif //FLUXNEAT_SINGLE_ACTIVITY_BLACK_BOX_H
