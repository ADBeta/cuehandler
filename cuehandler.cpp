/*******************************************************************************
* This file is part of cuehandler. 
* See the GitHub for more information: https://github.com/ADBeta/cuehandler
*
* A Simple & Efficient Library to Create, Modify and Impliment .CUE files in C++
*******************************************************************************/
#include <string>
#include <fstream>

#include <iostream> //TODO

#include "cuehandler.hpp"


/*** API Member Functions *****************************************************/
//Reads the given file, Parses it, sets up the CueData structure tree
//then returns
int CueFile::Read() {
	//Guard against use without a set filename
	if(this->filename.empty()) return -1;
	
	//Attempt to open the file. Return error if failed
	if(this->Open() != 0) return -2;
	
	//Reset file pointer/flags, then read the file into a RAM File
	this->cue_file.clear();
	this->cue_file.seekg(0, std::ios::beg);
	
	//Read the File line by line.
	std::string line_str;
	while(getline(this->cue_file, line_str)) {
		//Skip Blank Lines
		if(line_str.empty()) continue;
		
		//TODO Parse shit here
	}
	
	//Close the File and return success
	this->Close();
	return 0;
}






/*** Private Member Functions *************************************************/
int CueFile::Open() {
	int errcode = 0;
	
	this->cue_file.open(this->filename, std::ios::in | std::ios::out);
	if(this->cue_file.is_open() == false) errcode = -1;
	
	return errcode;
}

int CueFile::Close() {
	int errcode = 0;
	
	this->cue_file.close();
	if(this->cue_file.fail()) errcode = -1;
	
	return errcode;
}



