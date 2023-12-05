/*******************************************************************************
* This file is part of cuehandler. 
* See the GitHub for more information: https://github.com/ADBeta/cuehandler
*
* A Simple & Efficient Library to Create, Modify and Impliment .CUE files in C++
*
* ADBeta    05 Dec 2023    V0.4.3
*******************************************************************************/
#ifndef CUEHANDLER_H
#define CUEHANDLER_H

#include <string>
#include <list>
#include <fstream>

class CueFile {
	/*** Public Members *******************************************************/
	public:
	//Constructor with Member Initilizer List
	CueFile(const char *fn) : filename(fn){}
	
	
	//Destructor to clear RAM overhead

	
	/*** File Management ******************************************************/
	//Reads the CueFile and parses it into the CueData structres. 
	//Returns 0 on success, or negative values on error code
	int Read();
	
	//TODO write
	
	//TODO Modifiers
	
	//TODO windows to linux line end
	//


	/*** Cue Handling *********************************************************/
	//Line Type Enumeration
	enum class CueLineType {
		Invalid,
		File,
		Track,
		Index,
		Remark
	};
	
	//TRACK Type Enumeration
	enum class CueTrackType {
		AUDIO,                     //Audio/Music (2352 — 588 samples)
		CDG,                       //Karaoke CD+G (2448)
		MODE_12048,                //CD-ROM Mode 1 Data (cooked)
		MODE_12352,                //CD-ROM Mode 1 Data (raw)
		MODE_22336,                //CD-ROM XA Mode 2 Data (form mix)
		MODE_22352,                //CD-ROM XA Mode 2 Data (raw)
		CDI_2336,                  //CDI Mode 2 Data
		CDI_2352                   //CDI Mode 2 Data   
	};
	
	struct CueFileObj {
		std::string filename;
		std::string filetype;
		
		struct CueTrackObj {
			unsigned int id;
			CueTrackType type;
		
			struct CueIndexObj {
			
			//	CueIndexObj(const unsigned int v_id, const unsigned long v_bytes) 
		       //     : id(v_id), bytes(v_bytes) {}
			
				unsigned int id;
				unsigned long bytes;
			};
			std::list<CueIndexObj> Index;
		
		};
		std::list<CueTrackObj> Track;
	
	
	};
	
	//Create a List of CueFiles, this constitutes a CueSheet structure
	std::list<CueFileObj> CueSheet;
	
	
	
	
	
	
	
	

	/*** Private Members ******************************************************/
	//private:

	/*** File Management ******************************************************/
	std::fstream cue_file;
	std::string filename;
	
	//Low level file management. Return 0on success or error codes on failure
	int Open();
	int Close();
	
	/*** Cue Handling *********************************************************/
	//Take std::string, parse and return the type of line it is
	CueLineType GetCueLineType(const std::string &line);
	
};






















#endif

/*

//Valid FILE formats. (only binary is supported for now)
enum t_FILE {ftBINARY};
//String of LINE types mapped to enum
extern const std::string t_FILE_str[];




** CueHandler Class ********************************************************
class CueHandler {
	public:
	//Constructor takes a filename (char *) and passes it to the TeFiEd cueFile
	//Also creates the metadata handler structures
	CueHandler(const std::string filename);
	
	//Destructor, deletes metadata array and cleans up TeFiEd object
	~CueHandler();
	
	TeFiEd *cueFile; //TeFiEd text file object
	
	** Cue file data structs ***********************************************
	//INDEXs. Grandchild (3rd level) value. Max 99
	struct IndexData {
		unsigned int ID = 0;		
		//Number of bytes the current index holds
		unsigned long BYTES = 0;
	};
	
	//Child TRACK (2nd level) object. Max 99
	struct TrackData {
		unsigned int ID = 0;
		t_TRACK TYPE;
		//Vector of INDEXs
		std::vector <IndexData> INDEX;
	};
	
	//Parent FILE (Top level) object
	struct FileData {
		//FILE data - Default values
		std::string FILENAME;
		t_FILE TYPE;
		//Vector of TRACKs for each FILE
		std::vector <TrackData> TRACK;
	};
	
	//Vector of FILEs (Cue Data is stored in this nested vector)
	std::vector <FileData> FILE;
	
	//Clears the FILE vector and deallocates its RAM
	void cleanFILE();
	
	** Cue file data functions **********************************************
	//Returns the t_LINE of the string passed (whole line from cue file)
	t_LINE LINEStrToType(const std::string lineStr);
	
	//Return the t_FILE of the string passed
	t_FILE FILEStrToType(const std::string fileStr);	
	
	//Returns the t_TRACK of the string passed
	t_TRACK TRACKStrToType(const std::string trackStr);
	
	//Returns the FILE type string from t_FILE_str via enum
	std::string FILETypeToStr(const t_FILE);
	
	//Returns the TRACK type string from t_TRACK_str via enum
	std::string TRACKTypeToStr(const t_TRACK);
	
	////////////////////////////////////////////////////////////////////////////
	//Validation functions. Returns specific error codes
	//Validate an input .cue file string (argv[1])
	int validateCueFilename(std::string);
	
	//Validate TRACK
	int validateTRACK(const TrackData &);
	
	//Validate INDEX
	int validateINDEX(const IndexData &);
	
	////////////////////////////////////////////////////////////////////////////
	//Push a new FILE to FILE[]
	void pushFILE(const std::string FN, const t_FILE TYPE);
	
	//Push a new TRACK to the last entry in FILE[]
	void pushTRACK(const unsigned int ID, const t_TRACK TYPE);
	
	//Push a new INDEX to the last entry in FILE[].TRACK[]
	void pushINDEX(const unsigned int ID, const unsigned long BYTES);

	////////////////////////////////////////////////////////////////////////////
	//Converts FileData Object into a string for the CUE file
	std::string generateFILELine(const FileData &);
	
	//Converts TrackData Object into a string for the CUE file
	std::string generateTRACKLine(const TrackData &);
	
	//Converts IndexData Object into a string for the CUE file
	std::string generateINDEXLine(const IndexData &);
	
	** Input / Output CUE Handling ******************************************
	//Gets all the data from a .cue file and populates the FILE vector.
	void getCueData();
		
	//Outputs the CueData to the cueFile
	int outputCueData();
	
	//Prints a passed FileData Object to the cli
	void printFILE(FileData &);
	
	//Read in an existing cue file
	void read();
	
	//Create a new cue file
	void create();

	** Writing Functions ***************************************************
	//Output (overwrite) the TeFiEd object RAM into the file.
	void write();

	* AUX Functions *********************************************************
	//Converts a number of bytes into an Audio CD timestamp.
	std::string bytesToTimestamp(const unsigned long bytes);
	
	//Converts an Audio CD timestamp into number of bytes
	unsigned long timestampToBytes(const std::string timestamp);
	
	//Modified from TeFiEd. Returns -index- word in a string
	std::string getWord(const std::string input, unsigned int index);
	
	//Takes an input uint32_t, zero-pads to -pad- then return a string
	std::string padIntStr(const unsigned long val, const unsigned int len = 0,
	                      const char pad = '0');

}; //class CueHandler
#endif

*/
