#ifndef RUSTY_CLI_HPP
#define RUSTY_CLI_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <cctype>
#include "types/RustyCommonTypes.hpp"

class RustyAudioLib;

/**
 * @defgroup RustyCLI RustyCLI
 * @brief CLI class to handle run-time interaction
 */

//----------------------------------------------------------------------------
/**
 * @ingroup RustyCLI
 *
 * @class RustyCLI
 * @brief CLI class on low-priority UI thread
 *
 * This class handles user interaction with RustyProcessor instances and
 * RustyModulator instances. Running on a dedicated thread to ensure it doesn't
 * interfere with high priority audio thread.
 */
class RustyCLI
{
  public:
    /**
    //--------------------------------------------------------------------------
    * @brief Constructor for RustyCLI
    *
    * Takes initial parameters for RustyCLI which is just a callback reference
    * to RustyAudioLib. This allows parsing functions inside the CLI to
    * callback to RustyAudioLibAPI which then calls relevant functions in the
    * RustyAudioEngine.
    *
    * @param CLIcallback Takes reference to function for callback
    */
    RustyCLI( const CLIcallback& cb ) : _callback(cb) { std::cout << "RustyCLI constructed..." << std::endl; };
    ~RustyCLI() { std::cout << "RustyCLI destroyed..." << std::endl; }

    /**
    //--------------------------------------------------------------------------
    * @brief Initialises CLI
    *
    * Initialises CLI by beginning infinite loop that listens for user console
    * input. From here user input is passed to the parser() and then the
    * commandHandler().
    *
    * @see parser()
    * @see commandHandler()
    */
    void run();

    /**
    //--------------------------------------------------------------------------
    * @brief Parses user input from CLI run()
    *
    * Parses user input on a char by char basis. Function splits input at
    * whitespaces and places full strings of the command words into a vector of
    * parsed commands.
    *
    * @see commandHandler()
    */
    void parser(std::string rawInput);

    /**
    //--------------------------------------------------------------------------
    * @brief Handles parsed commands from parser()
    *
    * A series of logic statements that determine whether the user input was a
    * good input or not. If bad there should be a corresponding error message
    * and the commands will be deleted before execution to avoid runtime
    * crashes.
    *
    * @see parser()
    */
    void commandHandler();
    
    /**
    //--------------------------------------------------------------------------
    * @brief Prints list of commands
    *
    * Separated list command into it's own function to allow formatting of the
    * text easier.
    */
    void listCommand();

  private:
    CLIcallback _callback; ///< CLI callback
    std::vector<std::string> _inputParsed; ///< Vector of input words
    std::vector<std::string> topLevelCommands{"-ls", "--list", "-p", "--processors", "-m", "--modulators", "-param"}; ///< Vector of commands
};

#endif