/*******************************************************************************
* This is an example file of how to use the cuehandler library in projects.
* See cuehandler.hpp for more details on each function, notes about their
* return values, and how they function.
*
* ADBeta 28 Dec 2023
* Version 1.0.0
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
	//input file. Same with "JUKEBOX".
	CueData.Combine("demo-combined.bin", "JUKEBOX");
	
	//Print the CueSheet information again to show that is has been modified
	std::cout << "\n\n    Data After Combining\n" 
	          << "----------------------------------------------------------\n";
	CueData.Print();
	
	//You can push or FILEs, TRACKs, and INDEXs to a CueSheet Object:
	//Files have 3 fields - Filename, Filetype and Bytes. You can get the file
	//Bytes by using a function defined in the CueFile header -
	uint32_t filebytes = CueFile::GetFileBytes("./test-in.cue");
	CueSheet::FileObj file("example.file", "example", filebytes);
	CueData.PushFile(&file);
	
	//Tracks have 2 data fields, ID and type
	CueSheet::FileObj::TrackObj track(99, CueSheet::TrackType::AUDIO);
	CueData.PushTrack(&track);
	
	//Indexs have 2 data fields. ID and offset bytes (timestamp in byte format)
	CueSheet::FileObj::TrackObj::IndexObj index(99, 0);
	CueData.PushIndex(&index);
	
	//Print the CueSheet information again to show that is has been modified
	std::cout << "\n\n    Data After Pushing\n" 
	          << "----------------------------------------------------------\n";
	CueData.Print();
	
	//Now I pop those modifcations back off the CueSheet. I can call PopFile and
	//all garbage collection will be handled automatically, but for example
	//purposes:
	CueData.PopIndex();
	CueData.PopTrack();
	CueData.PopFile();
	
	//Create and write out to the OutputFile in the local directory
	std::cout << "\n\nWriting to output file....\n";
	FileO.WriteCueData(CueData);
	
	return 0;
}
