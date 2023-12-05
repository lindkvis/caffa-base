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
#include "cafLogger.h"

#include "cafStringTools.h"

#include "spdlog/sinks/base_sink.h"
#include "spdlog/sinks/ostream_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <random>
#include <thread>
#include <vector>

using namespace caffa;

std::function<std::string( std::string )> Logger::s_functionNameReplacer = nullptr;

void Logger::setApplicationLogLevel( Logger::Level applicationLogLevel )
{
    spdlog::set_level( static_cast<spdlog::level::level_enum>( applicationLogLevel ) );
}

std::map<Logger::Level, std::string> Logger::logLevels()
{
    std::map<Logger::Level, std::string> all_levels;
    all_levels[Logger::Level::off]      = fmt::format( SPDLOG_LEVEL_NAME_OFF );
    all_levels[Logger::Level::critical] = fmt::format( SPDLOG_LEVEL_NAME_CRITICAL );
    all_levels[Logger::Level::err]      = fmt::format( SPDLOG_LEVEL_NAME_ERROR );
    all_levels[Logger::Level::warn]     = fmt::format( SPDLOG_LEVEL_NAME_WARNING );
    all_levels[Logger::Level::info]     = fmt::format( SPDLOG_LEVEL_NAME_INFO );
    all_levels[Logger::Level::debug]    = fmt::format( SPDLOG_LEVEL_NAME_DEBUG );
    all_levels[Logger::Level::trace]    = fmt::format( SPDLOG_LEVEL_NAME_TRACE );
    return all_levels;
}

Logger::Level Logger::logLevelFromLabel( const std::string& label )
{
    auto levels = logLevels();
    for ( auto [levelValue, levelLabel] : levels )
    {
        if ( levelLabel == label ) return levelValue;
    }
    assert( false );
    return Logger::Level::trace;
}

void Logger::registerDefaultFileLogger( const std::string& logFile,
                                        size_t             maxFileSizeMiB /*= 5u */,
                                        size_t             maxRotatedFiles /*= 3u */ )
{
    auto logger = spdlog::rotating_logger_mt( "default", logFile, maxFileSizeMiB * 1024u * 1024u, maxRotatedFiles, true );
    spdlog::set_default_logger( logger );
}

void Logger::registerFileLogger( const std::string& logFile,
                                 const std::string& loggerName,
                                 size_t             maxFileSizeMiB /*= 5u */,
                                 size_t             maxRotatedFiles /*= 3u */ )
{
    std::shared_ptr<spdlog::logger> logger = spdlog::get( loggerName );
    if ( logger )
    {
        auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>( logFile,
                                                                            maxFileSizeMiB * 1024u * 1024u,
                                                                            maxRotatedFiles,
                                                                            true );
        logger->sinks().push_back( sink );
    }
    else
    {
        logger = spdlog::rotating_logger_mt( loggerName, logFile, maxFileSizeMiB * 1024u * 1024u, maxRotatedFiles, true );
    }
}

void Logger::registerStdOutLogger( const std::string& loggerName )
{
    auto logger = spdlog::stdout_color_mt( loggerName );
    if ( loggerName == "default" )
    {
        spdlog::set_default_logger( logger );
    }
}

void Logger::registerCustomSink( const std::string& loggerName, std::shared_ptr<spdlog::sinks::sink> sink )
{
    std::shared_ptr<spdlog::logger> logger = spdlog::get( loggerName );
    if ( !logger )
    {
        logger = std::make_shared<spdlog::logger>( loggerName );
        spdlog::register_logger( logger );
    }
    logger->sinks().push_back( sink );
}

void Logger::log( const std::string& loggerName, Level level, const std::string& message )
{
    std::shared_ptr<spdlog::logger> logger = spdlog::get( loggerName );
    if ( !logger ) logger = spdlog::default_logger();

    logger->log( static_cast<spdlog::level::level_enum>( level ), message );
}
void Logger::log( Level level, const std::string& message )
{
    std::shared_ptr<spdlog::logger> logger = spdlog::default_logger();
    logger->log( static_cast<spdlog::level::level_enum>( level ), message );
}

void Logger::set_default_pattern( const std::string& pattern )
{
    spdlog::set_pattern( pattern );
}
void Logger::set_default_flush_interval( std::chrono::seconds seconds )
{
    spdlog::flush_every( seconds );
}

void Logger::set_default_flush_level( Level level )
{
    spdlog::flush_on( static_cast<spdlog::level::level_enum>( level ) );
}

void Logger::set_logger_pattern( const std::string& loggerName, const std::string& pattern )
{
    spdlog::get( loggerName )->set_pattern( pattern );
}

void Logger::set_logger_flush_level( const std::string& loggerName, Level level )
{
    spdlog::get( loggerName )->flush_on( static_cast<spdlog::level::level_enum>( level ) );
}

std::string Logger::simplifyFileName( const std::string& fullFilePath )
{
    auto filePath = std::filesystem::path( fullFilePath );
    return filePath.stem().generic_string();
}

std::string Logger::simplifyFunctionName( const std::string& functionName )
{
    if ( s_functionNameReplacer )
    {
        return s_functionNameReplacer( functionName );
    }
    return functionName;
}

void Logger::setFunctionNameReplacer( std::function<std::string( std::string )> functionNameReplacer )
{
    s_functionNameReplacer = functionNameReplacer;
}
