// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.

#include "PlatformAndBoxesTrainer.h"

#include <utility>
#include <sstream>
#include <fstream>

#include "GameFramework/Actor.h"

#include <Engine/World.h>
#include <Engine/StaticMesh.h>

#include "PlatformBoxes/PlatformInputSensorUnit.h"
#include "PlatformBoxes/PlatformMovementOutputUnit.h"
#include "PlatformBoxes/PlatformBoxesContext.h"
#include "Engine/Engine.h"

using namespace flux;
using namespace std;

class FallingBoxesEvaluationUnit : public IEvaluationOutputUnit
{
public:
	FallingBoxesEvaluationUnit(const string &id, const shared_ptr<PlatformBoxesContext> &context) : IEvaluationOutputUnit(id, context)
	{}

	void Apply(const std::vector<flux::NeuralOutput> &outputs) const override
	{
	}

	bool IsEvaluationCompleted() override
	{
		const auto sandbox = std::static_pointer_cast<PlatformBoxesContext>(GetContext());
		return sandbox->Sandbox().IsEvaluationCompleted();
	}

	float_fl GetAggregatedFitness() override
	{
		const auto sandbox = std::static_pointer_cast<PlatformBoxesContext>(GetContext());
		return sandbox->Sandbox().GetAggregatedFitness();
	}

	void Reset() override
	{
	}

	shared_ptr<IContextUnit> Clone(std::shared_ptr<IContext> context) const override
	{
		return CloneToContext<FallingBoxesEvaluationUnit>(context);
	}
};

const int GridWidth = 4;

class ContextRegistry : public IContextRegistry
{
public:
	explicit ContextRegistry(APlatformAndBoxesTrainer *trainer) : _trainer(std::move(trainer))
	{
		for (int i = 0; i < trainer->ParallelEvaluationsAmount; i++)
		{
			_indexes.Add(i);
		}
	}

	shared_ptr<IContext> RetrieveContext() override
	{
		UWorld* world = _trainer->GetWorld();
		APlatformAndBoxesSandbox* sandbox = nullptr;


		uint16 freeCell = _indexes.Last();
		_indexes.RemoveAtSwap(_indexes.Num() - 1);
		
		if (world && _trainer->SandboxPrefab)
		{
			auto x = (freeCell % GridWidth) * _trainer->SpawnGridSize;
			auto y = static_cast<int>(freeCell / GridWidth) * _trainer->SpawnGridSize;
			auto z = _trainer->GetActorLocation().Z;
			FActorSpawnParameters spawnParameters;
			spawnParameters.Owner = _trainer;
			sandbox = world->SpawnActor<APlatformAndBoxesSandbox>(_trainer->SandboxPrefab, FVector(x, y, z), FRotator::ZeroRotator, spawnParameters);
			//sandbox->AttachToActor(_trainer, FAttachmentTransformRules::KeepRelativeTransform);
			sandbox->MovementAcceleration = _trainer->MovementAcceleration;

			_trainer->CurrentEvaluatedSandboxes.Add(sandbox);
		}
		else
		{
			UE_LOG(LogActor, Error, TEXT("Unable to spawn Platform!"));
		}
		return std::make_shared<PlatformBoxesContext>(Cast<APlatformAndBoxesSandbox>(sandbox), freeCell);
	}

	void ReleaseContext(const IContext &context) override
	{
		auto &platformContext = reinterpret_cast<const PlatformBoxesContext&>(context);
		const auto sandbox = &platformContext.Sandbox();
		_trainer->CurrentEvaluatedSandboxes.Remove(sandbox);
		sandbox->CleanUp();
		sandbox->Destroy();
		_indexes.Add(platformContext.GetIndex());
	}

	~ContextRegistry() = default;

private:
	TArray<uint16> _indexes;
	APlatformAndBoxesTrainer *_trainer;
};

// Sets default values
APlatformAndBoxesTrainer::APlatformAndBoxesTrainer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

static TArray<uint8> LoadFromFile(FString name)
{
	TArray<uint8> res;
	FString SaveDirectory = FString("E:/");
	FString FileName = name;
	bool AllowOverwriting = true;

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// CreateDirectoryTree returns true if the destination
	// directory existed prior to call or has been created
	// during the call.
	if (PlatformFile.CreateDirectoryTree(*SaveDirectory))
	{
		// Get absolute file path
		FString AbsoluteFilePath = SaveDirectory + "/" + FileName;

		// Allow overwriting or file doesn't already exist
		if (AllowOverwriting || !PlatformFile.FileExists(*AbsoluteFilePath))
		{
			FFileHelper::LoadFileToArray(res, *AbsoluteFilePath);
		}
	}
	return res;
}

