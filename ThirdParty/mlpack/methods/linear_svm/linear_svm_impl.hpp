/**
 * @file linear_svm.cpp
 * @author Ayush Chamoli
 *
 * Implementation of Linear SVM.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_LINEAR_SVM_LINEAR_SVM_IMPL_HPP
#define MLPACK_METHODS_LINEAR_SVM_LINEAR_SVM_IMPL_HPP

// In case it hasn't been included yet.
#include "linear_svm.hpp"

namespace mlpack {
namespace svm {

template <typename MatType>
template <typename OptimizerType>
LinearSVM<MatType>::LinearSVM(
    const MatType& data,
    const arma::Row<size_t>& labels,
    const size_t numClasses,
    const double lambda,
    const double delta,
    const bool fitIntercept,
    OptimizerType optimizer) :
    numClasses(numClasses),
    lambda(lambda),
    delta(delta),
    fitIntercept(fitIntercept)
{
  Train(data, labels, numClasses, optimizer);
}

template <typename MatType>
LinearSVM<MatType>::LinearSVM(
    const size_t inputSize,
    const size_t numClasses,
    const double lambda,
    const double delta,
    const bool fitIntercept) :
    numClasses(numClasses),
    lambda(lambda),
    delta(delta),
    fitIntercept(fitIntercept)
{
  LinearSVMFunction<MatType>::InitializeWeights(parameters, inputSize,
      numClasses, fitIntercept);
}

template <typename MatType>
template <typename OptimizerType>
double LinearSVM<MatType>::Train(
    const MatType& data,
    const arma::Row<size_t>& labels,
    const size_t numClasses,
    OptimizerType optimizer)
{
  LinearSVMFunction<MatType> svm(data, labels, numClasses, lambda, delta,
      fitIntercept);
  if (parameters.is_empty())
    parameters = svm.InitialPoint();

  // Train the model.
  Timer::Start("linear_svm_optimization");
  const double out = optimizer.Optimize(svm, parameters);
  Timer::Stop("linear_svm_optimization");

  Log::Info << "LinearSVM::LinearSVM(): final objective of "
            << "trained model is " << out << "." << std::endl;

  return out;
}

template <typename MatType>
void LinearSVM<MatType>::Classify(
    const MatType& data,
    arma::Row<size_t>& labels) const
{
  arma::mat scores;
  Classify(data, labels, scores);
}

template <typename MatType>
void LinearSVM<MatType>::Classify(
    const MatType& data,
    arma::Row<size_t>& labels,
    arma::mat& scores) const
{
  Classify(data, scores);

  #if ARMA_VERSION_MAJOR > 7 || \
      (ARMA_VERSION_MAJOR == 7 && \
       ARMA_VERSION_MINOR >= 300)

  // Prepare necessary data
  labels.zeros(data.n_cols);

  labels = arma::conv_to<arma::Row<size_t>>::from(
      arma::index_max(scores));

  #else
  // Once the minimum version is Armadillo is increased, remove this part.

  // Prepare necessary data
  labels.zeros(data.n_cols);
  double maxScore = 0;

  // For each test input.
  for (size_t i = 0; i < data.n_cols; ++i)
  {
    // For each class.
    for (size_t j = 0; j < numClasses; ++j)
    {
      // If a higher class probability is encountered, change score.
      if (scores(j, i) > maxScore)
      {
        maxScore = scores(j, i);
        labels(i) = j;
      }
    }

    // Set maximum probability to zero for next input.
    maxScore = 0;
  }
  #endif
}

template <typename MatType>
void LinearSVM<MatType>::Classify(
    const MatType& data,
    arma::mat& scores) const
{
  if (data.n_rows != FeatureSize())
  {
    std::ostringstream oss;
    oss << "LinearSVM::Classify(): dataset has " << data.n_rows
        << " dimensions, but model has " << FeatureSize() << " dimensions!";
    throw std::invalid_argument(oss.str());
  }

  if (fitIntercept)
  {
    scores = parameters.rows(0, parameters.n_rows - 2).t() * data
        + arma::repmat(parameters.row(data.n_rows - 1).t(), 1,
        data.n_cols);
  }
  else
  {
    scores = parameters.t() * data;
  }
}

template <typename MatType>
template <typename VecType>
size_t LinearSVM<MatType>::Classify(const VecType& point) const
{
  arma::Row<size_t> label(1);
  Classify(point, label);
  return size_t(label(0));
}

template <typename MatType>
double LinearSVM<MatType>::ComputeAccuracy(
    const MatType& testData,
    const arma::Row<size_t>& testLabels) const
{
  arma::Row<size_t> labels;

  // Get predictions for the provided data.
  Classify(testData, labels);

  // Increment count for every correctly predicted label.
  size_t count = 0;
  for (size_t i = 0; i < labels.n_elem ; i++)
    if (testLabels(i) == labels(i))
      count++;

  // Return the accuracy.
  return (double) count / labels.n_elem;
}

} // namespace svm
} // namespace mlpack

#endif // MLPACK_METHODS_LINEAR_SVM_LINEAR_SVM_IMPL_HPP
