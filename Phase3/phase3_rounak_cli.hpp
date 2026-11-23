#pragma once
#include "phase2_rounak_cli.hpp"
#include "phase3_tanish_folder_explorer.hpp"
#include "phase3_tushar_journal.hpp"
void cmdHealth(){
    int freeInodes = 0;
    for(int i = 0; i < MAX_INODES; i++){
        if(inodes[i].used == false) freeInodes++;
    }
    cout << "Disk : " << totalBlocks << " blocks open (max " << MAX_BLOCKS << "), "
         << countFreeBlocks() << " free\n";
    cout << "Inodes : " << freeInodes << " / " << MAX_INODES << " free\n";
    cache.show();
    cout << "Journal : " << journal.size() << " entries"
         << (journal.isCrashed() ? ", CRASH waiting for restore" : ", fine") << "\n";
}
class HealthCommand : public NoArgCommand{
public:
    HealthCommand() : NoArgCommand("health", "health", "disk, cache and journal summary"){}
    void run() {cmdHealth();}
};
class LogbookCommand : public NoArgCommand{
public:
    LogbookCommand() : NoArgCommand("logbook", "logbook", "show the journal"){}
    void run() {journal.show();}
};
class PowercutCommand : public NamedCommand{
public:
    PowercutCommand() : NamedCommand("powercut", "powercut <operation>", "simulate a crash, e.g. powercut store a.txt"){}
    void run(Input in){
        string op = in.name;
        if(in.text != "") op += " " + in.text;
        journal.crash(op);
    }
};
class RestoreCommand : public NoArgCommand{
public:
    RestoreCommand() : NoArgCommand("restore", "restore", "recover after a crash"){}
    void run() {journal.recover();}
};
class LayoutCommand : public NoArgCommand{
public:
    LayoutCommand() : NoArgCommand("layout", "layout", "show all folders and files as a tree"){}
    void run() {explorer.showLayout(currentFolder);}
};
class SeekCommand : public NamedCommand{
public:
    SeekCommand() : NamedCommand("seek", "seek <name>", "find a name in every folder"){}
    void run(Input in) {explorer.seek(in.name);}
};
void hideRealContents(int folder){
    for(int i = 1; i < MAX_INODES; i++){
        if(inodes[i].used && inodes[i].parent == folder){
            if(inodes[i].isFolder){
                hideRealContents(i);
                realRemoveFolder(pathOf(i));
            }
            else{
                realRemoveFile(pathOf(i));
            }
        }
    }
}
void syncRealFolder(int folder){
    for(int i = 1; i < MAX_INODES; i++){
        if(inodes[i].used && inodes[i].parent == folder){
            if(inodes[i].isFolder){
                realMakeFolder(pathOf(i));
                if(inodes[i].password != "") hideRealContents(i);
                else syncRealFolder(i);
            }
            else{
                realWriteFile(pathOf(i), loadText(inodes[i].blocks, inodes[i].size));
            }
        }
    }
}
class Phase3Shell : public Shell{
private:
    bool changesData(string cmd){
        string changing[10] = {"makedir", "newfile", "shift", "setmode", "store",
                               "addto", "clone", "erase", "seal", "unseal"};
        for(int k = 0; k < 10; k++){
            if(changing[k] == cmd) return true;
        }
        return false;
    }
public:
    Phase3Shell() : Shell(){
        add(new HealthCommand());
        add(new LogbookCommand());
        add(new PowercutCommand());
        add(new RestoreCommand());
        add(new LayoutCommand());
        add(new SeekCommand());
    }
    void runCommand(Command* command, Input in){
        if(changesData(command->getName()) == false){
            command->execute(in);
            return;
        }
        if(journal.isCrashed()){
            cout << "Error: last operation crashed (" << journal.getPending()
                 << "). Type restore first\n";
            return;
        }
        string op = command->getName();
        if(in.name != "") op += " " + in.name;
        journal.begin(op);
        command->execute(in);
        journal.commit();
        syncRealFolder(0);
    }
    void runLine(string line){
        Input in = parse(line);
        if(in.cmd == "") return;
        if(lookup.count(in.cmd) == 0){
            cout << "Unknown command. Type guide\n";
        } else {
            runCommand(lookup[in.cmd], in);
        }
        rebuildIndex();
        saveFileSystem();
    }
};
void runPhase3Shell(){
    if(journal.isCrashed()){
        cout << "!! The last session crashed during: " << journal.getPending() << "\n";
        cout << "!! Type restore before making changes.\n";
    }
    syncRealFolder(0);
    Phase3Shell shell;
    shell.run();
}