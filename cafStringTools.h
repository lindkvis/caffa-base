/////////////////////////////////////////////////////////////////////////////////////////////////
//
//   Caffa
//   Copyright (C) Kontur AS
//
//   GNU Lesser General Public License Usage
//   This library is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Lesser General Public License as published by
//   the Free Software Foundation; either version 2.1 of the License, or
//   (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful, but WITHOUT ANY
//   WARRANTY; without even the implied warranty of MERCHANTABILITY or
//   FITNESS FOR A PARTICULAR PURPOSE.
//
//   See the GNU Lesser General Public License at <<http://www.gnu.org/licenses/lgpl-2.1.html>>
//   for more details.
/////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cctype>
#include <functional>
#include <list>
#include <memory>
#include <numeric>
#include <optional>
#include <regex>
#include <stdexcept>
#include <string>
#include <type_traits>

#ifdef MSVC
#pragma warning( disable : 4996 )
#endif

namespace caffa::StringTools
{
/**
 * A fixed string class that is possible to use as a template parameter
 */
template <std::size_t N>
struct FixedString
{
    std::array<char, N + 1> data{};

    constexpr FixedString( const char* string ) noexcept { std::copy_n( string, N + 1, data.data() ); }

    constexpr operator std::string_view() const noexcept { return std::string_view( data.data(), N ); }

    constexpr auto size() const noexcept { return N; }
};

template <unsigned N>
FixedString( char const ( & )[N] ) -> FixedString<N - 1>;

/**
 * @brief Join together all strings covered by the iterators with delimiters
 *
 * @tparam InputIt A templated iterator type. Usually automatically deduced.
 * @param first Start iterator
 * @param last End iterator
 * @param delimiter String to join words with
 * @return std::string One joined text string
 */
template <class InputIt>
std::string join( InputIt begin, InputIt end, const std::string& delimiter )
{
    if ( end == begin ) return std::string();
    return std::accumulate( next( begin ), // there is at least 1 element, so OK.
                            end,
                            *begin, // the initial value
                            [&delimiter]( auto a, auto b ) { return a + delimiter + b; } );
}

/**
 * @brief Split text string by a given delimiter
 *
 * @tparam Container the type of string container to create
 * @param string The text string to split
 * @param delimiter String to split on
 * @return A container of strings
 */
template <class Container = std::list<std::string>>
Container split( const std::string& string, const std::string& delimiter, bool skipEmptyParts = false )
{
    static_assert( std::is_same<typename Container::value_type, std::string>::value,
                   "split() only creates containers of std::strings" );
    Container output;

    size_t start = 0u;
    size_t end   = string.find( delimiter );
    while ( true )
    {
        auto token = string.substr( start, end - start );
        if ( !skipEmptyParts || token.length() > 0u )
        {
            output.push_back( token );
        }
        if ( end == std::string::npos ) break;

        start = end + delimiter.length();
        end   = string.find( delimiter, start );
    }

    return output;
}

/**
 * @brief Split text string by regex tokens
 *
 * @tparam Container the type of string container to create
 * @param string The text string to split
 * @param regex the regular expression
 * @param skipEmptyParts If true will drop any empty entry
 * @return A container of strings
 */
template <class Container = std::list<std::string>>
Container split( const std::string& string, const std::regex& regex, bool skipEmptyParts = false )
{
    static_assert( std::is_same<typename Container::value_type, std::string>::value,
                   "split() only creates containers of std::strings" );

    Container output;

    std::sregex_token_iterator it( string.begin(), string.end(), regex, -1 );
    std::sregex_token_iterator end;

    while ( it != end )
    {
        auto token = *it++;
        if ( !skipEmptyParts || token.length() > 0u ) output.push_back( token );
    }

    return output;
}

/**
 * @brief Trim away white-space at start and end of string
 *
 * @param s string
 * @return std::string
 */
std::string trim( std::string s );

/**
 * @brief Turn string to lower case
 *
 * @param s string
 * @return std::string
 */
std::string tolower( std::string data );

/**
 * @brief Replace a portion of a string with something else
 * @param data The full string to replace in
 * @param replace What to replace
 * @param with What to replace with
 * @return Returns a copy with the replaced part
 */
std::string replace( const std::string& data, const std::string& replace, const std::string& with );

/**
 * @brief Create a formatted string from any nunber of arguments
 *
 * @tparam Args
 * @param format
 * @param args
 * @return std::string
 */
template <typename... Args>
std::string string_format( const std::string& format, Args... args )
{
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args... ) + 1; // Extra space for '\0'
    if ( size_s <= 0 )
    {
        throw std::runtime_error( "Error during formatting." );
    }
    auto size = static_cast<size_t>( size_s );
    auto buf  = std::make_unique<char[]>( size );
    std::snprintf( buf.get(), size, format.c_str(), args... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

constexpr bool islower( char c ) noexcept
{
    return c >= 'a' && c <= 'z';
}

constexpr bool isupper( char c ) noexcept
{
    return c >= 'A' && c <= 'Z';
}

constexpr bool isalpha( char c ) noexcept
{
    return islower( c ) || isupper( c );
}

constexpr bool isdigit( int c ) noexcept
{
    return c >= '0' && c <= '9';
}

/**
 * Convert a string to an int64 with fail checking
 * @param string A string to convert
 * @return An optional int64_t where it is valid if the conversion was valid.
 */
std::optional<int64_t> toInt64( const std::string& string );

/**
 * Convert a string to an unsigned int64 with fail checking
 * @param string A string to convert
 * @return An optional uint64_t where it is valid if the conversion was valid.
 */
std::optional<uint64_t> toUint64( const std::string& string );

/**
 * Convert a string to an uint32 with fail checking
 * @param string A string to convert
 * @return An optional uint32_t where it is valid if the conversion was valid.
 */
std::optional<uint32_t> toUint32( const std::string& string );

/**
 * Convert a string to a double with fail checking
 * @param string A string to convert
 * @return An optional double where it is valid if the conversion was valid.
 */
std::optional<double> toDouble( const std::string& string );

} // namespace caffa::StringTools
