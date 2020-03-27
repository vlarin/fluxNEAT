//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#include <flux/global_error.h>
#include <sstream>

static char *LAST_ERROR = nullptr;

bool flux::hasErrors()
{
    return LAST_ERROR != nullptr;
}

void flux::resetErrors()
{
    LAST_ERROR = nullptr;
}

void flux::setLastError(const char *what)
{
    delete[] LAST_ERROR;

    LAST_ERROR = const_cast<char *>(what);
}

void flux::setLastErrorF(const char *what, const char *context)
{
    delete[] LAST_ERROR;
    LAST_ERROR = new char[strlen(what) + strlen(context)];
    snprintf(LAST_ERROR, strlen(LAST_ERROR), what, context);
}

void flux::setLastErrorD(const char *what, double context)
{
    delete[] LAST_ERROR;
    LAST_ERROR = new char[strlen(what) + 20];
    snprintf(LAST_ERROR, strlen(LAST_ERROR), what, context);
}

const char *flux::getLastError()
{
    return LAST_ERROR;
}
