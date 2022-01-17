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

#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

namespace caffa
{
class Logger
{
public:
    enum class Level
    {
        CRITICAL,
        ERROR,
        WARNING,
        REPLAY,
        INFO,
        DEBUG,
        TRACE
    };

    enum class TimeGranularity
    {
        NONE,
        SECONDS,
        MILLISECONDS
    };

    static void        log( Level              level,
                            const std::string& message,
                            char const*        function,
                            char const*        file,
                            int                line,
                            const std::string& binName = "default" );
    static Level       applicationLogLevel();
    static void        setApplicationLogLevel( Level applicationLogLevel );
    static std::string logLevelLabel( Level level );
    static Level       logLevelFromLabel( const std::string& label );
    static void        setLogFile( const std::string& logFile, const std::string& logBinName = "default" );
    static std::map<Level, std::string> logLevels();
    static void                         setTimeGranularity( TimeGranularity granularity );

    static void registerThreadName( const std::string& name );

private:
    static Level                                                s_applicationLogLevel;
    static std::map<std::string, std::shared_ptr<std::ostream>> s_streams;

    static std::mutex s_mutex;

    static std::map<std::thread::id, std::string> s_threadNames;
    static std::chrono::system_clock::time_point  s_startTime;
    static TimeGranularity                        s_timeGranularity;
};

} // namespace caffa

#define CAFFA_LOG( BIN_NAME, LOG_LEVEL, MESSAGE )                                                          \
    caffa::Logger::log( LOG_LEVEL,                                                                         \
                        static_cast<std::ostringstream&>( std::ostringstream().flush() << MESSAGE ).str(), \
                        __FUNCTION__,                                                                      \
                        __FILE__,                                                                          \
                        __LINE__,                                                                          \
                        BIN_NAME );

#define CAFFA_ERROR_BIN( BinName_, Message_ ) CAFFA_LOG( BinName_, caffa::Logger::Level::ERROR, Message_ )
#define CAFFA_WARNING_BIN( BinName_, Message_ ) CAFFA_LOG( BinName_, caffa::Logger::Level::WARNING, Message_ )
#define CAFFA_INFO_BIN( BinName_, Message_ ) CAFFA_LOG( BinName_, caffa::Logger::Level::INFO, Message_ )
#define CAFFA_REPLAY_BIN( BinName_, Message_ ) CAFFA_LOG( BinName_, caffa::Logger::Level::REPLAY, Message_ )
#define CAFFA_DEBUG_BIN( BinName_, Message_ ) CAFFA_LOG( BinName_, caffa::Logger::Level::DEBUG, Message_ )
#define CAFFA_TRACE_BIN( BinName_, Message_ ) CAFFA_LOG( BinName_, caffa::Logger::Level::TRACE, Message_ )

#define CAFFA_CRITICAL_BIN( BinName_, Message_ )                         \
    {                                                                    \
        CAFFA_LOG( BinName_, caffa::Logger::Level::CRITICAL, Message_ ); \
        exit( 1 );                                                       \
    }

#define CAFFA_CRITICAL( Message_ ) CAFFA_CRITICAL_BIN( "default", Message_ )
#define CAFFA_ERROR( Message_ ) CAFFA_LOG( "default", caffa::Logger::Level::ERROR, Message_ )
#define CAFFA_WARNING( Message_ ) CAFFA_LOG( "default", caffa::Logger::Level::WARNING, Message_ )
#define CAFFA_INFO( Message_ ) CAFFA_LOG( "default", caffa::Logger::Level::INFO, Message_ )
#define CAFFA_DEBUG( Message_ ) CAFFA_LOG( "default", caffa::Logger::Level::DEBUG, Message_ )
#define CAFFA_TRACE( Message_ ) CAFFA_LOG( "default", caffa::Logger::Level::TRACE, Message_ )
