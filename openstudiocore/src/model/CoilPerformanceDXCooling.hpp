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

#ifndef MODEL_COILPERFORMANCEDXCOOLING_HPP
#define MODEL_COILPERFORMANCEDXCOOLING_HPP

#include <model/ModelAPI.hpp>
#include <model/ParentObject.hpp>

namespace openstudio {

namespace model {

class Curve;
class Node;

namespace detail {

  class CoilPerformanceDXCooling_Impl;

} // detail

/** CoilPerformanceDXCooling is a ModelObject that wraps the OpenStudio IDD object 'OS:CoilPerformance:DX:Cooling'. */
class MODEL_API CoilPerformanceDXCooling : public ParentObject {
 public:
  /** @name Constructors and Destructors */
  //@{

  explicit CoilPerformanceDXCooling(const Model& model);

  explicit CoilPerformanceDXCooling(const Model& model,
    const Curve& coolingCapacityFunctionofTemperature,
    const Curve& coolingCapacityFunctionofFlowFraction,
    const Curve& energyInputRatioFunctionofTemperature,
    const Curve& energyInputRatioFunctionofFlowFraction,
    const Curve& partLoadFractionCorrelation);

  virtual ~CoilPerformanceDXCooling() {}

  //@}

  static IddObjectType iddObjectType();

  static std::vector<std::string> condenserTypeValues();

  /** @name Getters */
  //@{

  boost::optional<double> grossRatedTotalCoolingCapacity() const;

  bool isGrossRatedTotalCoolingCapacityAutosized() const;

  boost::optional<double> grossRatedSensibleHeatRatio() const;

  bool isGrossRatedSensibleHeatRatioAutosized() const;

  double grossRatedCoolingCOP() const;

  boost::optional<double> ratedAirFlowRate() const;

  bool isRatedAirFlowRateAutosized() const;

  double fractionofAirFlowBypassedAroundCoil() const;

  Curve totalCoolingCapacityFunctionofTemperatureCurve() const;

  Curve totalCoolingCapacityFunctionofFlowFractionCurve() const;

  Curve energyInputRatioFunctionofTemperatureCurve() const;

  Curve energyInputRatioFunctionofFlowFractionCurve() const;

  Curve partLoadFractionCorrelationCurve() const;

  double nominalTimeforCondensateRemovaltoBegin() const;

  double ratioofInitialMoistureEvaporationRateandSteadyStateLatentCapacity() const;

  double maximumCyclingRate() const;

  double latentCapacityTimeConstant() const;

  boost::optional<Node> condenserAirInletNode() const;

  std::string condenserType() const;

  double evaporativeCondenserEffectiveness() const;

  boost::optional<double> evaporativeCondenserAirFlowRate() const;

  bool isEvaporativeCondenserAirFlowRateAutosized() const;

  boost::optional<double> evaporativeCondenserPumpRatedPowerConsumption() const;

  bool isEvaporativeCondenserPumpRatedPowerConsumptionAutosized() const;

  boost::optional<Curve> sensibleHeatRatioFunctionofTemperatureCurve() const;

  boost::optional<Curve> sensibleHeatRatioFunctionofFlowFractionCurve() const;

  //@}
  /** @name Setters */
  //@{

  bool setGrossRatedTotalCoolingCapacity(double grossRatedTotalCoolingCapacity);

  void autosizeGrossRatedTotalCoolingCapacity();

  bool setGrossRatedSensibleHeatRatio(double grossRatedSensibleHeatRatio);

  void autosizeGrossRatedSensibleHeatRatio();

  bool setGrossRatedCoolingCOP(double grossRatedCoolingCOP);

  bool setRatedAirFlowRate(double ratedAirFlowRate);

  void autosizeRatedAirFlowRate();

  bool setFractionofAirFlowBypassedAroundCoil(double fractionofAirFlowBypassedAroundCoil);

  bool setTotalCoolingCapacityFunctionofTemperatureCurve(const Curve& curve);

  bool setTotalCoolingCapacityFunctionofFlowFractionCurve(const Curve& curve);

  bool setEnergyInputRatioFunctionofTemperatureCurve(const Curve& curve);

  bool setEnergyInputRatioFunctionofFlowFractionCurve(const Curve& curve);

  bool setPartLoadFractionCorrelationCurve(const Curve& curve);

  bool setNominalTimeforCondensateRemovaltoBegin(double nominalTimeforCondensateRemovaltoBegin);

  bool setRatioofInitialMoistureEvaporationRateandSteadyStateLatentCapacity(double ratioofInitialMoistureEvaporationRateandSteadyStateLatentCapacity);

  bool setMaximumCyclingRate(double maximumCyclingRate);

  bool setLatentCapacityTimeConstant(double latentCapacityTimeConstant);

  bool setCondenserAirInletNode(const Node& node);

  void resetCondenserAirInletNode();

  bool setCondenserType(std::string condenserType);

  bool setEvaporativeCondenserEffectiveness(double evaporativeCondenserEffectiveness);

  bool setEvaporativeCondenserAirFlowRate(double evaporativeCondenserAirFlowRate);

  void autosizeEvaporativeCondenserAirFlowRate();

  bool setEvaporativeCondenserPumpRatedPowerConsumption(double evaporativeCondenserPumpRatedPowerConsumption);

  void autosizeEvaporativeCondenserPumpRatedPowerConsumption();

  bool setSensibleHeatRatioFunctionofTemperatureCurve(const Curve& curve);

  void resetSensibleHeatRatioFunctionofTemperatureCurve();

  bool setSensibleHeatRatioFunctionofFlowFractionCurve(const Curve& curve);

  void resetSensibleHeatRatioFunctionofFlowFractionCurve();

  //@}
  /** @name Other */
  //@{

  //@}
 protected:
  /// @cond
  typedef detail::CoilPerformanceDXCooling_Impl ImplType;

  explicit CoilPerformanceDXCooling(std::shared_ptr<detail::CoilPerformanceDXCooling_Impl> impl);

  friend class detail::CoilPerformanceDXCooling_Impl;
  friend class Model;
  friend class IdfObject;
  friend class openstudio::detail::IdfObject_Impl;
  /// @endcond
 private:
  REGISTER_LOGGER("openstudio.model.CoilPerformanceDXCooling");
};

/** \relates CoilPerformanceDXCooling*/
typedef boost::optional<CoilPerformanceDXCooling> OptionalCoilPerformanceDXCooling;

/** \relates CoilPerformanceDXCooling*/
typedef std::vector<CoilPerformanceDXCooling> CoilPerformanceDXCoolingVector;

} // model
} // openstudio

#endif // MODEL_COILPERFORMANCEDXCOOLING_HPP

