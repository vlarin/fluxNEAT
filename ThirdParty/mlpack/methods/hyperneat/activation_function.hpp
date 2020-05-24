/**
 * @file activation_function.hpp
 * @author Vlad Larin
 *
 * Definition of the Hyper NEAT activation function class.
 */
#ifndef MLPACK_METHODS_HNEAT_FUNC_HPP
#define MLPACK_METHODS_HNEAT_FUNC_HPP

#include <mlpack/prereqs.hpp>
#include <mlpack/methods/ann/activation_functions/identity_function.hpp>
#include <mlpack/methods/ann/activation_functions/hard_sigmoid_function.hpp>

namespace mlpack{
namespace hyperneat /** NeuroEvolution of Augmenting Topologies */ {

/**
 * Defines multiple activation function that covers symmetry, variation, periodic patterns, Used in Hyper NEAT
 */
class MultimodalActivationFunction
{
public:

    static double Size() { return 4; }

    /**
     * Computes the activation function.
     *
     * @param id Function type
     * @param x Input data.
     * @return f(x).
     */
    static double Fn(const size_t id, const double x)
    {
        switch (id) {
            case 0: return mlpack::ann::IdentityFunction::Fn(x); // translation
            case 1: return mlpack::ann::HardSigmoidFunction::Fn(x); //variation
            case 2: return sin(x); //periodic
            case 3: return exp(-(x*x)/2.0); //symmetry
        }
        return x;
    }
};

} // namespace neat
} // namespace mlpack

#endif
