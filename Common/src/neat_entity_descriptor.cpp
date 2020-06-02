#include <flux/neat/neat_entity_descriptor.h>

#include <sstream>

flux::NeatEntityDescriptor::NeatEntityDescriptor(int32_t id, int32_t specieId, size_t complexity, float_fl fitness,
        size_t neuronCount, size_t connectionsCount,
        Neuron *neurons, NeuronConnection *neuronConnections)
 : Id(id), SpecieId(specieId), Complexity(complexity), Fitness(fitness),
 NeuronCount(neuronCount), ConnectionsCount(connectionsCount),
 Neurons(neurons), NeuronConnections(neuronConnections) {}

flux::NeatEntityDescriptor::~NeatEntityDescriptor()
{
    delete [] Neurons;
    delete [] NeuronConnections;
}

std::string flux::NeatEntityDescriptor::ToString()
{
    std::stringstream s;

    s << "Element: " << Id << " SpecieId: " << SpecieId << " Neuron Count: "
    << NeuronCount << " Complexity: " << Complexity << " Fitness: " << Fitness << std::endl;

    for (int i = 0 ; i < NeuronCount; i++)
    {
        s << "Neuron: " << Neurons[i].Id << " Type: " << Neurons[i].Type << " Position: " << Neurons[i].NormalizedPosition << std::endl;
    }

    for (int i = 0 ; i < ConnectionsCount; i++)
    {
        s << "Connection From: " << NeuronConnections[i].OriginId << " To: " << NeuronConnections[i].DestinationId
        << " W: " << NeuronConnections[i].Weight << " M: " << NeuronConnections[i].Meta << std::endl;
    }

    s << std::endl;
    return s.str();
}

flux::NeatEntityDescriptor::NeatEntityDescriptor(const flux::NeatEntityDescriptor &other)
        : Id(other.Id), SpecieId(other.SpecieId), Complexity(other.Complexity), Fitness(other.Fitness),
          NeuronCount(other.NeuronCount), ConnectionsCount(other.ConnectionsCount)
{
	if (NeuronCount > 0)
	{
		Neurons = new NeatEntityDescriptor::Neuron[NeuronCount];

		memcpy(Neurons, other.Neurons, sizeof(Neuron) * NeuronCount);
	}
	else
	{
		Neurons = nullptr;
	}
	
	if (ConnectionsCount > 0)
	{
		NeuronConnections = new NeatEntityDescriptor::NeuronConnection[ConnectionsCount];
		memcpy(NeuronConnections, other.NeuronConnections, sizeof(NeuronConnection) * ConnectionsCount);
	}
	else
	{
		NeuronConnections = nullptr;
	}
}

flux::NeatEntityDescriptor &flux::NeatEntityDescriptor::operator=(const flux::NeatEntityDescriptor &other)
{
    if (this == &other)
    {
        return *this;
    }

    Id = other.Id;
    SpecieId = other.SpecieId;
    Complexity = other.Complexity;
    Fitness = other.Fitness;
    NeuronCount = other.NeuronCount;
    ConnectionsCount = other.ConnectionsCount;

	delete[] Neurons;
	delete[] NeuronConnections;

	if (NeuronCount > 0)
	{
		Neurons = new NeatEntityDescriptor::Neuron[NeuronCount];
		memcpy(Neurons, other.Neurons, sizeof(Neuron) * NeuronCount);
	}
	else
	{
		Neurons = nullptr;
	}

	if (ConnectionsCount > 0)
	{
		NeuronConnections = new NeatEntityDescriptor::NeuronConnection[ConnectionsCount];
		memcpy(NeuronConnections, other.NeuronConnections, sizeof(NeuronConnection) * ConnectionsCount);
	}
	else
	{
		NeuronConnections = nullptr;
	}
	
    return *this;
}
