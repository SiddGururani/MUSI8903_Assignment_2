# MUSIC-8903-2016: Assignment 2
Vibrato Library

This repository contains the Vibrato library implemented for the MUSI8903 Audio Software Engineering assignment and a demo program that uses this library to apply a vibrato effect on an audio file. It also contains the test suite for the library called Vibrato_Test, which implements the tests mentioned in the assignment problem statement.

The readme file contains information for the usage of the program and the design decisions taken by the developers of this library.

The repository contains a folder which has Matlab plots which verify the functionality of the filters by comparing Matlab's result with the result generated by the library's process function.

#Usage Instructions:

The file MUSI8903Exec.cpp contains the source code for a command line program that takes as input the audio file path, the modulation frequency and the modulation amplitude.

The command line application has to be run as follow:

   \<MUSI8903Exec binary> \<file path> \<modulation frequency in Hertz> \<modulation amplitude in seconds>
   
 - \<modulation frequency in Hertz>: The desired frequency of the sinusoidal LFO for the vibrato effect. Should be non-negative.

 - \<modulation amplitude in seconds> The desired oscillation amplitude of the vibrato effect. Should be non-negative. Should also be less than the delay_width set by the developers, in this case: 0.1 seconds.

The program outputs a text file, containing the output filtered signal data so that it may be compared to the Matlab implementation of the vibrato. Note that the Matlab implementation provided was buggy and has hence been fixed. All plots are generated using the new Matlab function which is also in the repository.

The file Test_Vibrato.cpp in the TestExec build target contains all the tests mentioned in the assignment with an additional test to check the behavior of the program against zero modulation frequency input.

To run the tests:
   \<TestExec binary> Vibrato_Test

Note that the above targets may also be run from within an IDE like Visual Studio or XCode.

# Interface Design Decisions:

Unlike the CombFilter, the Vibrato library is a more simple design when it comes to class structure.

The Library provides access to three parameters to the developer: 
  - Modulation Frequency
  - Delay Width
  - Modulation Amplitude

These are all private data members. Modulation Frequency and Amplitude have corresponding getter and setter functions but the Delay Width may only be set during initialization of a Vibrato object pointer. Such an interface was chosen to enable the developer to implement on-the-fly changes to the modulation amplitude and frequency but not allowing the user to change the change the delay_width while processing. A new instance has to be created for a different delay_width.

Another feature of the library is that a constant data member for the maximum allowed delay width is also present which restricts the siz of the ring buffer used to store the delayed samples. This imposes a restriction on the developer to choose a delay_width less than half of the imposed maximum which is set to 2 seconds by us, which is a reasonably large delay.

Data members are private. 

The adjustable parameters within this interface are the modulation amplitude and frequency.

The parameters which the user can get using this interface are:
 - Modulation amplitude
 - Modulation frequency
 - Delay_width (which the user himself needs to decide during instantiation)

The user can set the sample rate and channel configuration while instantiating an object via the create() method. The init function is used to set the parameters of the vibrato effect. Of the three arguments, only the modulation amplitude and frequency may be changed for the initialized vibrato object.

The maximum delay, as mentioned above is set in 2 steps. One is to limit the largest ring buffer size, which is set by us in a constant data member, and the other is imposed by the user/developer when he/she initializes an object.

The interface should work with all sample rates and channel configurations.

The process function is not inplace.