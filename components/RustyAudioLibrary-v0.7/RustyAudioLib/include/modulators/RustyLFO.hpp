#ifndef RUSTY_LFO_HPP
#define RUSTY_LFO_HPP

#include <iostream>
#include <cmath>
#include "types/RustyCommonTypes.hpp"
#include "modulators/RustyModulator.hpp"

//----------------------------------------------------------------------------
/**
 * @ingroup RustyModulators
 *
 * @class RustyLFO
 * @brief LFO class to perform modulation on RustyProcessor Parameters.
 *
 * Simple LFO class designed to allow user to manipulate parameters on
 * processing units.
 * 
 * Inherited from RustyModulator 
 */
class RustyLFO : public RustyModulator
{
  public:
    /**
    //--------------------------------------------------------------------------
    * @brief Constructor for RustyLFO
    *
    * Takes initial parameters for RustyLFO and preps the object for processing.
    *
    * @param frequency Float of LFO frequency
    * @param minValue Float of LFO minimum range
    * @param maxValue Float of LFO maximum range
    */
    RustyLFO( double frequency, double minValue, double maxValue );
    double getCurrentValue() override;
    void updateState() override;
    void setParameter(std::string parameter, std::string value) override;

    std::string getModulatorType() override;
    std::string getParams() override;

  private:
    // If value is rewritten we use pointers to ensure there is no dynamic memory allocation in the real-time audio thread.
    // double _rawValue; ///< Raw oscillator value before bounding to range
    double _value; ///< Oscillator value after being bound to range
    double _phase; ///< Phase position used to update LFO
    double _frequency; ///< Frequency of Oscillator in ms
    double _minValue; ///< Minimum value of LFO range
    double _maxValue; ///< Maximum value of LFO range
    double _sampleRate; ///< Sample rate to calculate LFO positions against

  /**
  //--------------------------------------------------------------------------
  * @brief Calculates LFO position
  *
  * Uses phase calculated across the sample rate to calculate LFO's sine wave position.
  * 
  * @see getCurrentValue()
  */
    double oscillate();
};

#endif