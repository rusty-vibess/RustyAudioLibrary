#include "dsp/RustyFilter.hpp"

RustyFilter::RustyFilter(RustyFilterType type, float frequency, float q) :  _frequency(frequency), 
                                                                            _q(q), 
                                                                            _type(type)
{
  _sampleRate = SAMPLE_RATE; // From common types
  calcCoefficients();
}

void RustyFilter::process(float* sample)
{
  auto it = _modulators.find("frequency");

  if(it != _modulators.end()) { // Check if we have a modulator for frequency
    // If there is a modulator with the associated name we use it and assign it's value to _gain
    _frequency = it->second->getCurrentValue();
    it->second->usedByProcessor(); // Telling modulator that this processor has used it
    calcCoefficients();
  }

  it = _modulators.find("q");

  if(it != _modulators.end()) { // Check if we have a modulator for q
    // If there is a modulator with the associated name we use it and assign it's value to _gain
    _q = constrain(it->second->getCurrentValue(), 0.1, 10); // Contraint for LFO to q to stop a bad q value breaking the filter (which is a common IIR filter issue)
    it->second->usedByProcessor(); // Telling modulator that this processor has used it
    calcCoefficients();
  }

  _sum = _b0 * *sample + // Biquad filter calculation
        _b1 * _x1 +
        _b2 * _x2 -
        _a1 * _y1 -
        _a2 * _y2;

  _x2 = _x1;
  _x1 = *sample;
  _y2 = _y1;
  _y1 = _sum;
        
  *sample = _sum;
}

void RustyFilter::setModulatorForParameter(std::string parameter, RustyModulator* modulator)
{
  // Check parameter is valid for DSP unit
  if(parameter == "frequency") {
    _modulators[parameter] = modulator; // If the parameter is good we add the modulator to the map of modulators
    modulator->addReference(); // Telling modulator it has a reference to it, this is to allow for state to be updated appropriately
  } else {
    std::cerr << "Error: invalid parameter for modulation '" << parameter << "'" << std::endl;
  }
}

void RustyFilter::setParameter(std::string parameter, std::string value)
{
  float val;
  if(isNumeric(value)) // Checking if value is numeric
  {
    val = std::stof(value);
  } 
  else
  {
    std::cout << "`VALUE` argument is not numeric " << std::endl;
    return;
  }

  if(parameter == "frequency") // Checking for frequency parameter
  {
    if(val > 20000 || val <= 20) {
      std::cout << "Constrain frequency values between " << 20 << " and " << 20000 << std::endl;
      return;
    }
    _frequency = val;
    calcCoefficients();
    return;
  }

  if(parameter == "q") // Checking for q parameter
  {
    if(val > 10 || val <= 0.1) { // Value constraint on realtime adjustment
      std::cout << "Constrain q values between " << 0.1 << " and " << 10 << std::endl;
      return;
    }
    _q = val;
    calcCoefficients();
    return;
  }

  else 
  {
    std::cout << "No parameter with name: " << parameter << std::endl; // Bad CLI input case
    return;
  }
}

void RustyFilter::calcCoefficients()
{
  _k = tanf(M_PI * _frequency / _sampleRate);
  _norm = 1.0 / (1 + _k / _q + _k * _k);

  switch (_type) {
    case LOW_PASS: // Lowpass Filter Coefficient Formula
      _b0 = _k * _k * _norm;
      _b1 = 2.0 * _b0;
      _b2 = _b0;
      _a1 = 2 * (_k * _k - 1) * _norm;
      _a2 = (1 - _k / _q + _k * _k) * _norm;
      break;
    
    case HIGH_PASS: // Highpass Filter Coefficient Formula
      _b0 = 1 * _norm;
      _b1 = -2 * _b0;
      _b2 = _b0;
      _a1 = 2 * (_k * _k -1) * _norm;
      _a2 = (1 - _k / _q + _k * _k) * _norm;
      break;
    
    case BAND_PASS: // Bandpass Filter Coefficients Formula
      _b0 = _k / _q * _norm;
      _b1 = 0;
      _b2 = -_b1;
      _a1 = 2 * (_k * _k - 1) * _norm;
      _a2 = (1 - _k / _q + _k * _k) * _norm;
      break;
        
    case NOTCH: // Notch Filter Coefficients Formula
      _b0 = (1 + _k * _k) * _norm;
      _b1 = 2 * (_k * _k - 1) * _norm;
      _b2 = _b0;
      _a1 = _b1;
      _a2 = (1 - _k / _q + _k * _k) * _norm;
      break;
  }
}

std::string RustyFilter::getProcessorType()
{
  return "RustyFilter";
}

std::string RustyFilter::getParams()
{
  return "frequency, q";
}