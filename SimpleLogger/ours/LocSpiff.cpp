/*
 * LocSpiff.cpp
 *
 *  Created on: Aug 4, 2019
 *      Author: annuar
 */

#include <LocSpiff.h>
#include "SPIFFS.h"
#include "FS.h"
#include <vector>
#include <string>

LocSpiff *iniSpiff;

LocSpiff::LocSpiff(String created) {
	iniSpiff = this;

	iniSpiff->_mana = created;

	log_i("LocSpiff ---------------------------- bina @ %s", iniSpiff->_mana.c_str());
}

LocSpiff::~LocSpiff() {
	log_i("LocSpiff ---------------------------- musnah @ %s", iniSpiff->_mana.c_str());

}

void LocSpiff::listAllFiles() {

	if(SPIFFS.begin(true)){
		File root = SPIFFS.open("/");

		File file = root.openNextFile();

		while(file){
//			il.setMsg("File: " + String(file.name()));
			log_i("File: %s",String(file.name()).c_str());

			file = root.openNextFile();
		}

		root.close();
		file.close();

		SPIFFS.end();
	}


}



bool isDelim(const char& query, const String& delimList){
    for(int i = 0; i < delimList.length(); i++)
        if(query == delimList[i])
            return true;
    return false;
}

std::vector<String> split(const String& line, const String& delimList){
    int len = line.length();
    String temp;
    std::vector<String> output;
    for(int i = 0; i < len; i++){
        if(isDelim(line[i], delimList)){
            if(temp.length() > 0)
                output.push_back(temp);
            temp="";//.clear();
        }
        else
            temp += line[i];
    }
    if(temp.length() > 0)
        output.push_back(temp);
    return output;
}

std::vector<std::vector<String>> LocSpiff::readCSV(const char* path) {


	std::vector<std::vector<String>> rows;
	std::vector<String> cols;

    if(SPIFFS.begin(true)){
        File file = SPIFFS.open(path);
        if(!file || file.isDirectory()){
            log_e("- failed to open file for reading");
            file.close();
            SPIFFS.end();
            return rows;
        }

        log_i("- read from file:");
        while(file.available()){
        	String ss = file.readStringUntil('\n');

        	cols = split(ss, ",");
        	rows.push_back(cols);
        }
        file.close();
        SPIFFS.end();
    }

    return rows;

}





void LocSpiff::renameFile(const char* path1, const char* path2) {
    if(SPIFFS.begin(true)){

        if (SPIFFS.rename(path1, path2)) {
        	log_i("- file renamed");
        } else {
        	log_e("- rename failed");
        }
        SPIFFS.end();
    }


}

void LocSpiff::appendFile(const char* path, const char* message) {
    if(SPIFFS.begin(true)){
        File file = SPIFFS.open(path, FILE_APPEND);
        if(!file){
        	log_e("- failed to open file for appending");
        	file.close();
        	SPIFFS.end();
            return;
        }

        file.print(message);
        file.close();
        SPIFFS.end();
    }
}

bool LocSpiff::writeFile(const char* path, const char* message) {
	bool res= false;
    if(SPIFFS.begin(true)){

    	log_i("Path = %s", path);

        File file;

        file.clearWriteError();
        log_i("SINI");
        file  = SPIFFS.open(path, FILE_WRITE);


        if(!file){
        	log_e("- failed to open file for writing");
        	file.close();
        	SPIFFS.end();
            return res;
        }

        log_i("Kat sini %s", message);

        if(file.print(message)){
        	log_i("- file written");
        	res = true;
        } else {
        	log_e("- write failed");
        }
        file.close();
        SPIFFS.end();


    }
    else{
    	log_i("~~~~~~spiff begin fail");
    }

    SPIFFS.end();

    return res;
}

