#pragma once
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif
using namespace std;
string realRoot = "vfs_root";
void realDeleteAll(){
#ifdef _WIN32
    string command = "if exist " + realRoot + " rmdir /s /q " + realRoot;
#else
    string command = "rm -rf " + realRoot;
#endif
    system(command.c_str());
}
void realMakeFolder(string path){
    string fullPath = realRoot + path;          
#ifdef _WIN32
    _mkdir(fullPath.c_str());
#else
    mkdir(fullPath.c_str(), 0755);
#endif
}
void realWriteFile(string path, string text){
    string fullPath = realRoot + path;
    ofstream file(fullPath.c_str());
    file << text;
}
void realRemoveFile(string path){
    string fullPath = realRoot + path;
    remove(fullPath.c_str());
}
void realRemoveFolder(string path){
    string fullPath = realRoot + path;
#ifdef _WIN32
    _rmdir(fullPath.c_str());
#else
    rmdir(fullPath.c_str());
#endif
}
void realRename(string oldPath, string newPath){
    string from = realRoot + oldPath;
    string to = realRoot + newPath;
    rename(from.c_str(), to.c_str());
}
void realStart(){
    realDeleteAll();
    realMakeFolder("");
}