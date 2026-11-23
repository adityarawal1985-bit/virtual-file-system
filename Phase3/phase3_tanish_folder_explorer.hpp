#pragma once
#include "phase2_tanish_directory_tree.hpp"
class FolderExplorer{
private:
    int shownCount;
    int foundCount;
    bool insideLockedFolder(int i){
        int p = inodes[i].parent;
        while(p > 0){
            if(inodes[p].password != "") return true;
            p = inodes[p].parent;
        }
        return false;
    }
    bool isLastChild(int folder, int child){
        for(int k = child + 1; k < MAX_INODES; k++){
            if(inodes[k].used && inodes[k].parent == folder) return false;
        }
        return true;
    }
    void printTree(int folder, string indent){
        for(int i = 1; i < MAX_INODES; i++){
            if(inodes[i].used && inodes[i].parent == folder){
                bool last = isLastChild(folder, i);
                cout << indent << (last ? "`-- " : "|-- ") << inodes[i].name;
                if(inodes[i].isFolder) cout << "/";
                if(inodes[i].password != "") cout << "  [locked]";
                cout << "\n";
                shownCount++;
                if(inodes[i].isFolder && inodes[i].password == ""){
                    printTree(i, indent + (last ? "    " : "|   "));
                }
            }
        }
    }
public:
    FolderExplorer(){
        shownCount = 0;
        foundCount = 0;
    }
    void showLayout(int folder){
        shownCount = 0;
        cout << pathOf(folder) << "\n";
        printTree(folder, "");
        if (shownCount == 0) cout << "(empty)\n";
    }
    void seek(string name){
        foundCount = 0;
        for(int i = 1; i < MAX_INODES; i++){
            if(inodes[i].used && toLowerCase(inodes[i].name) == toLowerCase(name)
                && insideLockedFolder(i) == false) {
                cout << pathOf(i) << (inodes[i].isFolder ? "   (folder)" : "   (file)") << "\n";
                foundCount++;
            }
        }
        if (foundCount == 0) cout << "Nothing named " << name << " found\n";
        else cout << foundCount << " found\n";
    }
    int lastShown(){
        return shownCount;
    }
    int lastFound(){
        return foundCount;
    }
};
FolderExplorer explorer;