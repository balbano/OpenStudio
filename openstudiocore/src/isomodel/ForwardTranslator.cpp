/**********************************************************************
 *  Copyright (c) 2008-2013, Alliance for Sustainable Energy.
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

// TODO: ifdef _OPENSTUDIOS doesn't seem to be working anymore. BAA@2015-08-05.
// #ifdef _OPENSTUDIOS
#include "ForwardTranslator.hpp"
#include "UserModel.hpp"

#include "model/Model.hpp"

namespace openstudio {
  namespace isomodel {

    ForwardTranslator::ForwardTranslator()
    {
      m_logSink.setLogLevel(Warn);
      m_logSink.setChannelRegex(boost::regex("openstudio\\.isomodel\\.ForwardTranslator"));
    }

    ForwardTranslator::~ForwardTranslator()
    {
    }

    UserModel ForwardTranslator::translateModel(const openstudio::model::Model& model)
    {
      m_logSink.resetStringStream();

      return UserModel();
    }

    std::vector<LogMessage> ForwardTranslator::warnings() const
    {
      std::vector<LogMessage> result;

      for(const auto logMessage : m_logSink.logMessages()) {
        if (logMessage.logLevel() == Warn) {
          result.push_back(logMessage);
        }
      }

      return result;
    }

    std::vector<LogMessage> ForwardTranslator::errors() const
    {
      std::vector<LogMessage> result;

      for(const auto logMessage : m_logSink.logMessages()) {
        if (logMessage.logLevel() > Warn) {
          result.push_back(logMessage);
        }
      }

      return result;
    }

  } // isomodel
} // openstudio
// #endif
