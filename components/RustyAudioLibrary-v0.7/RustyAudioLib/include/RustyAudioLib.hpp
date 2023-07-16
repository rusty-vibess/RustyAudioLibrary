#ifndef RUSTY_AUDIO_LIB
#define RUSTY_AUDIO_LIB

#include <vector>
#include <thread>
#include <typeinfo>
#include "types/RustyCommonTypes.hpp"
#include "engine/RustyAudioEngine.hpp"

#include "dsp/RustyProcessor.hpp"
#include "dsp/RustyFilter.hpp"
#include "dsp/RustyGain.hpp"
#include "dsp/RustyOverdrive.hpp"
#include "dsp/RustyCompressor.hpp"

#include "modulators/RustyModulator.hpp"
#include "modulators/RustyLFO.hpp"

#include "cli/RustyCLI.hpp"

/**
 * @defgroup API RustyAudioLibAPI
 * @brief API class to allow for easy library usage
 */

//----------------------------------------------------------------------------
/**
 * @ingroup API
 * 
 * @class RustyAudioLibAPI
 * @brief Top-Level API to interact with RustyAudioLibrary (RustyAudioLib.hpp).
 *
 * RustyAudioLibAPI follows a factory style top level implementation. Meaning
 * instead of interacting with the library directly, instead there are creation functions.
 * Creation functions will create instances of RustyAudioLibrary units. Creation functions will then return a pointer to the created unit allowing for further
 * manipulation at runtime or within the user's `main()` code.
 *
 * Must call init() function to open audio stream and start CLI. Call it at the
 * end of `main()`.
 *
 * Audio-related heavy lifting is handled by RustyAudioEngine. CLI handled by
 * RustyCLI.
 *
 * @see RustyAudioEngine
 * @see RustyCLI
 */
class RustyAudioLibAPI
{
public:
  //--------------------------------------------------------------------------
  /**
   * @brief RustyAudioLibAPI constructor.
   *
   * Constructor creates relevant objects. No interaction from user needed.
   */
  RustyAudioLibAPI() : cli([this](std::vector<std::string> inputParsed) {
    this->cliCallback(inputParsed); // CLI callback for calling correct functions from parse commands
  }) { std::cout << "RustyAudioLib constructed..." << std::endl; };

  //--------------------------------------------------------------------------
  /**
   * @brief RustyAudioLibAPI destructor.
   *
   * Joins CLI thread before initialising shutdown. No interaction from user
   * needed.
   */
  ~RustyAudioLibAPI() {
    if(cliThread.joinable()) {
      cliThread.join(); // Joining CLI thread so it is destroyed with Library
    }
    std::cout << "RustyAudioLib destroyed..." << std::endl;
  }

  //--------------------------------------------------------------------------
  /**
   * @brief Initialisation function for RustyAudioLib.
   *
   * RustyAudioLib initialise function calls the relevant functions inside
   * RustyCLI and RustyAudioEngine to start them running. 
   *
   * For the RustyAudioEngine it starts the stream, opening it to the chosen
   * output/input. 
   *
   * For RustyCLI it begins the loop needed to check the console for inputs.
   *
   * RustyAudioEngine and RustyCLI also run on separate threads, this functions
   * helps instantiate that.
   * 
   * Ensure this function is called at the END of your main.
   *
   * @see initCLI()
   */
  void init() {
    engine.init(); // Calls RustyAudioEngine init
    initCLI(); // Inits CLI
  }

  //--------------------------------------------------------------------------
  /**
   * @brief Callback from CLI.
   *
   * Function is called from RustyCLI when users input commands that need to
   * interact with information only available in the RustyAudioLibAPI.
   *
   * @param inputParsed Parsed user CLI commands.
   *
   * @see RustyCLI::commandHandler()
   */
  void cliCallback(std::vector<std::string> inputParsed)
  {
    // Prints relevant information based on users command
    if(inputParsed[0] == "-p" || inputParsed[0] == "--processors")
    {
      printProcessors();
      return;
    }
    if(inputParsed[0] == "-m" || inputParsed[0] == "--modulators")
    {
      printModulators();
      return;
    }
    if(inputParsed[0] == "-param")
    { 
      // If comment is -param (run-time paramater change),  this function passed the commands to the setParam function
      setParam(inputParsed[1], inputParsed[2], inputParsed[3]);
      return;
    }
  }

