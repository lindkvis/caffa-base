// ##################################################################################################
//
//    Custom Visualization Core library
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
#pragma once

#include <boost/uuid/uuid_generators.hpp>

#include <memory>
#include <mutex>
#include <random>

namespace caffa
{
    class UuidGenerator
    {
    public:
        static std::string generate();

        static bool isUuid(const std::string& string);

#ifndef NDEBUG
    static bool     s_useDummyUuids;
    static uint64_t s_dummyUuidCounter;
#endif

    private:
        static std::unique_ptr<boost::uuids::random_generator> s_uuidGenerator;
        static std::mutex s_mutex;
    };
} // namespace caffa
