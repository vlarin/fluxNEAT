#ifndef MLPACK_METHODS_HNEAT_GENE_HPP
#define MLPACK_METHODS_HNEAT_GENE_HPP

#include <mlpack/prereqs.hpp>

#include "../neat/acyclic_net.hpp"
#include "acyclic_net.hpp"
#include "gene.hpp"

namespace mlpack{
namespace hyperneat /** NeuroEvolution of Augmenting Topologies */ {

/**
 * A class that represents substrate of ANN generated using pattern producing network (CPPN)
 */
template <class ActivationFunction>
class Substrate {
public:
    enum LayoutType {
        CubeTopLeftToBottomRightLayout,
        RadialLayout
    };

    inline Substrate(const size_t dimensionsCount,
                     const LayoutType layoutType,
                     const std::vector<size_t> &substrateLayers,
                     const hyperneat::AcyclicNet<ActivationFunction> &cppn,
                     std::map<size_t, std::map<size_t, ConnectionGene>> cppnDirectedGraph,
                     std::map<size_t, size_t> cppnActivationGraph,
                     std::vector<size_t> cppnNodeDepths) :
                     isInitialized(true),
                     dimensionsCount(dimensionsCount),
                     layoutType(layoutType),
                     substrateLayers(substrateLayers),
                     cppn(cppn),
                     cppnDirectedGraph(cppnDirectedGraph),
                     cppnActivationGraph(cppnActivationGraph),
                     cppnNodeDepths(cppnNodeDepths) {
        size_t nodeCount = 0;
        for (int i = 0; i < substrateLayers.size(); i++)
        {
            nodeCount += substrateLayers[i];
        }

        ann = AcyclicNet<ActivationFunction>(nodeCount, substrateLayers[0], substrateLayers[substrateLayers.size() - 1], 1.0);

        //initializing node depth
        for (int i = 0; i < substrateLayers.size(); i++)
        {
            int startSize = i > 0 ? substrateLayers[i - 1] : 0;
            for (int j = startSize; j < substrateLayers[i]; j++)
            {
                nodeDepths.push_back(i);
            }
        }

        //building substrate
        float hyperLengthDelta = 1.0f / substrateLayers.size();
        float hyperLength = 0.0f;
        for (int i = 0; i < substrateLayers.size() - 1; i++)
        {
            int startSize = i > 0 ? substrateLayers[i - 1] : 0;
            int endSizeLength = (int)(pow(substrateLayers[i], 1.0 / dimensionsCount));
            int endSize = (int)(pow(endSizeLength, dimensionsCount));
            int nextLayerEndSizeLength = (int)(pow(substrateLayers[i + 1], 1.0 / dimensionsCount));
            int nextLayerEndSize = (int)(pow(nextLayerEndSizeLength, dimensionsCount));
            for (int j = startSize; j < endSize; j++)
            {
                directedGraph.insert(std::make_pair(j, std::map<size_t, ConnectionGene>()));
                auto from = GetNeuronCubePosition(j, endSizeLength, hyperLength);
                for (int k = substrateLayers[i]; k < nextLayerEndSize; k++)
                {
                    auto to = GetNeuronCubePosition(k, nextLayerEndSizeLength, hyperLength + hyperLengthDelta);

                    arma::vec weightInput(from.size() * 2, arma::fill::zeros);
                    for (int l = 0; l < from.size(); l++)
                    {
                        weightInput(l) = from(l);
                        weightInput(from.size() + l) = to(l);
                    }

                    arma::vec weightOutput(1, arma::fill::zeros);
                    cppn.Evaluate(weightInput, weightOutput, cppnDirectedGraph, cppnActivationGraph, cppnNodeDepths);
                    ConnectionGene conn(0, weightOutput(0), 0, 0, 0, true);
                    directedGraph[j].insert(std::make_pair(k, conn));
                }
            }
            hyperLength += hyperLengthDelta;
        }
    }

    inline arma::vec GetNeuronCubePosition(int index, int length, float hyperPosition)
    {
        arma::vec position(dimensionsCount + 1, arma::fill::zeros);
        int div = 1;
        int modulus = length;
        for (int i = 0; i < dimensionsCount; i++)
        {
            float p = ((index / div) % modulus) / (float)length;
            position(i) = p;
            div = modulus;
            modulus *= length;
        }

        position(position.size() - 1) = hyperPosition;
        return position;
    }

    inline void Evaluate(const arma::vec &input, arma::vec &output)
    {
        if (!isInitialized)
        {
            return arma::vec(0);
        }

        ann.Evaluate(input, output, directedGraph, nodeDepths);
    }

    bool isInitialized;

private:

    size_t dimensionsCount;

    LayoutType layoutType;

    std::vector<size_t> substrateLayers;

    std::map<size_t, std::map<size_t, ConnectionGene>> &directedGraph;
    std::vector<size_t> &nodeDepths;

    AcyclicNet<ActivationFunction> cppn;
    neat::AcyclicNet<ActivationFunction> ann;

    std::map<size_t, std::map<size_t, ConnectionGene>>& cppnDirectedGraph;
    std::map<size_t, size_t> cppnActivationGraph;
    std::vector<size_t>& cppnNodeDepths;
};

} // namespace neat
} // namespace mlpack

#endif
