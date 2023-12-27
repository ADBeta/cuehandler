/*******************************************************************************
* Example of how to use cuehandler
*
*
* TODO
*
*******************************************************************************/
#include <iostream>
#include <string>

#include "cuehandler.hpp"

int main(const int argc, const char *argv[]) {

	//Create an input and output cue file, using the CueFile handler class
	CueFile FileI("./test-in.cue");
	CueFile FileO("./test-out.cue");
	
	//Create a Cue Sheet data object using the CueSheet class 
	CueSheet CueData;
	
	
	try {
		//Read the Cue Data from the Input File into the CueSheet object CueData
		FileI.ReadCueData(CueData);
		//Read the Size of each FILE Entry given in the Input Cue File.
		//Note that this is only necessary for the .Combine method, and is
		//optional if your uses of the library do not need the functionality 
		//to know the size of input files	
		FileI.GetCueFileSizes(CueData);
	
	} catch(CueException &e) {
		std::cerr << "Error while reading Cue Data: " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	//Print the information held in CueData. This is for debugging and verbosity
	std::cout << "    Data Before Combining\n"
	          << "----------------------------------------------------------\n";
	CueData.Print();
	
	//Combine the CueSheet entries into a single file. This was specifically
	//added for a project called psx-combine, but is useful in other limited
	//applicaiton.
	//I define the output FILE test as demo-combined.bin just for example.
	//If this field is left empty it will inherit the first FILE of the original
	//input file
	CueData.Combine("demo-combined.cue");
	
	
	CueData.PopTrack();
	CueData.PopTrack();
	CueData.PopTrack();
	CueData.PopTrack();
	CueData.PopTrack();
	
	//Print the CueSheet information again to show that is has been modified
	std::cout << "\n\n    Data After Combining\n" 
	          << "----------------------------------------------------------\n";
	CueData.Print();
	
	
	//Create and write out to the OutputFile in the local directory
	FileO.WriteCueData(CueData);
	
	
	return 0;
}
