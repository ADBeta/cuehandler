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
	
	
	CueSheet test;
	
	uint32_t tsb = CueSheet::TimestampToBytes("00:02:00", CueSheet::TrackType::AUDIO);	
	std::string tss = CueSheet::BytesToTimestamp(tsb, CueSheet::TrackType::AUDIO);
	
	std::cout << "Tsb: " << tsb << "   Tss: " << tss << std::endl;
	
	//std::cout << File.Open() << std::endl;
	File.Read(test);
	test.Print();
	
	
	
	return 0;
}
