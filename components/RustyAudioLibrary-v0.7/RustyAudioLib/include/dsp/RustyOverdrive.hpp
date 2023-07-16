#ifndef RUSTY_OVERDRIVE_HPP
#define RUSTY_OVERDRIVE_HPP

#include "dsp/RustyProcessor.hpp"

//----------------------------------------------------------------------------
/**
 * @ingroup RustyProcessors
 *
 * @class RustyOverdrive
 * @brief Overdrive distortion unit
 *
 * Arc-Tan soft-clip style overdrive distortion unit.
 *
 * Inherited from RustyProcessor.
 */
class RustyOverdrive : public RustyProcessor
{
public: 
    /**
    //--------------------------------------------------------------------------
    * @brief Constructor for RustyOverdrive
    *
    * Takes initial parameters for RustyOverdrive and preps the unit for
    * sample processing.
    *
    * @param drive Float of drive multiplier to distort signal by.
    */
  RustyOverdrive(float drive);

  void process(float* sample) override;
  void setModulatorForParameter(std::string parameter, RustyModulator* modulator) override;
  void setParameter(std::string parameter, std::string value) override;
  std::string getProcessorType() override;
  std::string getParams() override;

private:
  /**
  //--------------------------------------------------------------------------
  * @brief Sample distortion calculation
  *
  * Distorts sample using arc-tan algorithm.
  * 
  * @param inputSample Float* of input sample.
  */
  float distortSample(float* inputSample);

  float _drive; ///< Gain value
};

#endif