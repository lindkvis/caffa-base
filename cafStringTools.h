/////////////////////////////////////////////////////////////////////////////////////////////////
//
//   Caffa
//   Copyright (C) Gaute Lindkvist
//
//   This library may be used under the terms of either the GNU General Public License or
//   the GNU Lesser General Public License as follows:
//
//   GNU General Public License Usage
//   This library is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful, but WITHOUT ANY
//   WARRANTY; without even the implied warranty of MERCHANTABILITY or
//   FITNESS FOR A PARTICULAR PURPOSE.
//
//   See the GNU General Public License at <<http://www.gnu.org/licenses/gpl.html>>
//   for more details.
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
#include <iostream>
#include <list>
#include <locale>
#include <memory>
#include <numeric>
#include <regex>
#include <stdexcept>
#include <string>

#ifdef MSVC
#pragma warning( disable : 4996 )
#endif

namespace caffa::StringTools
{
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
Container split( const std::string& string, const std::string& delimiter )
{
    static_assert( std::is_same<typename Container::value_type, std::string>::value,
                   "split() only creates containers of std::strings" );
    Container output;

    size_t start = 0u;
    size_t end   = string.find( delimiter );
    while ( end != std::string::npos )
    {
        output.push_back( string.substr( start, end - start ) );
        start = end + delimiter.length();
        end   = string.find( delimiter, start );
    }
    output.push_back( string.substr( start, end ) );
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

} // namespace caffa::StringTools