String LocSpiff::readFile(const char* path) {
	String res = "";
    if(SPIFFS.begin(true)){
        File file = SPIFFS.open(path, FILE_READ);
        if(!file || file.isDirectory()){
            log_e("- failed to open file for reading");
            res = "NAN";

//            file = SPIFFS.open(path, FILE_WRITE);
//            if(file.print("")){
//            	log_i("- file written");
//            	res = true;
//            } else {
//            	log_e("- write failed");
//            }

//            file.close();
            SPIFFS.end();
            return res;
        }

        log_i("- read from file:");
        while(file.available()){
        	res += file.readString();
        }
        file.close();
        SPIFFS.end();
    }
    return res;
}
void LocSpiff::listDir(const char* dirname, uint8_t levels) {


    if(SPIFFS.begin(true)){
        File root = SPIFFS.open(dirname);
        if(!root){
            Serial.println("Failed to open directory");
//            root.close();
            return;
        }
        if(!root.isDirectory()){
            Serial.println("Not a directory");
//            root.close();
            return;
        }

        File file = root.openNextFile();
        while(file){
            if(file.isDirectory()){
                Serial.print("  DIR : ");
                Serial.print (file.name());
                time_t t= file.getLastWrite();
                struct tm * tmstruct = localtime(&t);
                Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
                if(levels){
                    listDir(file.name(), levels -1);
                }
            } else {
                Serial.print("  FILE: ");
                Serial.print(file.name());
                Serial.print("  SIZE: ");
                Serial.print(file.size());
                time_t t= file.getLastWrite();
                struct tm * tmstruct = localtime(&t);
                Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
            }
            file = root.openNextFile();
        }


        root.close();
        file.close();

        SPIFFS.end();
    }
}

FileInfo_t LocSpiff::getInfo(const char* path) {
	FileInfo_t res;

	res.filename = "";
	res.lastWrite = 0;
	res.size = -1;

    if(SPIFFS.begin(true)){

    	if(SPIFFS.exists(path)){
            File file = SPIFFS.open(path, FILE_READ);
            if(!file){
            	log_e("- failed to open file for writing");
            	file.close();
            	SPIFFS.end();
                return res;
            }

            res.filename = file.name();// path;
            res.lastWrite = file.getLastWrite();
            res.size = file.size();
            file.close();
    	}
    	else{
    		res.filename = "NOFILE";
    		res.lastWrite = 0;
    		res.size = -1;
    	}
    }
	SPIFFS.end();


	return res;

}

void LocSpiff::deleteFile(const char* path) {
    if(SPIFFS.begin(true)){

        if(SPIFFS.remove(path)){
        	log_i("- file deleted");
        } else {
        	log_e("- delete failed");
        }
        SPIFFS.end();
    }
}

//void LocSpiff::createDir(const char* path) {
//    if(SPIFFS.begin(true)){
//
//        Serial.printf("Creating Dir: %s\n", path);
//        if(SPIFFS.mkdir(path)){
//        	il.setMsg("Dir created");
//        } else {
//        	il.setMsg("mkdir failed");
//        }
//        SPIFFS.end();
//    }
//
//
//}
//
//void LocSpiff::removeDir(const char* path) {
//    if(SPIFFS.begin(true)){
//
//        if(SPIFFS.rmdir(path)){
//            Serial.println("Dir removed");
//        } else {
//            Serial.println("rmdir failed");
//        }
//        SPIFFS.end();
//    }
//
//
//
//}

void LocSpiff::format() {
    if(SPIFFS.begin(true)){

    	SPIFFS.format();
        SPIFFS.end();
    }

}

//void LocSpiff::mulaFiles() {
//}

