/*******************************************************************************
* Example of how to use cuehandler
*
*
*
*
*******************************************************************************/
#include <iostream>
#include <string>

#include "cuehandler.hpp"

int main(/*const int argc, const char *argv[]*/) {
	CueFile File("/home/ash/Downloads/test.txt");
	
	CueSheet test;
	
	
	
	//std::cout << File.Open() << std::endl;
	File.Read(test);
	
	test.Print();
	
	return 0;
}
