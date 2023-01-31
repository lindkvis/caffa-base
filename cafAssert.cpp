#include "cafAssert.h"

#include "cafLogger.h"

#include <cstdlib>

void caffa::_caffa_assert( const std::string& errorMessage )
{
    CAFFA_CRITICAL( errorMessage );
    std::abort();
}
