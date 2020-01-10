//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#include <flux/common.h>

#include <mlpack/core.hpp>
#include <mlpack/methods/neat/neat.hpp>

using namespace std;

using namespace flux;

using namespace mlpack::neat;

class DebugOutputUnit : public IOutputUnit
{
public:
    DebugOutputUnit(const string &id, const shared_ptr<IContext> &context) : IOutputUnit(id, context) {}

    void Apply(const std::vector<flux::NeuralOutput> &outputs) const override
    {
        cout << "Black box activation result:" << endl;
        for (const auto &output : outputs) {
            cout << output.GetOutput().GetId() << ": " << output.GetValue() << endl;
        }
    }
};

int main()
{
    cout << "Starting black box test..." << endl;
    auto context = std::make_shared<UnitContext>("Empty Context");
    SingleActivityBlackBox blackBox("test", context);

    NeuralInputId inputA("a");
    NeuralInputId inputB("b");
    set<NeuralInputId> inputIds = {inputA, inputB};
    auto manualInput = std::make_shared<ManualRawInputSensor>("xor_emu", context, inputIds, true);

    manualInput->SetInputsSequence( vector<vector<NeuralInput>>
    {
        (vector<NeuralInput> { NeuralInput(inputA, 0), NeuralInput(inputB, 0) }),
        (vector<NeuralInput> { NeuralInput(inputA, 1), NeuralInput(inputB, 0) }),
        (vector<NeuralInput> { NeuralInput(inputA, 0), NeuralInput(inputB, 1) }),
        (vector<NeuralInput> { NeuralInput(inputA, 1), NeuralInput(inputB, 1) })
    });

    blackBox.AddRawInput(manualInput);

    set<NeuralOutputId> outputIds = { NeuralOutputId("xor_value") };
    auto neatActivity = std::make_shared<NeatActivityUnit>("xor", context, inputIds, outputIds);

    Genome<> test(2, 1, 0, 0, 0.2, 1, 0.2, 1, 0.05, 0.2, 0.2, false);
    test.Mutate();

    std::stringstream stream;
    boost::archive::binary_oarchive oar(stream);
    oar << test;
    neatActivity->UpdateScheme(stream);

    blackBox.AddActivity(neatActivity);

    auto output = std::make_shared<DebugOutputUnit>("out", context);
    blackBox.AddOutput(output);

    cout << "Initializing complete. Enabling black box:" << endl;
    blackBox.Step();
    blackBox.Step();
    blackBox.Step();
    blackBox.Step();
    blackBox.Step();
    cout << "Completed." << endl;
}