  //--------------------------------------------------------------------------
  /**
   * @brief Prints instantiated processors.
   *
   * Function is normally called by the CLI when user is interacting with
   * RustyAudioTool at runtime.
   * 
   * ```
   * # CLI call
   * [-p, --processors]
   * ```
   * This function will print to console all RustyProcessors that have been
   * created. This will include RustyProcessors that have not been added to the
   * processing chain in RustyAudioEngine.
   *
   * @see RustyCLI::commandHandler()
   */
  void printProcessors()
  {
    // Prints all instantiated processors along with their type and adjustable params.
    std::cout << "Processors currently instantiated: " << std::endl << std::endl;
    for(const auto &unit : _processors)
    {
      std::cout << "PROCESSOR NAME: `" << unit.first << "`" <<std::endl; 
      std::cout << "RustyProcessor: " << unit.second->getProcessorType() << std::endl;
      std::cout << "PARAMS available: " << unit.second->getParams() << std::endl << std::endl;
    }
  }

  //--------------------------------------------------------------------------
  /**
   * @brief Prints instantiated modulators.
   *
   * Function is normally called by the CLI when user is interacting with
   * RustyAudioTool at runtime.
   *
   * ```
   * # CLI call
   * [-m, --modulators]
   * ```
   * This function will print to console all RustyModulators that have been
   * created. This will include RustyModulators that have not been added to any
   * RustyProcessors.
   *
   * @see RustyCLI::commandHandler()
   */
  void printModulators()
  {
    // Prints all instantiated modulators along with their type and adjustable params.
    std::cout << "Modulators currently instantiated: " << std::endl << std::endl;
    for(const auto &unit : _modulators)
    {
      std::cout << "MODULATOR NAME: `" << unit.first << "`" <<std::endl; 
      std::cout << "RustyModulator: " << unit.second->getModulatorType() << std::endl;
      std::cout << "PARAMS available: " << unit.second->getParams() << std::endl << std::endl;
    }
  }

  //--------------------------------------------------------------------------
  /**
   * @brief Set Parameters inside units at run time.
   *
   * Function is normally called by the CLI when user is interacting with
   * RustyAudioTool at runtime.
   * 
   * ```
   * # CLI call
   * [-param], [MODULATOR/PROCESSOR NAME], [PARAM NAME], [NEW VALUE]
   * ```
   *
   * This function allows users to update parameters of units at runtime. This
   * function is called via callback by RustyCLI::commandHandler(); 
   *
   * @param processor Name given to unit when created.
   * @param param Name of parameter to change.
   * @param value New value for parameter.
   *
   * @exception BAD_UNIT_NAME If given processor name doesn't match an
   * existing unit.
   *
   * @see RustyCLI::commandHandler()
   */
  void setParam( std::string unit, std::string param, std::string value ) {
    auto p = _processors.find(unit); // Checking for matching name in _modulators and _processors
    auto m = _modulators.find(unit);

    if(p == _processors.end() && m == _modulators.end()) { // BAD_CASE no match
      std::cerr << "BAD_UNIT_NAME `" << unit << "` does not match a known processor" << std::endl; 
      return;
    }
    if(p != _processors.end()) // PROC_MATCH processor match case
    {
      p->second->setParameter(param, value); // Set relevant param to value
    }
    if(m != _modulators.end()) // MODU_MATCH modulator match case 
    {
      m->second->setParameter(param, value); // Set relevant param to value
    }
  }

  //--------------------------------------------------------------------------
  /**
   * @brief Adds RustyProcessors to signal chain.
   *
   * This function will place the passed in processing unit into the signal
   * chain, allowing the processor to affect the audio. You can add as many
   * units as you want to the signal chain but bare in mind they will be used
   * in the sequence you added them in. First in first used.
   *
   * @param processor A pointer to an instantiated RustyProcessor unit.
   *
   * @see RustyEngine::addProcessor()
   */
  void addProcessorToSignalChain( RustyProcessor* processor ) {
    engine.addProcessor(processor); // Passes pointer of processor to engine to handle signal chain and processor usage
  }

