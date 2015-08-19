set(target_name openstudio_isomodel)

set(${target_name}_src
  Building.cpp
  Building.hpp
  Cooling.cpp
  Cooling.hpp
  EpwData.cpp
  EpwData.hpp
  ForwardTranslator.cpp
  ForwardTranslator.hpp
  Heating.cpp
  Heating.hpp
  HourlyModel.cpp
  HourlyModel.hpp
  ISOModelAPI.hpp
  ISOResults.cpp
  ISOResults.hpp
  Lighting.cpp
  Lighting.hpp
  Location.cpp
  Location.hpp
  MonthlyModel.cpp
  MonthlyModel.hpp
  PhysicalQuantities.cpp
  PhysicalQuantities.hpp
  Population.cpp
  Population.hpp
  Properties.cpp
  Properties.hpp
  Simulation.cpp
  Simulation.hpp
  SimulationSettings.cpp
  SimulationSettings.hpp
  SolarRadiation.cpp
  SolarRadiation.hpp
  Structure.cpp
  Structure.hpp
  TimeFrame.cpp
  TimeFrame.hpp
  UserModel.cpp
  UserModel.hpp
  Ventilation.cpp
  Ventilation.hpp
  WeatherData.cpp
  WeatherData.hpp
  mainpage.hpp
)

set(${target_name}_test_src
  Test/ISOModelFixture.hpp
  Test/ISOModelFixture.cpp
  #  Temporarily don't compile these tests. BAA@2015-08-13.
  #  Test/ForwardTranslator_GTest.cpp
  Test/MonthlyModel_GTest.cpp
  # Test/UserModel_GTest.cpp
)

set(${target_name}_swig_src
  ISOModel.i
)

set(${target_name}_depends
  ${Boost_LIBRARIES}
  ${CMAKE_THREAD_LIBS}
  ${QT_LIBS}
  openstudio_utilities
  openstudio_model
  zkexpat
)

add_library(${target_name} SHARED ${${target_name}_src})
target_link_libraries(${target_name} ${${target_name}_depends})
AddPCH(${target_name})

#if(NOT APPLE)
install(TARGETS ${target_name} 
  RUNTIME DESTINATION bin 
  LIBRARY DESTINATION lib
)
#endif()

CREATE_SRC_GROUPS("${${target_name}_src}")
CREATE_SRC_GROUPS("${${target_name}_test_src}")
CREATE_SRC_GROUPS("${${target_name}_swig_src}")

CREATE_TEST_TARGETS(${target_name} "${${target_name}_test_src}" "${${target_name}_depends}")
if(BUILD_TESTING)
  add_dependencies(${target_name}_tests openstudio_isomodel_resources)
endif()

MAKE_SWIG_TARGET(OpenStudioISOModel ISOModel "${CMAKE_CURRENT_SOURCE_DIR}/ISOModel.i" "${${target_name}_swig_src}" ${target_name} OpenStudioModel)
