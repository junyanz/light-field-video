#include "Utility.h"

const int MAX_FILENAME_LENGTH = 512;

#ifdef _WIN32
#include "io.h"
#include "direct.h"
#include "windows.h"
int utility::CUtility::FindImageFiles(string fName, vector<string>& nameVec,
        vector<string> &extVec) {
    _assert(fName.length() != 0);

    WIN32_FIND_DATA fileName;
    WCHAR wch_fileName[MAX_FILENAME_LENGTH];

    fName += "*.*";
    MultiByteToWideChar(0, 0, fName.c_str(), MAX_FILENAME_LENGTH - 1,
            wch_fileName, MAX_FILENAME_LENGTH);
    LPCWSTR wstr_fileName = wch_fileName;
    HANDLE handle = FindFirstFile(wstr_fileName, &fileName);
    int nFiles = 0;

    if (handle != INVALID_HANDLE_VALUE) {
        do {
            int nLen = lstrlen(fileName.cFileName);
            char* nPtr = new char[nLen + 1];
            nFiles++;

            FOR (i, nLen)
                nPtr[i] = char(fileName.cFileName[i]);

            nPtr[lstrlen(fileName.cFileName)] = '\0';

            if (nFiles != 1 && nFiles != 2) {
                string strFileName = string(nPtr);
                string ext;
                if (ParseImageFileName(strFileName, ext)) {
                    nameVec.push_back(strFileName);
                    extVec.push_back(ext);
                }
            }
        } while (FindNextFile(handle, &fileName));
    } else
        DEBUG_ERROR("Fail to open folder.");

    FindClose(handle);
    return nameVec.size();
}


void utility::CUtility::mkdirs( vector<string> _dirNameVec ) {
    FOR (i, (int)_dirNameVec.size()) {
        string dir = _dirNameVec[i];
        //if (_access(dir.c_str(), 0) == -1)
            _mkdir(dir.c_str());
    }
}

int utility::CUtility::FindFiles( string _fName, vector<string>& _nameVec ) {
    _assert(_fName.length() != 0);

    WIN32_FIND_DATA fileName;
    WCHAR wch_fileName[MAX_FILENAME_LENGTH];

    _fName += "*.*";
    char chr[MAX_FILENAME_LENGTH];
    strcpy(chr, _fName.c_str());
    MultiByteToWideChar(0, 0, chr, MAX_FILENAME_LENGTH,
        wch_fileName, MAX_FILENAME_LENGTH);
    LPCWSTR wstr_fileName = wch_fileName;
    HANDLE handle = FindFirstFile(wstr_fileName, &fileName);
    int nFiles = 0;

    if (handle != INVALID_HANDLE_VALUE) {
        do {
            int nLen = lstrlen(fileName.cFileName);
            char* nPtr = new char[nLen + 1];
            nFiles++;

            FOR (i, nLen)
                nPtr[i] = char(fileName.cFileName[i]);

            nPtr[lstrlen(fileName.cFileName)] = '\0';
            string strFileName = string(nPtr);
            if (nFiles != 1 && nFiles != 2)
                _nameVec.push_back(strFileName);
        } while (FindNextFile(handle, &fileName));
    } else
        DEBUG_ERROR("Fail to open folder.");

    FindClose(handle);
    return _nameVec.size();
}


#endif

#ifdef __linux__
#include "sys/types.h"
#include "dirent.h"
int utility::CUtility::FindImageFiles(string fName, vector<string>& nameVec,
        vector<string> &extVec) {
    // sorting
    extVec.clear();
    nameVec.clear();
    struct dirent* de = NULL;
    DIR* dir = opendir(fName.c_str());

    vectorString tmpnames;
    vectorString tmpexts;

    if (dir == NULL) {
        DEBUG_ERROR("Fail to open folder.", fName.c_str());
        return -1;
    }

    while ((de = readdir(dir)) != NULL) {
        string fileName = string(de->d_name);
        string ext;
        if (ParseImageFileName(fileName, ext)) {
            tmpexts.push_back(ext);
            tmpnames.push_back(fileName);
        }
    }
    closedir(dir);
    // sorting
    int nImgs = (int)tmpnames.size();

    vector<SortableElement<string> > strs(nImgs);
    vectori order;
    FOR (i, nImgs) {
        strs[i]._ind = i;
        strs[i]._val = tmpnames[i];
    }
    sortOrder(strs, order);
    nameVec.clear();
    extVec.clear();
    FOR (i, nImgs) {
        nameVec.push_back(strs[i]._val);
        extVec.push_back(tmpexts[strs[i]._ind]);
//        cout << strs[i]._val << " " << strs[i]._ind << " " << tmpexts[strs[i]._ind] << endl;
    }
    tmpnames.clear();
    tmpexts.clear();
    strs.clear();
    order.clear();
    return (int) nameVec.size();
}

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void utility::CUtility::mkdirs( vector<string> _dirNameVec ) {

    FOR (i, (int)_dirNameVec.size()) {
        string dir = _dirNameVec[i];
        struct stat sb;

        if (stat(dir.c_str(), &sb) == -1)
            mkdir(dir.c_str(), 0700);
    }
}


#endif



// for both linux and windows
bool utility::CUtility::ParseImageFileName(string& fileName, string& ext) {
    string::size_type pos = string::npos;
    if ((pos = fileName.find_last_of('.')) != string::npos) {
        ext = fileName.substr(pos +1);
        if (ext == "jpg" || ext == "JPG" || ext == "png" || ext == "PNG"
                || ext == "bmp" || ext == "BMP" || ext == "jpeg" || ext == "JPEG") {
            ext = fileName.substr(pos);
            fileName = fileName.substr(0, pos);

            return true;
        }
    }

    return false;
}

void utility::CUtility::AddSuffix( vector<string>& _nameVec, string _suffix, vector<string>& _outNameVec ) {
    int nNames = (int)_nameVec.size();
    _outNameVec.resize(nNames);
     #pragma omp parallel for
    FOR (i, nNames)
        _outNameVec[i] = _nameVec[i] + _suffix;
}


void utility::CUtility::mkdirs( string _dirName ) {
    vectorString tmp;
    tmp.push_back(_dirName);
    mkdirs(tmp);
    tmp.clear();
}

string utility::CUtility::fullfile(string _path, string _fName) {

}


