#ifndef RUSTY_FILTER_HPP
#define RUSTY_FILTER_HPP

#include "types/RustyCommonTypes.hpp"
#include "dsp/RustyProcessor.hpp"
#include <cctype>

//----------------------------------------------------------------------------
/**
 * @ingroup RustyProcessors
 *
 * @class RustyFilter
 * @brief Variable state IIR Filter.
 *
 * 2-order IIR filter with four states LOW_PASS, HIGH_PASS, BAND_PASS, NOTCH.
 *
 * Inherited from RustyProcessor.
 */
class RustyFilter : public RustyProcessor
{
  public:
    /**
    //--------------------------------------------------------------------------
    * @brief Constructor for RustyFilter
    *
    * Takes initial parameters for RustyFilter and preps the unit for
    * sample processing.
    *
    * @param type RustyFilterType to define filter type.
    * @param frequency Float of filter cutoff frequency.
    * @param ratio Float of q ratio.
    */
    RustyFilter(RustyFilterType type, float frequency, float q);

    void process(float* sample) override;
    void setModulatorForParameter(std::string parameter, RustyModulator* modulator) override;
    void setParameter(std::string parameter, std::string value) override;
    std::string getProcessorType() override;
    std::string getParams() override;

  private:
    /**
    //--------------------------------------------------------------------------
    * @brief Biquad variable calculator
    *
    * Calculates filter biquads based on user's input variables or modulator changes.
    */
    void calcCoefficients();

    /**
    //--------------------------------------------------------------------------
    * @brief Constrain function between a min and max.
    *
    * Constrains particular variables that would break IIR Filter.
    */    
    float constrain(float val, float min, float max)
    { return std::min(std::max(val, min), max); }

    float _a1 = 0; ///< Biquad variable
    float _a2 = 0; ///< Biquad variable
    float _b0 = 1; ///< Biquad variable
    float _b1 = 0; ///< Biquad variable
    float _b2 = 0; ///< Biquad variable
    float _x1; ///< Current input sample
    float _y1; ///< Previous output sample
    float _x2; ///< Previous input sample
    float _y2; ///< Previous output sample value -1
    float _k; ///< Biquad variable
    float _norm; ///< Biquad variable
    float _sum; ///< Filter output

    float _sampleRate, _frequency, _q; ///< Filter variables
    RustyFilterType _type; ///< Filter type
};

#endif