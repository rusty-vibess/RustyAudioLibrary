#ifndef RUSTY_PROCESSOR_HPP
#define RUSTY_PROCESSOR_HPP

#include <iostream>
#include <string>
#include <map>
#include <cmath>
#include "modulators/RustyModulator.hpp"

/**
 * @defgroup RustyProcessors RustyProcessors
 * @brief Processor classes to perform DSP.
 */

//----------------------------------------------------------------------------
/**
 * @ingroup RustyProcessors
 *
 * @class RustyProcessor
 * @brief PARENT - DSP unit (RustyProcessors) parent class.
 *
 * Standardises processing within the library. All processor classes inherit
 * from this base class.
 *
 * RustyProcessor uses process() to handle audio manipulation on a sample by
 * sample basis.
 */
class RustyProcessor
{
public:
    virtual ~RustyProcessor() { std::cout << "RustyProcessor destroyed..." << std::endl; };

  /**
  //--------------------------------------------------------------------------
   * @brief Performs DSP on an audio sample.
   *
   * This pure virtual function performs DSP algorithms on a sample by sample
   * basis. A pointer to a 32bitFloat audio sample is passed as an argument and
   * this will be the value used as the input to the DSP function f(x).
   *
   * @param sample Pointer to a 32bitFloat audio sample value.
   */
    virtual void process(float* sample) = 0;

  /**
  //--------------------------------------------------------------------------
   * @brief Binds a RustyModulator to a RustyProcessor instance parameter.
   *
   * This is pure virtual function assigns modulator manipulation to parameters
   * within the DSP unit. Each RustyProcessor derived DSP unit will check the
   * parameter name against suitable parameters, if there is a match the
   * modulator will affect the parameter.
   *
   * @param parameter String of the parameter to affect.
   * @param modulator Pointer to instance of RustyModulator.
   *
   * @see setParameter()
   */
    virtual void setModulatorForParameter(std::string parameter, RustyModulator* modulator) = 0;

  /**
  //--------------------------------------------------------------------------
   * @brief Sets RustyProcessor parameter to new value.
   *
   * This is pure virtual function that assigns a new value to a parameter of
   * RustyProcessor derived DSP units. This function is used at runtime
   * from CLI interaction.
   *
   * @param parameter String of the parameter to affect.
   * @param modulator Pointer to instance of RustyModulator.
   *
   * @see setModulatorForParameter()
   */
    virtual void setParameter(std::string parameter, std::string value) = 0;


  /**
  //--------------------------------------------------------------------------
   * @brief Function to test if value contains numeric characters.
   *
   * Function takes a const reference to string and checks it's component chars
   * with std::isDigit(). If there are any values that are non-numeric and could
   * cause a conversion error at runtime the function return false. Else it
   * returns true.
   *
   * @param str Constant reference of a string.
   *
   * @return Returns true if string characters are numerical.
   */
    bool isNumeric(const std::string& str) {
      for (char c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
      }
      return true;
    }

  /**
  //--------------------------------------------------------------------------
   * @brief Function to get processor type.
   *
   * Simple function allows CLI to test Processor types for user interaction.
   *
   * @return Returns string of RustyProcessor child type.
   */
    virtual std::string getProcessorType() = 0;

  /**
  //--------------------------------------------------------------------------
   * @brief Function to get processor params.
   *
   * Simple function allows CLI to get available params for RustyProcessor child.
   *
   * @return Returns string of parameters available for RustyProcessor child.
   */
    virtual std::string getParams() = 0;

    std::map<std::string, RustyModulator*> _modulators; ///< Map of processors affecting derived RustyProcessor.
};

#endif