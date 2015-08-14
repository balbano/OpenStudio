/**********************************************************************
 *  Copyright (c) 2008-2015, Alliance for Sustainable Energy.
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

#include "ISOResults.hpp"

namespace openstudio {
namespace isomodel {

// TODO: should totalEnergyUse sum up both hourly and monthly results (with the
// assumption that one of them is empty, require the user to indicate which
// reults vector the want the total of, or is the distinction between the two
// results vectors not meaninful and we should comine them into
// ISOResults::results rather than having both ISOResults::monthlyResults and
// ISOResults::hourlyResults. - BAA@2015-08-14
double ISOResults::totalEnergyUse() {
  auto total = 0.0;

  for (const auto& fuelType : EndUses::fuelTypes()) {
    for (const auto& category : EndUses::categories()) {
      for (const auto& month : monthlyResults) {
        total += month.getEndUse(fuelType, category);
      }
    }
  }

  return total;
}

}
}