void APlatformAndBoxesTrainer::ChangeTrainingMode(TrainingMode newMode)
{
	_trainingMode = newMode;
	if (newMode == TM_TRAINING)
	{
		auto contextTemplate = std::make_shared<PlatformBoxesContext>(_sandbox, 0);
		auto evaluator = std::make_shared<FallingBoxesEvaluationUnit>("platform_evaluator", contextTemplate);

		std::shared_ptr<ContextRegistry> registry = std::make_shared<ContextRegistry>(this);
		_trainer = std::make_unique<NeatActivityTrainer>(PopulationSize, SpeciesAmount, ParallelEvaluationsAmount, 6, 10, GetEvolutionParameters(),
			_neatActivity,
			evaluator,
			_blackBox,
			registry);

		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Turquoise, TEXT("Initializing complete. Enabling platform sandboxes:"));
		UE_LOG(LogTemp, Log, TEXT("Initializing complete. Enabling platform sandboxes:"));

		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Turquoise, TEXT("Enabled. Starting training..."));
		UE_LOG(LogTemp, Log, TEXT("Enabled. Starting training..."));
		_trainer->Step();
	}
	else if (newMode == TM_REVIEW)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Turquoise, TEXT("Enabled. Loading champion..."));
		UE_LOG(LogTemp, Log, TEXT("Enabled. Loading champion..."));
		auto data = LoadFromFile(TestingEpoch);
		stringstream stream(std::string(reinterpret_cast<const char *>(data.GetData()), data.Num()));
		_neatActivity->UpdateScheme(stream);
	}
	else
	{
		_epoch = 0;
		_trainer.reset();
		for (auto &sandbox : CurrentEvaluatedSandboxes)
		{
			sandbox->CleanUp();
			sandbox->Destroy();
		}

		CurrentEvaluatedSandboxes.Reset();
	}
}

// Called when the game starts or when spawned
void APlatformAndBoxesTrainer::BeginPlay()
{
	Super::BeginPlay();

	_epoch = 0;
	_trainingMode = TM_DISABLED;

	UWorld* world = GetWorld();
	if (world && SandboxPrefab)
	{
		FActorSpawnParameters spawnParameters;
		spawnParameters.Owner = this;
		_sandbox = world->SpawnActor<APlatformAndBoxesSandbox>(SandboxPrefab, FVector(-1000, -1000, 50), FRotator::ZeroRotator, spawnParameters);
		_sandbox->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		_sandbox->IsTesting = true;
		_sandbox->MovementAcceleration = MovementAcceleration;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to init..."));
		UE_LOG(LogTemp, Error, TEXT("Failed to init..."));
		return;
	}

	auto contextTemplate = std::make_shared<PlatformBoxesContext>(_sandbox, 0);
	_blackBox = std::make_shared<SingleActivityBlackBox>("platform_sandbox", contextTemplate);

	auto environment = std::make_shared<PlatformInputSensorUnit>("sandbox_environment", contextTemplate);

	_blackBox->AddRawInput(environment);

	_neatActivity = std::make_shared<NeatActivityUnit>("platform_activity", contextTemplate, GInputIds, GOutputIds);

	_blackBox->AddActivity(_neatActivity);

	auto output = std::make_shared<PlatformMovementOutputUnit>("platform_movement", contextTemplate);

	_blackBox->AddOutput(output);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, TEXT("Initialized platform sandbox..."));
	UE_LOG(LogTemp, Log, TEXT("Initialized platform sandbox..."));
}

static void SaveToFile(int epoch, const char *data, int32 size)
{
	FString SaveDirectory = FString("E:/");
	FString FileName = FString::Printf(TEXT("champion_%d.txt"), epoch);
	bool AllowOverwriting = true;

	TArrayView<const uint8> bytes(reinterpret_cast<const uint8 *>(data), size);
	
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// CreateDirectoryTree returns true if the destination
	// directory existed prior to call or has been created
	// during the call.
	if (PlatformFile.CreateDirectoryTree(*SaveDirectory))
	{
		// Get absolute file path
		FString AbsoluteFilePath = SaveDirectory + "/" + FileName;

		// Allow overwriting or file doesn't already exist
		if (AllowOverwriting || !PlatformFile.FileExists(*AbsoluteFilePath))
		{
			FFileHelper::SaveArrayToFile(bytes, *AbsoluteFilePath);
		}
	}
}
// Called every frame
void APlatformAndBoxesTrainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_trainingMode == TM_REVIEW)
	{
		_blackBox->Step();
		return;
	}
	
	if (_trainingMode == TM_TRAINING)
	{
		if (_trainer->IsEpochCompleted())
		{
			++_epoch;

			auto msg = FString::Format(TEXT("Completed {0} epoch. Champion - {1}"), { _epoch, _trainer->GetChampionFitness() });
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, msg);
			UE_LOG(LogTemp, Log, TEXT("Completed %d epoch. Champion - %f"), _epoch, _trainer->GetChampionFitness());
			
			std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
			_trainer->SaveCurrentChampionActivity(stream);

			auto data = stream.str();
			SaveToFile(_epoch, data.c_str(), data.length());
			
			_trainer->Step();
			return;
		}
		
		if (_trainer->GetChampionFitness() < TargetFitness)
		{
			_trainer->Step();
		}
		else
		{
			auto msg = FString::Format(TEXT("Training completed by {0} epochs."), { _epoch });
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, msg);
			UE_LOG(LogTemp, Log, TEXT("Training completed by %d epochs."), _epoch);

			std::stringstream stream;
			_trainer->SaveCurrentChampionActivity(stream);
			_blackBox->UpdateChildScheme("platform_activity", stream);
			
			_trainingMode = TM_REVIEW;
			_trainer.reset();
		}
	}

}
