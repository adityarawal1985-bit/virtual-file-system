#pragma once
#include <fstream>
#include <iostream>
#include <string>
using namespace std;
class Journal{
private:
    static const int MAX_ENTRIES = 100;
    string entries[MAX_ENTRIES];
    int count;
    bool crashed;
    string pendingOp;
    string fileName;
    void addEntry(string entry){
        if(count == MAX_ENTRIES){
            for(int k = 1; k < MAX_ENTRIES; k++){
                entries[k - 1] = entries[k];
            }
            count--;
        }
        entries[count] = entry;
        count++;
        save();
    }
public:
    Journal(){
        count = 0;
        crashed = false;
        pendingOp = "";
        fileName = "vfs_journal.txt";
    }
    void setFile(string name){
        fileName = name;
    }
    void clearAll(){
        count = 0;
        crashed = false;
        pendingOp = "";
        save();
    }
    void save(){
        ofstream file(fileName.c_str());
        file << (crashed ? 1 : 0) << "\n";
        file << (pendingOp == "" ? "-" : pendingOp) << "\n";
        file << count << "\n";
        for (int k = 0; k < count; k++) {
            file << entries[k] << "\n";
        }
    }
    void load(){
        ifstream file(fileName.c_str());
        if(!file) return;
        int crashedFlag = 0, n = 0;
        string line;
        file >> crashedFlag;
        getline(file, line);
        getline(file, pendingOp);
        if(pendingOp == "-") pendingOp = "";
        file >> n;
        getline(file, line);
        count = 0;
        for(int k = 0; k < n && k < MAX_ENTRIES; k++){
            if(!getline(file, line)) break;
            entries[count] = line;
            count++;
        }
        crashed = (crashedFlag == 1);
    }
    void begin(string op){
        pendingOp = op;
        addEntry("BEGIN " + op);
    }
    void commit(){
        addEntry("COMMIT " + pendingOp);
        pendingOp = "";
        save();
    }
    void crash(string op){
        if(crashed){
            cout << "Error: already crashed. Type restore first\n";
            return;
        }
        crashed = true;
        begin(op);
        cout << "!! CRASH while doing: " << op << "\n";
        cout << "!! Type logbook to see it, and restore to fix it.\n";
    }
    void recover(){
        if(crashed == false){
            cout << "Nothing to restore. The journal is fine.\n";
            return;
        }
        crashed = false;
        string op = pendingOp;
        pendingOp = "";
        addEntry("ROLLBACK " + op);
        cout << "Rolled back: " << op << "\n";
    }
    bool isCrashed(){
        return crashed;
    }
    string getPending(){
        return pendingOp;
    }
    int size(){
        return count;
    }
    string entry(int k){
        return entries[k];
    }
    void show(){
        if(count == 0){
            cout << "(journal is empty)\n";
            return;
        }
        for(int k = 0; k < count; k++){
            cout << "  " << k + 1 << ". " << entries[k] << "\n";
        }
        if(crashed) cout << "  ^ BEGIN without COMMIT = crash! Type restore\n";
    }
};
Journal journal;