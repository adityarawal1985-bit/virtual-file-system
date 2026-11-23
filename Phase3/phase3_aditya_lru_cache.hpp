#pragma once
#include <iostream>
#include <string>
using namespace std;
class LRUCache{
private:
    static const int SLOTS = 4;
    int blockNo[SLOTS];
    string data[SLOTS];
    int lastUsed[SLOTS];
    int clock;
    int hits;
    int misses;
    int findSlot(int block) {
        for(int s = 0; s < SLOTS; s++){
            if (blockNo[s] == block) return s;
        }
        return -1;
    }
public:
    LRUCache(){
        reset();
    }
    void reset(){
        for(int s = 0; s < SLOTS; s++){
            blockNo[s] = -1;
            data[s] = "";
            lastUsed[s] = 0;
        }
        clock = 0;
        hits = 0;
        misses = 0;
    }
    bool get(int block, string& out){
        int s = findSlot(block);
        if(s == -1){
            misses++;
            return false;
        }
        hits++;
        clock++;
        lastUsed[s] = clock;
        out = data[s];
        return true;
    }
    void put(int block, string blockData){
        int s = findSlot(block);
        if(s == -1){
            for(int k = 0; k < SLOTS; k++){
                if(blockNo[k] == -1){
                    s = k;
                    break;
                }
            }
        }
        if(s == -1){
            s = 0;
            for(int k = 1; k < SLOTS; k++){
                if(lastUsed[k] < lastUsed[s]) s = k;
            }
        }
        clock++;
        blockNo[s] = block;
        data[s] = blockData;
        lastUsed[s] = clock;
    }
    void remove(int block){
        int s = findSlot(block);
        if(s != -1) {
            blockNo[s] = -1;
            data[s] = "";
            lastUsed[s] = 0;
        }
    }
    int getHits(){
        return hits;
    }
    int getMisses(){
        return misses;
    }
    int usedSlots(){
        int count = 0;
        for(int s = 0; s < SLOTS; s++){
            if(blockNo[s] != -1) count++;
        }
        return count;
    }
    void show(){
        cout << "Cache : " << usedSlots() << " / " << SLOTS << " slots used, "
             << hits << " hits, " << misses << " misses\n";
        for(int s = 0; s < SLOTS; s++){
            cout << "  slot " << s << " : ";
            if(blockNo[s] == -1) cout << "empty\n";
            else cout << "block " << blockNo[s] << " (lastUsed " << lastUsed[s] << ")\n";
        }
    }
};