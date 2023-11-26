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
	
	
	CueFile File("./test.txt");
	//std::cout << File.Open() << std::endl;
	File.Read();
	
	
	std::cout << "\nagain\n\n";
	File.Read();
	
	
	
	return 0;
}
