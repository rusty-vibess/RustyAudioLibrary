#ifndef RUSTY_AUDIO_ENGINE_HPP
#define RUSTY_AUDIO_ENGINE_HPP

#include <string>
#include <iostream>
#include <memory>
#include "types/RustyCommonTypes.hpp"
#include "engine/portaudio.h"
#include "dsp/RustyProcessor.hpp"


/**
 * @defgroup AudioEngine RustyAudioEngine
 * @brief Audio Engine class built on PortAudio
 */

//----------------------------------------------------------------------------
/**
 * @ingroup AudioEngine
 *
 * @class RustyAudioEngine
 * @brief Handles all run-time execution of audio code.
 *
 * From managing the signal chain to initialising the IO with device drivers.
 * This class handles all things audio. 
 *
 * Makes use of PortAudio to open IO streams to devices and to create an
 * audioCallback() on a high priority interrupt thread.
 *
 * RustyAudioEngine cannot be instantiated more than once due to the
 * audioCallback from PortAudio's IO stream being static.
 *
 * Instantiated inside RustyAudioLibAPI as a private member.
 *
 * @see RustyAudioLibAPI
 * @see <a href="http://www.portaudio.com">PortAudio</a>
 */
class RustyAudioEngine; // Forward declaration

//----------------------------------------------------------------------------
/**
 * @struct RustyUserData
 * @brief Passed to audioCallback as arg.
 *
 * This struct is crucial to the management of the audio signal. Non-static
 * RustyProcessor methods can't be called from the static audioCallback linked
 * to the PortAudio IO stream. Therefore you write a static
 * audioCallbackWrapper() and pass this struct in as an argument. You then call
 * the non static audioCallback() from the RustyAudioEngine pointer that is
 * pointing to the instance of the RustyAudioEngine it was instantiated in.
 */
struct RustyUserData
{
  RustyAudioEngine* engine;
};

class RustyAudioEngine
{
public:
  // Constructor and Destructor
  RustyAudioEngine(); ///< Audio Engine constructor - Handles startup
  ~RustyAudioEngine(); ///< Audio Engine destructor - Handles proper shutdown

  //--------------------------------------------------------------------------
  /**
   * @brief Non-static audio callback.
   *
   * Non-static audio callback used to perform any manipulation of the audio
   * signal. DSP units are passed audio samples from this function.
   * 
   * Called by static audioCallbackWrapper().
   *
   * @param inputBuffer Pointer to buffer of input audio samples from PaStream.
   * @param ouputBuffer Pointer to buffer of output audio samples to be sent to PaStream.
   * @param framesPerBuffer Frames refers to number of samples without multiplying by the number of interweaved samples.
   * @param timeInfo Pointer to information about ADC and DAC capture times - Callback is required to meet it's time deadlines.
   * @param statusFlags PaStatus flags.
   * @param userData Raw pointer to user data.
   *
   * @return Int of PaStreamCallbackResult {continue, complete, abort}
   *
   * @see audioCallbackWrapper()
   */
  int audioCallback( const void* inputBuffer, 
                      void* outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void* userData );

  //--------------------------------------------------------------------------
  /**
   * @brief Static audio callback.
   *
   * Static audio callback used as workaround. In current implementation (and
   * seemingly all IO device libraries) the callback function is written in C
   * meaning it is static. This meant I was unable to call members from my
   * non-static RustyAudioEngine class. This wrapper layer allowed for a
   * workaround, by passing a reference to the static function I could
   * essentially trick it into calling non-static member functions. I then use that to
   * call the other audioCallback() function and process audio there.
   *
   * @param inputBuffer Pointer to buffer of input audio samples from PaStream.
   * @param ouputBuffer Pointer to buffer of output audio samples to be sent to
   * PaStream.
   * @param framesPerBuffer Frames refers to number of samples without
   * multiplying by the number of interweaved samples.
   * @param timeInfo Pointer to information about ADC and DAC capture times -
   * Callback is required to meet it's time deadlines.
   * @param statusFlags PaStatus flags.
   * @param userData Raw pointer to user data.
   *
   * @return Int of PaStreamCallbackResult {continue, complete, abort}
   *
   * @see audioCallback()
   */
  static int audioCallbackWrapper( const void* inputBuffer, 
                      void* outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void* userData );

