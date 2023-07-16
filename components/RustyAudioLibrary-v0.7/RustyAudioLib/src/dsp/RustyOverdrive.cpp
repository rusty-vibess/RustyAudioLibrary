#include "dsp/RustyOverdrive.hpp"

RustyOverdrive::RustyOverdrive(float drive) : _drive(drive) {}

void RustyOverdrive::process(float* sample) 
{
  auto it = _modulators.find("drive");

  if(it != _modulators.end()) { // Check if we have a modulator for gain
    // If there is a modulator with the associated name we use it and assign it's value to _gain
    _drive = it->second->getCurrentValue();
    it->second->usedByProcessor(); // Telling modulator that this processor has used it]
  }

  *sample = distortSample(sample); // Distorting sample with arc tan

  *sample /= (M_PI / 2.0); // Normalising to maximum range of signal
}

void RustyOverdrive::setModulatorForParameter(std::string parameter, RustyModulator* modulator)
{
  // Check parameter is valid for DSP unit
  if(parameter == "drive") {
    _modulators[parameter] = modulator; // If the parameter is good we add the modulator to the map of modulators
    modulator->addReference(); // Telling modulator it has a reference to it, this is to allow for state to be updated appropriately
  } else {
    std::cerr << "Error: invalid parameter for modulation '" << parameter << "'" << std::endl;
  }
}

void RustyOverdrive::setParameter(std::string parameter, std::string value)
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

  if(parameter == "drive") // Drive parameter input check
  {
    if(val > 30 || val <= 0) {
      std::cout << "Constrain frequency values between " << 0 << " and " << 30 << std::endl;
      return;
    }
    _drive = val;
    return;
  }
  else // Dealing with bad parameter name case
  {
    std::cout << "No parameter with name: " << parameter << std::endl;
    return;
  }
}

float RustyOverdrive::distortSample(float* inputSample)
{
  // Distortion algorithm using drive variable and arc-tan (std::atan)
  return std::atan(*inputSample * _drive);
}

std::string RustyOverdrive::getProcessorType()
{
  return "RustyOverdrive";
}

std::string RustyOverdrive::getParams()
{
  return "drive";
}