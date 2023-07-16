#ifndef RUSTY_MODULATOR_HPP
#define RUSTY_MODULATOR_HPP

#include "types/RustyCommonTypes.hpp"

/**
 * @defgroup RustyModulators RustyModulators
 * @brief Modulator classes to perform parameter manipulation
 */

//----------------------------------------------------------------------------
/**
 * @ingroup RustyModulators
 *
 * @class RustyModulator
 * @brief PARENT - Modulator unit (RustyModulators) parent class.
 *
 * Standardises modulator behaviour within the library. All modulator classes
 * inherit from this base class.
 *
 * RustyModulator uses getCurrentValue() to return the current position of the
 * modulator and then updateState() to iterate the modulator one step when all
 * processors that are attached to the modulator have used that value.
 *
 */
class RustyModulator
{
  public:
    virtual ~RustyModulator() { std::cout << "RustyModulator destroyed..." << std::endl; };

  /**
  //--------------------------------------------------------------------------
   * @brief Returns a modulator value for use on a RustyProcessor.
   *
   * This pure virtual function will get the state of the modulator at that
   * moment in time. Ie. LFO position.
   *
   * @return Double corresponding to a modulator position.
   */
    virtual double getCurrentValue() = 0;

  /**
  //--------------------------------------------------------------------------
   * @brief Update modulator state and iterate forward a step.
   *
   * This function iterates the modulator. This isn't called directly by any
   * RustyProcessor but is instead called from within the RustyModulator when
   * it has been informed that all attached processors have used the modulator.
   *
   * @see usedByProcessor()
   */
    virtual void updateState() = 0;

  /**
  //--------------------------------------------------------------------------
   * @brief Sets RustyModulator parameter to new value.
   *
   * This is pure virtual function that assigns a new value to a parameter of
   * RustyModulator derived units. This function is used at runtime
   * from CLI interaction.
   *
   * @param parameter String of the parameter to affect.
   * @param modulator Pointer to instance of RustyModulator.
   */
    virtual void setParameter(std::string parameter, std::string value) = 0;

  /**
  //--------------------------------------------------------------------------
   * @brief Tells RustyModulator that a RustyProcessor has been attached.
   *
   * This function keeps track of how many processor units have parameters
   * bound to an instance of a RustyModulator derived class. This is so update
   * state isn't called after every read of the RustyModulator. 
   *
   * @see removeReference()
   * @see updateState()
   * @see usedByProcessor()
   */
    void addReference() { _refCount++; }

  /**
  //--------------------------------------------------------------------------
   * @brief Tells RustyModulator that a RustyProcessor has been removed.
   *
   * This function removes a processor reference. This function isn't actually
   * used at any point but was added to allow for expansion in the future.
   *
   * @see removeReference()
   * @see updateState()
   * @see usedByProcessor()
   */
    void removeReference() { if( _refCount > 0) _refCount--; }

  /**
  //--------------------------------------------------------------------------
   * @brief Called after every modulator read.
   *
   * This function keeps track of whether all RustyProcessors have read the
   * RustyModulator at that point in time. This was necessary as if there was
   * more than one processor using the same LFO, how else would it know when to
   * update.
   *
   * @see removeReference()
   * @see updateState()
   * @see usedByProcessor()
   */
    void usedByProcessor() {
      if(++_useCount == _refCount) {
        this->updateState();
        _useCount = 0;
      }
    }

  /**
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
   * @brief Function to get modulator type.
   *
   * Simple function allows CLI to test modulator types for user interaction.
   *
   * @return Returns string of RustyModulator child type.
   */
    virtual std::string getModulatorType() = 0;

  /**
  //--------------------------------------------------------------------------
   * @brief Function to get modulator params.
   *
   * Simple function allows CLI to get available params for RustyModulator child.
   *
   * @return Returns string of parameters available for RustyModulator child.
   */
    virtual std::string getParams() = 0;

  protected:
    int _refCount = 0; ///< Count of RustyProcessors that modulator is attached to
    int _useCount = 0; ///< Count of RustyProcessors usage at that moment in time
};

#endif