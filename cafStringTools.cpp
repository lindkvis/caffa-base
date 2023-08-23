#include "cafStringTools.h"

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <boost/archive/iterators/transform_width.hpp>

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

std::string caffa::StringTools::decodeBase64( std::string input )
{
    using namespace boost::archive::iterators;
    typedef transform_width<binary_from_base64<remove_whitespace<std::string::const_iterator>>, 8, 6> ItBinaryT;

    try
    {
        // If the input isn't a multiple of 4, pad with =
        size_t num_pad_chars( ( 4 - input.size() % 4 ) % 4 );
        input.append( num_pad_chars, '=' );

        size_t pad_chars( std::count( input.begin(), input.end(), '=' ) );
        std::replace( input.begin(), input.end(), '=', 'A' );
        std::string output( ItBinaryT( input.begin() ), ItBinaryT( input.end() ) );
        output.erase( output.end() - pad_chars, output.end() );
        return output;
    }
    catch ( std::exception const& )
    {
        return std::string( "" );
    }
}