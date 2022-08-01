//##################################################################################################
//
//   Caffa
//   Copyright (C) 2021- 3D-Radar AS
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
//
//##################################################################################################
#pragma once

#include <spdlog/spdlog.h>

#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>

namespace caffa
{
class Logger
{
public:
    static void setApplicationLogLevel( spdlog::level::level_enum applicationLogLevel );

    static std::map<spdlog::level::level_enum, std::string> logLevels();
    static spdlog::level::level_enum                        logLevelFromLabel( const std::string& label );

    static void
        registerDefaultFileLogger( const std::string& logFile, size_t maxFileSizeMiB = 5u, size_t maxRotatedFiles = 3u );
    static void registerFileLogger( const std::string& logFile,
                                    const std::string& logSinkName,
                                    size_t             maxFileSizeMiB  = 5u,
                                    size_t             maxRotatedFiles = 3u );
    static void registerStdOutLogger( const std::string& logSinkName = "default" );

    static std::shared_ptr<spdlog::logger> get( const std::string& sinkName );

    static std::string simplifyFileName( const std::string& fileName );
};

} // namespace caffa

#define CAFFA_GENERATE_MSG( MESSAGE ) \
    static_cast<std::ostringstream&>( std::ostringstream().flush() << __FUNCTION__ << MESSAGE ).str()

#ifndef NDEBUG
#define CAFFA_GENERATE_CODE_LINE_MSG( MESSAGE )                                                                     \
    static_cast<std::ostringstream&>( std::ostringstream().flush()                                                  \
                                      << caffa::Logger::simplifyFileName( __FILE__ ) << "::" << __FUNCTION__ << "[" \
                                      << __LINE__ << "]: " << MESSAGE )                                             \
        .str()
#else
#define CAFFA_GENERATE_CODE_LINE_MSG( MESSAGE ) CAFFA_GENERATE_MSG( MESSAGE )
#endif

#define CAFFA_CRITICAL_SINK( SINK_NAME, MESSAGE ) \
    caffa::Logger::get( SINK_NAME )->critical( CAFFA_GENERATE_MSG( MESSAGE ) )
#define CAFFA_ERROR_SINK( SINK_NAME, MESSAGE ) caffa::Logger::get( SINK_NAME )->error( CAFFA_GENERATE_MSG( MESSAGE ) )
#define CAFFA_WARNING_SINK( SINK_NAME, MESSAGE ) caffa::Logger::get( SINK_NAME )->warn( CAFFA_GENERATE_MSG( MESSAGE ) )
#define CAFFA_INFO_SINK( SINK_NAME, MESSAGE ) caffa::Logger::get( SINK_NAME )->info( CAFFA_GENERATE_MSG( MESSAGE ) )

#ifndef NDEBUG
#define CAFFA_DEBUG_SINK( SINK_NAME, MESSAGE ) caffa::Logger::get( SINK_NAME )->debug( CAFFA_GENERATE_MSG( MESSAGE ) )
#define CAFFA_TRACE_SINK( SINK_NAME, MESSAGE ) caffa::Logger::get( SINK_NAME )->trace( CAFFA_GENERATE_MSG( MESSAGE ) )
#else
#define CAFFA_DEBUG_SINK( SINK_NAME, MESSAGE ) \
    {                                          \
    }
#define CAFFA_TRACE_SINK( SINK_NAME, MESSAGE ) \
    {                                          \
    }
#endif

#define CAFFA_CRITICAL_SINK_CODE_LINE( SINK_NAME, MESSAGE ) \
    caffa::Logger::get( SINK_NAME )->critical( CAFFA_GENERATE_CODE_LINE_MSG( MESSAGE ) )
#define CAFFA_ERROR_SINK_CODE_LINE( SINK_NAME, MESSAGE ) \
    caffa::Logger::get( SINK_NAME )->error( CAFFA_GENERATE_CODE_LINE_MSG( MESSAGE ) )
#define CAFFA_WARNING_SINK_CODE_LINE( SINK_NAME, MESSAGE ) \
    caffa::Logger::get( SINK_NAME )->warn( CAFFA_GENERATE_CODE_LINE_MSG( MESSAGE ) )
#define CAFFA_INFO_SINK_CODE_LINE( SINK_NAME, MESSAGE ) \
    caffa::Logger::get( SINK_NAME )->info( CAFFA_GENERATE_CODE_LINE_MSG( MESSAGE ) )

#ifndef NDEBUG
#define CAFFA_DEBUG_SINK_CODE_LINE( SINK_NAME, MESSAGE ) \
    caffa::Logger::get( SINK_NAME )->debug( CAFFA_GENERATE_CODE_LINE_MSG( MESSAGE ) )
#define CAFFA_TRACE_SINK_CODE_LINE( SINK_NAME, MESSAGE ) \
    caffa::Logger::get( SINK_NAME )->trace( CAFFA_GENERATE_CODE_LINE_MSG( MESSAGE ) )
#else
#define CAFFA_DEBUG_SINK_CODE_LINE( SINK_NAME, MESSAGE ) \
    {                                                    \
    }
#define CAFFA_TRACE_SINK_CODE_LINE( SINK_NAME, MESSAGE ) \
    {                                                    \
    }
#endif

#define CAFFA_CRITICAL( MESSAGE ) spdlog::critical( CAFFA_GENERATE_CODE_LINE_MSG( MESSAGE ) )
#define CAFFA_ERROR( MESSAGE ) spdlog::error( CAFFA_GENERATE_CODE_LINE_MSG( MESSAGE ) )
#define CAFFA_WARNING( MESSAGE ) spdlog::warn( CAFFA_GENERATE_CODE_LINE_MSG( MESSAGE ) )
#define CAFFA_INFO( MESSAGE ) spdlog::info( CAFFA_GENERATE_CODE_LINE_MSG( MESSAGE ) )
#ifndef NDEBUG
#define CAFFA_DEBUG( MESSAGE ) spdlog::debug( CAFFA_GENERATE_CODE_LINE_MSG( MESSAGE ) )
#define CAFFA_TRACE( MESSAGE ) spdlog::trace( CAFFA_GENERATE_CODE_LINE_MSG( MESSAGE ) )
#else
#define CAFFA_DEBUG( MESSAGE ) \
    {                          \
    }
#define CAFFA_TRACE( MESSAGE ) \
    {                          \
    }
#endif
