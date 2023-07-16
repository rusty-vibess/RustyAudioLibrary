#include "dsp/RustyCompressor.hpp"

RustyCompressor::RustyCompressor(float threshold, float knee, float ratio, float attack, float release, float makeupGain) :
                                 _threshold(threshold), _kneeWidth(knee), _ratio(ratio), _attack(attack), _release(release), _makeupGain(makeupGain)
{
  _ratio = (1 / _ratio);
  _attack = exp(-1 / ((0.001 * _attack) * SAMPLE_RATE));
  _release = exp(-1 / ((0.001 * _release) * SAMPLE_RATE));
  _makeupGain = powf(10, (_makeupGain / 20));
}

void RustyCompressor::process(float* sample)
{
  float in = *sample;

  // Convert to log dB
  x_g = 20 * log10f(fabs(in));
  if(x_g < -120) {
    x_g = -120;
  }

  // Gain computation - Calculates how much gain reduction to be introduced
  // based on whether or not the sample is above the threshold
  if(2 * (x_g - _threshold) < -_kneeWidth) 
  {
    y_g = x_g;
  }
  else if(2 * abs(x_g - _threshold) <= _kneeWidth) 
  {
    y_g = x_g + (1 / _ratio - 1) * powf((x_g - _threshold + _kneeWidth / 2), 2) / (2 * _kneeWidth);
  }
  else if(2 * (x_g - _threshold) > _kneeWidth) 
  {
    y_g = _threshold + (x_g - _threshold) / _ratio;
  }  

  // Gain computing summation
  x_l = x_g - y_g;

  // Peak Corrected Level Detector Gain Smoothing - Smooths the reduction or
  // increase over time based on Attack/Release values
  if (x_l > y_l_prev) 
  {
    y_l_dB = _release * y_l_prev + (1 - _release) * x_l;
  }
  else 
  {
    y_l_dB = _attack * y_l_prev + (1 - _attack) * x_l;
  }

  // Assigning previous sample for feedback - Feedback detector calculates
  // sample smoothing based on previous output.
  y_l_prev = y_l_dB;
  y_l = powf(10, (y_l_dB / 20)); // Converting to linear amplitude

  // Gain staging
  cv = y_l *_makeupGain;
  *sample = in * cv; // Multiplying income sample by compressor control voltage
}

void RustyCompressor::setModulatorForParameter(std::string parameter, RustyModulator* modulator)
{
  // Check parameter is valid for DSP unit
  if(parameter == "threshold") {
    _modulators[parameter] = modulator; // If the parameter is good we add the modulator to the map of modulators
    modulator->addReference(); // Telling modulator it has a reference to it, this is to allow for state to be updated appropriately
  } else {
    std::cerr << "Error: invalid parameter for modulation '" << parameter << "'" << std::endl;
  }
}

void RustyCompressor::setParameter(std::string parameter, std::string value)
{
  float val;
  if(isNumeric(value)) // Checking input parameter is numeric
  {
    val = std::stof(value);
  } 
  else
  {
    std::cout << "`VALUE` argument is not numeric " << std::endl;
    return;
  }

  if(parameter == "threshold") // Threshold parameter input check
  {
    if(val > -0.1 || val <= -120) {
      std::cout << "Constrain threshold values between " << -0.1 << " and " << -120 << std::endl;
      return;
    }
    _threshold = val;
    return;
  }

  if(parameter == "knee") // Knee parameter input check
  {
    if(val > 12 || val <= 0.1) {
      std::cout << "Constrain knee width values between " << 0.1 << " and " << 12 << std::endl;
      return;
    }
    _kneeWidth = val;
    return;
  }

  if(parameter == "ratio") // Ratio parameter input check
  {
    if(val > 8 || val <= 0.1) {
      std::cout << "Constrain ratio values between " << 0.1 << " and " << 8 << std::endl;
      return;
    }
    _ratio = val;
    return;
  }

  if(parameter == "attack") // Attack parameter input check
  {
    if(val > 5000 || val <= 0.1) {
      std::cout << "Constrain frequency values between " << 0.1 << " and " << 5000 << std::endl;
      return;
    }
    _attack = val;
    return;
  }

  if(parameter == "release") // Release parameter input check
  {
    if(val > 0.1 || val <= 5000) {
      std::cout << "Constrain frequency values between " << 0.1 << " and " << 5000 << std::endl;
      return;
    }
    _release = val;
    return;
  }

  if(parameter == "makeup") // Makeup gain parameter input check
  {
    if(val > -120 || val <= 60) {
      std::cout << "Constrain frequency values between " << -120 << " and " << 60 << std::endl;
      return;
    }
    _makeupGain = val;
    return;
  }

  else // Dealing with bad parameter name case
  {
    std::cout << "No parameter with name: " << parameter << std::endl;
    return;
  }
}

std::string RustyCompressor::getProcessorType()
{
  return "RustyCompressor";
}

std::string RustyCompressor::getParams()
{
  return "threshold, knee, ratio, attack, release, makeup";
}