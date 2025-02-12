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
    
    std::set<NeuralNodeId> GetOutputIds() const override { return std::set<NeuralNodeId> { } ; }
    
	void Apply(const std::map<flux::NeuralNodeId, flux::NeuralNode> &outputs) const override
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
			sandbox->MovementAcceleration = 10;

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

static TArray<uint8> LoadFromFile(FString workingDir, FString name)
{
	TArray<uint8> res;
	FString SaveDirectory = workingDir;
	FString FileName = name;

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// CreateDirectoryTree returns true if the destination
	// directory existed prior to call or has been created
	// during the call.
	if (PlatformFile.CreateDirectoryTree(*SaveDirectory))
	{
		// Get absolute file path
		FString AbsoluteFilePath = SaveDirectory + "/" + FileName;

		// Allow overwriting or file doesn't already exist
		if (PlatformFile.FileExists(*AbsoluteFilePath))
		{
			FFileHelper::LoadFileToArray(res, *AbsoluteFilePath);
		}
        else
        {
            UE_LOG(LogActor, Error, TEXT("Unable to open champion!"))
        }
	}
	return res;
}

static void ConvertDescriptorsToUEntries(TArray<UNeatEntityEntry*> &Array, const flux::NeatActivityTrainer &Trainer)
{
	std::vector<NeatEntityDescriptor> Entities = Trainer.GetCurrentEntities();
	Array.Init(nullptr, Entities.size());
	for (int i = 0; i < Entities.size(); i++)
	{
		UNeatEntityEntry *Entry = NewObject<UNeatEntityEntry>();
		Entry->Id = Entities[i].Id;
		Entry->SpeciesId = Entities[i].SpecieId;
		Entry->Complexity = Entities[i].Complexity;
		Entry->Fitness = Entities[i].Fitness;
		Array[i] = Entry;
	}
}

void APlatformAndBoxesTrainer::ChangeTrainingMode(TrainingMode newMode)
{
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

		_trainingMode = newMode;
		UE_LOG(LogTemp, Log, TEXT("Initializing complete. Enabling platform sandboxes:"));
        
		_trainer->Step(0.0001);
        UE_LOG(LogTemp, Log, TEXT("Enabled. Starting training..."));
	}
	else if (newMode == TM_REVIEW)
	{
		UE_LOG(LogTemp, Log, TEXT("Enabled. Loading champion..."));
		auto data = LoadFromFile(WorkingDirPath, TestingEpoch);
		if (data.Num() > 0)
		{
			stringstream stream(std::string(reinterpret_cast<const char *>(data.GetData()), data.Num()));
			_neatActivity->UpdateScheme(stream);
			_trainingMode = newMode;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Unable to load chamption!"));
		}
	}
	else
	{
		_trainingMode = newMode;
		_trainer.reset();
		for (auto &sandbox : CurrentEvaluatedSandboxes)
		{
			sandbox->CleanUp();
			sandbox->Destroy();
		}

		CurrentEvaluatedSandboxes.Reset();
	}
}

void APlatformAndBoxesTrainer::SelectNeuralMap(int Index)
{
	const auto Entities = _trainer->GetCurrentEntities();
	if (Index < 0 || Index >= Entities.size())
	{
		UE_LOG(LogActor, Error, TEXT("Unable to select neural map! Index error!"));
		return;
	}

	const auto Target = Entities[Index];
	const float MaxSize = 6;
	FNeuralMap Map;
	TArray<FVector2D> NeuronPositions;
	NeuronPositions.Init(FVector2D::ZeroVector, Target.NeuronCount);
	for (int i = 0; i < Target.NeuronCount; i++)
	{
		if (Target.Neurons[i].Type == NeatEntityDescriptor::INPUT || Target.Neurons[i].Type == NeatEntityDescriptor::BIAS)
		{
			NeuronPositions[i] = FVector2D(Target.Neurons[i].NormalizedPosition, (Map.InputNodes.Num()) / MaxSize);
			Map.InputNodes.Add(NeuronPositions[i]);
		}
		else if (Target.Neurons[i].Type == NeatEntityDescriptor::HIDDEN)
		{
			NeuronPositions[i] = FVector2D(Target.Neurons[i].NormalizedPosition, (Map.HiddenNodes.Num()) / MaxSize);
			Map.HiddenNodes.Add(NeuronPositions[i]);
		}
		else
		{
			NeuronPositions[i] = FVector2D(Target.Neurons[i].NormalizedPosition, (Map.OutputNodes.Num()) / MaxSize);
			Map.OutputNodes.Add(NeuronPositions[i]);
		}
	}

	for (int i = 0; i < Target.ConnectionsCount; i++)
	{
		FNeuralConnection Connection;
		Connection.From = NeuronPositions[Target.NeuronConnections[i].OriginId];
		Connection.To = NeuronPositions[Target.NeuronConnections[i].DestinationId];
		Connection.Weight = FMath::Abs(Target.NeuronConnections[i].Weight);
		if (Connection.Weight < 0.6)
		{
			continue;
		}
		
		if (Connection.Weight > 1)
		{
			Connection.Weight = FMath::Sqrt(Connection.Weight);
		} 
		
		Connection.IsPositive = Target.NeuronConnections[i].Weight > 0;
		
		Map.Connections.Add(Connection);
	}

	CurrentNeuralMap = Map;
}

