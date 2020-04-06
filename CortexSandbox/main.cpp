//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#include <flux/common.h>
#include <flux/training.h>
#include <flux/cortex.h>

#include <utility>
#include <sstream>

using namespace std;
using namespace flux;

class XorContext : public UnitContext
{
public:
    explicit XorContext(const std::shared_ptr<ManualRawInputSensor>& origin) : UnitContext("xor"),
                                                                               _inputStub(*std::static_pointer_cast<ManualRawInputSensor>(origin->Clone(origin->GetContext())))
   {
        _inputStub.IsAutoPlayed() = false;
   }

    std::vector<NeuralInput> getWorldInputs() { return  _inputStub.Fetch(); }
    void Step() { _inputStub.Step(); }
private:
    ManualRawInputSensor _inputStub;
};

class XorFeedbackUnit : public IFeedbackUnit
{
public:
    explicit XorFeedbackUnit(NeuralInputId contextId, MediatorId mediatorId,
            const string &id, const shared_ptr<IContext> &context) : IFeedbackUnit(id, context),
            _targetContextId(std::move(contextId)), _mediatorId(std::move(mediatorId)) {}

    std::set<NeuralInputId> GetInputIds() const override
    {
        return std::set<NeuralInputId> { _targetContextId };
    }

    vector<MediatorValue> Activate(const std::vector<NeuralInput> &inputs) const override
    {
        auto world = std::static_pointer_cast<XorContext>(GetContext())->getWorldInputs();
        float_fl answer = (world[0].GetValue() + world[1].GetValue()) * (!world[0].GetValue() + !world[1].GetValue());

        float_fl error = 0.0;
        //cout << "Black box activation result:" << endl;
        for (const auto &input : inputs)
        {
            if (input.GetInputId().GetId() == _targetContextId.GetId())
            {
                error += std::pow(answer - input.GetValue(), 2);
                //cout << input.GetOutputId().GetId() << ": Expected [" << answer << "] Actual [" << input.GetValue() << "]" << endl;
            }
        }
        return std::vector<MediatorValue> { MediatorValue(_mediatorId, 1 - sqrt(error))};
    }

    shared_ptr<IContextUnit> Clone(std::shared_ptr<IContext> context) const override
    {
        return CloneToContext<XorFeedbackUnit>(context);
    }

private:
    MediatorId _mediatorId;
    NeuralInputId _targetContextId;
};

int main()
{
    cout << "Starting cortex XOR black box test..." << endl;
    auto emptyContext = std::make_shared<UnitContext>("Empty Context");

    NeuralInputId inputA("a");
    NeuralInputId inputB("b");
    NeuralInputId inputC("c");
    set<NeuralInputId> rawInputIds = {inputA, inputB};
    set<NeuralInputId> inputIds = {inputA, inputB, inputC};
    auto manualInput = std::make_shared<ManualRawInputSensor>("xor_emu", emptyContext, rawInputIds, false);

    manualInput->SetInputsSequence( vector<vector<NeuralInput>>
                                            {
                                                    (vector<NeuralInput> { NeuralInput(inputA, 0), NeuralInput(inputB, 0) }),
                                                    (vector<NeuralInput> { NeuralInput(inputA, 1), NeuralInput(inputB, 0) }),
                                                    (vector<NeuralInput> { NeuralInput(inputA, 0), NeuralInput(inputB, 1) }),
                                                    (vector<NeuralInput> { NeuralInput(inputA, 1), NeuralInput(inputB, 1) })
                                            });

    auto context = std::make_shared<XorContext>(manualInput);
    std::shared_ptr<CortexBlackBox> blackBox = std::make_shared<CortexBlackBox>("test", context);
    blackBox->AddRawInput(std::static_pointer_cast<IRawSensorUnit>(manualInput->Clone(context)));
    blackBox->AddContextInput(inputC);

    NeuralOutputId outputId("xor_value");
    set<NeuralOutputId> outputIds = { outputId };
    auto activity = std::make_shared<IdentityActivityUnit>(inputC, outputId, "manipulator", context);

    blackBox->AddActivity(activity);

    MediatorId mediatorId("xor");
    auto feedback = std::make_shared<XorFeedbackUnit>(inputC, mediatorId, "xorFeedback", context);

    blackBox->AddFeedback(feedback);

    CortexTransition identityTransition("identity", "manipulator",
            std::map<NeuralInputId, NeuralInput> { { inputC, NeuralInput(inputC, 0.5)}},
            std::map<NeuralInputId, float_fl> { { inputC, 0.5}},
            std::map<NeuralInputId, NeuralInput> { { inputC, NeuralInput(inputC, 0.5)}},
            std::map<NeuralInputId, float_fl> { { inputC, 0.5}},
            std::map<MediatorId, MediatorValue>() );

    blackBox->AddBuiltinTransition(identityTransition);

    int lastXorUnfulfill = 0;
    for (int i = 0; i < 100; i++)
    {
        context->Step();
        blackBox->Step();
        if ( blackBox->GetMediatorOf("xor").GetValue() < 0.98)
        {
            lastXorUnfulfill = i;
        }
        cout << "Current xor fulfillment: " << blackBox->GetMediatorOf("xor").GetValue() << ". Iteration - " << i << std::endl;
    }

    cout << "Full training after " << lastXorUnfulfill << " attempts";
}
