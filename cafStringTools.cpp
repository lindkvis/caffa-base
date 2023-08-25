#include "cafStringTools.h"

#include <algorithm>

//--------------------------------------------------------------------------------------------------
///
//--------------------------------------------------------------------------------------------------
std::string caffa::StringTools::trim( std::string s )
{
    s.erase( s.begin(), std::find_if( s.begin(), s.end(), []( unsigned char ch ) { return !std::isspace( ch ); } ) );
    s.erase( std::find_if( s.rbegin(), s.rend(), []( unsigned char ch ) { return !std::isspace( ch ); } ).base(), s.end() );
    return s;
}

//--------------------------------------------------------------------------------------------------
///
//--------------------------------------------------------------------------------------------------
std::string caffa::StringTools::tolower( std::string data )
{
    std::transform( data.begin(), data.end(), data.begin(), []( unsigned char c ) { return (char)std::tolower( c ); } );
    return data;
}

//--------------------------------------------------------------------------------------------------
///
//--------------------------------------------------------------------------------------------------
std::string caffa::StringTools::replace( const std::string& data, const std::string& what, const std::string& with )
{
    std::string out   = data;
    size_t      count = 0;
    for ( size_t pos = 0; out.npos != ( pos = out.find( what.data(), pos, what.length() ) ); pos += with.length(), ++count )
    {
        out.replace( pos, what.length(), with.data(), with.length() );
    }
    return out;
}

std::optional<int64_t> caffa::StringTools::toInt64( const std::string& string )
{
    char*   endptr;
    int64_t result = strtoll( string.c_str(), &endptr, 10 );
    if ( endptr && *endptr == 0 )
    {
        return result;
    }
    return std::optional<int64_t>();
}

std::optional<uint32_t> caffa::StringTools::toUint32( const std::string& string )
{
    char*    endptr;
    uint32_t result = strtoul( string.c_str(), &endptr, 10 );
    if ( endptr && *endptr == 0 )
    {
        return result;
    }
    return std::optional<uint32_t>();
}

std::optional<double> caffa::StringTools::toDouble( const std::string& string )
{
    char*  endptr;
    double result = strtod( string.c_str(), &endptr );
    if ( endptr && *endptr == 0 )
    {
        return result;
    }
    return std::optional<double>();
}
