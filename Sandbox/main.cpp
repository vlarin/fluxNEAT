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

    vector<NeuralNodeId> GetOutputIds() const override
    {
        return std::vector<NeuralNodeId> { NeuralNodeId("xor_value") };
    }

    void Apply(const std::map<flux::NeuralNodeId, flux::NeuralNode> &outputs) const override
    {
        auto world = std::static_pointer_cast<XorContext>(GetContext())->getWorldInputs();
        float_fl answer = (world[0].GetValue() + world[3].GetValue() - world[1].GetValue() - world[2].GetValue());
        if (answer < 0) { answer = 0; }
        if (answer > 1) { answer = 1; }

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

    shared_ptr<IContext> RetrieveContext(bool isInPreviewMode) override
    {
        return std::make_shared<XorContext>(_origin);
    }

    void ReleaseContext(const IContext &context) override {}
    void ForceReleaseAll() override {}

private:
    std::shared_ptr<ManualRawInputSensor> _origin;
};

void FillNeuronCubeIndices(const int i, int index, float weight, const std::vector<std::pair<int, float>> &preindices, std::vector<std::pair<int, float>> &indices)
{
    if (i >= preindices.size())
    {
        return;
    }

    index += preindices[i].first;
    weight *= preindices[i].second;

    int nextIndex = (i % 2 == 0) ? i + 2 : i + 1;
    if (nextIndex >= preindices.size() && weight > 0.001)
    {
        indices.emplace_back(index, weight);
    }

    FillNeuronCubeIndices(nextIndex, index, weight, preindices, indices);
    FillNeuronCubeIndices(nextIndex + 1, index, weight, preindices, indices);
}

std::vector<std::pair<int, float>> GetNeuronCubeIndices(std::vector<float> position, int length)
{
    std::vector<std::pair<int, float>> preindices {};
    std::vector<std::pair<int, float>> indices {};

    auto dimensionsCount = position.size();
    int div = 1;
    int modulus = length;

    for (int i = 0; i < dimensionsCount; i++)
    {
        position[i] *= (length - 1);
        if (position[i] >= (length - 1))
        {
            position[i] = length - 1.000001;
        }

        float rawPreIndex;
        float weight = modf(position[i], &rawPreIndex);
        preindices.emplace_back(rawPreIndex * div, 1.0 - weight);
        preindices.emplace_back((rawPreIndex + 1) * div, weight);

        div = modulus;
        modulus *= length;
    }

    FillNeuronCubeIndices(0, 0, 1.0, preindices, indices);
    FillNeuronCubeIndices(1, 0, 1.0, preindices, indices);

    return indices;
}

int main()
{
    cout << "Starting black box test..." << endl;
    auto context = std::make_shared<UnitContext>("Empty Context");
    std::shared_ptr<SingleActivityBlackBox> blackBox = std::make_shared<SingleActivityBlackBox>("test", context);

    std::vector<std::pair<int, float>> test1 = GetNeuronCubeIndices(std::vector<float> { 1, 1, 1}, 9);
    NeuralNodeId inputA("topLeft"); // 1
    NeuralNodeId inputB("topRight");
    NeuralNodeId inputC("bottomLeft");
    NeuralNodeId inputD("bottomRight"); // 1
    vector<NeuralNodeId> inputIds = {inputA, inputB, inputC, inputD};
    auto manualInput = std::make_shared<ManualRawInputSensor>("xor_emu", context, inputIds, false);

    manualInput->SetInputsSequence( vector<vector<NeuralNode>>
                                            {
                                                    (vector<NeuralNode> { NeuralNode(inputA, 1), NeuralNode(inputB, 0), NeuralNode(inputC, 0), NeuralNode(inputD, 0) }),
                                                    (vector<NeuralNode> { NeuralNode(inputA, 0), NeuralNode(inputB, 1), NeuralNode(inputC, 0), NeuralNode(inputD, 0) }),
                                                    (vector<NeuralNode> { NeuralNode(inputA, 0), NeuralNode(inputB, 0), NeuralNode(inputC, 1), NeuralNode(inputD, 0) }),
                                                    (vector<NeuralNode> { NeuralNode(inputA, 0), NeuralNode(inputB, 0), NeuralNode(inputC, 0), NeuralNode(inputD, 1) }),
                                            });

    blackBox->AddRawInput(manualInput);

    vector<NeuralNodeId> outputIds = { NeuralNodeId("xor_value") };
    auto neatActivity = std::make_shared<HyperNeatActivityUnit>("xor", context, inputIds, outputIds);

    blackBox->AddActivity(neatActivity);

    auto output = std::make_shared<DebugOutputUnit>("out", context);

    std::shared_ptr<ContextRegistry> registry = std::make_shared<ContextRegistry>(manualInput);
    NeatEvolutionParameters evolutionParameters;
    evolutionParameters.IsAcyclic = true;

    auto substrateLayers = std::vector<size_t> { 4, 1, 1 };
    auto dimensionsCount = 2;
    NeatActivityTrainer trainer(150, 15, 1, 2, 10, dimensionsCount, evolutionParameters,
                                substrateLayers,
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

        cout << " A: " << blackBox->GetInputOf("topLeft").GetValue()
             << " B: " << blackBox->GetInputOf("topRight").GetValue()
             << " C: " << blackBox->GetInputOf("bottomLeft").GetValue()
             << " D: " << blackBox->GetInputOf("bottomRight").GetValue()
             << " O: " << blackBox->GetOutputOf("xor_value").GetValue() << endl;
    }
}