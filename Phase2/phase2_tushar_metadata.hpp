#pragma once
#include "phase2_aditya_block_manager.hpp"
#include <unordered_map>
void cmdLs(){
    bool empty = true;
    for(int i = 1; i < MAX_INODES; i++){
        if(inodes[i].used && inodes[i].parent == currentFolder){
            empty = false;
            if(inodes[i].isFolder){
                cout << "[FOLDER] " << inodes[i].name << "   (inode " << i << ")";
            } 
            else{
                cout << "[FILE]   " << inodes[i].name << "   (inode " << i << ", "
                     << inodes[i].size << " bytes, blocks:";
                for(int k = 0; k < (int)inodes[i].blocks.size(); k++){
                    cout << " " << inodes[i].blocks[k];
                }
                cout << ")";
            }
            if(inodes[i].readOnly)       cout << "  [read-only]";
            if(inodes[i].password != "") cout << "  [locked]";
            cout << "\n";
        }
    }
    if(empty) cout << "(empty)\n";
}
bool isFolderEmpty(int folder){
    for(int i = 1; i < MAX_INODES; i++){
        if(inodes[i].used && inodes[i].parent == folder) return false;
    }
    return true;
}
void cmdRm(string name, string password){
    int i = findInCurrent(name);
    if(i == -1){
        cout << "Error: " << name << " not found\n";
        return;
    }
    if(inodes[i].readOnly){
        cout << "Error: " << name << " is read-only. Use: chmod " << name << " rw\n";
        return;
    }
    if(inodes[i].password != "" && inodes[i].password != password){
        cout << "Error: " << name << " is locked. Use: rm " << name << " <password>\n";
        return;
    }
    if(inodes[i].isFolder && isFolderEmpty(i) == false){
        cout << "Error: folder " << name << " is not empty. Delete what is inside first\n";
        return;
    }
    if(inodes[i].isFolder) realRemoveFolder(pathOf(i));
    else realRemoveFile(pathOf(i));
    freeBlocks(inodes[i].blocks);           
    inodes[i].used = false;                 
    inodes[i].name = "";
    inodes[i].size = 0;
    inodes[i].blocks.clear();
    inodes[i].readOnly = false;
    inodes[i].password = "";
    cout << "Removed " << name << "\n";
}
void cmdStat(string name){
    int i = findInCurrent(name);
    if(i == -1){
        cout << "Error: " << name << " not found\n";
        return;
    }
    cout << "  inode : " << i << "\n";
    cout << "  name : " << inodes[i].name << "\n";
    cout << "  path : " << pathOf(i) << "\n";
    cout << "  type : " << (inodes[i].isFolder ? "folder" : "file") << "\n";
    cout << "  parent : inode " << inodes[i].parent << "\n";
    cout << "  size : " << inodes[i].size << " bytes\n";
    cout << "  blocks :";
    for(int k = 0; k < (int)inodes[i].blocks.size(); k++){
        cout << " " << inodes[i].blocks[k];
    }
    cout << "\n";
    cout << "  read-only : " << (inodes[i].readOnly ? "yes" : "no") << "\n";
    cout << "  locked    : " << (inodes[i].password != "" ? "yes" : "no") << "\n";
}
unordered_map<string, int> pathIndex;   
void rebuildIndex(){
    pathIndex.clear();
    for(int i = 0; i < MAX_INODES; i++){
        if(inodes[i].used) pathIndex[toLowerCase(pathOf(i))] = i;
    }
}
void cmdFind(string path){
    if(path[0] != '/'){
        cout << "Error: give the full path, for example: find /docs/a.txt\n";
        return;
    }
    string key = toLowerCase(path);
    if(pathIndex.count(key) == 0){
        cout << "Not found: " << path << "\n";
        return;
    }
    int i = pathIndex[key];
    cout << "Found " << pathOf(i) << " -> inode " << i
         << (inodes[i].isFolder ? " (folder)" : " (file)") << "\n";
}