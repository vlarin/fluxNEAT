// Fill out your copyright notice in the Description page of Project Settings.

#include "TrainingConsoleWidget.h"

#include <flux/common.h>
#include <flux/training.h>

#include <utility>
#include <sstream>
#include "Engine/Engine.h"

using namespace std;
using namespace flux;

class XorContext : public UnitContext
{
public:
	explicit XorContext(const std::shared_ptr<ManualRawInputSensor>& origin) : UnitContext("xor"),
		_inputStub(*std::static_pointer_cast<ManualRawInputSensor>(origin->Clone(origin->GetContext()))) {}

	std::vector<NeuralInput> getWorldInputs() { return  _inputStub.Fetch(); }

	~XorContext() = default;
private:
	ManualRawInputSensor _inputStub;
};

class DebugOutputUnit : public IEvaluationOutputUnit
{
public:
	DebugOutputUnit(const string &id, const shared_ptr<IContext> &context) : IEvaluationOutputUnit(id, context),
		_evalutionCount(0), _error(0) {}

	void Apply(const std::vector<flux::NeuralOutput> &outputs) const override
	{
		auto world = std::static_pointer_cast<XorContext>(GetContext())->getWorldInputs();
		float_fl answer = (world[0].GetValue() + world[1].GetValue()) * (!world[0].GetValue() + !world[1].GetValue());

		for (const auto &output : outputs)
		{
			_error += std::pow(answer - output.GetValue(), 2);
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

	~ContextRegistry() = default;

private:
	std::shared_ptr<ManualRawInputSensor> _origin;
};

UTrainingConsoleWidget::UTrainingConsoleWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UTrainingConsoleWidget::NativeConstruct()
{
	// Do some custom setup

	// Call the Blueprint "Event Construct" node
	Super::NativeConstruct();
}


void UTrainingConsoleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	// Make sure to call the base class's NativeTick function
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Do your custom tick stuff here
	if (_isTraining && _trainer)
	{
		if (_trainer->GetChampionFitness() < 3.95)
		{
			for (int i = 0; i < 100; i++)
			{
				_trainer->Step();

				if (_trainer->IsEpochCompleted())
				{
					++_epoch;

					auto msg = FString::Format(TEXT("Completed {0} epoch."),{ _epoch });
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, msg);
					UE_LOG(LogTemp, Log, TEXT("Completed %d epoch."), _epoch);
					_trainer->Step();
				}
			}
		}
		else
		{
			auto msg = FString::Format(TEXT("Training completed by {0} epochs."), { _epoch });
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, msg);
			UE_LOG(LogTemp, Log, TEXT("Training completed by %d epochs."), _epoch);

			std::stringstream stream;
			_trainer->SaveCurrentChampionActivity(stream);
			_blackBox->UpdateChildScheme("xor", stream);

			for (int i = 0; i < 4; ++i)
			{
				_blackBox->Step();

				msg = FString::Format(TEXT("A: {0} B: {1} O: {2}"), 
					{
						_blackBox->GetInputOf("a").GetValue(),
						_blackBox->GetInputOf("b").GetValue(),
						_blackBox->GetOutputOf("xor_value").GetValue()
					});
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, msg);
				UE_LOG(LogTemp, Log, TEXT("A: %f B: %f O: %f"),
					_blackBox->GetInputOf("a").GetValue(),
					_blackBox->GetInputOf("b").GetValue(),
					_blackBox->GetOutputOf("xor_value").GetValue());
			}

			_isTraining = false;
			_trainer.reset();
			_blackBox.reset();
		}
	}
}

void UTrainingConsoleWidget::OnTrainingClick()
{
	TrainXOR();
}

void UTrainingConsoleWidget::TrainXOR()
{
	_epoch = 0;
	_isTraining = false;
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, TEXT("Starting black box test..."));
	UE_LOG(LogTemp, Log, TEXT("Starting black box test..."));
	auto context = std::make_shared<UnitContext>("Empty Context");
	_blackBox = std::make_shared<SingleActivityBlackBox>("test", context);

	NeuralInputId inputA("a");
	NeuralInputId inputB("b");
	set<NeuralInputId> inputIds = { inputA, inputB };
	auto manualInput = std::make_shared<ManualRawInputSensor>("xor_emu", context, inputIds, true);

	manualInput->SetInputsSequence(vector<vector<NeuralInput>>
	{
		(vector<NeuralInput> { NeuralInput(inputA, 0), NeuralInput(inputB, 0) }),
			(vector<NeuralInput> { NeuralInput(inputA, 1), NeuralInput(inputB, 0) }),
			(vector<NeuralInput> { NeuralInput(inputA, 0), NeuralInput(inputB, 1) }),
			(vector<NeuralInput> { NeuralInput(inputA, 1), NeuralInput(inputB, 1) })
	});

	_blackBox->AddRawInput(manualInput);

	set<NeuralOutputId> outputIds = { NeuralOutputId("xor_value") };
	auto neatActivity = std::make_shared<NeatActivityUnit>("xor", context, inputIds, outputIds);

	_blackBox->AddActivity(neatActivity);

	auto output = std::make_shared<DebugOutputUnit>("out", context);
	//blackBox->AddOutput(output);

	std::shared_ptr<ContextRegistry> registry = std::make_shared<ContextRegistry>(manualInput);
	NeatEvolutionParameters evolutionParameters;
	_trainer = std::make_unique<NeatActivityTrainer>(150, 10, 1, 6, 10, evolutionParameters,
		neatActivity,
		output,
		_blackBox,
		registry);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, TEXT("Initializing complete. Enabling black box:"));
	UE_LOG(LogTemp, Log, TEXT("Initializing complete. Enabling black box:"));
	_isTraining = true;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, TEXT("Enabling. Starting training..."));
	UE_LOG(LogTemp, Log, TEXT("Enabling. Starting training..."));
	_trainer->Step();	
}
