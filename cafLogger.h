// ##################################################################################################
//
//    Caffa
//    Copyright (C) 2021- 3D-Radar AS
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

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>

namespace spdlog::sinks
{
class sink;
}

namespace caffa
{
class Logger
{
public:
    enum class Level : int
    {
        trace    = 0,
        debug    = 1,
        info     = 2,
        warn     = 3,
        err      = 4,
        critical = 5,
        off      = 6,
        n_levels
    };

    static void setApplicationLogLevel( Level applicationLogLevel );
    static void setLogLevel( const std::string& loggerName, Level applicationLogLevel );

    static std::map<Level, std::string> logLevels();
    static Level                        logLevelFromLabel( const std::string& label );

    static void
        registerDefaultFileLogger( const std::string& logFile, size_t maxFileSizeMiB = 5u, size_t maxRotatedFiles = 3u );
    static void registerFileLogger( const std::string& logFile,
                                    const std::string& loggerName,
                                    size_t             maxFileSizeMiB  = 5u,
                                    size_t             maxRotatedFiles = 3u );
    static void registerStdOutLogger( const std::string& loggerName = "default" );
    static void registerCustomSink( const std::string& loggerName, std::shared_ptr<spdlog::sinks::sink> sink );

    static void log( const std::string& loggerName, Level level, const std::string& message );
    static void log( Level level, const std::string& message );

    static void set_default_pattern( const std::string& pattern );
    static void set_default_flush_interval( std::chrono::seconds seconds );
    static void set_default_flush_level( Level level );

    static void set_logger_pattern( const std::string& loggerName, const std::string& pattern );
    static void set_logger_flush_level( const std::string& loggerName, Level level );

    static std::string simplifyFileName( const std::string& fileName );
    static std::string simplifyFunctionName( const std::string& functionName );

    static void setFunctionNameReplacer( std::function<std::string( std::string )> functionNameReplacer );

private:
    static std::mutex s_mutex;

    static std::function<std::string( std::string )> s_functionNameReplacer;
};

} // namespace caffa

#define CAFFA_GENERATE_SIMPLE_MSG( MESSAGE ) \
    dynamic_cast<std::ostringstream&>( std::ostringstream().flush() << std::boolalpha << MESSAGE ).str()

#ifndef NDEBUG
#define CAFFA_GENERATE_MSG( MESSAGE )                                                                                \
    dynamic_cast<std::ostringstream&>( std::ostringstream().flush()                                                  \
                                       << caffa::Logger::simplifyFileName( __FILE__ ) << "::" << __FUNCTION__ << "[" \
                                       << __LINE__ << "]: " << std::boolalpha << MESSAGE )                           \
        .str()
#else
#define CAFFA_GENERATE_MSG( MESSAGE ) \
    dynamic_cast<std::ostringstream&>( std::ostringstream().flush() << std::boolalpha << MESSAGE ).str()

#endif

#define CAFFA_CRITICAL_SINK( LOGGER_NAME, MESSAGE ) \
    caffa::Logger::log( LOGGER_NAME, caffa::Logger::Level::critical, CAFFA_GENERATE_SIMPLE_MSG( MESSAGE ) )
#define CAFFA_ERROR_SINK( LOGGER_NAME, MESSAGE ) \
    caffa::Logger::log( LOGGER_NAME, caffa::Logger::Level::err, CAFFA_GENERATE_SIMPLE_MSG( MESSAGE ) )
#define CAFFA_WARNING_SINK( LOGGER_NAME, MESSAGE ) \
    caffa::Logger::log( LOGGER_NAME, caffa::Logger::Level::warn, CAFFA_GENERATE_SIMPLE_MSG( MESSAGE ) )
#define CAFFA_INFO_SINK( LOGGER_NAME, MESSAGE ) \
    caffa::Logger::log( LOGGER_NAME, caffa::Logger::Level::info, CAFFA_GENERATE_SIMPLE_MSG( MESSAGE ) )
#define CAFFA_DEBUG_SINK( LOGGER_NAME, MESSAGE ) \
    caffa::Logger::log( LOGGER_NAME, caffa::Logger::Level::debug, CAFFA_GENERATE_SIMPLE_MSG( MESSAGE ) )

#ifndef NDEBUG
#define CAFFA_TRACE_SINK( LOGGER_NAME, MESSAGE ) \
    caffa::Logger::log( LOGGER_NAME, caffa::Logger::Level::trace, CAFFA_GENERATE_SIMPLE_MSG( MESSAGE ) )
#else
#define CAFFA_TRACE_SINK( LOGGER_NAME, MESSAGE ) \
    {                                            \
    }
#endif

#define CAFFA_CRITICAL_SINK_CODE_LINE( LOGGER_NAME, MESSAGE ) \
    caffa::Logger::log( LOGGER_NAME, caffa::Logger::Level::critical, CAFFA_GENERATE_MSG( MESSAGE ) )
#define CAFFA_ERROR_SINK_CODE_LINE( LOGGER_NAME, MESSAGE ) \
    caffa::Logger::log( LOGGER_NAME, caffa::Logger::Level::err, CAFFA_GENERATE_MSG( MESSAGE ) )
#define CAFFA_WARNING_SINK_CODE_LINE( LOGGER_NAME, MESSAGE ) \
    caffa::Logger::log( LOGGER_NAME, caffa::Logger::Level::warn, CAFFA_GENERATE_MSG( MESSAGE ) )
#define CAFFA_INFO_SINK_CODE_LINE( LOGGER_NAME, MESSAGE ) \
    caffa::Logger::log( LOGGER_NAME, caffa::Logger::Level::info, CAFFA_GENERATE_MSG( MESSAGE ) )
#define CAFFA_DEBUG_SINK_CODE_LINE( LOGGER_NAME, MESSAGE ) \
    caffa::Logger::log( LOGGER_NAME, caffa::Logger::Level::debug, CAFFA_GENERATE_MSG( MESSAGE ) )

#ifndef NDEBUG
#define CAFFA_TRACE_SINK_CODE_LINE( LOGGER_NAME, MESSAGE ) \
    caffa::Logger::log( LOGGER_NAME, caffa::Logger::Level::trace, CAFFA_GENERATE_MSG( MESSAGE ) )
#else
#define CAFFA_TRACE_SINK_CODE_LINE( LOGGER_NAME, MESSAGE ) \
    {                                                      \
    }
#endif

#define CAFFA_CRITICAL( MESSAGE ) caffa::Logger::log( caffa::Logger::Level::critical, CAFFA_GENERATE_MSG( MESSAGE ) )
#define CAFFA_ERROR( MESSAGE ) caffa::Logger::log( caffa::Logger::Level::err, CAFFA_GENERATE_MSG( MESSAGE ) )
#define CAFFA_WARNING( MESSAGE ) caffa::Logger::log( caffa::Logger::Level::warn, CAFFA_GENERATE_MSG( MESSAGE ) )
#define CAFFA_INFO( MESSAGE ) caffa::Logger::log( caffa::Logger::Level::info, CAFFA_GENERATE_MSG( MESSAGE ) )
#define CAFFA_DEBUG( MESSAGE ) caffa::Logger::log( caffa::Logger::Level::debug, CAFFA_GENERATE_MSG( MESSAGE ) )
#ifndef NDEBUG
#define CAFFA_TRACE( MESSAGE ) caffa::Logger::log( caffa::Logger::Level::trace, CAFFA_GENERATE_MSG( MESSAGE ) )
#else
#define CAFFA_TRACE( MESSAGE ) \
    {                          \
    }
#endif
