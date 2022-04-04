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
#include "cafLogger.h"

#include "cafStringTools.h"

#include <assert.h>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <random>
#include <thread>

using namespace caffa;

std::map<std::string, Logger::Level> Logger::s_applicationLogLevels;

std::map<std::string, std::shared_ptr<std::ostream>> Logger::s_streams = {
    { "default", std::make_shared<std::ostream>( std::cout.rdbuf() ) } };

std::mutex                             Logger::s_mutex;
std::map<std::thread::id, std::string> Logger::s_threadNames;
std::chrono::system_clock::time_point  Logger::s_startTime       = std::chrono::system_clock::now();
Logger::TimeGranularity                Logger::s_timeGranularity = Logger::TimeGranularity::MILLISECONDS;
std::unique_ptr<std::mt19937>          Logger::s_randomGenerator;
std::unique_ptr<std::exponential_distribution<double>> Logger::s_randomDistribution;
unsigned                                               Logger::s_maxDelayUs = 0u;

void Logger::log( Level              level,
                  const std::string& message,
                  char const*        function,
                  char const*        file,
                  int                line,
                  const std::string& binName /*="default"*/ )
{
    std::scoped_lock lock( s_mutex );

    auto now            = std::chrono::system_clock::now();
    auto s_since_epoch  = std::chrono::duration_cast<std::chrono::seconds>( now.time_since_epoch() );
    auto ms_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>( now.time_since_epoch() );
    std::chrono::seconds      secs( s_since_epoch.count() );
    std::chrono::milliseconds ms              = std::chrono::duration_cast<std::chrono::milliseconds>( secs );
    auto                      ms_since_last_s = ms_since_epoch - ms;
    const std::time_t         t_now           = std::chrono::system_clock::to_time_t( now );

    auto threadId     = std::this_thread::get_id();
    auto threadNameIt = s_threadNames.find( threadId );
    auto threadName   = threadNameIt != s_threadNames.end() ? threadNameIt->second : "UNKNOWN_THREAD";

    auto stream_it = s_streams.find( binName );
    if ( stream_it == s_streams.end() || !stream_it->second || !stream_it->second->good() )
    {
        stream_it = s_streams.find( "default" );
    }
    auto& stream = stream_it->second;

    if ( level <= applicationLogLevel( binName ) )
    {
        auto filePath       = caffa::StringTools::split( file, "/" );
        auto fileName       = !filePath.empty() ? filePath.back() : file;
        auto fileComponents = caffa::StringTools::split( fileName, "." );
        fileName            = !fileComponents.empty() ? fileComponents.front() : fileName;

        if ( s_randomGenerator && s_randomDistribution && s_maxDelayUs > 0u )
        {
            double   randomNumber = ( *s_randomDistribution )( *s_randomGenerator );
            unsigned sleepUs      = randomNumber / 5.0 * s_maxDelayUs;

            if ( level >= Level::TRACE )
            {
                *stream << "[" << logLevelLabel( level ) << "] Sleeping for " << sleepUs << " microseconds" << std::endl;
            }
            std::this_thread::sleep_for( std::chrono::microseconds( sleepUs ) );
        }

        if ( level == Level::REPLAY )
        {
            *stream << "[" << logLevelLabel( level ) << "] " << function << " " << message << std::endl;
        }
        else
        {
            if ( s_timeGranularity != TimeGranularity::NONE )
            {
                *stream << "[" << std::put_time( std::localtime( &t_now ), "%F %T" );
                if ( s_timeGranularity == TimeGranularity::MILLISECONDS )
                {
                    *stream << "." << std::setfill( '0' ) << std::setw( 3 ) << ms_since_last_s.count();
                }
                *stream << "] ";
            }
            *stream << "[" << logLevelLabel( level ) << "] " << fileName << "::" << function << "[" << line << "]";
            if ( !threadName.empty() )
            {
                *stream << "{" << threadName << "}";
            }

            *stream << ": " << message << std::endl;
        }
    }
}

Logger::Level Logger::applicationLogLevel( const std::string& binName )
{
    auto it = s_applicationLogLevels.find( binName );
    return it == s_applicationLogLevels.end() ? Logger::Level::WARNING : it->second;
}

void Logger::setApplicationLogLevel( Level applicationLogLevel, const std::string& binName )
{
    std::scoped_lock lock( s_mutex );
    s_applicationLogLevels[binName] = applicationLogLevel;
}

std::string Logger::logLevelLabel( Level level )
{
    return Logger::logLevels()[level];
}

Logger::Level Logger::logLevelFromLabel( const std::string& label )
{
    for ( auto [level, levelLabel] : logLevels() )
    {
        if ( levelLabel == label ) return level;
    }
    assert( false );
    return Logger::Level::CRITICAL;
}

void Logger::setLogFile( const std::string& logFile, const std::string& logBinName /*="default"*/ )
{
    std::scoped_lock lock( s_mutex );
    s_streams[logBinName] = std::make_shared<std::ofstream>( logFile );
}

void Logger::resetLogFileForBin( const std::string& logBinName )
{
    std::scoped_lock lock( s_mutex );
    s_streams.erase( logBinName );
}

std::map<Logger::Level, std::string> Logger::logLevels()
{
    return { { Level::TRACE, "trace" },
             { Level::DEBUG, "debug" },
             { Level::REPLAY, "replay" },
             { Level::INFO, "info" },
             { Level::WARNING, "warning" },
             { Level::ERROR, "error" },
             { Level::CRITICAL, "critical" } };
}

void Logger::setTimeGranularity( TimeGranularity granularity )
{
    std::scoped_lock lock( s_mutex );
    s_timeGranularity = granularity;
}

void Logger::registerThreadName( const std::string& name )
{
    std::scoped_lock lock( s_mutex );
    s_threadNames[std::this_thread::get_id()] = name;
}

void Logger::addRandomDelay( unsigned maxDelayUs, unsigned seed )
{
    s_randomGenerator    = std::make_unique<std::mt19937>( seed );
    s_randomDistribution = std::make_unique<std::exponential_distribution<double>>( 1.5 );
    s_maxDelayUs         = maxDelayUs;
}