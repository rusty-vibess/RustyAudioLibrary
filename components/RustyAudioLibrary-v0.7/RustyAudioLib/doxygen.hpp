/**
 * @mainpage RustyAudioLibrary WELCOME
 *
 * @section intro_sec Getting Started with RustyAudioLib
 *
 *
 * RustyAudioLib is an easy to use AudioLibrary that allows users to
 * instantiate a variety of processing units and apply them to a signal chain.
 * RustyAudioLib projects can then be compiled out and used to create real-time
 * audio processing effects.
 *
 * Let's get you on the way to using RustyAudioLibrary.
 *
 * @section project_build Building your Library
 *
 * RustyAudioLib comes with a package handling script. This script builds an
 * appropriate version of PortAudio to be used on your machine, then compiles
 * RustyAudioLib into a static binary and brings everything together in a nice
 * neat project folder. The `project/` directory. From `project/` you can start
 * using the library.
 *
 * To begin the build script, open a terminal in the `RustyAudioLib-dist/`
 * directory and run:
 *
 * ```{sh}
 * sh ./build
 * ```
 *
 * This will begin the build script, then run and shutdown a copy of
 * RustyAudioLib. 
 *
 * Provided there are no issues, you should have a ready to use
 * RustyAudioLibrary build. Navigate to `project/` and open the `main.cpp` to
 * begin.
 *
 * @section using_lib Using RustyAudioLib
 *
 * Once built the example project comes with a very simple `.cpp` file.
 *
 * ```{cpp}
 * #include <iostream>
 * #include "RustyAudioLib.hpp" // This #include is needed to access RustyAudioLbrary functions
 *
 * int main()
 * {
 *   RustyAudioLibAPI lib; // Library object is of type RustyAudioLibAPI
 *
 *   std::cout << std::endl << "RustyAudioLibrary -- TEST RUN " << std::endl << std::endl;
 *
 *   return 0;
 * }
 * ```
 *
 * This example project simply tests if the project can run. To actually use
 * the library you'll want something closer to this:
 *
 * ```{cpp}
 * #include <iostream>
 * #include "RustyAudioLib.hpp" // This #include is needed to access RustyAudioLbrary functions
 *
 * int main()
 * {
 *   RustyAudioLibAPI lib; // Library object is of type RustyAudioLibAPI
 *
 *   RustyProcessor* filter = lib.createFilterUnit("filter", LOW_PASS, 500, 1.5); // Creating a RustyFilter
 *   RustyProcessor* overdrive = lib.createOverdriveUnit("drive", 10); // Creating a RustyOverdrive
 *   RustyModulator* lfo = lib.createLFO("lfo", 8.3, 125, 3000); // Creating a RustyLFO
 *
 *   lib.addProcessorToSignalChain(filter1); // Adding the filter to the signal chain
 *   lib.addProcessorToSignalChain(overdrive); // Adding the overdrive to the signal chain
 *
 *   filter->setModulatorForParameter("frequency", lfo); // Adding LFO to filter modulation
 *
 *   lib.setIO("My-VAD", MONO, 64, "My-VAD", STEREO, 64);  // Setting input and output to my desired devices
 *   lib.init(); // Starting audio playback and CLI
 *   return 0;
 * }
 * ```
 * Here we've made use of our creation functions to create two RustyProcessor
 * units, a RustyFilter and a RustyOverdrive. We've also created a
 * RustyModulator, that is our RustyLFO. We then add our processors to the
 * signal chain in a sequential order (the order of adding to the signal chain
 * will be the order of processing). We then bind our LFO to the frequency of
 * our filter. And finally we set the IO devices appropriately before
 * initialising the library.
 *
 * Feel free to click the functions in the above code snippets to go to their
 * documentation page and get more information about how to use them.
 *
 * RustyAudioLib requires an instantiation (RustyAudioLibAPI), IO settings
 * (@link RustyAudioLibAPI::setIO() setIO@endlink) and an initialise call
 * (@link RustyAudioLibAPI::init() init@endlink) to work properly.
 *
 * ```{cpp}
 * RustyAudioLibAPI lib;
 *
 * lib.setIO(<SOME_INPUT_DEVICE>, <MONO_OR_STEREO>, <SOME_CHANNEL_INDEX>, <SOME_OUTPUT_DEVICE>, <MONO_OR_STEREO>, <SOME_CHANNEL_INDEX>);
 *
 * lib.init();
 * ```
 *
 * You should now be able to play around with RustyAudioLib. Have fun!!!
 *
 * Consult the <a href="modules.html">Modules</a> section of these docs to
 * better understand what the RustyAudioLibrary is made from. \n
*/