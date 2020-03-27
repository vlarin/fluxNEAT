//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_GLOBAL_ERROR_H
#define FLUXNEAT_GLOBAL_ERROR_H

namespace flux
{
    const char *getLastError();
    bool hasErrors();

    void resetErrors();
    void setLastError(const char *what);
    void setLastErrorF(const char *what, const char *context);
    void setLastErrorD(const char *what, double context);
}

#endif //FLUXNEAT_GLOBAL_ERROR_H
