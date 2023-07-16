#ifndef RUSTY_COMMON_TYPES_HPP
#define RUSTY_COMMON_TYPES_HPP

#include <functional>
#include <vector>
#include <string>

static int BUFFER_SIZE; // Number of samples in each buffer without accounting for channel number
static int SAMPLE_RATE = 44100; // SAMPLE_RATE used by effects, can be adjusted by setSampleRate() method. Defaults to Nyquist (44100Hz).
static int FRAMES_PER_BUFFER = 256; // FRAMES in each buffer, decided by user input being MONO or STEREO and BUFFER_SIZE. Defaults to stereo.

using CLIcallback = std::function<void(std::vector<std::string>)>; ///< Callback to return user input to RustyAudioLibAPI top-level

/**
 * @defgroup RustyCommonTypes RustyCommonTypes
 * @brief Cross class referenced Enums and Structs
 */

// enum RustyAudioEngineState ///< 
// {
//   INITIALISING_ENGINE,
//   CHOOSING_INPUT_DEVICE,
//   CHOOSING_OUTPUT_DEVICE,
//   OPENING_STREAMS,
//   ENGINE_RUNNING,
// };

enum RustyFilterType ///< 4 Different filter types that can be used by RustyFilter
{
  LOW_PASS,
  HIGH_PASS,
  BAND_PASS,
  NOTCH
};

enum AudioType ///< Two Available steaming options for audio devices
{
  STEREO = 2,
  MONO = 1
};

struct SettingsIO ///< Settings struct to keep track of user's IO selection
{
  std::string inputString;
  AudioType inputType;
  int inputIx;
  int inputSize;
  std::string outputString;
  AudioType outputType;
  int outputIx;
  int outputSize;
};

#endif