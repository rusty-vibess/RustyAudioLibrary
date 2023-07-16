#ifndef RUSTY_COMPRESSOR_HPP_OLD
#define RUSTY_COMPRESSOR_HPP_OLD

#include "dsp/RustyProcessor.hpp"
#include "utils/RustyRingBuffer.hpp"
#include "types/RustyCommonTypes.hpp"
#include <tuple>
#include <cmath>
#include <cstdlib>
#include <memory>

//----------------------------------------------------------------------------
/**
 * @ingroup RustyProcessors
 *
 * @class RustyCompressor
 * @brief Dynamic level compression DSP unit.
 *
 * Log-domain feedback compressor, that is able to dynamically reduce audio signals.
 *
 * Inherited from RustyProcessor.
 */
class RustyCompressor : public RustyProcessor
{
public:
  /**
  //--------------------------------------------------------------------------
   * @brief Constructor for RustyCompressor
   *
   * Takes initial parameters for RustyCompressor and preps the compressor for
   * sample processing.
   *
   * @param threshold Float of threshold in dB.
   * @param knee Float of knee curve in dB.
   * @param ratio Float of compression ratio.
   * @param attack Float of attack speed in Ms.
   * @param release Float of release speed in ms.
   * @param makeupGain Float of gain makeup in dB.
   */
  RustyCompressor(float threshold, float knee, float ratio, float attack, float release, float makeupGain);

  void process(float* sample) override;
  void setModulatorForParameter(std::string parameter, RustyModulator* modulator) override;
  void setParameter(std::string parameter, std::string value) override;

  virtual std::string getProcessorType() override;
  virtual std::string getParams() override;

private:
  // Lookahead on compressor was the one thing I couldn't implement to work properly
  RustyRingBuffer _lookAheadBuffer{32}; // TODO - Lookahead causing Compressor to not work

  float _threshold; ///< Compression threshold
  float _kneeWidth = 4; ///< Compression knee
  float _ratio; ///< Compression reduction ratio
  float _attack; ///< Compressor attack speed
  float _release; ///< Compressor release speed
  float _makeupGain; ///< Gain makeup variable

  float x_g; ///< dB input sample
  float y_g; ///< Gain Computation output
  float x_l; ///< Gain computation post sum
  float y_l_dB; ///< Gain reduction with smoothing in dB
  float y_l; ///< Gain reduction with smoothing in linear value
  float y_l_prev = 0.001; ///< Previous compressor sidechain output
  float cv = 0; ///< Compressor multiplier

  float* read_ptr = new float; ///< Read pointer for RingBuffer
};

#endif