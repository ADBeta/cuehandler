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
	CueFile FileI(argv[1]);
	CueFile FileO("./test-out.cue");
	
	//TODO Internal example folder and files
	//TODO try catch blocks and more examples here
	CueSheet test;
	
	
	std::cout << FileI.OpenRead() << std::endl;
	std::cout << FileI.Close() << std::endl;
	
	FileI.ReadCueData(test);
	
	std::cout << "here" << std::endl;
	
	
	FileI.GetCueFileSizes(test);
	
	test.Print();
	
	
	std::cout << "\n\n\n";
	
	test.Combine("test-out.cue");
	
	
	test.Print();
	FileO.WriteCueData(test);
	
	
	return 0;
}
