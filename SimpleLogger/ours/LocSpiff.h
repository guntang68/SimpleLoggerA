/*
 * LocSpiff.h
 *
 *  Created on: Aug 4, 2019
 *      Author: annuar
 */

#ifndef LOCAL_LOCSPIFF_H_
#define LOCAL_LOCSPIFF_H_

#include "Arduino.h"
#include "SPIFFS.h"
#include "FS.h"




typedef struct {
	String	filename;
	time_t	lastWrite;
    int 	size;
} FileInfo_t;

class LocSpiff {
private:
	String _mana;
public:
	LocSpiff(String created);
	virtual ~LocSpiff();
	void listAllFiles();
	void listDir(const char * dirname, uint8_t levels);

	String readFile(const char * path);
	std::vector<std::vector<String>> readCSV(const char * path);

	bool writeFile(const char * path, const char * message);
	void appendFile(const char * path, const char * message);
	void renameFile(const char * path1, const char * path2);
	void deleteFile(const char * path);
	void format();
	FileInfo_t getInfo(const char * path);

	//ambil dari SpiffsHandler
//	void mulaFiles();
	void mulaFiles(bool mulai);

};

#endif /* LOCAL_LOCSPIFF_H_ */
