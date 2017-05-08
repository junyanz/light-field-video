#ifndef UTILITY_H_
#define UTILITY_H_
#include "common.h"

namespace utility {
class CUtility {
public:
	CUtility() {}
	~CUtility() {}
	static int FindImageFiles(string fName, vectorString& nameVec,
			vectorString &extVec);
	static int itoa(int val, char* buf);
	static void AddSuffix(vectorString& _nameVec, string _suffix, vectorString& _outNameVec);
	static void mkdirs( vectorString _dirNameVec);  
	static void mkdirs(string _dirName); 
    static string fullfile(string _path, string _fName);
private:
	static bool ParseImageFileName(string& fileName, string& ext);
};
}

#endif //UTILITY_H_
