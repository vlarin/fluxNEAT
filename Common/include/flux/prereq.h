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
      #define EXPORTED __attribute__ ((dllexport))
    #else
      #define EXPORTED __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
#else
#ifdef __GNUC__
#define EXPORTED __attribute__ ((dllimport))
#else
#define EXPORTED __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#endif
#endif
#define NOT_EXPORTED
#else
#if __GNUC__ >= 4
    #define EXPORTED __attribute__ ((visibility ("default")))
    #define NOT_EXPORTED  __attribute__ ((visibility ("hidden")))
  #else
    #define EXPORTED
    #define NOT_EXPORTED
  #endif
#endif

namespace flux {

    /* Defines standard floating point type used by fluxNEAT */
    using float_fl = std::remove_reference_t<decltype(arma::vec().at(0))>;

} // namespace flux
#endif //FLUXNEAT_PREREQ_H
