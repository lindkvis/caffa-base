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

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <cassert>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <random>
#include <thread>
#include <vector>

using namespace caffa;

void Logger::setApplicationLogLevel( spdlog::level::level_enum applicationLogLevel )
{
    std::cout << "Setting log level to " << logLevels()[applicationLogLevel] << std::endl;
    spdlog::set_level( applicationLogLevel );
}

std::map<spdlog::level::level_enum, std::string> Logger::logLevels()
{
    std::map<spdlog::level::level_enum, std::string> all_levels;
    all_levels[spdlog::level::off]      = fmt::format( SPDLOG_LEVEL_NAME_OFF );
    all_levels[spdlog::level::critical] = fmt::format( SPDLOG_LEVEL_NAME_CRITICAL );
    all_levels[spdlog::level::err]      = fmt::format( SPDLOG_LEVEL_NAME_ERROR );
    all_levels[spdlog::level::warn]     = fmt::format( SPDLOG_LEVEL_NAME_WARNING );
    all_levels[spdlog::level::info]     = fmt::format( SPDLOG_LEVEL_NAME_INFO );
    all_levels[spdlog::level::debug]    = fmt::format( SPDLOG_LEVEL_NAME_DEBUG );
    all_levels[spdlog::level::trace]    = fmt::format( SPDLOG_LEVEL_NAME_TRACE );
    return all_levels;
}

spdlog::level::level_enum Logger::logLevelFromLabel( const std::string& label )
{
    auto levels = logLevels();
    for ( auto [levelValue, levelLabel] : levels )
    {
        if ( levelLabel == label ) return levelValue;
    }
    assert( false );
    return spdlog::level::trace;
}

void Logger::registerDefaultFileLogger( const std::string& logFile,
                                        size_t             maxFileSizeMiB /*= 5u */,
                                        size_t             maxRotatedFiles /*= 3u */ )
{
    auto logger = spdlog::rotating_logger_mt( "default", logFile, maxFileSizeMiB * 1024u * 1024u, maxRotatedFiles, true );
    spdlog::set_default_logger( logger );
}

void Logger::registerFileLogger( const std::string& logFile,
                                 const std::string& logSinkName,
                                 size_t             maxFileSizeMiB /*= 5u */,
                                 size_t             maxRotatedFiles /*= 3u */ )
{
    if ( spdlog::get( logSinkName ) )
    {
        spdlog::drop( logSinkName );
    }
    auto logger = spdlog::rotating_logger_mt( logSinkName, logFile, maxFileSizeMiB * 1024u * 1024u, maxRotatedFiles, true );
}

void Logger::registerStdOutLogger( const std::string& logSinkName )
{
    auto console = spdlog::stdout_color_mt( logSinkName );
}

std::shared_ptr<spdlog::logger> Logger::get( const std::string& sinkName )
{
    auto logger = spdlog::get( sinkName );
    if ( logger ) return logger;

    return spdlog::default_logger();
}

std::string Logger::simplifyFileName( const std::string& fullFilePath )
{
    auto filePath       = caffa::StringTools::split( fullFilePath, "/" );
    auto fileName       = !filePath.empty() ? filePath.back() : fullFilePath;
    auto fileComponents = caffa::StringTools::split( fileName, "." );
    fileName            = !fileComponents.empty() ? fileComponents.front() : fileName;
    return fileName;
}