  //--------------------------------------------------------------------------
  /**
   * @brief Initialisation function for RustyAudioEngine.
   *
   * Init() function used to initialise audio to chosen IO devices. Opening
   * streams to user selected drivers.
   *
   * @return Returns true if init successful.
   *
   * @exception PortAudioErrors PortAudio exceptions related to IO streaming and initialisation.
   */
  bool init();

  //--------------------------------------------------------------------------
  /**
   * @brief Adds a RustyProcessor to signal chain.
   *
   * Allows user to pass in a pointer to any object that inherits from
   * RustyProcessor. Processor will then be added to the signal chain via the
   * _processingUnit vector.
   *
   * Realised later on that the template wasn't necessary and I could just use
   * RustyProcessor as the argument but I like `static_assert` macro to check
   * the type so kept this function as is.
   *
   * @tparam DSPType Must be derived from RustyProcessor.
   *
   * @exception INVALID_DSP Object was not derived from RustyProcessor
   *
   * @see RustyProcessor()
   */

  //--------------------------------------------------------------------------
  template <typename DSPType> // Template classes are commonly instantiated in the header file
  void addProcessor(DSPType* dspUnit) ///< Adds processor to audio engine signal chain
  {
    // Ensuring the template type is a subclass of my DSP unit base class RustyProcessor
    static_assert(std::is_base_of<RustyProcessor, DSPType>::value, "INVALID_DSP must be a subclass of RustyProcessor");

    _processingUnits.push_back(dspUnit); // Moving ownership of unique_ptr to vector, unique_ptr requires move as it cannot be copied.
  }

  /**
  //--------------------------------------------------------------------------
  * @brief Passes user settings into engine
  *
  * User decides their IO devices via the setIO() function, that function
  * eventually results in an object of user settings being handed to the
  * engine.
  *
  * @param settings SettingsIO object of user IO parameters
  */
  void addSettings(SettingsIO settings)
  {
    _settings = settings;
  }

private:
  /**
  //--------------------------------------------------------------------------
  * @brief Initialises audio stream to chosen input device
  *
  * Creates necessary PortAudio objects and opens a stream to the device with
  * name chosen by user.
  *
  * @param inputDevice String of users input device
  * @return Returns true if successful
  */
  bool initInputDevice( std::string inputDevice );

  /**
  //--------------------------------------------------------------------------
  * @brief Initialises audio stream to chosen output device
  *
  * Creates necessary PortAudio objects and opens a stream to the device with
  * name chosen by user.
  *
  * @param outputDevice String of users output device
  * @return Returns true if successful
  */
  bool initOutputDevice( std::string outputDevice );

  /**
  //--------------------------------------------------------------------------
  * @brief Begins audio engine shutdown process
  *
  * Shutdowns down PortAudio streams and deletes any allocated data that needs
  * to be removed.
  * @return Returns true if successful
  */
  bool terminate();

  /**
  //--------------------------------------------------------------------------
  * @brief Starts streams when all IO preparation is successful
  *
  * Opens stream between users IO devices and audio engine.
  * @return Returns true if successful
  */
  bool setupStream();

  PaError _err; ///< PortAudio object to catch PA errors
  PaStream* _stream; ///< PortAudio stream
  PaStreamParameters _inputParameters; ///< PortAudio input parameters
  PaStreamParameters _outputParameters;///< PortAudio output parameters
  bool _streaming; ///< Keeps track of audio engine streaming state

  RustyUserData* _data = nullptr; ///< Pointer to instance of engine for audioCallback workaround
  SettingsIO _settings; ///< User IO settings object

  std::vector<RustyProcessor*> _processingUnits; ///< A map of all instantiated RustyProcessors in the audio chain
};

#endif
