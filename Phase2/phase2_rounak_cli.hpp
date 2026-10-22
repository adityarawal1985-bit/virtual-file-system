
#pragma once
#include "phase2_tushar_metadata.hpp"
#include <map>
#include <sstream>
#include <vector>
void cmdPwd(){
    cout << pathOf(currentFolder) << "\n";
}
void cmdLock(string name, string password){
    int i = findInCurrent(name);
    if(i == -1){
        cout << "Error: " << name << " not found\n";
        return;
    }
    if(inodes[i].isFolder == false){
        cout << "Error: only folders can be locked\n";
        return;
    }
    if(inodes[i].password != ""){
        cout << "Error: " << name << " is already locked\n";
        return;
    }
    if(password == "-"){                   
        cout << "Error: choose another password\n";
        return;
    }
    inodes[i].password = password;
    cout << "Folder " << name << " is now locked\n";
}
void cmdUnlock(string name, string password){
    int i = findInCurrent(name);
    if(i == -1){
        cout << "Error: " << name << " not found\n";
        return;
    }
    if(inodes[i].password == ""){
        cout << "Error: " << name << " is not locked\n";
        return;
    }
    if(inodes[i].password != password){
        cout << "Error: wrong password\n";
        return;
    }
    inodes[i].password = "";
    cout << "Folder " << name << " is now unlocked\n";
}
struct Input{
    string cmd;
    string name;
    string text;     
    string arg2;     
};
class Command{
protected:
    string name;          
    string usage;        
    string description;           
public:
    Command(string n, string u, string d){
        name = n;
        usage = u;
        description = d;
    }
    virtual ~Command(){}
    string getName(){
        return name;
    }
    virtual bool endsShell(){
        return false;
    }
    void printHelpLine(){
        string left = usage;
        while((int)left.size() < 27) left += ' ';
        string middle = description;
        while((int)middle.size() < 36) middle += ' ';
        cout << left << description << "\n";
    }
    virtual void execute(Input in) = 0;
};
class NoArgCommand : public Command{
public:
    NoArgCommand(string n, string u, string d) : Command(n, u, d ){}

    void execute(Input in){
        (void)in;                           
        run();
    }
    virtual void run() = 0;
};
class NamedCommand : public Command{
public:
    NamedCommand(string n, string u, string d ) : Command(n, u, d){}
    void execute(Input in){
        if(in.name == ""){
            cout << "Error: missing name. Type help\n";
            return;
        }
        run(in);
    }
    virtual void run(Input in) = 0;
};
class TextCommand : public NamedCommand{
public:
    TextCommand(string n, string u, string d ) : NamedCommand(n, u, d ){}
    void run(Input in) {
        if(in.text == ""){
            cout << "Error: use  " << name << " <file> <text>\n";
            return;
        }
        runWithText(in.name, in.text);
    }
    virtual void runWithText(string file, string text) = 0;
};
class TwoNameCommand : public NamedCommand{
public:
    TwoNameCommand(string n, string u, string d) : NamedCommand(n, u, d ){}
    void run(Input in){
        if(in.arg2 == ""){
            cout << "Error: " << name << " needs two names. Type help\n";
            return;
        }
        runWithTwo(in.name, in.arg2);
    }
    virtual void runWithTwo(string first, string second) = 0;
};
class MkdirCommand : public NamedCommand{
public:
    MkdirCommand() : NamedCommand("mkdir", "mkdir <name>", "create a folder"){}
    void run(Input in) {cmdMkdir(in.name);}
};
class TouchCommand : public NamedCommand{
public:
    TouchCommand() : NamedCommand("touch", "touch <name>", "create a file"){}
    void run(Input in) {cmdTouch(in.name);}
};

