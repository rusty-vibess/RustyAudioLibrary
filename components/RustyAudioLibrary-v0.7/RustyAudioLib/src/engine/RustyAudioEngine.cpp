#include "engine/RustyAudioEngine.hpp"

RustyAudioEngine::RustyAudioEngine()
{
  std::cout << "RustyEngine constructed..." << std::endl;
}

RustyAudioEngine::~RustyAudioEngine()
{
  terminate();
  if(_data != nullptr) // Deleting hanging memory
  {
    delete _data;
  }
  
  std::cout << "RustyEngine destroyed..." << std::endl;
}

// Audio callback handles all high-priority audio execution
// Called from audioCallback wrapper
// This method allowed me to make use of non-static members during run-time execution
int RustyAudioEngine::audioCallback( const void* inputBuffer, 
                       void* outputBuffer,
                       unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo,
                       PaStreamCallbackFlags statusFlags,
                       void* userData )
{
  if(userData == nullptr) {
    std::cerr << "Error: userData is nullptr" << std::endl;
    return paAbort;
  }

  // Pulling out and casting necessary values
  RustyUserData* data = static_cast<RustyUserData*>(userData);
  RustyAudioEngine* engine = data->engine;
  float* in = (float*)inputBuffer;
  float* out = (float*)outputBuffer;

  for(int i = 0; i < framesPerBuffer; ++i) // Iterate through audioBuffer
  {
    // Case if input MONO and output STEREO inc channel adjustment
    if(_settings.inputType == MONO && _settings.outputType == STEREO)
    {
      for(const auto& processor: engine->_processingUnits) // Iterates through all processors in audio chain, passing them one sample at a time
      {
        processor->process(&in[(i * _inputParameters.channelCount) + _settings.inputIx]);
      }

      out[(i * _outputParameters.channelCount) + _settings.outputIx] = in[(i * _inputParameters.channelCount) + _settings.inputIx];
      out[(i * _outputParameters.channelCount) + _settings.outputIx + 1] = in[(i * _inputParameters.channelCount) + _settings.inputIx];
    }

    // Case if input STEREO and output MONO inc channel adjustment
    if(_settings.inputType == STEREO && _settings.outputType == MONO)
    {
      for(const auto& processor: engine->_processingUnits) // Iterates through all processors in audio chain, passing them one sample at a time
      {
        processor->process(&in[(i * _inputParameters.channelCount) + _settings.inputIx]);
      }

      out[(i * _outputParameters.channelCount) + _settings.outputIx] = in[(i * _inputParameters.channelCount) + _settings.inputIx]; 
    }

    // Case if input STEREO and output STEREO inc channel adjustment
    if(_settings.inputType == 2 && _settings.outputType == 2)
    {
      for(const auto& processor: engine->_processingUnits) // Iterates through all processors in audio chain, passing them one sample at a time
      {
        processor->process(&in[(i * _inputParameters.channelCount) + _settings.inputIx]);
        processor->process(&in[(i * _inputParameters.channelCount) + _settings.inputIx + 1]); // Additional call for STEREO to STEREO
      }

      out[(i * _outputParameters.channelCount) + _settings.outputIx] = in[(i * _inputParameters.channelCount) + _settings.inputIx];
      out[(i * _outputParameters.channelCount) + _settings.outputIx + 1] = in[(i * _inputParameters.channelCount) + _settings.inputIx + 1];
    }
  }
  return paContinue;
}

// Static wrapper used as a workaround
int RustyAudioEngine::audioCallbackWrapper( const void* inputBuffer, 
                       void* outputBuffer,
                       unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo,
                       PaStreamCallbackFlags statusFlags,
                       void* userData )
{
  RustyUserData* data = static_cast<RustyUserData*>(userData); // Casting my user data which contains a pointer to the instance of the RustyAudioEngine created during execution

  // Then able to use that reference to call the regular audio callback from here
  return data->engine->audioCallback(inputBuffer, outputBuffer, framesPerBuffer, timeInfo, statusFlags, data);
}


bool RustyAudioEngine::init()
{
  //----------------------------------
  //        INIT PORTAUDIO
  //----------------------------------
  _err = Pa_Initialize(); // Calling PA initialise function and assigning to _err
  if(_err != paNoError) {
    std::cerr << "RustyAudioEngine -- PortAudio error: " << Pa_GetErrorText(_err) << std::endl;
    return 0; // If fails return false
  }
  
  if(_settings.outputString.empty() || _settings.inputString.empty())
  {
    std::cerr << "You have not defined your input or output -- Use `setIO()` before calling `init()`" << std::endl;
    std::cerr << "Refer to Documentation 'Getting Started' for help" << std::endl;
    std::cerr << "Type `-exit` to stop RustyAudioLib" << std::endl;
    return 0;
  }

  if(initOutputDevice(_settings.outputString) && initInputDevice(_settings.inputString))
  {
    setupStream();
  }
  return 1; // If good return true
}

