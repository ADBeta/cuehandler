# cuehandler
**cuehandler** is a small and efficient library to create, modify, and 
impliment .CUE files into projects in C++11 and newer.

**cuehandler** is perfect for the following:  
* Handle the following TRACK Tpyes: AUDIO, CDG, MODE1_2048, MODE1_2352, 
MODE2_2336, MODE2_2352, CDI_2336, CDI_2352
* Guards against TRACKs, INDEXs and Timestamps from exceeding 99, to comply with
CD standards
* Makes Creaton, modification and reading of .cue files easy
* Get filesize of each file inside the cue, and easily use the filenames to 
create a full path - this makes reading and writing the files in the .cue very
easy
* Allows for very easy combination of .cue FILEs - this was specifically added
for (psx-combine)[https://github.com/ADBeta/psx-comBINe]

## Usage
See the (example)[https://github.com/ADBeta/cuehandler/tree/main/example]
directory for demonstation code, there is  also has a makefile to create a demo
program.

----
This project is under GPLv2.0. See LICENSE for more info. ADBeta(c) Nov 2023
