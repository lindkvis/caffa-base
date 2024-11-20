// ##################################################################################################
//
//    Caffa
//    Copyright (C) 2022- Kontur AS
//
//    This library may be used under the terms of the GNU Lesser General Public License as follows:
//
//    GNU Lesser General Public License Usage
//    This library is free software; you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation; either version 2.1 of the License, or
//    (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or
//    FITNESS FOR A PARTICULAR PURPOSE.
//
//    See the GNU Lesser General Public License at <<http://www.gnu.org/licenses/lgpl-2.1.html>>
//    for more details.
//
// ##################################################################################################
#include "cafUuidGenerator.h"

#include <iostream>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>

#ifndef NDEBUG
#include <iomanip>
#include <sstream>
#endif

using namespace caffa;
using namespace std::chrono;

#ifndef NDEBUG
bool     UuidGenerator::s_useDummyUuids    = false;
uint64_t UuidGenerator::s_dummyUuidCounter = 0u;
#endif

std::unique_ptr<boost::uuids::random_generator> UuidGenerator::s_uuidGenerator;
std::mutex                                      UuidGenerator::s_mutex;

std::string UuidGenerator::generate()
{
    std::scoped_lock lock( s_mutex );
#ifndef NDEBUG
    if (s_useDummyUuids)
    {
        std::stringstream ss;
        ss << std::hex << "00000000-0000-0000-0000-" << std::setfill( '0' ) << std::setw( 12 ) << s_dummyUuidCounter++;
        return ss.str();
    }
#endif
    if (!s_uuidGenerator)
    {
        s_uuidGenerator = std::make_unique<boost::uuids::random_generator>();
    }

    return boost::uuids::to_string( ( *s_uuidGenerator )() );
}

bool UuidGenerator::isUuid( const std::string& string )
{
    try
    {
        constexpr boost::uuids::string_generator generator;
        const auto                               uuid = generator( string );
        return !to_string( uuid ).empty();
    }
    catch (const std::exception& e)
    {
        return false;
    }
}