class CdCommand : public NamedCommand{
public:
    CdCommand() : NamedCommand("cd", "cd <name> [password]", "go into a folder, cd .. = back"){}
    void run(Input in) {cmdCd(in.name, in.arg2);}
};
class MvCommand : public TwoNameCommand{
public:
    MvCommand() : TwoNameCommand("mv", "mv <name> <new name>", "rename, or move into a folder"){}
    void runWithTwo(string first, string second) {cmdMv(first, second);}
};
class ChmodCommand : public TwoNameCommand{
public:
    ChmodCommand() : TwoNameCommand("chmod", "chmod <name> <ro|rw>", "read-only / read-write"){}
    void runWithTwo(string first, string second) {cmdChmod(first, second);}
};
class WriteCommand : public TextCommand{
public:
    WriteCommand() : TextCommand("write", "write <file> <text>", "save text in a file"){}
    void runWithText(string file, string text) {cmdWrite(file, text);}
};
class AppendCommand : public TextCommand{
public:
    AppendCommand() : TextCommand("append", "append <file> <text>", "add text at the end of a file"){}
    void runWithText(string file, string text) {cmdAppend(file, text);}
};
class CatCommand : public NamedCommand{
public:
    CatCommand() : NamedCommand("cat", "cat <file>", "show a file"){}
    void run(Input in) {cmdCat(in.name);}
};
class CpCommand : public TwoNameCommand{
public:
    CpCommand() : TwoNameCommand("cp", "cp <file> <new file>", "copy a file"){}
    void runWithTwo(string first, string second) {cmdCp(first, second);}
};
class DfCommand : public NoArgCommand{
public:
    DfCommand() : NoArgCommand("df", "df", "show free space (bitmap)"){}
    void run() {cmdDf();}
};
class LsCommand : public NoArgCommand{
public:
    LsCommand() : NoArgCommand("ls", "ls", "show what is in this folder"){}
    void run() {cmdLs();}
};
class RmCommand : public NamedCommand{
public:
    RmCommand() : NamedCommand("rm", "rm <name> [password]", "delete a file or empty folder"){}
    void run(Input in) {cmdRm(in.name, in.arg2);}
};
class StatCommand : public NamedCommand{
public:
    StatCommand() : NamedCommand("stat", "stat <name>", "show all details of one inode"){}
    void run(Input in) {cmdStat(in.name);}
};
class FindCommand : public NamedCommand{
public:
    FindCommand() : NamedCommand("find", "find <full path>", "hash index search, e.g. find /a.txt"){}
    void run(Input in) {cmdFind(in.name);}
};
class PwdCommand : public NoArgCommand{
public:
    PwdCommand() : NoArgCommand("pwd", "pwd", "show where you are"){}
    void run() {cmdPwd();}
};
class LockCommand : public TwoNameCommand{
public:
    LockCommand() : TwoNameCommand("lock", "lock <folder> <password>", "lock a folder"){}
    void runWithTwo(string first, string second) {cmdLock(first, second);}
};
class UnlockCommand : public TwoNameCommand{
public:
    UnlockCommand() : TwoNameCommand("unlock", "unlock <folder> <password>", "unlock a folder"){}
    void runWithTwo(string first, string second){ cmdUnlock(first, second);}
};
class HelpCommand : public NoArgCommand{
private:
    vector<Command*>& allCommands;
public:
    HelpCommand(vector<Command*>& list)
        : NoArgCommand("help", "help", "show this list"), allCommands(list){}
    void run(){
        for(int k = 0; k < (int)allCommands.size(); k++){
            allCommands[k]->printHelpLine();         
        }
    }
};
class ExitCommand : public NoArgCommand{
public:
    ExitCommand() : NoArgCommand("exit", "exit", "quit"){}
    bool endsShell(){return true;}
    void run(){}
};
class Shell{
private:
    vector<Command*> commands;             
    map<string, Command*> lookup;           

    void add(Command* c){
        commands.push_back(c);
        lookup[c->getName()] = c;
    }
    Input parse(string line){
        Input in;
        stringstream words(line);
        words >> in.cmd >> in.name;                   
        getline(words, in.text);                     
        if(in.text != "" && in.text[0] == ' ') in.text = in.text.substr(1);   
        stringstream rest(in.text);
        rest >> in.arg2;                              
        return in;
    }

public:
    Shell(){
        add(new MkdirCommand());
        add(new TouchCommand());
        add(new CdCommand());
        add(new MvCommand());
        add(new ChmodCommand());
        add(new WriteCommand());
        add(new AppendCommand());
        add(new CatCommand());
        add(new CpCommand());
        add(new DfCommand());
        add(new LsCommand());
        add(new RmCommand());
        add(new StatCommand());
        add(new FindCommand());
        add(new PwdCommand());
        add(new LockCommand());
        add(new UnlockCommand());
        add(new HelpCommand(commands));
        add(new ExitCommand());
    }
    ~Shell(){
        for(int k = 0; k < (int)commands.size(); k++){
            delete commands[k];
        }
    }
    Shell(const Shell&) = delete;
    Shell& operator=(const Shell&) = delete;
    void run(){
        cout << "===== Rootrix Virtual File System =====\n";
        rebuildIndex();
        while(true){
            cout << "\nvfs:" << pathOf(currentFolder) << "> ";
            string line;
            if(!getline(cin, line)) break;          
            Input in = parse(line);
            if(in.cmd == "") continue;
            if(lookup.count(in.cmd) == 0){
                cout << "Unknown command. Type help\n";
            } 
            else{
                Command* command = lookup[in.cmd];
                if(command->endsShell()) break;      
                command->execute(in);                 
            }
            rebuildIndex();                          
            saveFileSystem();                         
        }
    }
};
void runShell(){
    Shell shell;
    shell.run();
}