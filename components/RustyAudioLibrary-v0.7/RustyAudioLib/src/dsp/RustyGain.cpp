#include "dsp/RustyGain.hpp"

RustyGain::RustyGain(float gain) : _gain(gain) {};

void RustyGain::process(float* sample)
{
  auto it = _modulators.find("gain");

  if(it != _modulators.end()) { // Check if we have a modulator for gain
    // If there is a modulator with the associated name we use it and assign it's value to _gain
    _gain = it->second->getCurrentValue();
    it->second->usedByProcessor(); // Telling modulator that this processor has used it
  }

  *sample *= _gain;
}

void RustyGain::setModulatorForParameter(std::string parameter, RustyModulator* modulator)
{
  // Check parameter is valid for DSP unit
  if(parameter == "gain") {
    _modulators[parameter] = modulator; // If the parameter is good we add the modulator to the map of modulators
    modulator->addReference(); // Telling modulator it has a reference to it, this is to allow for state to be updated appropriately
  } else {
    std::cerr << "Error: invalid parameter for modulation '" << parameter << "'" << std::endl;
  }
}

void RustyGain::setParameter(std::string parameter, std::string value) //
{
  float val;
  if(isNumeric(value)) // Checking value is numeric to avoid runtime crashes
  {
    val = std::stof(value);
  }
  else
  {
    std::cout << "`VALUE` argument is not numeric " << std::endl;
    return;
  }

  if(parameter == "gain") // Checking for gain parameter input
  {
    if(val > 1.0 || val <= 0.0) {
      std::cout << "Constrain frequency values between " << 0.0 << " and " << 1.0 << std::endl;
      return;
    }
    _gain = val;
    return;
  }
  else // Dealing with bad parameter names
  {
    std::cout << "No parameter with name: " << parameter << std::endl;
    return;
  }
}

std::string RustyGain::getProcessorType()
{
  return "RustyGain";
}

std::string RustyGain::getParams()
{
  return "gain";
}