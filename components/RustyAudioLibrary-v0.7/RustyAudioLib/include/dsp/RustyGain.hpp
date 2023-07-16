#ifndef RUSTY_GAIN_HPP
#define RUSTY_GAIN_HPP

#include "dsp/RustyProcessor.hpp"

//----------------------------------------------------------------------------
/**
 * @ingroup RustyProcessors
 *
 * @class RustyGain
 * @brief Gain control unit.
 *
 * Simple gain multiplication unit to amplify or reduce audio signal.
 *
 * Inherited from RustyProcessor.
 */
class RustyGain : public RustyProcessor
{
public:
    /**
    //--------------------------------------------------------------------------
    * @brief Constructor for RustyGain
    *
    * Takes initial parameters for RustyGain and preps the unit for
    * sample processing.
    *
    * @param gain Float of multiplier to increase gain by.
    */
    RustyGain(float gain);

    void process(float* sample) override;
    void setModulatorForParameter(std::string parameter, RustyModulator* modulator) override;
    void setParameter(std::string parameter, std::string value) override;
    std::string getProcessorType() override;
    std::string getParams() override;

private:  
    float _gain; ///< Gain value
};

#endif