  //--------------------------------------------------------------------------
  /**
   * @brief Creation function for RustyGain.
   *
   * This function creates an instance of a RustyGain and stores it inside a
   * map of instantiated processors.
   *
   * @param name Name given to the created RustyProcessor.
   * @param drive Float of gain multiplier.
   *
   * @return Pointer to created unit.
   */
  RustyProcessor* createGainUnit( std::string name, float gain ) { // Creates a gain unit
    auto processor = std::make_unique<RustyGain>(gain); // Using RAII pointers to ensure memory safety
    _processors[name] = std::move(processor);
    RustyProcessor* ptr = _processors.find(name)->second.get(); // Getting a pointer to the unique_ptr to return to user
    return ptr;
  }

  //--------------------------------------------------------------------------
  /**
   * @brief Creation function for RustyOverdrive.
   *
   * This function creates an instance of a RustyOverdrive and stores it inside a
   * map of instantiated processors.
   *
   * @param name Name given to the created RustyProcessor.
   * @param drive Float of distortion multiplier.
   *
   * @return Pointer to created unit.
   */
  RustyProcessor* createOverdriveUnit( std::string name, float drive)  { // Creates a gain unit
    auto processor = std::make_unique<RustyOverdrive>(drive); // Using RAII pointers to ensure memory safety
    _processors[name] = std::move(processor);
    RustyProcessor* ptr = _processors.find(name)->second.get(); // Getting a pointer to the unique_ptr to return to user
    return ptr;
  }

  //--------------------------------------------------------------------------
  /**
   * @brief Creation function for RustyFilter.
   *
   * This function creates an instance of a RustyFilter and stores it inside a
   * map of instantiated processors.
   *
   * @param name Name given to the created RustyProcessor.
   * @param type Selects filter mode from RustyFilterType.
   * @param frequency Cutoff frequency of filter - Runtime adjustable.
   * @param q Q of filter - Runtime adjustable.
   *
   * @return Pointer to created unit.
   */
  RustyProcessor* createFilterUnit(std::string name, RustyFilterType type, float frequency, float q) { // Creates a filter unit
    auto processor = std::make_unique<RustyFilter>(type, frequency, q); // Using RAII pointers to ensure memory safety
    _processors[name] = std::move(processor);
    RustyProcessor* ptr = _processors.find(name)->second.get(); // Getting a pointer to the unique_ptr to return to user
    return ptr;
  }

  //--------------------------------------------------------------------------
  /**
   * @brief Creation function for RustyCompressor.
   *
   * This function creates an instance of a RustyCompressor and stores it inside a
   * map of instantiated processors.
   *
   * @param name Name given to the created RustyProcessor.
   * @param threshold Float of threshold in dB.
   * @param knee Float of knee curve in dB.
   * @param ratio Float of compression ratio.
   * @param attack Float of attack speed in Ms.
   * @param release Float of release speed in ms.
   * @param makeupGain Float of gain makeup in dB.
   *
   * @return Pointer to created unit.
   */
  RustyProcessor* createCompressorUnit(std::string name, float threshold, float knee, float ratio, float attack, float release, float makeupGain) { // Creates a filter unit
    auto processor = std::make_unique<RustyCompressor>(threshold, knee, ratio, attack, release, makeupGain); // Using RAII pointers to ensure memory safety
    _processors[name] = std::move(processor);
    RustyProcessor* ptr = _processors.find(name)->second.get(); // Getting a pointer to the unique_ptr to return to user
    return ptr;
  }

  //--------------------------------------------------------------------------
  /**
   * @brief Adds RustyModulator to RustyProcessor.
   *
   * This function allows you to add a RustyModulator unit to a RustyProcessor
   * unit. You define the parameter you wish to manipulate and then the
   * RustyModulator will affect the parameter at the rate defined during it's
   * creation.
   *
   * @param parameter Name of the RustyProcessor parameter to modulate .
   * @param processor Pointer to RustyProcessor to affect.
   * @param modulator Pointer to RustyModulator that will be used to affect parameter.
   */
  void setModulator(std::string parameter, RustyProcessor* processor, RustyModulator* modulator) {
    processor->setModulatorForParameter(parameter, modulator);
  }

