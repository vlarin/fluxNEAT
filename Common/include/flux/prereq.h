//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_PREREQ_H
#define FLUXNEAT_PREREQ_H

#include <cmath>
#include <armadillo>

#include <utility>

// Define EXPORTED for any platform
#if defined _WIN32 || defined __CYGWIN__
#ifdef FLUX_EXPORT
// Exporting...
    #ifdef __GNUC__
      #define FLUX_API __attribute__ ((dllexport))
    #else
      #define FLUX_API __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
#else
#ifdef __GNUC__
#define FLUX_API __attribute__ ((dllimport))
#else
#define FLUX_API __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#endif
#endif
#define FLUX_API_HIDE
#else
#if __GNUC__ >= 4
    #define FLUX_API __attribute__ ((visibility ("default")))
    #define FLUX_API_HIDE  __attribute__ ((visibility ("hidden")))
  #else
    #define FLUX_API
    #define FLUX_API_HIDE
  #endif
#endif

namespace flux {

    /* Defines standard floating point type used by fluxNEAT */
    using float_fl = std::remove_reference_t<decltype(arma::vec().at(0))>;

} // namespace flux
#endif //FLUXNEAT_PREREQ_H