bool RustyAudioEngine::initInputDevice( std::string inputDevice )
{
  // inputDevice = "MacBook Pro Microphone";
  int deviceIndex = -1; // Device index -1 allows for error call if device cannot be used

  for(int i = 0; i < Pa_GetDeviceCount(); i++) // Getting device names and comparing them to string
  {
    const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
    if(deviceInfo && inputDevice == deviceInfo->name) // If user input name matches a device
    {
      deviceIndex = i; // Taking index of matching device
      break;
    }
  }
  if(deviceIndex == -1)
  {
    std::cerr << "RustyAudioEngine -- Error[initOutputDevice()]: " << "BAD_INPUT_DEVICE_NAME_OR_SETTINGS" << std::endl;
    return 0; // Return false if there are no device matches
  }

  // Assigning device and settings to inputParameters
  _inputParameters.device = deviceIndex;
  _inputParameters.channelCount = Pa_GetDeviceInfo(_inputParameters.device)->maxInputChannels;
  BUFFER_SIZE = FRAMES_PER_BUFFER * _inputParameters.channelCount;
  _inputParameters.sampleFormat = paFloat32;
  _inputParameters.suggestedLatency =
  Pa_GetDeviceInfo(_inputParameters.device)->defaultLowOutputLatency;
  _inputParameters.hostApiSpecificStreamInfo = NULL;
  return 1; // Return true if good
}

bool RustyAudioEngine::initOutputDevice( std::string outputDevice )
{
  int deviceIndex = -1; // Device index -1 allows for error call if device cannot be used

  for(int i = 0; i < Pa_GetDeviceCount(); i++) // Getting device names and comparing them to string
  {
    const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
    if(deviceInfo && outputDevice == deviceInfo->name) // If user input name matches a device
    {
      deviceIndex = i; // Taking index of matching device
      break; 
    }
  }
  if(deviceIndex == -1)
  {
    std::cerr << "RustyAudioEngine -- Error[initOutputDevice()]: " << "BAD_OUTPUT_DEVICE_NAME_OR_SETTINGS" << std::endl;
    return 0; // Return false if there are no device matches
  }

  // Assigning device and settings to outputParameters
  _outputParameters.device = deviceIndex;
  _outputParameters.channelCount = 
  Pa_GetDeviceInfo(_outputParameters.device)->maxOutputChannels;
  _outputParameters.sampleFormat = paFloat32;
  _outputParameters.suggestedLatency =
  Pa_GetDeviceInfo(_outputParameters.device)->defaultLowOutputLatency;
  _outputParameters.hostApiSpecificStreamInfo = NULL;
  return 1; // Return true if good
}

bool RustyAudioEngine::terminate()
{
  if(!_streaming) { // Test case for bad terminate call
    return 0;
  }

  _err = Pa_StopStream(_stream); // PortAudio stream shutdown method
    if(_err != paNoError) {
    std::cerr << "RustyAudioEngine -- PortAudio error: " << Pa_GetErrorText(_err) << std::endl;
    return 0; // Return false if _err
  }  

    _err = Pa_CloseStream(_stream); // PortAudio stream shutdown method
    if(_err != paNoError) {
    std::cerr << "RustyAudioEngine -- PortAudio error: " << Pa_GetErrorText(_err) << std::endl;
    return 0; // Return false if _err
  }

  std::cout << "Stream closed successfully... " << std::endl;
  return 1;
}

bool RustyAudioEngine::setupStream()
{
  _data = new RustyUserData{this}; // Creating RustyUserData object to be passed to callback with pointer to this so static function can call things from non-static RustyAudioEngine instance

  // Opening stream to users IO devices
  _err = Pa_OpenStream(&_stream, &_inputParameters, &_outputParameters, SAMPLE_RATE, FRAMES_PER_BUFFER, paClipOff, audioCallbackWrapper, _data);
  if(_err != paNoError) {
    std::cerr << "RustyAudioEngine -- PortAudio error: " << Pa_GetErrorText(_err) << std::endl;
    return 0; // Return false if _err
  }  

  // Starting stream to users IO devices
  _err = Pa_StartStream(_stream);
    if(_err != paNoError) {
    std::cerr << "RustyAudioEngine -- PortAudio error: " << Pa_GetErrorText(_err) << std::endl;
    return 0; // Return false if _err
  }  

  _streaming = true;
  std::cout << "Stream initialised successfully..." << std::endl;
  return 1;
}