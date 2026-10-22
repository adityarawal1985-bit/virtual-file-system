#pragma once
#include "phase2_tanish_directory_tree.hpp"
const int BLOCK_SIZE   = 32;
const int TOTAL_BLOCKS = 64;
char disk[TOTAL_BLOCKS][BLOCK_SIZE];
bool blockUsed[TOTAL_BLOCKS];
void resetDisk(){
    for(int b = 0; b < TOTAL_BLOCKS; b++){
        blockUsed[b] = false;
    }
}
int countFreeBlocks(){
    int count = 0;
    for(int b = 0; b < TOTAL_BLOCKS; b++){
        if(blockUsed[b] == false) count++;
    }
    return count;
}
int findFreeBlock(){
    for(int b = 0; b < TOTAL_BLOCKS; b++){
        if(blockUsed[b] == false) return b;
    }
    return -1;
}
int blocksNeeded(int characters){
    return(characters + BLOCK_SIZE - 1) / BLOCK_SIZE;
}
vector<int> saveText(string text){
    vector<int> blocks;
    int b = -1;
    for(int i = 0; i < (int)text.size(); i++){
        if(i % BLOCK_SIZE == 0){          
            b = findFreeBlock();
            blockUsed[b] = true;
            blocks.push_back(b);
        }
        disk[b][i % BLOCK_SIZE] = text[i];
    }
    return blocks;
}
string loadText(vector<int> blocks, int size){
    string text = "";
    for(int i = 0; i < size; i++){
        int b = blocks[i / BLOCK_SIZE];
        text += disk[b][i % BLOCK_SIZE];
    }
    return text;
}
void freeBlocks(vector<int> blocks){
    for(int k = 0; k < (int)blocks.size(); k++){
        blockUsed[blocks[k]] = false;
    }
}
bool storeText(int i, string text){
    int needed = blocksNeeded(text.size());
    int available = countFreeBlocks() + inodes[i].blocks.size();  
    if(needed > available){
        cout << "Error: disk is full, file not changed\n";
        return false;
    }
    freeBlocks(inodes[i].blocks);            
    inodes[i].blocks = saveText(text);       
    inodes[i].size = text.size();
    realWriteFile(pathOf(i), text);
    return true;
}
int getWritableFile(string name){
    int i = findInCurrent(name);
    if(i == -1){
        cout << "Error: " << name << " not found. Use touch first\n";
        return -1;
    }
    if(inodes[i].isFolder){
        cout << "Error: " << name << " is a folder\n";
        return -1;
    }
    if(inodes[i].readOnly){
        cout << "Error: " << name << " is read-only. Use: chmod " << name << " rw\n";
        return -1;
    }
    return i;
}
void cmdWrite(string name, string text){
    int i = getWritableFile(name);
    if(i == -1) return;
    if(storeText(i, text)){
        cout << "Wrote " << text.size() << " bytes to " << name << "\n";
    }
}
void cmdAppend(string name, string text){
    int i = getWritableFile(name);
    if(i == -1) return;
    string newText = loadText(inodes[i].blocks, inodes[i].size) + text;
    if(storeText(i, newText)){
        cout << "Appended " << text.size() << " bytes to " << name << "\n";
    }
}
void cmdCat(string name){
    int i = findInCurrent(name);
    if(i == -1){
        cout << "Error: " << name << " not found\n";
        return;
    }
    if(inodes[i].isFolder){
        cout << "Error: " << name << " is a folder\n";
        return;
    }
    cout << loadText(inodes[i].blocks, inodes[i].size) << "\n";
}
void cmdCp(string source, string dest){
    int s = findInCurrent(source);
    if(s == -1){
        cout << "Error: " << source << " not found\n";
        return;
    }
    if(inodes[s].isFolder){
        cout << "Error: only files can be copied\n";
        return;
    }
    if(isValidName(dest) == false){
        cout << "Error: invalid name\n";
        return;
    }
    if(findInCurrent(dest) != -1){
        cout << "Error: " << dest << " already exists\n";
        return;
    }
    if(getFreeInode() == -1){
        cout << "Error: no free inodes\n";
        return;
    }
    string text = loadText(inodes[s].blocks, inodes[s].size);
    if(blocksNeeded(text.size()) > countFreeBlocks()){
        cout << "Error: disk is full\n";
        return;
    }
    createItem(dest, false);                 
    storeText(findInCurrent(dest), text);    
    cout << "Copied " << source << " to " << dest << "\n";
}
void cmdDf(){
    cout << "Bitmap (1 = used, 0 = free):\n";
    for(int b = 0; b < TOTAL_BLOCKS; b++){
        if(b % 16 == 0) cout << "  block " << b << "\t: ";
        cout << (blockUsed[b] ? 1 : 0);
        if(b % 16 == 15 || b == TOTAL_BLOCKS - 1) cout << "\n";
    }
    int freeInodes = 0;
    for(int i = 0; i < MAX_INODES; i++){
        if(inodes[i].used == false) freeInodes++;
    }
    cout << "Free blocks : " << countFreeBlocks() << " / " << TOTAL_BLOCKS
         << "  (" << countFreeBlocks() * BLOCK_SIZE << " characters)\n";
    cout << "Free inodes : " << freeInodes << " / " << MAX_INODES << "\n";
}
string saveFileName = "vfs_save.txt";
bool useSaveFile = true;         
void saveFileSystem(){
    if(useSaveFile == false) return;
    ofstream file(saveFileName.c_str());
    int count = 0;
    for(int i = 1; i < MAX_INODES; i++){
        if(inodes[i].used) count++;
    }
    file << "VFS2\n" << count << "\n";
    for(int i = 1; i < MAX_INODES; i++){
        if(inodes[i].used == false) continue;
        string password = inodes[i].password;
        if(password == "") password = "-";
        file << i << " " << inodes[i].name << " " << inodes[i].isFolder << " "
             << inodes[i].parent << " " << inodes[i].size << " " << inodes[i].readOnly << " "
             << password << " " << inodes[i].blocks.size();
        for(int k = 0; k < (int)inodes[i].blocks.size(); k++){
            file << " " << inodes[i].blocks[k];
        }
        file << "\n" << loadText(inodes[i].blocks, inodes[i].size) << "\n";
    }
}
void loadFileSystem(){
    ifstream file(saveFileName.c_str());
    if(!file) return;                      
    string version;
    file >> version;
    if(version != "VFS2"){
        cout << "Old or damaged " << saveFileName << " ignored. Starting empty.\n";
        return;
    }
    int count = 0;
    file >> count;
    for(int k = 0; k < count; k++){
        int i, isFolder, readOnly, blockCount;
        if(!(file >> i) || i < 1 || i >= MAX_INODES) break;   
        string password;
        file >> inodes[i].name >> isFolder >> inodes[i].parent >> inodes[i].size
             >> readOnly >> password >> blockCount;
        if(inodes[i].parent < 0 || inodes[i].parent >= MAX_INODES) inodes[i].parent = 0;
        inodes[i].used = true;
        inodes[i].isFolder = (isFolder == 1);
        inodes[i].readOnly = (readOnly == 1);
        inodes[i].password = (password == "-") ? "" : password;
        inodes[i].blocks.clear();
        for(int j = 0; j < blockCount; j++){
            int b;
            file >> b;
            if(b < 0 || b >= TOTAL_BLOCKS) continue;   
            inodes[i].blocks.push_back(b);
            blockUsed[b] = true;             
        }
        string endOfLine, text;
        getline(file, endOfLine);            
        getline(file, text);                 
        int fit = inodes[i].blocks.size() * BLOCK_SIZE;
        if(inodes[i].size > fit) inodes[i].size = fit;
        if((int)text.size() < inodes[i].size) inodes[i].size = text.size();
        for(int c = 0; c < inodes[i].size; c++){
            disk[inodes[i].blocks[c / BLOCK_SIZE]][c % BLOCK_SIZE] = text[c];
        }
    }
    cout << "Loaded " << count << " files/folders from " << saveFileName << "\n";
}
void copyToRealFolder(int folder){
    for(int i = 1; i < MAX_INODES; i++) {
        if(inodes[i].used && inodes[i].parent == folder){
            if(inodes[i].isFolder){
                realMakeFolder(pathOf(i));
                copyToRealFolder(i);         
            } 
            else{
                realWriteFile(pathOf(i), loadText(inodes[i].blocks, inodes[i].size));
            }
        }
    }
}
void setupFileSystem(){
    resetDisk();
    resetInodes();
    if (useSaveFile) loadFileSystem();
    realStart();
    copyToRealFolder(0);
}