void LocSpiff::mulaFiles(bool mulai) {
	SPIFFS.begin();

	String str, tmp;
	File root = SPIFFS.open("/", FILE_READ);
	File dir = root.openNextFile();
	while (dir) {
		str += dir.name();
		str += " / ";
		str += dir.size();
		str += "\r\n";

		if (mulai) {
			tmp = dir.name();
			if (tmp.indexOf("json") > 0 || tmp.indexOf("txt") > 0) {
				str = "Delete - ";
				str += dir.name();
				SPIFFS.remove(dir.name());
			}
		}
		else {
			str = "Exist -";
			str += dir.name();
			str += " / ";
			str += dir.size();
		}
//		hantu(str);
		log_i("---%s", str.c_str());
		dir = root.openNextFile();

	}

	File configFile;

	if (!SPIFFS.exists("/i.txt")) {

		configFile = SPIFFS.open("/i.txt", "w");
		if (configFile) {
			configFile.print("1");
			configFile.close();
//			hantu("Create - /i.txt");
			log_i("---Create - /i.txt");
		}
	}

	if (!SPIFFS.exists("/n.txt")) {

		configFile = SPIFFS.open("/n.txt", "w");
		if (configFile) {
			configFile.print("UNKNOWN");
			configFile.close();
//			hantu("Create - /n.txt");
			log_i("---Create - /n.txt");
		}
	}

	if (!SPIFFS.exists("/j.txt")) {

		configFile = SPIFFS.open("/j.txt", "w");
		if (configFile) {
			configFile.print("d");
			configFile.close();
//			hantu("Create - /j.txt");
			log_i("---Create - /j.txt");
		}
	}

	if (!SPIFFS.exists("/c.txt")) {

		configFile = SPIFFS.open("/c.txt", "w");
		if (configFile) {
			configFile.print("I4N2IP.png");
			configFile.close();
//			hantu("Create - /c.txt");
			log_i("---Create - /c.txt");
		}
	}

	if (!SPIFFS.exists("/s.txt")) {

		configFile = SPIFFS.open("/s.txt", "w");
		if (configFile) {
			configFile.print("I4N2IP.png");
			configFile.close();
//			hantu("Create - /s.txt");
			log_i("---Create - /s.txt");
		}
	}

	if (!SPIFFS.exists("/m.txt")) {

		configFile = SPIFFS.open("/m.txt", "w");
		if (configFile) {
			configFile.print("A00000");
			configFile.close();
//			hantu("Create - /m.txt");
			log_i("---Create - /m.txt");
		}
	}

	if (!SPIFFS.exists("/e.txt")) {

		configFile = SPIFFS.open("/e.txt", "w");
		if (configFile) {
			configFile.print("10800000");
			configFile.close();
//			hantu("Create - /e.txt");
			log_i("---Create - /e.txt");
		}
	}

	if (!SPIFFS.exists("/g.txt")) {

		configFile = SPIFFS.open("/g.txt", "w");
		if (configFile) {
			configFile.print("GF7TDK,G1OFJV");
			configFile.close();
//			hantu("Create - /g.txt");
			log_i("---Create - /g.txt");
		}
	}

	if (!SPIFFS.exists("/sonar.txt")) {

		configFile = SPIFFS.open("/sonar.txt", "w");
		if (configFile) {
			configFile.print("1");
			configFile.close();
//			hantu("Create - /sonar.txt");
			log_i("---Create - /sonar.txt");
		}
	}

	if (!SPIFFS.exists("/min.txt")) {

		configFile = SPIFFS.open("/min.txt", "w");
		if (configFile) {
			configFile.print("500");
			configFile.close();
//			hantu("Create - /min.txt");
			log_i("---Create - /min.txt");
		}
	}

	if (!SPIFFS.exists("/max.txt")) {

		configFile = SPIFFS.open("/max.txt", "w");
		if (configFile) {
			configFile.print("5000");
			configFile.close();
//			hantu("Create - /max.txt");
			log_i("---Create - /max.txt");
		}
	}

	if (!SPIFFS.exists("/hopper.txt")) {

		configFile = SPIFFS.open("/hopper.txt", "w");
		if (configFile) {
			configFile.print("0");
			configFile.close();
//			hantu("Create - /hopper.txt");
			log_i("---Create - /hopper.txt");
		}
	}

	SPIFFS.end();
}
