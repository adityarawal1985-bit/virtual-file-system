#pragma once
#include "phase2_rounak_real_folder.hpp"
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
const int MAX_INODES = 32;
struct Inode {
    bool used;           
    string name;
    bool isFolder;
    int parent;          
    int size;            
    vector<int> blocks;  
    bool readOnly;       
    string password;     
};
Inode inodes[MAX_INODES];
int currentFolder = 0;   
void resetInodes(){
    for(int i = 0; i < MAX_INODES; i++){
        inodes[i].used = false;
        inodes[i].name = "";
        inodes[i].isFolder = false;
        inodes[i].parent = -1;
        inodes[i].size = 0;
        inodes[i].blocks.clear();
        inodes[i].readOnly = false;
        inodes[i].password = "";
    }
    inodes[0].used = true;
    inodes[0].name = "/";
    inodes[0].isFolder = true;
    currentFolder = 0;
}
string toLowerCase(string s){
    for(int k = 0; k < (int)s.size(); k++){
        s[k] = tolower((unsigned char)s[k]);
    }
    return s;
}
int findIn(int folder, string name){
    for(int i = 1; i < MAX_INODES; i++){
        if(inodes[i].used && inodes[i].parent == folder &&
            toLowerCase(inodes[i].name) == toLowerCase(name)){
            return i;
        }
    }
    return -1;
}
int findInCurrent(string name){
    return findIn(currentFolder, name);
}
int getFreeInode(){
    for(int i = 1; i < MAX_INODES; i++){
        if(inodes[i].used == false) return i;
    }
    return -1;
}
string pathOf(int i){
    if(i == 0) return "/";
    string path = "";
    while(i != 0){
        path = "/" + inodes[i].name + path;
        i = inodes[i].parent;
    }
    return path;
}
bool isValidName(string name){
    if (name == "" || name == "." || name == "..") return false;
    if (name.find('/') != string::npos) return false;
    return true;
}
void createItem(string name, bool isFolder){
    if(isValidName(name) == false){
        cout << "Error: invalid name\n";
        return;
    }
    if(findInCurrent(name) != -1){
        cout << "Error: " << name << " already exists\n";
        return;
    }
    int i = getFreeInode();
    if(i == -1){
        cout << "Error: no free inodes\n";
        return;
    }
    inodes[i].used = true;
    inodes[i].name = name;
    inodes[i].isFolder = isFolder;
    inodes[i].parent = currentFolder;
    if(isFolder){
        realMakeFolder(pathOf(i));
        cout << "Folder created: " << name << " (inode " << i << ")\n";
    } else {
        realWriteFile(pathOf(i), "");
        cout << "File created: " << name << " (inode " << i << ")\n";
    }
}
void cmdMkdir(string name){
    createItem(name, true);
}
void cmdTouch(string name){
    createItem(name, false);
}
void cmdCd(string name, string password){
    if(name == "/"){                      
        currentFolder = 0;
        return;
    }
    if(name == ".."){                     
        if(currentFolder == 0) cout << "Error: already at root\n";
        else currentFolder = inodes[currentFolder].parent;
        return;
    }
    int i = findInCurrent(name);
    if(i == -1){
        cout << "Error: " << name << " not found\n";
        return;
    }
    if(inodes[i].isFolder == false){
        cout << "Error: " << name << " is a file\n";
        return;
    }
    if(inodes[i].password != "" && inodes[i].password != password){
        cout << "Error: " << name << " is locked. Use: cd " << name << " <password>\n";
        return;
    }
    currentFolder = i;
}
void cmdMv(string name, string dest){
    int i = findInCurrent(name);
    if(i == -1){
        cout << "Error: " << name << " not found\n";
        return;
    }
    if(inodes[i].readOnly){
        cout << "Error: " << name << " is read-only. Use: chmod " << name << " rw\n";
        return;
    }
    string oldPath = pathOf(i);
    int target = findInCurrent(dest);
    if(target != -1 && target != i && inodes[target].isFolder){
        if(inodes[target].password != ""){
            cout << "Error: folder " << dest << " is locked\n";
            return;
        }
        if(findIn(target, inodes[i].name) != -1){
            cout << "Error: " << dest << " already has " << inodes[i].name << "\n";
            return;
        }
        inodes[i].parent = target;           
        realRename(oldPath, pathOf(i));
        cout << "Moved " << name << " into " << dest << "\n";
        return;
    }
    if(target != -1 && target != i){
        cout << "Error: " << dest << " already exists\n";
        return;
    }
    if(isValidName(dest) == false){
        cout << "Error: invalid name\n";
        return;
    }
    inodes[i].name = dest;                   
    realRename(oldPath, pathOf(i));
    cout << "Renamed " << name << " to " << dest << "\n";
}
void cmdChmod(string name, string mode){
    int i = findInCurrent(name);
    if(i == -1){
        cout << "Error: " << name << " not found\n";
        return;
    }
    if(mode == "ro"){
        inodes[i].readOnly = true;
        cout << name << " is now read-only\n";
    } 
    else if(mode == "rw"){
        inodes[i].readOnly = false;
        cout << name << " is now read-write\n";
    }
    else{
        cout << "Error: mode must be ro or rw\n";
    }
}