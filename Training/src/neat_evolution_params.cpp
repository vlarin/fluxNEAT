#include <flux/training/neat_evolution_params.h>

flux::NeatEvolutionParameters *CreateEvolutionParameters(
      const double bias,
      const double initialWeight,
      const double weightMutationProb,
      const double weightMutationSize,
      const double biasMutationProb,
      const double biasMutationSize,
      const double nodeAdditionProb,
      const double connAdditionProb,
      const double connDeletionProb,
      const double elitismProportion,
      const bool isAcyclic)
{
    auto *parameters = new flux::NeatEvolutionParameters();
    parameters->Bias = bias;
    parameters->InitialWeight = initialWeight;
    parameters->WeightMutationProb = weightMutationProb;
    parameters->WeightMutationSize = weightMutationSize;
    parameters->BiasMutationProb = biasMutationProb;
    parameters->BiasMutationSize = biasMutationSize;
    parameters->NodeAdditionProb = nodeAdditionProb;
    parameters->ConnAdditionProb = connAdditionProb;
    parameters->ConnDeletionProb = connDeletionProb;
    parameters->ElitismProportion = elitismProportion;
    parameters->IsAcyclic = isAcyclic;

    return parameters;
}

void flux::DeleteEvolutionParameters(flux::NeatEvolutionParameters *parameters)
{
    delete parameters;
}

flux::NeatEvolutionParameters::NeatEvolutionParameters(double weightMutationProb,
                                                       double connAdditionProb,
                                                       double connDeletionProb,
                                                       double biasMutationProb,
                                                       double nodeAdditionProb,
                                                       double bias,
                                                       double initialWeight,
                                                       double weightMutationSize,
                                                       double biasMutationSize,
                                                       double elitismProportion,
                                                       bool isAcyclic)

        : Bias(bias), InitialWeight(initialWeight), WeightMutationProb(weightMutationProb),
          WeightMutationSize(weightMutationSize), BiasMutationProb(biasMutationProb),
          BiasMutationSize(biasMutationSize), NodeAdditionProb(nodeAdditionProb), ConnAdditionProb(connAdditionProb),
          ConnDeletionProb(connDeletionProb), ElitismProportion(elitismProportion), IsAcyclic(isAcyclic) {}