/*
 This file is part of psx-comBINe. Please see the github:
* https://github.com/ADBeta/psx-comBINe
*
* psx-comBINe is a simple program to combine multiple .bin files into a single
* file, and modified the .cue file indexing, this is ideal for PSX/PS1 CD-ROMs
* e.g. Rayman to get them ready for cue2pops or some emulators. I also find it 
* improves reliabilty when buring to a disk to only have one .bin file.
*
* Cue File manager. Uses TeFiEd to handle output file cue file track, file and 
* index text
* 
* this might be turned into its own project if I see it useful in future.
*
* (c) ADBeta
*******************************************************************************
#include "cueHandler.hpp"

#include <iostream>
#include <vector>

#include "helpers.hpp"
#include "TeFiEd.hpp"

//TODO add flag support (DCP mostly)

** Enum mapped strings (only for printFILE) *********************************
const std::string t_FILE_str[] = {"BINARY"};   
//
const std::string t_TRACK_str[] = {
	"AUDIO", "CDG", "MODE1/2048", "MODE1/2352", "MODE2/2336", 
	"MODE2/2352", "CDI/2336", "CDI/2352"
};

** CueHandler Functions *****************************************************
CueHandler::CueHandler(const std::string filename) {
	//Open the cue file passed
	cueFile = new TeFiEd(filename);
	
	//Set safety flag on the .cue input file. 100KB
	cueFile->setByteLimit(102400);
	
	//Make sure the input filename is a valid .cue file
	if( validateCueFilename(filename) != 0) {
		std::cout << "Could not open " << filename << " as a .cue file. Exiting" 
		          << std::endl;
		exit(EXIT_FAILURE);
	}
	
	//Debug option
	//cueFile->setVerbose(true);
}

CueHandler::~CueHandler() {
	//Delete the TeFiEd object
	delete cueFile;
	
	//Clear Vector RAM
	cleanFILE();
}

void CueHandler::read() {
	//Read the cueFile TeFiEd Obj into RAM
	if(cueFile->read() != 0) {
		exit(EXIT_FAILURE); 
	}
	
	//Make sure the Line Ending type is Unix
	cueFile->convertLineEnding(LineEnding::Unix);	
}

void CueHandler::create() {
	if(cueFile->create() != 0) {
		exit(EXIT_FAILURE);
	}
}

** FILE Vector Functions ****************************************************
void CueHandler::cleanFILE() {
	//Clear the FILE vector and de-allocate its RAM
	FILE.clear();
	FILE.shrink_to_fit();
}

t_LINE CueHandler::LINEStrToType(const std::string lineStr) {	
	//If line is empty return EMPTY
	if(lineStr.length() == 0) return ltEMPTY;

	//Check against known strings for types
	if(lineStr.find("    INDEX") != std::string::npos) return ltINDEX;
	if(lineStr.find("  TRACK") != std::string::npos) return ltTRACK;
	if(lineStr.find("FILE \"") != std::string::npos) return ltFILE;
	
	//Remark type (new feature)
	if(lineStr.find("REM ") != std::string::npos) return ltREM; 
	
	//Failure to find any known string means it's an invalid line
	return ltINVALID;
}

t_TRACK CueHandler::TRACKStrToType(const std::string trackStr) {
	//The TYPE of a TRACK is the 3rd (last) word in the string
	std::string typeStr = getWord(trackStr, 3);
	
	try {
		//Exit if string is blank. This file is very corrupt
		if(typeStr == "") throw file_corrupt;
	
		//Get number of strings in t_TRACK_str array.
		unsigned int TRACKTypes = sizeof(t_TRACK_str)/sizeof(t_TRACK_str[0]);
		
		//Go through all elements in t_TRACK_str (current track string)
		for(unsigned int cType = 0; cType < TRACKTypes; cType++) {
			//If the input string and the TRACKType string match
			if(typeStr.compare(t_TRACK_str[cType]) == 0) {
				//Return the matched type as enum int
				return (t_TRACK)cType;
			}
		}
	
		//If no match is found, throw an exception
		throw file_unknown_input;
		
	} catch(const PrgExcep &e) {
			std::cerr << e.what() << " in TRACK String" << std::endl;
	}
	
	return ttAUDIO;
}

t_FILE CueHandler::FILEStrToType(const std::string fileStr) {
	try {
		//The TYPE of a FILE is the last word in the string. Must do manually as
		//filenames can have spaces.
		std::string typeStr = fileStr.substr(fileStr.find_last_of(" ") + 1);
		//Exit if string is blank. This file is very corrupt
		if(typeStr == "") throw file_corrupt;
	
		//Get number of strings in t_FILE_str array.
		unsigned int FILETypes = sizeof(t_FILE_str)/sizeof(t_FILE_str[0]);
		
		//Go through all elements in t_FILE_str (current track string)
		for(unsigned int cType = 0; cType < FILETypes; cType++) {
			//If the input string and the TRACKType string match
			if(typeStr.compare(t_FILE_str[cType]) == 0) {
				//Return the matched type as enum int
				return (t_FILE)cType;
			}
		}
		
		//If no match is found, throw an exception
		throw file_unknown_input;
		
	} catch(const PrgExcep &e) {
			std::cerr << e.what() << " in FILE String" << std::endl;
	}
	
	return ftBINARY;
}

std::string CueHandler::FILETypeToStr(const t_FILE fileType) {
	try {
		//Get number of strings in t_FILE_str array.
		unsigned int FILETypes = sizeof(t_FILE_str)/sizeof(t_FILE_str[0]);
		
		//Go through all elements in t_FILE_str (current type string)
		for(unsigned int cType = 0; cType < FILETypes; cType++) {
			//If the input type matches type in enum 
			if(fileType == (t_FILE)cType) {
				//Return the matched type string
				return t_FILE_str[cType];
			}
		}
	
		//If no match is found, throw an exception
		throw file_unknown_input;
	
	} catch(const PrgExcep &e) {
			std::cerr << e.what() << " in FILE type" << std::endl;
	}
	
	return "";
}


std::string CueHandler::TRACKTypeToStr(const t_TRACK trackType) {
	try {
		//Get number of strings in t_TRACK_str array.
		unsigned int TRACKTypes = sizeof(t_TRACK_str)/sizeof(t_TRACK_str[0]);
		
		//Go through all elements in t_TRACK_str (current type string)
		for(unsigned int cType = 0; cType < TRACKTypes; cType++) {
			//If the input type matches type in enum 
			if(trackType == (t_TRACK)cType) {
				//Return the matched type string
				return t_TRACK_str[cType];
			}
		}
		
		//If no match is found, throw an exception
		throw file_unknown_input;
	
	} catch(const PrgExcep &e) {
			std::cerr << e.what() << " in TRACK type" << std::endl;
	}
	
	return "";
}

////////////////////////////////////////////////////////////////////////////////
//Validation functions. Returns specific error codes
int CueHandler::validateCueFilename(std::string cueStr) {
	//Convert cueStr to uppercase to make .cue vs .CUE etc. not matter
	cueStr = strToUppercase(cueStr);

	//Make sure the file extension is .cue
	if(cueStr.find(".CUE") == std::string::npos) {
		warnMsg("validateCueFilename", "Input file is not a .cue file");
		return 1;
	}
	
	return 0;
}

int CueHandler::validateTRACK(const TrackData &refTRACK) {
	if(refTRACK.ID > 99) {
		warnMsg("validateTRACK", "TRACK ID is greater than 99.");
		return 1;
	}

	//Normal/Valid return
	return 0;
}

int CueHandler::validateINDEX(const IndexData &refINDEX) {
	if(refINDEX.ID > 99) {
		warnMsg("validateINDEX", "INDEX ID is greater than 99.");
		return 1;
	}

	//Check if BYTES is divisible by SECTOR size
	if(refINDEX.BYTES % 2352 != 0) {
		warnMsg("validateINDEX", "INDEX BYTES not divisible by SECTOR size");
		return 2;
	}

	//Normal/Valid return
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

void CueHandler::pushFILE(const std::string FN, const t_FILE TYPE) {
	//Temporary FILE object
	FileData tempFILE;
	//Set the FILE Parameters
	tempFILE.FILENAME = FN;
	tempFILE.TYPE = TYPE;
	
	//Push tempFILE to the FILE vect
	FILE.push_back(tempFILE);
}

void CueHandler::pushTRACK(const unsigned int ID, const t_TRACK TYPE) {
	//Temporary TRACK object
	TrackData tempTRACK;
	//Set the TRACK Parameters
	tempTRACK.ID = ID;
	tempTRACK.TYPE = TYPE;
	
	if( validateTRACK(tempTRACK) != 0) return;
	
	//Get a pointer to the last entry in the FILE object
	FileData *pointerFILE = &FILE.back();
	//Push the tempTRACK to the back of the pointer 
	pointerFILE->TRACK.push_back(tempTRACK);
}

void CueHandler::pushINDEX(const unsigned int ID, const unsigned long BYTES) {
	//Temporary INDEX object
	IndexData tempINDEX;
	//Set INDEX parameters
	tempINDEX.ID = ID;
	tempINDEX.BYTES = BYTES;
	
	if( validateINDEX(tempINDEX) != 0) return;
	
	//Get a pointer to the last FILE and TRACK Object
	TrackData *pointerTRACK = &FILE.back().TRACK.back();
	//Push the INDEX to the end of current file
	pointerTRACK->INDEX.push_back(tempINDEX);
}

////////////////////////////////////////////////////////////////////////////////

std::string CueHandler::generateFILELine(const FileData &refFILE) {
	std::string outputLine = "FILE ";
	
	//Add " to the front and back of FILENAME, and a space after the second "
	//Append the FILENAME to the output
	outputLine.append("\"" + refFILE.FILENAME + "\" ");
	//Append the type
	outputLine.append(FILETypeToStr(refFILE.TYPE));
	
	return outputLine;
}
	
//Converts TrackData Object into a string which is a CUE file line
std::string CueHandler::generateTRACKLine(const TrackData &refTRACK) {
	if( validateTRACK(refTRACK) != 0) return "";
	
	std::string outputLine = "  TRACK ";
	
	//Append the TRACK ID (padded to 2 length) and a space
	outputLine.append( padIntStr(refTRACK.ID, 2) + " ");
	//Append the TRACK TYPE
	outputLine.append(TRACKTypeToStr(refTRACK.TYPE));
	
	return outputLine;
}
	
//Converts IndexData Object into a string which is a CUE file line
std::string CueHandler::generateINDEXLine(const IndexData &refINDEX) {
	if( validateINDEX(refINDEX) != 0) return "";
	
	std::string outputLine = "    INDEX ";
	
	//Append the INDEX ID (padded to 2 length) and a space
	outputLine.append( padIntStr(refINDEX.ID, 2) + " ");
	//Append the INDEX TIMESTAMP (string
	outputLine.append( bytesToTimestamp(refINDEX.BYTES) );
	
	return outputLine;
}

** Cue file data functions **************************************************
void CueHandler::getCueData() {
	//Clean the FILE vector RAM
	cleanFILE();

	//Go through all the lines in the cue file.
	for(size_t lineNo = 1; lineNo <= cueFile->lines(); lineNo++) {
		//Copy the current line to a new string
		std::string cLineStr = cueFile->getLine(lineNo);
		
		try {
			//Get the type of the current line
			t_LINE cLineType = LINEStrToType(cLineStr);
			
			//If the current line is invalid, exit
			if(cLineType == ltINVALID) throw line_invalid;
			
			//REM (remark) line type
			if(cLineType == ltREM) {
				//TODO Decide what to do with REMARKS
				//std::cout << "REMARK at line: " << lineNo << "\t message: " <<
				//cLineStr << std::endl;
			}
			
			//FILE line type
			if(cLineType == ltFILE) {
				t_FILE fileType = FILEStrToType(cLineStr);
				
				//Get the FILENAME string (always between quotations)
				size_t fQuote = cLineStr.find('\"') + 1;
				size_t lQuote = cLineStr.find('\"', fQuote);
				
				//push new FILE to the stack
				pushFILE(cLineStr.substr(fQuote, lQuote - fQuote), fileType);
			}
			
			//TRACK line type
			if(cLineType == ltTRACK) {
				//Make sure a FILE is availible to push to
				if(FILE.empty()) throw push_track_order;
			
				//Get ID (second word), and TYPE
				unsigned int lineID = std::stoi(getWord(cLineStr, 2));
				t_TRACK lineTYPE = TRACKStrToType(cLineStr);
				
				//Push new TRACK to the FILE vector
				pushTRACK(lineID, lineTYPE);
			}
			
			//INDEX line type	
			if(cLineType == ltINDEX) {
				//Make sure a TRACK is availible to push to
				if(FILE.back().TRACK.empty()) throw push_index_order;
				
				//Get ID (second word), and timestamp (third word)
				unsigned int lineID = std::stoi(getWord(cLineStr, 2));
				unsigned long lineBytes = timestampToBytes(getWord(cLineStr, 3));
				
				//Push new INDEX to TRACK sub-vector
				pushINDEX(lineID, lineBytes);
			}
			
		} catch(const PrgExcep &e) {
			std::cerr << "Error in Line number: " << lineNo 
			          << e.what() << std::endl;
		}
	}
}

int CueHandler::outputCueData() {
	//Go through all the callers' FILE vector
	for(size_t cFile = 0; cFile < this->FILE.size(); cFile++) {
		//Temporary FILE Object
		FileData pFILE = this->FILE[ cFile ];
		
		//Print Current FILE string to the cue file
		cueFile->append( generateFILELine(pFILE) );
		
		//Go through all the TRACKs
		for(size_t cTrack = 0; cTrack < pFILE.TRACK.size(); cTrack++) {
			//Temporary TRACK Object
			TrackData pTRACK = pFILE.TRACK[ cTrack ];
			
			//Print current TRACK string to the cue file
			cueFile->append( generateTRACKLine(pTRACK) );
			
			//Go through all INDEXs
			for(size_t cIndex = 0; cIndex < pTRACK.INDEX.size(); cIndex++) {
				//Temporary INDEX object
				IndexData pINDEX = pTRACK.INDEX[ cIndex ];
				
				//Print current INDEX string to the cue file
				cueFile->append( generateINDEXLine(pINDEX) );
			}
		}
	}
	
	return 0;
}

void CueHandler::printFILE(FileData & pFILE) {
	//Check if pFILE is empty
	if(pFILE.FILENAME.empty()) {
		throw file_object_fail;
	}

	//Print filename and data
	std::cout << "FILENAME: " << pFILE.FILENAME;
	std::cout << "\t\tTYPE: " << t_FILE_str[(int)pFILE.TYPE] << std::endl;
	//Seperator
	std::cout << "-----------------------------------------------------"
	          << std::endl;

	//Print all TRACKs in vector held by FILE
	for(size_t tIdx = 0; tIdx < pFILE.TRACK.size(); tIdx++) {
		//Set TrackData object to point to current TRACK
		TrackData pTRACK = pFILE.TRACK[tIdx];
		
		//Print track number (TrackIndex + 1 to not have 00 track) and PREGAP:
		std::cout << "TRACK " << padIntStr(pTRACK.ID, 2);
		
		//Print TRACK TYPE variable
		std::cout << "        TYPE: " << t_TRACK_str[pTRACK.TYPE] << std::endl;
		
		//Print all INDEXs contained in that TRACK
		for(size_t iIdx  = 0; iIdx < pTRACK.INDEX.size(); iIdx++) {
			IndexData pINDEX = pFILE.TRACK[tIdx].INDEX[iIdx];
			
			//Print the index number
			std::cout << "    INDEX " << padIntStr(pINDEX.ID, 2)
			//Print the raw BYTES format 
			<< "    OFFSET: " << padIntStr(pINDEX.BYTES, 9, ' ')
			//Print the timestamp version
			<< "    TIMESTAMP: " << bytesToTimestamp(pINDEX.BYTES) << std::endl;
		}
		
		//Print a blank line to split the TRACK fields
		std::cout << std::endl;
	}
	
	//TODO Catch?
}

* Writing functions *********************************************************
void CueHandler::write() {
	cueFile->overwrite();
}

* AUX functions *************************************************************
std::string CueHandler::bytesToTimestamp(const unsigned long bytes) {
	 The timestamp is in Minute:Second:Frame format.
	There are 75 sectors per second, 2352 bytes per sector. If the input number 
	is not divisible by 1 sector, then exit the program.
	
	I am trying to use divide then modulo so the compiler stands some chance of
	optimizing my code to use the remainder of the ASM div operator. 
	
	//There are 2352 byte per sector. Calculate how many sectors are in the file
	unsigned long sectors = bytes / 2352;
	
	//Error check if the input is divisible by a sector. Exit if not
	if(bytes % 2352 != 0) throw timestamp_fail;
	
	//75 sectors per second. Frames are the left over sectors from a second
	unsigned short seconds = sectors / 75;
	unsigned short rFrames = sectors % 75;
	
	//Convert seconds to minutes. Seconds is the remainder of itself after / 60
	unsigned short minutes = seconds / 60;
	seconds = seconds % 60;
	
	//If minutes exceeds 99, there is probably an error due to Audio CD Standard
	if(minutes > 99) throw timestamp_overflow;
	
	std::string timestamp;	
	
	//Now the string can be formed from the values. Need to 0 pad each value
	timestamp.append(padIntStr(minutes, 2) + ":" + padIntStr(seconds, 2) + ":"
	                           + padIntStr(rFrames, 2));
	
	return timestamp;
}

unsigned long CueHandler::timestampToBytes(const std::string timestamp) {
	//Make sure the string input is long enough to have xx:xx:xx timestamp
	if(timestamp.length() != 8) throw timestamp_str_invalid;

	//Strip values from the timestamp. "MM:SS:ff" ff = sectors
	unsigned short minutes = std::stoi(timestamp.substr(0, 2));
	unsigned short seconds = std::stoi(timestamp.substr(3, 2));
	unsigned short frames  = std::stoi(timestamp.substr(6, 2));
	
	//Add minutes to the seconds for sector calculation
	seconds += (minutes * 60);
	
	//75 sectors per second, plus frames left over in the timestamp	
	unsigned long sectors = (seconds * 75) + frames;
	
	//There are 2352 bytes per sector.
	unsigned long bytes = sectors * 2352;
	
	//Error check if the input is divisible by a sector. Exit if not
	if(bytes % 2352 != 0) throw timestamp_fail;
	
	return bytes;
}

** Modified function from TeFiEd to not depend on object ********************
std::string CueHandler::getWord(const std::string input, unsigned int index) {
	//If index is 0, set it to 1. always 1 indexed
	if(index == 0) index = 1;
	
	//Create output string object
	std::string output;

	//Set the delim string -- Regular delims, and Tab, Carriage Return (Windows)
	const std::string delim = " .,;\t\r";
	
	//Start and end of word, and current word found.
	size_t wordStart = 0, wordEnd = 0, wordIndex = 0;
	
	//Find the start and end of a word
	do {
		//Get the start and end of a word, detected by delim
		wordStart = input.find_first_not_of(delim, wordEnd);	
		wordEnd = input.find_first_of(delim, wordStart);
		
		//wordEnd can be allowed to overflow, but wordStart cannot.
		if(wordStart != std::string::npos) {
			//Incriment word index.
			++wordIndex;
			
			//If this index is the one requested, set output
			if(wordIndex == index) {
				output = input.substr(wordStart, wordEnd - wordStart);
				break;
			}
		}
	
	} while(wordStart < input.size());	
	
	//If the index could not be found, return an empty string
	if(wordIndex < index) {
		output = "";
	}
	
	return output;
}

std::string CueHandler::padIntStr(const unsigned long val, 
                                  const unsigned int len, const char pad) {
	std::string intStr = std::to_string(val);
	
	//Pad if selected
	if(len != 0 && len > intStr.length()) {
		unsigned int padDelta = len - intStr.length();
		intStr.insert(0, padDelta, pad);
	}
	
	return intStr;
}
*/
