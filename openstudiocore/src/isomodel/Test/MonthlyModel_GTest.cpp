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

#include <gtest/gtest.h>
#include "ISOModelFixture.hpp"
#include "../MonthlyModel.hpp"
#include "../UserModel.hpp"
#include <resources.hxx>
#include <sstream>

using namespace openstudio::isomodel;
using namespace openstudio;

void testGenericFunctions() {
  double scalar = 2;
  double test[] = {0,1,2,3,4,5,6,7,8,9,10,11};
  Vector vTest = Vector(12), vTest2 = Vector(12);
  for(unsigned int i = 0;i<vTest.size();i++){
    vTest[i] = i;
    vTest2[i] = 11-i;
  }
  for(unsigned int i = 0;i<12;i++){
    EXPECT_DOUBLE_EQ(i, test[i]);
    EXPECT_DOUBLE_EQ(i, vTest[i]);
    EXPECT_DOUBLE_EQ(11 - i, vTest2[i]);
  }


  Vector results = mult(test,scalar,12);
  for(unsigned int i = 0;i<12;i++){
    EXPECT_DOUBLE_EQ(i*scalar, results[i]);
  }

  results = mult(vTest, scalar);
  for(unsigned int i = 0;i<vTest.size();i++){
    EXPECT_DOUBLE_EQ(i * scalar, results[i]);
  }

  results = mult(vTest, test);
  for(unsigned int i = 0;i<vTest.size();i++){
    EXPECT_DOUBLE_EQ(i * i, results[i]);
  }

  results = mult(vTest, vTest);
  for(unsigned int i = 0;i<vTest.size();i++){
    EXPECT_DOUBLE_EQ(i * i, results[i]);
  }

  results = div(vTest, scalar);
  for(unsigned int i = 0;i<vTest.size();i++){
    EXPECT_DOUBLE_EQ(i / scalar, results[i]);
  }

  results = div(scalar, vTest);
  EXPECT_DOUBLE_EQ(std::numeric_limits<double>::max(), results[0]);
  for(unsigned int i = 1;i<vTest.size();i++){
    EXPECT_DOUBLE_EQ(scalar / i, results[i]);
  }

  results = div(vTest, vTest);
  EXPECT_DOUBLE_EQ(std::numeric_limits<double>::max(), results[0]);  //0 / 0
  for(unsigned int i = 1;i<vTest.size();i++){
    EXPECT_DOUBLE_EQ(1, results[i]);
  }

  results = sum(vTest, vTest);
  for(unsigned int i = 0;i<vTest.size();i++){
    EXPECT_DOUBLE_EQ(i + i, results[i]);
  }

  results = sum(vTest, scalar);
  for(unsigned int i = 0;i<vTest.size();i++){
    EXPECT_DOUBLE_EQ(i + scalar, results[i]);
  }

  results = dif(vTest, vTest);
  for(unsigned int i = 0;i<vTest.size();i++){
    EXPECT_DOUBLE_EQ(0, results[i]);
  }

  results = dif(vTest, scalar);
  for(unsigned int i = 0;i<vTest.size();i++){
    EXPECT_DOUBLE_EQ(i - scalar, results[i]);
  }

  results = dif(scalar, vTest);
  for(unsigned int i = 0;i<vTest.size();i++){
    EXPECT_DOUBLE_EQ(scalar - i, results[i]);
  }

  EXPECT_DOUBLE_EQ(11, openstudio::isomodel::maximum(vTest));

  results = maximum(vTest,vTest2);
  EXPECT_DOUBLE_EQ(11, results[0]);
  EXPECT_DOUBLE_EQ(10, results[1]);
  EXPECT_DOUBLE_EQ(9, results[2]);
  EXPECT_DOUBLE_EQ(8, results[3]);
  EXPECT_DOUBLE_EQ(7, results[4]);
  EXPECT_DOUBLE_EQ(6, results[5]);
  EXPECT_DOUBLE_EQ(6, results[6]);
  EXPECT_DOUBLE_EQ(7, results[7]);
  EXPECT_DOUBLE_EQ(8, results[8]);
  EXPECT_DOUBLE_EQ(9, results[9]);
  EXPECT_DOUBLE_EQ(10, results[10]);
  EXPECT_DOUBLE_EQ(11, results[11]);

  for(unsigned int i = 0;i<12;i++){
    EXPECT_DOUBLE_EQ(11-i, vTest2[i]);
  }

  results = maximum(vTest2,10);
  EXPECT_DOUBLE_EQ(11, results[0]);  
  for(unsigned int i = 1;i<vTest2.size();i++){
    EXPECT_DOUBLE_EQ(10, results[i]);
  }
  
  EXPECT_DOUBLE_EQ(0, openstudio::isomodel::minimum(vTest));

  results = minimum(vTest,1);
  EXPECT_DOUBLE_EQ(0, results[0]);  
  for(unsigned int i = 1;i<vTest.size();i++){
    EXPECT_DOUBLE_EQ(1, results[i]);
  }
  
  results = pow(vTest, 3);
  for(unsigned int i = 0;i<vTest.size();i++){
    EXPECT_DOUBLE_EQ(i*i*i, results[i]);
  }
  for(unsigned int i = 0;i<vTest.size();i++){
    if(i%2==0){
      vTest[i] = -1 * vTest[i];
    }
  }
  results = abs(vTest);
  for(unsigned int i = 0;i<vTest.size();i++){
    EXPECT_DOUBLE_EQ(i, results[i]);
  }
  
}
TEST_F(ISOModelFixture, MonthlyModel)
{
  //testGenericFunctions();
  UserModel userModel;
  userModel.load(resourcesPath() / openstudio::toPath("isomodel/exampleModel.ISO"));
  ASSERT_TRUE(userModel.valid());
  MonthlyModel monthlyModel = userModel.toMonthlyModel();    
  auto results = monthlyModel.simulate();

  EXPECT_DOUBLE_EQ(0, results[0].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(0, results[1].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(0, results[2].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(0, results[3].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(0, results[4].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(0, results[5].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(0, results[6].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(0, results[7].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(0, results[8].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(0, results[9].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(0, results[10].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(0, results[11].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Heating) );

  EXPECT_DOUBLE_EQ(0.34017664200890202, results[0].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(0.47747797661595698, results[1].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(1.3169933074695126, results[2].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(2.4228760061905459, results[3].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(3.7268950868670396, results[4].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(4.5866846768048868, results[5].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(5.2957488941600186, results[6].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(4.7728355657234216, results[7].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(3.9226543241145793, results[8].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(2.5539052604147932, results[9].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(1.2308504332601247, results[10].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(0.39346302413410666, results[11].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Cooling) );

  EXPECT_DOUBLE_EQ(3.0435906070795506, results[0].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorLights) );
  EXPECT_DOUBLE_EQ(2.7490495805879811, results[1].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorLights) );
  EXPECT_DOUBLE_EQ(3.0435906070795506, results[2].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorLights) );
  EXPECT_DOUBLE_EQ(2.9454102649156932, results[3].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorLights) );
  EXPECT_DOUBLE_EQ(3.0435906070795506, results[4].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorLights) );
  EXPECT_DOUBLE_EQ(2.9454102649156932, results[5].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorLights) );
  EXPECT_DOUBLE_EQ(3.0435906070795506, results[6].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorLights) );
  EXPECT_DOUBLE_EQ(3.0435906070795506, results[7].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorLights) );
  EXPECT_DOUBLE_EQ(2.9454102649156932, results[8].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorLights) );
  EXPECT_DOUBLE_EQ(3.0435906070795506, results[9].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorLights) );
  EXPECT_DOUBLE_EQ(2.9454102649156932, results[10].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorLights) );
  EXPECT_DOUBLE_EQ(3.0435906070795506, results[11].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorLights) );

  EXPECT_DOUBLE_EQ(0, results[0].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::ExteriorLights) );
  EXPECT_DOUBLE_EQ(0, results[1].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::ExteriorLights) );
  EXPECT_DOUBLE_EQ(0, results[2].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::ExteriorLights) );
  EXPECT_DOUBLE_EQ(0, results[3].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::ExteriorLights) );
  EXPECT_DOUBLE_EQ(0, results[4].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::ExteriorLights) );
  EXPECT_DOUBLE_EQ(0, results[5].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::ExteriorLights) );
  EXPECT_DOUBLE_EQ(0, results[6].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::ExteriorLights) );
  EXPECT_DOUBLE_EQ(0, results[7].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::ExteriorLights) );
  EXPECT_DOUBLE_EQ(0, results[8].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::ExteriorLights) );
  EXPECT_DOUBLE_EQ(0, results[9].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::ExteriorLights) );
  EXPECT_DOUBLE_EQ(0, results[10].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::ExteriorLights) );
  EXPECT_DOUBLE_EQ(0, results[11].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::ExteriorLights) );

  EXPECT_DOUBLE_EQ(0.63842346693363961, results[0].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Fans) );
  EXPECT_DOUBLE_EQ(0.58652953302205624, results[1].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Fans) );
  EXPECT_DOUBLE_EQ(1.1594322752799191, results[2].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Fans) );
  EXPECT_DOUBLE_EQ(2.0941842853293839, results[3].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Fans) );
  EXPECT_DOUBLE_EQ(3.2204732233014375, results[4].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Fans) );
  EXPECT_DOUBLE_EQ(3.9634287108669861, results[5].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Fans) );
  EXPECT_DOUBLE_EQ(4.5761426152904692, results[6].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Fans) );
  EXPECT_DOUBLE_EQ(4.1242847167812258, results[7].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Fans) );
  EXPECT_DOUBLE_EQ(3.3896293582675732, results[8].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Fans) );
  EXPECT_DOUBLE_EQ(2.2071953370955941, results[9].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Fans) );
  EXPECT_DOUBLE_EQ(1.0817759398239362, results[10].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Fans) );
  EXPECT_DOUBLE_EQ(0.60343839818338818, results[11].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Fans) );

  EXPECT_DOUBLE_EQ(0.10115033983397403, results[0].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Pumps) );
  EXPECT_DOUBLE_EQ(0.092928384780081766, results[1].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Pumps) );
  EXPECT_DOUBLE_EQ(0.18369777229888676, results[2].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Pumps) );
  EXPECT_DOUBLE_EQ(0.33179772221319914, results[3].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Pumps) );
  EXPECT_DOUBLE_EQ(0.51024434068463553, results[4].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Pumps) );
  EXPECT_DOUBLE_EQ(0.62795649247899088, results[5].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Pumps) );
  EXPECT_DOUBLE_EQ(0.72503346859816364, results[6].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Pumps) );
  EXPECT_DOUBLE_EQ(0.65344214660243516, results[7].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Pumps) );
  EXPECT_DOUBLE_EQ(0.53704504808815079, results[8].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Pumps) );
  EXPECT_DOUBLE_EQ(0.34970293228646099, results[9].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Pumps) );
  EXPECT_DOUBLE_EQ(0.17139408183562516, results[10].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Pumps) );
  EXPECT_DOUBLE_EQ(0.095607386329774752, results[11].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Pumps) );

  EXPECT_DOUBLE_EQ(2.7583969507693009, results[0].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(2.4914553103722721, results[1].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(2.7583969507693009, results[2].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(2.6694164039702915, results[3].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(2.7583969507693009, results[4].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(2.6694164039702915, results[5].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(2.7583969507693009, results[6].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(2.7583969507693009, results[7].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(2.6694164039702915, results[8].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(2.7583969507693009, results[9].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(2.6694164039702915, results[10].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(2.7583969507693009, results[11].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorEquipment) );

  EXPECT_DOUBLE_EQ(1.868625049820434, results[0].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(0.94352030602805137, results[1].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(0.11607038752689436, results[2].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(0.0029172731542854565, results[3].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(1.4423899246658913e-05, results[4].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(2.348596441320849e-10, results[5].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(0, results[6].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(1.4643812476787042e-11, results[7].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(3.1551923170925401e-07, results[8].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(0.0017593638950890019, results[9].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(0.09860920039620702, results[10].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Heating) );
  EXPECT_DOUBLE_EQ(1.4290645202713919, results[11].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Heating) );

  EXPECT_DOUBLE_EQ(0, results[0].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[1].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[2].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[3].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[4].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[5].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[6].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[7].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[8].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[9].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[10].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[11].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::WaterSystems) );


  EXPECT_DOUBLE_EQ(0, results[0].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(0, results[1].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(0, results[2].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(0, results[3].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(0, results[4].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(0, results[5].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(0, results[6].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(0, results[7].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(0, results[8].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(0, results[9].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(0, results[10].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Cooling) );
  EXPECT_DOUBLE_EQ(0, results[11].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Cooling) );

  EXPECT_DOUBLE_EQ(8.0840634632175608, results[0].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(7.3017347409706996, results[1].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(8.0840634632175608, results[2].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(7.8232872224686067, results[3].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(8.0840634632175608, results[4].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(7.8232872224686067, results[5].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(8.0840634632175608, results[6].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(8.0840634632175608, results[7].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(7.8232872224686067, results[8].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(8.0840634632175608, results[9].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(7.8232872224686067, results[10].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::InteriorEquipment) );
  EXPECT_DOUBLE_EQ(8.0840634632175608, results[11].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::InteriorEquipment) );

  EXPECT_DOUBLE_EQ(0, results[0].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[1].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[2].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[3].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[4].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[5].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[6].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[7].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[8].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[9].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[10].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::WaterSystems) );
  EXPECT_DOUBLE_EQ(0, results[11].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::WaterSystems) );
}
