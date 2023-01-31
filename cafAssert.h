
#pragma once

#include <sstream>

namespace caffa
{
void _caffa_assert( const std::string& errorMessage );
}

#define CAFFA_ASSERT( expr )                                                                \
    do                                                                                      \
    {                                                                                       \
        if ( !( expr ) )                                                                    \
        {                                                                                   \
            std::stringstream str;                                                          \
            str << __FILE__ << ":" << __LINE__ << ": CAFFA_ASSERT(" << #expr << ") failed"; \
            caffa::_caffa_assert( str.str() );                                              \
        }                                                                                   \
    } while ( false )
