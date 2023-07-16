#include "modulators/RustyLFO.hpp"

RustyLFO::RustyLFO(double frequency, double minValue, double maxValue) : _frequency(frequency), _minValue(minValue), _maxValue(maxValue)
{
  std::cout << "RustyLFO constructed..." << std::endl;
  _sampleRate = SAMPLE_RATE;
}

double RustyLFO::getCurrentValue()
{
  double rawValue = oscillate();

  return _minValue + (rawValue + 1) / 2 * (_maxValue - _minValue);
}

void RustyLFO::updateState() 
{
  _phase += _frequency / _sampleRate;
  if(_phase >= 1.0) _phase -= 1.0;
}

void RustyLFO::setParameter(std::string parameter, std::string value)
{
  float val;
  if(isNumeric(value))
  {
    val = std::stof(value);
  } 
  else
  {
    std::cout << "`VALUE` argument is not numeric " << std::endl;
    return;
  }
  if(parameter == "frequency")
  {
    if(val > 1000 || val <= 0) {
      std::cout << "Constrain frequency values between " << 20 << " and " << 20000 << std::endl;
      return;
    }
    _frequency = val;
    return;
  }
  if(parameter == "minVal")
  {
    _minValue = val;
    return;
  }
  if(parameter == "maxVal")
  {
    _maxValue = val;
    return;
  }
  else 
  {
    std::cout << "No parameter with name: " << parameter << std::endl;
    return;
  }
}

double RustyLFO::oscillate()
{
  _value = sin(2.0 * M_PI * _phase); 

  return _value;
}

std::string RustyLFO::getModulatorType()
{
  return "RustyLFO";
}

std::string RustyLFO::getParams()
{
  return "frequency, minVal, maxVal";
}