int APlatformAndBoxesTrainer::GetCurrentEpoch() const
{
	if (!_trainer)
	{
		return 0;
	}
	
	return static_cast<int>(_trainer->GetCurrentEpoch());
}

bool APlatformAndBoxesTrainer::IsComplexifying() const
{
	if (!_trainer)
	{
		return false;
	}
	
	return _trainer->IsComplexifying();
}

float APlatformAndBoxesTrainer::GetBestFitness() const
{
	if (!_trainer)
	{
		return 0;
	}

	return _trainer->GetBestFitness();
}

float APlatformAndBoxesTrainer::GetChampionFitness() const
{
	if (!_trainer)
	{
		return 0;
	}

	return _trainer->GetChampionFitness();
}

float APlatformAndBoxesTrainer::GetMeanFitness() const
{
	if (!_trainer)
	{
		return 0;
	}

	return _trainer->GetMeanFitness();
}

float APlatformAndBoxesTrainer::GetMeanComplexity() const
{
	if (!_trainer)
	{
		return 0;
	}

	return _trainer->GetMeanComplexity();
}

float APlatformAndBoxesTrainer::GetMeanEvaluationDuration() const
{
	if (!_trainer)
	{
		return 0;
	}

	return _trainer->GetMeanEvaluationDuration();
}

float APlatformAndBoxesTrainer::GetEvaluationPerSec() const
{
	if (!_trainer)
	{
		return 0;
	}

	return _trainer->GetEvaluationPerSec();
}

float APlatformAndBoxesTrainer::GetTotalEvaluations() const
{
	if (!_trainer)
	{
		return 0;
	}

	return _trainer->GetTotalEvaluations();
}

float APlatformAndBoxesTrainer::GetTotalEvaluationTime() const
{
	if (!_trainer)
	{
		return 0;
	}

	return _trainer->GetTotalEvaluationTime();
}

// Called when the game starts or when spawned
void APlatformAndBoxesTrainer::BeginPlay()
{
	Super::BeginPlay();

	_trainingMode = TM_DISABLED;
	CurrentSpeciesDistribution.Init(0, 10);

	UWorld* world = GetWorld();
	if (world && SandboxPrefab)
	{
		FActorSpawnParameters spawnParameters;
		spawnParameters.Owner = this;
		_sandbox = world->SpawnActor<APlatformAndBoxesSandbox>(SandboxPrefab, FVector(-1000, -1000, 50), FRotator::ZeroRotator, spawnParameters);
		_sandbox->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		_sandbox->IsTesting = true;
		_sandbox->MovementAcceleration = 10;
	}
	else
	{
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

	UE_LOG(LogTemp, Log, TEXT("Initialized platform sandbox..."));
}

static void SaveToFile(int epoch, const char *data, int32 size, FString workingDir)
{
	FString SaveDirectory = workingDir;
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
			UE_LOG(LogTemp, Log, TEXT("Completed %d epoch. Champion - %f"), (int)_trainer->GetCurrentEpoch(), _trainer->GetChampionFitness());
			
			std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
			_trainer->SaveCurrentChampionActivity(stream);
			ConvertDescriptorsToUEntries(_currentEntities, *_trainer);
			EntitiesList->SetListItems(_currentEntities);

			CurrentSpeciesDistribution.Init(0, 10);
			auto entities = _trainer->GetCurrentEntities();
			for (const auto &e : entities)
			{
				CurrentSpeciesDistribution[e.SpecieId]++;
			}
			
			float aggregated = 0;
			for (int i = 0; i < SpeciesAmount && i < 10; i++)
			{
				CurrentSpeciesDistribution[i] = (CurrentSpeciesDistribution[i]) + aggregated;
				aggregated = CurrentSpeciesDistribution[i];
			}

			auto data = stream.str();
			SaveToFile(static_cast<int>(_trainer->GetCurrentEpoch()), data.c_str(), data.length(), WorkingDirPath);
			
			_trainer->Step(DeltaTime);
			return;
		}
		
		if (_trainer->GetChampionFitness() < TargetFitness)
		{
			_trainer->Step(DeltaTime);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Training completed by %d epochs."), (int)_trainer->GetCurrentEpoch());

			std::stringstream stream;
			_trainer->SaveCurrentChampionActivity(stream);
			_blackBox->UpdateChildScheme("platform_activity", stream);
			
			_trainingMode = TM_REVIEW;
			_trainer.reset();
		}
	}

}
