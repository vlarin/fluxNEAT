#ifndef MLPACK_METHODS_HNEAT_SUBSTRATE_HPP
#define MLPACK_METHODS_HNEAT_SUBSTRATE_HPP

#include <mlpack/prereqs.hpp>

#include "../neat/acyclic_net.hpp"
#include "acyclic_net.hpp"
#include "gene.hpp"

namespace mlpack {
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

    inline Substrate() {}

    inline Substrate(const size_t dimensionsCount,
                     const LayoutType layoutType,
                     const std::vector<size_t> &substrateLayers,
                     hyperneat::AcyclicNet<ActivationFunction> &cppn,
                     std::map<size_t, std::map<size_t, ConnectionGene>> cppnDirectedGraph,
                     std::map<size_t, size_t> cppnActivationGraph,
                     std::vector<size_t> cppnNodeDepths) :
                     isInitialized(true),
                     dimensionsCount(dimensionsCount),
                     layoutType(layoutType),
                     substrateLayers(substrateLayers),
                     cppnDirectedGraph(cppnDirectedGraph),
                     cppnActivationGraph(cppnActivationGraph),
                     cppnNodeDepths(cppnNodeDepths) {
        size_t nodeCount = 1;
        for (int i = 0; i < substrateLayers.size(); i++)
        {
            nodeCount += substrateLayers[i];
        }

        ann = neat::AcyclicNet<ActivationFunction>(nodeCount, substrateLayers[0], substrateLayers[substrateLayers.size() - 1], 1.0);

        //initializing node depth
        nodeDepths.push_back(0);
        int nodeIndex = 0;
        for (int i = 0; i < substrateLayers.size(); i++)
        {
            for (int j = nodeIndex; j < nodeIndex + substrateLayers[i]; j++)
            {
                nodeDepths.push_back(i);
            }
            nodeIndex += substrateLayers[i];
        }

        //building substrate
        directedGraph.insert(std::make_pair(0, std::map<size_t, neat::ConnectionGene>())); //bias node
        float hyperLengthDelta = 1.0f / substrateLayers.size();
        float hyperLength = 0.0f;
        nodeIndex = 0;
        for (int i = 0; i < substrateLayers.size() - 1; i++)
        {
            int endSizeLength = (int)(pow(substrateLayers[i], 1.0 / dimensionsCount));
            int endSize = (int)(pow(endSizeLength, dimensionsCount));
            int nextLayerEndSizeLength = (int)(pow(substrateLayers[i + 1], 1.0 / dimensionsCount));
            int nextLayerEndSize = (int)(pow(nextLayerEndSizeLength, dimensionsCount));
            for (int j = nodeIndex; j < nodeIndex + endSize; j++)
            {
                directedGraph.insert(std::make_pair(j + 1, std::map<size_t, neat::ConnectionGene>()));
                auto from = GetNeuronCubePosition(j, endSizeLength, hyperLength);
                for (int k = nodeIndex + substrateLayers[i]; k < nodeIndex + substrateLayers[i] + nextLayerEndSize; k++)
                {
                    if (i == 0 && j == 0 && k == nodeIndex + substrateLayers[i])
                    {
                        neat::ConnectionGene bconn(0, 1.0, 0, k + 1, true);
                        directedGraph[0].insert(std::make_pair(k + 1, bconn));
                    }

                    auto to = GetNeuronCubePosition(k, nextLayerEndSizeLength, hyperLength + hyperLengthDelta);

                    arma::vec weightInput(from.size() * 2, arma::fill::zeros);
                    for (int l = 0; l < from.size(); l++)
                    {
                        weightInput(l) = from(l);
                        weightInput(from.size() + l) = to(l);
                    }

                    arma::vec weightOutput(1, arma::fill::zeros);
                    cppn.Evaluate(weightInput, weightOutput, cppnDirectedGraph, cppnActivationGraph, cppnNodeDepths);
                    neat::ConnectionGene conn(0, weightOutput(0), j + 1, k + 1, true);
                    directedGraph[j + 1].insert(std::make_pair(k + 1, conn));
                }
            }
            hyperLength += hyperLengthDelta;
            nodeIndex = nodeIndex + substrateLayers[i];
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
            return;
        }

        ann.Evaluate(input, output, directedGraph, nodeDepths);
    }

    bool isInitialized;

private:

    size_t dimensionsCount;

    LayoutType layoutType;

    std::vector<size_t> substrateLayers;

    std::map<size_t, std::map<size_t, neat::ConnectionGene>> directedGraph;
    std::vector<size_t> nodeDepths;

    neat::AcyclicNet<ActivationFunction> ann;

    std::map<size_t, std::map<size_t, ConnectionGene>> cppnDirectedGraph;
    std::map<size_t, size_t> cppnActivationGraph;
    std::vector<size_t> cppnNodeDepths;
};

} // namespace neat
} // namespace mlpack

#endif
