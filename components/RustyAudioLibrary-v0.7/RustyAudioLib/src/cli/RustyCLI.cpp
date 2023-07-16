#include "cli/RustyCLI.hpp"

void RustyCLI::run()
{
  std::string input;
  while(true) { // Starts infinite loop to listen for input
    std::cout << "Enter command: ";
    std::getline(std::cin, input);
    // Checking for exit command
    if(input == "-exit" || input == "-Exit" || input == "-EXIT") {
      break;
    }
    parser(input); // Parses user input
    commandHandler(); // Handles parsed input
    _inputParsed.clear(); // Clear input
  }
}

void RustyCLI::parser(std::string rawInput)
{
  std::string tempString;
  int i = 0; // Keeps track of char index
  int n = 0; // Keeps track of number of chars to manipulate
  for(char c : rawInput) {
    n++;
    if(std::isspace(c)) { // Parsing commands based on whitespace 
      tempString = rawInput.substr(i, n - i - 1); // Pulling commands into individual string
      _inputParsed.push_back(tempString); // Pushing into parsed commands vector
      i = n; // Moving index to n, as previous chars are now parsed
    }
  }
  tempString = rawInput.substr(i, n);
  if(tempString != "")
  {
    _inputParsed.push_back(tempString);
  }
}

void RustyCLI::commandHandler()
{
  // If statements here handle all available commands for user

  // -ls --list
  if(_inputParsed.size() == 0) {
    return;
  }

  if(_inputParsed[0] == topLevelCommands[0] || _inputParsed[0] == topLevelCommands[1]) {
    std::cout << "list command" << std::endl;
    listCommand();
    return;
  }
  // -p --processors
  if(_inputParsed[0] == topLevelCommands[2] || _inputParsed[0] == topLevelCommands[3]) {
    _callback(_inputParsed);
    return;
  }
  // -m --modulators
  if(_inputParsed[0] == topLevelCommands[4] || _inputParsed[0] == topLevelCommands[5]) {
    _callback(_inputParsed);
    return;
  }
  // -param
  if(_inputParsed[0] == topLevelCommands[6]) {
    if(_inputParsed.size() < 4 || _inputParsed.size() > 4) {
      std::cout << "Invalid number of arguments for `-param` command." << std::endl;
      return;
    }

    _callback(_inputParsed);
    return;
  }

  std::cout << "Command: `" << _inputParsed[0] << "` did not match any known commands. \nUse `-ls` or `--list` for command list." << std::endl;
}

void RustyCLI::listCommand()
{
  std::cout <<
  "Available commands:\n"
  "[-ls, --list]      ---- List all commands\n"
  "[-p, --processors] ---- List available processors\n"
  "[-m, --modulators] ---- List available modulators\n"
  "[-param], [MODULATOR/PROCESSOR NAME], [PARAM NAME], [NEW VALUE] ---- Allows for runtime parameter adjustment\n"
  "[-EXIT, -Exit, -exit] ---- Stops execution of RustyAudioTool and begins shutdown\n" << std::endl;
}