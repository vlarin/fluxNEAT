//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_PREREQ_H
#define FLUXNEAT_PREREQ_H

#include <cmath>

#include <string>

#include <map>
#include <set>
#include <vector>

#include <memory>
#include <utility>
#include <exception>

#include <flux/global_error.h>

// Define DLL export support for any platform
#define FLUX_SHARED 0
#if FLUX_SHARED
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
#else
#define FLUX_API
#define FLUX_API_HIDE
#endif

#if __cplusplus <= 201103L && _MSC_VER < 1500
namespace std {
/**
 *  std::make_unique replacement for C++11
 *
 *  @param args the arguments to make_unique (forwarded)
 *
 *  @return the unique pointer
 */
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args &&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}
#endif

namespace flux {

    /* Defines standard floating point type used by fluxNEAT (must match armadillo float type) */
    using float_fl = double;

} // namespace flux
#endif //FLUXNEAT_PREREQ_H
