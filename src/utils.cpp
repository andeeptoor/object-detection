/*
 * Utils.cpp
 *
 *  Created on: Feb 12, 2013
 *      Author: atoor
 */
#include "utils.h"
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <sys/stat.h>

using namespace std;

namespace utils {

string findAndReplace(const string find, const string replace, const string fullString) {
	int index;
	index = fullString.find(find);
	if (index > -1) {
		string keepString = fullString.substr(index + find.length());
		return append(replace, keepString);
	}
	return "";
}

vector<string> &split(const string &s, char delim, vector<string> &elems) {
	stringstream ss(s);
	string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}

bool notEquals(char * one, string two) {
	return strcmp(one, two.c_str());
}

bool equals(string one, string two) {
	return !strcmp(one.c_str(), two.c_str());
}

int stringToInt(string arg) {
	return stringToInt((char *) arg.c_str());
}

double stringToDouble(string arg) {
	return stringToDouble((char *) arg.c_str());
}

string append(string one, string two) {
	stringstream stream1;
	stream1 << one << two;
	return stream1.str();
}

int stringToInt(char * arg) {
	stringstream stream1;
	stream1.str(arg);
	int stringToInt;
	stream1 >> stringToInt;
	return stringToInt;
}

double stringToDouble(char * arg) {
	stringstream stream1;
	stream1.str(arg);
	double stringToDouble;
	stream1 >> stringToDouble;
	return stringToDouble;
}

//File operations
bool isDirectory(string name) {
	struct stat st;
	lstat(name.c_str(), &st);
	return S_ISDIR(st.st_mode);
//	return opendir(name.c_str()) != NULL;
}

string getFileExtension(char* fileName) {
	stringstream stream1;
	stream1.str(fileName);
	return getFileExtension(stream1.str());
}

string getFileExtension(string fileName) {
	std::string::size_type indexOfDot;
	indexOfDot = fileName.rfind('.');
	if (indexOfDot != std::string::npos) {
		string actualFileExtension = fileName.substr(indexOfDot + 1);
		return actualFileExtension;
	}
	return "";
}

string getFileWithoutExtension(string fileName) {
	std::string::size_type indexOfDot;
	indexOfDot = fileName.rfind('.');
	if (indexOfDot != std::string::npos) {
		string actualFileWithoutExtension = fileName.substr(0, indexOfDot);
		return actualFileWithoutExtension;
	}
	return "";
}

string getFileWithoutParentDirectory(string fileName) {
	std::string::size_type index;
	index = fileName.rfind('/');
	if (index != std::string::npos) {
		string converted = fileName.substr(index + 1);
		return converted;
	}
	return "";
}

string convertToParentDirectory(string file, string parentDirectory) {
	string fileWithoutParentDirectory = utils::getFileWithoutParentDirectory(file);
	stringstream stream1;
	stream1 << parentDirectory << "/" << fileWithoutParentDirectory;
	string convertedFileName = stream1.str();
	return convertedFileName;
}

string getParentDirectory(string fileName) {
	std::string::size_type index;
	index = fileName.rfind('/');
	if (index != std::string::npos) {
		string converted = fileName.substr(0, index);
		return converted;
	}
	return "";
}

string convertToFileExtension(string file, string extension) {
	string fileWithoutExtension = utils::getFileWithoutExtension(file);
	stringstream stream1;
	stream1 << fileWithoutExtension << "." << extension;
	string convertedFileName = stream1.str();
	return convertedFileName;
}

bool matchesFileExtension(string fileName, string fileExtension) {
	std::string::size_type indexOfDot;
	indexOfDot = fileName.rfind('.');
	if (indexOfDot != std::string::npos) {
		string actualFileExtension = fileName.substr(indexOfDot + 1);
		return !strcmp(actualFileExtension.c_str(), fileExtension.c_str());
	} else {
		return false;
	}
}

void recursivelySearchDirectoryForFiles(string directoryName, string fileExtension, vector<string> *files) {
	struct dirent *ep;

	DIR *directory = opendir(directoryName.c_str());
	if (directory != NULL) {
		while ((ep = readdir(directory))) {
//			cout << ep->d_name << endl;
			if (utils::notEquals(ep->d_name, ".") && utils::notEquals(ep->d_name, "..")) {
				stringstream fileNameStream;
				fileNameStream << directoryName << "/" << ep->d_name;
				string fileName = fileNameStream.str();
//				cout << "[" << fileName << "]" << endl;
				if (utils::isDirectory(fileName)) {
//					cout << "Directory: [" << fileName << "]" << endl;
					recursivelySearchDirectoryForFiles(fileName, fileExtension, files);
				} else {
					if (utils::matchesFileExtension(fileName, fileExtension)) {
						files->push_back(fileName);
					} else {
//						cout << "Rejecting: [" << fileName << "]" << endl;
					}
				}
			}
		}
		closedir(directory);
	} else {
		cout << "Could not find directory: " << directoryName << endl;
	}

//	cout << "Found [" << files->size() << "]" << endl;
}

}

