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

    std::vector<NeuralNode> getWorldInputs() { return  _inputStub.Fetch(); }
private:
    ManualRawInputSensor _inputStub;
};

class DebugOutputUnit : public IEvaluationOutputUnit
{
public:
    DebugOutputUnit(const string &id, const shared_ptr<IContext> &context) : IEvaluationOutputUnit(id, context),
    _error(0), _evalutionCount(0) {}

    set<NeuralNodeId> GetOutputIds() const override
    {
        return std::set<NeuralNodeId> { NeuralNodeId("xor_value") };
    }

    void Apply(const std::map<flux::NeuralNodeId, flux::NeuralNode> &outputs) const override
    {
        auto world = std::static_pointer_cast<XorContext>(GetContext())->getWorldInputs();
        float_fl answer = (world[0].GetValue() + world[1].GetValue()) * (!world[0].GetValue() + !world[1].GetValue());

        //cout << "Black box activation result:" << endl;
        for (const auto &output : outputs)
        {
            _error += std::pow(answer - output.second.GetValue(), 2);
            //cout << output.GetId().GetNodeId() << ": Expected [" << answer << "] Actual [" << output.GetValue() << "]" << endl;
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

    NeuralNodeId inputA("a");
    NeuralNodeId inputB("b");
    set<NeuralNodeId> inputIds = {inputA, inputB};
    auto manualInput = std::make_shared<ManualRawInputSensor>("xor_emu", context, inputIds, false);

    manualInput->SetInputsSequence( vector<vector<NeuralNode>>
    {
        (vector<NeuralNode> { NeuralNode(inputA, 0), NeuralNode(inputB, 0) }),
        (vector<NeuralNode> { NeuralNode(inputA, 1), NeuralNode(inputB, 0) }),
        (vector<NeuralNode> { NeuralNode(inputA, 0), NeuralNode(inputB, 1) }),
        (vector<NeuralNode> { NeuralNode(inputA, 1), NeuralNode(inputB, 1) })
    });

    blackBox->AddRawInput(manualInput);

    set<NeuralNodeId> outputIds = { NeuralNodeId("xor_value") };
    auto neatActivity = std::make_shared<NeatActivityUnit>("xor", context, inputIds, outputIds);

    blackBox->AddActivity(neatActivity);

    auto output = std::make_shared<DebugOutputUnit>("out", context);

    std::shared_ptr<ContextRegistry> registry = std::make_shared<ContextRegistry>(manualInput);
    NeatEvolutionParameters evolutionParameters;
    evolutionParameters.IsAcyclic = false;
    NeatActivityTrainer trainer(150, 15, 1, 2, 10, evolutionParameters,
            neatActivity,
            output,
            blackBox,
            registry);

    cout << "Initializing complete. Enabling black box..." << endl;
    trainer.Step(1);

    cout << "Enabling. Starting training..." << endl;
    while (trainer.GetChampionFitness() < 3.95)
    {
        while (!trainer.IsEpochCompleted()) {
            trainer.Step(1);
        }
        cout << "Completed " << trainer.GetCurrentEpoch() << " epoch. Current Champion - " << trainer.GetChampionFitness()
    	<< ". IsComplexifying - " << (trainer.IsComplexifying() ? "true" : "false") << "." << endl;
        trainer.Step(1);
    }
    cout << "Training completed by " << trainer.GetCurrentEpoch() << " epochs." << endl;

    std::stringstream stream;
    trainer.SaveCurrentChampionActivity(stream);
    blackBox->UpdateChildScheme("xor", stream);

    cout << trainer.GetChampionEntity().ToString();

    for (int i = 0; i < 4; ++i)
    {
        blackBox->Step();

        cout << " A: " << blackBox->GetInputOf("a").GetValue()
             << " B: " << blackBox->GetInputOf("b").GetValue()
             << " O: " << blackBox->GetOutputOf("xor_value").GetValue() << endl;
    }
}
