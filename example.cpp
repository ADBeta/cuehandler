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

int main(/*const int argc, const char *argv[]*/) {
	CueFile File("/home/ash/Downloads/test.txt");
	CueFile File2("./test.cue");
	
	//TODO Internal example folder and files
	//TODO try catch blocks and more examples here
	
	CueSheet test;
	
	
	
	//std::cout << File.Open() << std::endl;
	File.Read(test);
	
	test.Print();
	
	
	std::cout << "\n\n\n";
	
	test.Combine("test.out");
	
	
	test.Print();
	File2.Write(test);
	
	
	return 0;
}
