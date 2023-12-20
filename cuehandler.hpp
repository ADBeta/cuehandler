/*******************************************************************************
* This file is part of cuehandler. 
* See the GitHub for more information: https://github.com/ADBeta/cuehandler
*
* A Simple & Efficient Library to Create, Modify and Impliment .CUE files in C++
* cuehandler is under GPL 2.0. See LICENSE for more information
*
* ADBeta    20 Dec 2023    V1.0.0
*******************************************************************************/
//TODO Allow disable of safeties

#ifndef CUEHANDLER_H
#define CUEHANDLER_H

#include <string>
#include <list>
#include <fstream>
#include <cstdint>
#include <limits>

/*** Cue File/Sheet Exceptions ************************************************/
class CueException : public std::exception {
	public:
	CueException(const char *msg) : errMsg(msg) {}
	const char *what () const noexcept override { 
		return errMsg;
	}

	private:
	const char *errMsg;
};

//Exceptions
extern CueException file_invalid;

extern CueException file_push_null_input;
extern CueException track_push_null_input;
extern CueException index_push_null_input;

extern CueException track_push_overspec;
extern CueException index_push_overspec;

extern CueException track_push_null_file;
extern CueException index_push_null_track;


/*** Cue Sheet Data Handling & Structure **************************************/
//Hierarchical structure of all the infomation contained in a .cue file and 
//Functions to handle the data structures
//All IDs and the Timestamp is limited to 99 to conform to the CUE/CD Standards
struct CueSheet {
//TODO Remove this?
	static const uint32_t timestamp_nval = std::numeric_limits<uint32_t>::max();

	/*** Cue Sheet Data Structures ********************************************/
	//TRACK Type Enumeration
	enum class TrackType {
		Invalid,                   //Catch-all if error occurs
		AUDIO,                     //Audio/Music (2352 — 588 samples)
		CDG,                       //Karaoke CD+G (2448)
		MODE1_2048,                //CD-ROM Mode 1 Data (cooked)
		MODE1_2352,                //CD-ROM Mode 1 Data (raw)
		MODE2_2336,                //CD-ROM XA Mode 2 Data (form mix)
		MODE2_2352,                //CD-ROM XA Mode 2 Data (raw)
		CDI_2336,                  //CDI Mode 2 Data
		CDI_2352                   //CDI Mode 2 Data   
	};
	
	//Line Type Enumeration
	enum class LineType {
		Invalid,
		File,
		Track,
		Index,
		Remark
	};
	
	//TODO add destructor
	/*** Cue Structure ********************************************************/
	//Cue "FILE" Object, Top level. Contains TRACKs and INDEXs
	struct FileObj {
		//Initializer list
		FileObj(std::string fn, std::string ft, uint32_t fb) 
			                          : filename(fn), filetype(ft), bytes(fb) {}
		std::string filename;
		std::string filetype;
		uint32_t bytes;
		
		//Cue "TRACK" Object, has "INDEX"s and some Track info
		struct TrackObj {
			TrackObj(uint16_t t_id, TrackType t_type)
			                                         : id(t_id), type(t_type) {}
			uint16_t id;
			TrackType type;
			
			//Cue "INDEX" Object. Has Index data
			struct IndexObj {
				IndexObj(uint16_t i_id, uint32_t i_offset)
				                                 : id(i_id), offset(i_offset) {}
				uint16_t id;
				uint32_t offset;
			};
			//List of INDEXs inside each TRACK
			std::list<IndexObj> IndexList;
		};
		//List of TRACKs inside each FILE
		std::list<TrackObj> TrackList;
	};
	//List of FILEs inside each Cue Sheet
	std::list<FileObj> FileList;
	
	/*** Structure Functions **************************************************/
	//Take std::string, parse and return the type of line it is
	//Returns ::Invalid on failure
	static LineType StrToLineType(const std::string &input);
	//Take LineType and return a string representation of it
	//Returns Empty string on failure
	static std::string LineTypeToStr(const LineType type);
	
	//Take a std::string of a string line and return the TrackType it is
	//Returns ::Invalid on failure
	static TrackType StrToTrackType(const std::string &input);
	//Take a TrackType return the string Representation of it
	//Returns Empty string on failure
	static std::string TrackTypeToStr(const TrackType type);
	
	//Returns how many bytes are in a sector depending on TrackType.
	//Returns 0 on error
	static uint16_t GetSectorBytesInTrackType(const TrackType type); 
	
	//Converts bytes value to a timestamp string
	//Returns Empty string on error
	static std::string BytesToTimestamp(const uint32_t bytes, const TrackType);	
	//Convers a timestamp string to a bytes offset value
	//Returns timestamp_nval on error.
	static uint32_t TimestampToBytes(const std::string &timestamp, const TrackType);
	
	//Convert CueSheet Objects into standard file strings
	static std::string FileToStr(const FileObj                      *file_ptr);
	static std::string TrackToStr(const FileObj::TrackObj           *track_ptr);
	static std::string IndexToStr(const FileObj::TrackObj::IndexObj *index_ptr,
	                                                      const TrackType type);
	
	/*** API / Helper Functions ***********************************************/
	//Push a FILE, TRACK, or INDEX to the CueSheet. Always pushed to end.
	//Returns -1 and throws error on failure. Returns 0 on success
	//TODO Add safety 99 disable
	int PushFile(FileObj                      *file_ptr);
	int PushTrack(FileObj::TrackObj           *track_ptr);
	int PushIndex(FileObj::TrackObj::IndexObj *index_ptr);
		
	//Takes CueSheet Obj and returns parent in structure of type requested
	//Returns NULL if there is an error
	FileObj                     *GetLastFile();
	FileObj::TrackObj           *GetLastTrack();
	FileObj::TrackObj::IndexObj *GetLastIndex();
	
	//Deletes all data from the CueSheet and frees up memory
	void Clear();
	
	//Copies the data from [this] to the passed CueSheet
	//Returns negative values on error. Target Object may get malformed on error
	int CopyTo(CueSheet &target);
	
	//Combines multiple FILE Objects into one, with offset and indexing.
	//Optional output filename. If left blank will inherit file[0]'s name
	//Returns negative value on error - leaves object unmodified if failed.
	int Combine(std::string op_filename = "");
	
	//Creates a cue file spec string output based on CueSheet
	//Returns an empty string on error TODO maybe throw
	std::string ToString() const;
	
	//TODO
	//Prints all the information stored in the CueSheet to std out
	int Print() const;
};

/*** Cue File Management ******************************************************/
class CueFile {
	/*** Public Members *******************************************************/
	public:
	//Constructor with Member Initilizer List
	CueFile(const char *fn) : filename(fn) {}	
	
	//Reads the CueFile and parses it into the passed CueSheet Structure
	//Returns 0 on success. Throws Exception and returns negative value on error
	int Read(CueSheet &cs);
	
	//Write the passed CueSheet out to the CueFile file.
	//Returns negative values and throws exceptions on failures
	int Write(const CueSheet &cs);
	
	//File Attribute & Data functions
	//Gets and returns bytes in a file. Throws exceptions and returns 0 on error
	uint32_t GetFileBytes(const std::string &fname);
	
	
	
	
	
	

	/*** Private Members ******************************************************/
	//private:

	/*** File Management ******************************************************/
	std::fstream cue_file;
	std::string filename;
	
	int OpenRead();
	int OpenWrite();
	int Close();
};




















#endif
