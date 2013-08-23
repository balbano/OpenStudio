/**********************************************************************
* Copyright (c) 2008-2013, Alliance for Sustainable Energy.  
*  All rights reserved.
*  
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 2.1 of the License, or (at your option) any later version.
*  
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*  
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**********************************************************************/

#ifndef UTILITIES_CLOUD_OSSERVER_IMPL_HPP
#define UTILITIES_CLOUD_OSSERVER_IMPL_HPP

#include <utilities/UtilitiesAPI.hpp>
#include <utilities/core/UUID.hpp>
#include <utilities/core/Url.hpp>
#include <utilities/core/Path.hpp>
#include <utilities/core/Logger.hpp>

#include <QObject>

// DLM: temporary
#include <boost/bimap.hpp>

#include <string>

class QNetworkAccessManager;
class QNetworkReply;

namespace openstudio{
namespace detail{

  /// OSServer is a class for accessing the rails server started on machines provided by a CloudProvider.
  class UTILITIES_API OSServer_Impl : public QObject {

    Q_OBJECT

  public:

    /** @name Constructor */
    //@{

    /// default constructor
    OSServer_Impl(const Url& url);

    //@}
    /** @name Destructors */
    //@{

    /// virtual destructor
    virtual ~OSServer_Impl();

    //@}
    /** @name Class members */
    //@{

    /// Returns true if the server can be reached
    /// blocking call, clears errors and warnings
    bool available() const;

    /// Get a list of all project UUIDs
    /// blocking call, clears errors and warnings
    std::vector<UUID> projectUUIDs() const; 

    /// Get a list of all analysis UUIDs
    /// blocking call, clears errors and warnings
    std::vector<UUID> analysisUUIDs() const; 

    /// Send an analysis server view JSON file for analsysis, adds the analyis to project
    /// projectUUID can be an existing project or a new one
    /// analysisJSON includes analysis UUID 
    /// blocking call, clears errors and warnings
    bool postAnalysisJSON(const UUID& projectUUID, const std::string& analysisJSON) const;

    /// Send a datapoint server view JSON file for analysis, adds the dataPoint to analysis
    /// analysisUUID must be an existing analysis
    /// dataPointJSON includes data point UUID 
    /// blocking call, clears errors and warnings
    bool postDataPointJSON(const UUID& analysisUUID, const std::string& dataPointJSON) const;

    /// Upload a zip file of all the files needed for analysis
    /// blocking call, clears errors and warnings
    // DLM: is the structure of this defined somewhere? 
    bool uploadAnalysisFiles(const UUID& analysisUUID, const openstudio::path& analysisZipFile);

    /// Send the server a request to start the analysis
    /// blocking call, clears errors and warnings
    bool start(const UUID& analysisUUID) const;

    /// Returns true if the analysis is running
    /// blocking call, clears errors and warnings
    bool isAnalysisRunning(const UUID& analysisUUID) const;

    /// Send the server a request to stop the analysis
    /// blocking call, clears errors and warnings
    bool stop(const UUID& analysisUUID) const;

    /// Get a list of all dataPoint UUIDs in the analysis
    /// blocking call, clears errors and warnings
    std::vector<UUID> dataPointsJSON(const UUID& analysisUUID) const;

    /// Get a list of all running dataPoint UUIDs in the analysis
    /// blocking call, clears errors and warnings
    std::vector<UUID> runningDataPointsJSON(const UUID& analysisUUID) const;

    /// Get a list of all queued dataPoint UUIDs in the analysis
    /// blocking call, clears errors and warnings
    std::vector<UUID> queuedDataPointsJSON(const UUID& analysisUUID) const;

    /// Get a list of all complete dataPoint UUIDs in the analysis
    /// blocking call, clears errors and warnings 
    std::vector<UUID> completeDataPointsJSON(const UUID& analysisUUID) const;

    /// Get a full deserializable JSON of dataPoint
    /// blocking call, clears errors and warnings 
    std::string getDataPointJSON(const UUID& analysisUUID, const UUID& dataPointUUID) const;

    /// Get a detailed results for dataPoint, will be a zip file of the dataPoint directory
    /// blocking call, clears errors and warnings 
    bool downloadDataPoint(const UUID& analysisUUID, const UUID& dataPointUUID, const openstudio::path& downloadPath) const;

    /// returns errors generated by the last operation
    std::vector<std::string> errors() const;
    
    /// returns warnings generated by the last operation
    std::vector<std::string> warnings() const;

    //@}

  private:

    Url m_url;
    boost::shared_ptr<QNetworkAccessManager> m_networkAccessManager;
    mutable std::vector<std::string> m_errors;
    mutable std::vector<std::string> m_warnings;

    bool block(QNetworkReply* reply, int timeout=3000) const;
    void clearErrorsAndWarnings() const;
    void logError(const std::string& error) const;
    void logWarning(const std::string& warning) const;

    // DLM: temporary
    mutable boost::bimap<UUID,QString> m_uuidToIdMap;

    // configure logging
    REGISTER_LOGGER("utilities.cloud.OSServer");
  };

} // detail
} // openstudio

#endif // UTILITIES_CLOUD_OSSERVER_IMPL_HPP
