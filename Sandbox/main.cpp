//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#include <flux/common.h>
#include <flux/training.h>

#include <utility>
#include <sstream>

using namespace std;
using namespace flux;

class XorContext : public UnitContext
{
public:
    explicit XorContext(const std::shared_ptr<ManualRawInputSensor>& origin) : UnitContext("xor"),
                                                                        _inputStub(*std::static_pointer_cast<ManualRawInputSensor>(origin->Clone(origin->GetContext())))  {}

    std::vector<NeuralInput> getWorldInputs() { return  _inputStub.Fetch(); }
private:
    ManualRawInputSensor _inputStub;
};

class DebugOutputUnit : public IEvaluationOutputUnit
{
public:
    DebugOutputUnit(const string &id, const shared_ptr<IContext> &context) : IEvaluationOutputUnit(id, context),
    _error(0), _evalutionCount(0) {}

    void Apply(const std::vector<flux::NeuralOutput> &outputs) const override
    {
        auto world = std::static_pointer_cast<XorContext>(GetContext())->getWorldInputs();
        float_fl answer = (world[0].GetValue() + world[1].GetValue()) * (!world[0].GetValue() + !world[1].GetValue());

        //cout << "Black box activation result:" << endl;
        for (const auto &output : outputs)
        {
            _error += std::pow(answer - output.GetValue(), 2);
            //cout << output.GetOutputId().GetId() << ": Expected [" << answer << "] Actual [" << output.GetValue() << "]" << endl;
        }

        ++_evalutionCount;
    }

    bool IsEvaluationCompleted() override
    {
        return _evalutionCount >= 4;
    }

    float_fl GetAggregatedFitness() override
    {
        return 4 - _error;
    }

    void Reset() override
    {
        _error = 0;
        _evalutionCount = 0;
    }

    shared_ptr<IContextUnit> Clone(std::shared_ptr<IContext> context) const override
    {
        return CloneToContext<DebugOutputUnit>(context);
    }

private:
    mutable int _evalutionCount;
    mutable float_fl _error;
};

class ContextRegistry : public IContextRegistry
{
public:
    explicit ContextRegistry(std::shared_ptr<ManualRawInputSensor> origin) : _origin(std::move(origin)) {}

    shared_ptr<IContext> RetrieveContext() override
    {
        return std::make_shared<XorContext>(_origin);
    }

    void ReleaseContext(const IContext &context) override {}

private:
    std::shared_ptr<ManualRawInputSensor> _origin;
};

int main()
{
    cout << "Starting black box test..." << endl;
    auto context = std::make_shared<UnitContext>("Empty Context");
    std::shared_ptr<SingleActivityBlackBox> blackBox = std::make_shared<SingleActivityBlackBox>("test", context);

    NeuralInputId inputA("a");
    NeuralInputId inputB("b");
    set<NeuralInputId> inputIds = {inputA, inputB};
    auto manualInput = std::make_shared<ManualRawInputSensor>("xor_emu", context, inputIds, false);

    manualInput->SetInputsSequence( vector<vector<NeuralInput>>
    {
        (vector<NeuralInput> { NeuralInput(inputA, 0), NeuralInput(inputB, 0) }),
        (vector<NeuralInput> { NeuralInput(inputA, 1), NeuralInput(inputB, 0) }),
        (vector<NeuralInput> { NeuralInput(inputA, 0), NeuralInput(inputB, 1) }),
        (vector<NeuralInput> { NeuralInput(inputA, 1), NeuralInput(inputB, 1) })
    });

    blackBox->AddRawInput(manualInput);

    set<NeuralOutputId> outputIds = { NeuralOutputId("xor_value") };
    auto neatActivity = std::make_shared<NeatActivityUnit>("xor", context, inputIds, outputIds);

    blackBox->AddActivity(neatActivity);

    auto output = std::make_shared<DebugOutputUnit>("out", context);

    std::shared_ptr<ContextRegistry> registry = std::make_shared<ContextRegistry>(manualInput);
    NeatEvolutionParameters evolutionParameters;
    NeatActivityTrainer trainer(150, 10, 1, 6, 10, evolutionParameters,
            neatActivity,
            output,
            blackBox,
            registry);

    cout << "Initializing complete. Enabling black box..." << endl;
    trainer.Step();

    cout << "Enabling. Starting training..." << endl;
    int epoch = 0;
    while (trainer.GetChampionFitness() < 3.95)
    {
        while (!trainer.IsEpochCompleted()) {
            trainer.Step();
        }
        cout << "Completed " << ++epoch << " epoch" << endl;
        trainer.Step();
    }
    cout << "Training completed by " << epoch << " epochs." << endl;

    std::stringstream stream;
    trainer.SaveCurrentChampionActivity(stream);
    blackBox->UpdateChildScheme("xor", stream);

    for (int i = 0; i < 4; ++i)
    {
        blackBox->Step();

        cout << " A: " << blackBox->GetInputOf("a").GetValue()
             << " B: " << blackBox->GetInputOf("b").GetValue()
             << " O: " << blackBox->GetOutputOf("xor_value").GetValue() << endl;
    }
}
