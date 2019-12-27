#include <mlpack/core.hpp>
#include <mlpack/methods/neat/neat.hpp>

using namespace arma;
using namespace mlpack;
using namespace mlpack::tree;
using namespace mlpack::neat;


class XORTask
{
public:
    std::vector<ConnectionGene> StartingGenome()
    {
        // The list of all connection genes.
        std::vector<ConnectionGene> connGeneList;

        // Emplace the connection genes in the starting genome. It is important to
        // provide the innovation IDs for the connection genes, starting with zero.
        connGeneList.emplace_back(ConnectionGene(0, 1, 0, 3));
        connGeneList.emplace_back(ConnectionGene(1, 1, 1, 3));
        connGeneList.emplace_back(ConnectionGene(2, 1, 2, 4));
        connGeneList.emplace_back(ConnectionGene(3, 1, 4, 3));

        // Return the gene list.
        return connGeneList;
    }

    double Evaluate(Genome<>& genome)
    {
        // Describes the input we will use for the XOR test.
        arma::mat input = {{0, 0, 1, 1},
                           {0, 1, 0, 1}};

        double error = 0;
        for (size_t i = 0; i < input.n_cols; i++)
        {
            // Choose input.
            arma::vec inputVec = input.col(i);

            // The genome evaluates the input provided, and returns the output of the // neural net.
            arma::vec output = genome.Evaluate(inputVec);

            // Find the error in the expected output.
            arma::vec answer = {(input(0, i) + input(1, i)) * (!input(0, i) +
                                                               !input(1, i))};
            error += std::pow(answer[0] - output[0], 2);
        }

        // Return the fitness of the genome based on the error.
        return 4 - error;
    }
};

void TournamentSelectionTest();

int main() {
    // Instantiate a task class instance.
    XORTask task;

    // Instantiate a NEAT class instance.
    NEAT<XORTask> model(task, 2, 1, 100, 150, 10);

    // Set the complexity threshold.
    model.ComplexityThreshold() = 6;

    // Optionally, we can set the number of simplification generations (this is 10  // by default)
    model.MaxSimplifyGen() = 15;

    // Initialize a genome to store the best genome.
    //Genome<> bestStepGenome;
    // Find the best genome from a single generation.
    //model.Step(bestStepGenome);
    //std::cout << "Step complexity - " << bestStepGenome.Complexity() << ". Step fitness - " << bestStepGenome.Fitness() << std::endl;

    // Initialize a genome to store the best genome.
    Genome<> bestGenome;
    // Train NEAT on the task and find the best genome.
    model.Train(bestGenome);

    std::cout << "Best complexity - " << bestGenome.Complexity() << ". Best fitness - " << bestGenome.Fitness() << std::endl;

    mlpack::data::Save("neat.bin", "genome", bestGenome, false);

    TournamentSelectionTest();
}

void TournamentSelectionTest()
{
    XORTask task;
    NEAT<XORTask, mlpack::ann::HardSigmoidFunction, TournamentSelection> model(task, 2, 1, 100, 150, 10);

    // Set the number of contenders and the base probability.
    model.ContenderNum() = 5;
    model.TournamentSelectProb() = 0.7;

    // Train NEAT on the XOR task.
    Genome<> bestGenome;
    model.Train(bestGenome);

    std::cout << "Best T complexity - " << bestGenome.Complexity() << ". Best T fitness - " << bestGenome.Fitness() << std::endl;
}