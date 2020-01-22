//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_GLOBAL_ERROR_H
#define FLUXNEAT_GLOBAL_ERROR_H

namespace flux {

    extern char *LAST_ERROR;

    inline bool hasErrors() { return LAST_ERROR != nullptr; }

    inline void resetErrors() { LAST_ERROR = nullptr; }
    inline void setLastError(const char *what) { LAST_ERROR = const_cast<char *>(what); }
}

#endif //FLUXNEAT_GLOBAL_ERROR_H