  //--------------------------------------------------------------------------
  /**
   * @brief Creation function for RustyLFO.
   *
   * This function creates an instance of a RustyLFO and stores it inside a
   * map of instantiated modulators.
   *
   * @param name  Name given to the created RustyModulator.
   * @param frequency Frequency of LFO in Hz.
   * @param min Minimum value to be oscillated between.
   * @param max Maximum value to be oscillated between.
   *
   * @return Pointer to created unit.
   */
  RustyModulator* createLFO(std::string name, float frequency, float min, float max) {
    auto modulator = std::make_unique<RustyLFO>(frequency, min, max); // Using RAII pointers to ensure memory safety
    _modulators[name] = std::move(modulator);
    RustyModulator* ptr = _modulators.find(name)->second.get(); // Getting a pointer to the unique_ptr to return to user
    return ptr;
  }

  //--------------------------------------------------------------------------
  /**
   * @brief Defines IO devices for RustyAudioEngine to connect to
   *
   * This function creates an objects of RustyCommonTypes::SettingsIO. This
   * keeps all of the users selected IO in one neat place then passes it down
   * to RustyAudioEngine so it can open streams to the appropriate devices.
   * 
   * ```{cpp}
   * lib.setIO(<SOME_INPUT_DEVICE>, <MONO_OR_STEREO>, <SOME_CHANNEL_INDEX>, <SOME_OUTPUT_DEVICE>, <MONO_OR_STEREO>, <SOME_CHANNEL_INDEX>);
   * ```
   *
   * @param inputString String of input device name.
   * @param inputType AudioType of input (MONO or STEREO).
   * @param inputIx Int of used channel index(s) within device (Channel 1 = index 0).
   * @param outputString String of output device name.
   * @param outputType AudioType of output (MONO or STEREO).
   * @param outputIx Int of used channel index(s) within device (Channel 1 = index 0).
   */
  void setIO(std::string inputString, AudioType inputType, int inputIx, std::string outputString, AudioType outputType, int outputIx) {
    // Binding user settings to SettingsIO object
    settings.inputString = inputString;
    settings.inputType = inputType;
    settings.inputIx = inputIx;
    settings.outputString = outputString;
    settings.outputType = outputType;
    settings.outputIx = outputIx;

    engine.addSettings(settings); // Passing SettingsIO to RustyAudioEngine
  }

private:
  std::map<std::string, std::unique_ptr<RustyProcessor>> _processors; ///< Map of all instantiated RustyProcessor units
  std::map<std::string, std::unique_ptr<RustyModulator>> _modulators; ///< Map of all instantiated RustyModulator units

  RustyAudioEngine engine; ///< RustyAudioEngine instance saved here
  RustyCLI cli; ///< RustyCLI instance saved here
  std::thread cliThread; ///< Dedicated thread to run RustyCLI on
  SettingsIO settings; ///< RustyAudioLib IO settings

  /**
   * @brief Initialise CLI and CLI thread.
   *
   * A simple initialisation function to start the CLI loop and start the CLI
   * separate thread.
   *
   * @see init()
   */
  void initCLI() {
    cliThread = std::thread(&RustyCLI::run, &cli);
  };
};

/**
 * @page CreationFunctions Creation Functions
 *
 * @section sec_overview Overview
 *
 * This page describes creation functions. 
 *
 * Some APIs work with a factory system. Meaning you create objects from the
 * library through creation functions. 
 *
 * Here all the types of unit you can make using a RustyAudioLibAPI.
 *
 * @section sec_functions Functions
 *
 * - @link RustyAudioLibAPI::createFilterUnit()
 *   RustyAudioLibAPI::createFilterUnit() @endlink : Creates a RustyFilter
 *   unit.
 * - @link RustyAudioLibAPI::createGainUnit() @endlink : Creates a RustyGain
 *   unit. function2.
 * - @link RustyAudioLibAPI::createOverdriveUnit() @endlink : Creates a
 *   RustyOverdrive unit.
 *
 * Detailed descriptions of these functions can be found on their respective
 * documentation pages.
 */

#endif