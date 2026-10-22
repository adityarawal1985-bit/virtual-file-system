
#include "phase2_rounak_cli.hpp"
int passed = 0, failed = 0;
void check(bool ok, string what){
    if(ok){ cout << "   PASS: " << what << "\n"; passed++; }
    else{ cout << "   FAIL: " << what << "\n"; failed++; }
}
void startTest(string title){
    cout << "\n===== " << title << " =====\n";
    setupFileSystem();
}

bool realFileExists(string path){
    ifstream file(path.c_str());
    return file.good();
}
string textOf(string name){
    int i = findInCurrent(name);
    if (i == -1) return "<missing>";
    return loadText(inodes[i].blocks, inodes[i].size);
}
int main(){
    realRoot = "test_root";
    useSaveFile = false;          
    startTest("TEST 1: touch");
    cmdTouch("a.txt");
    check(findInCurrent("a.txt") == 1, "a.txt got inode 1");
    check(realFileExists("test_root/a.txt"), "real file test_root/a.txt was created");
    startTest("TEST 2: mkdir, cd, pwd path");
    cmdMkdir("docs");
    cmdCd("docs", "");
    check(pathOf(currentFolder) == "/docs", "we are in /docs");
    cmdTouch("inside.txt");
    check(realFileExists("test_root/docs/inside.txt"), "real file created inside docs");
    cmdCd("..", "");
    check(currentFolder == 0, "cd .. goes back to root");
    startTest("TEST 3: write and cat");
    cmdTouch("a.txt");
    cmdWrite("a.txt", "hello world");
    check(textOf("a.txt") == "hello world", "same text comes back");
    check(inodes[findInCurrent("a.txt")].blocks.size() == 1, "11 characters use 1 block");
    startTest("TEST 4: long text uses many blocks");
    string longText = "";
    for (int k = 0; k < 2 * BLOCK_SIZE + 6; k++) longText += (char)('a' + k % 26);
    cmdTouch("big.txt");
    cmdWrite("big.txt", longText);
    check(inodes[findInCurrent("big.txt")].blocks.size() == 3, "2 full blocks + 6 characters use 3 blocks");
    check(textOf("big.txt") == longText, "all 3 blocks read back in order");
    startTest("TEST 5: append");
    cmdTouch("log.txt");
    cmdWrite("log.txt", "line1 ");
    cmdAppend("log.txt", "line2");
    check(textOf("log.txt") == "line1 line2", "text added at the end");
    startTest("TEST 6: cp makes an independent copy");
    cmdTouch("a.txt");
    cmdWrite("a.txt", "original");
    cmdCp("a.txt", "b.txt");
    cmdWrite("a.txt", "changed");
    check(textOf("b.txt") == "original", "copy did not change");
    check(inodes[findInCurrent("a.txt")].blocks[0] != inodes[findInCurrent("b.txt")].blocks[0],
          "copy has its own block");
    startTest("TEST 7: mv rename and move into folder");
    cmdTouch("old.txt");
    int ino = findInCurrent("old.txt");
    cmdMv("old.txt", "new.txt");
    check(findInCurrent("new.txt") == ino, "rename keeps the same inode");
    check(realFileExists("test_root/new.txt"), "real file renamed too");
    cmdMkdir("box");
    cmdMv("new.txt", "box");
    check(inodes[ino].parent == findInCurrent("box"), "move changes the parent number");
    check(realFileExists("test_root/box/new.txt"), "real file moved too");
    startTest("TEST 8: rm gives back block and inode");
    cmdTouch("a.txt");
    cmdWrite("a.txt", "bye");
    cmdRm("a.txt", "");
    check(countFreeBlocks() == TOTAL_BLOCKS, "block is free again");
    check(getFreeInode() == 1, "inode 1 is free again");
    check(realFileExists("test_root/a.txt") == false, "real file deleted");
    startTest("TEST 9: rm refuses a folder that is not empty");
    cmdMkdir("docs");
    cmdCd("docs", "");
    cmdTouch("x.txt");
    cmdCd("..", "");
    cmdRm("docs", "");
    check(findInCurrent("docs") != -1, "docs still exists");
    startTest("TEST 10: chmod read-only");
    cmdTouch("cfg.txt");
    cmdWrite("cfg.txt", "original");
    cmdChmod("cfg.txt", "ro");
    cmdWrite("cfg.txt", "hacked");
    cmdAppend("cfg.txt", "more");
    cmdRm("cfg.txt", "");
    cmdMv("cfg.txt", "other.txt");
    check(textOf("cfg.txt") == "original", "write, append, rm and mv were all refused");
    cmdChmod("cfg.txt", "rw");
    cmdRm("cfg.txt", "");
    check(findInCurrent("cfg.txt") == -1, "rm works after chmod rw");
    startTest("TEST 11: lock and unlock");
    cmdMkdir("secret");
    cmdLock("secret", "pass123");
    cmdCd("secret", "");
    check(currentFolder == 0, "cd without password refused");
    cmdCd("secret", "wrong");
    check(currentFolder == 0, "cd with wrong password refused");
    cmdCd("secret", "pass123");
    check(pathOf(currentFolder) == "/secret", "cd with correct password works");
    cmdCd("..", "");
    cmdRm("secret", "");
    check(findInCurrent("secret") != -1, "rm without password refused");
    cmdUnlock("secret", "pass123");
    cmdCd("secret", "");
    check(pathOf(currentFolder) == "/secret", "after unlock no password is needed");
    startTest("TEST 12: find uses the hash index");
    cmdMkdir("docs");
    cmdCd("docs", "");
    cmdTouch("a.txt");
    cmdCd("/", "");
    rebuildIndex();
    check(pathIndex.count("/docs/a.txt") == 1, "/docs/a.txt is in the hash index");
    check(pathIndex["/docs/a.txt"] == 2, "it points to inode 2");
    cmdMv("docs", "papers");
    rebuildIndex();
    check(pathIndex.count("/docs/a.txt") == 0 && pathIndex.count("/papers/a.txt") == 1,
          "after renaming the folder, the path of the file is updated");
    startTest("TEST 13: Rahul and rahul are the same name");
    cmdMkdir("Rahul");
    cmdMkdir("rahul");
    check(getFreeInode() == 2, "second mkdir was refused");
    cmdCd("RAHUL", "");
    check(pathOf(currentFolder) == "/Rahul", "cd RAHUL enters Rahul, name shown as typed");
    startTest("TEST 14: wrong names are refused");
    cmdMkdir("a/b");
    cmdTouch("..");
    check(getFreeInode() == 1, "nothing was created");
    startTest("TEST 15: disk full keeps old text");
    cmdTouch("a.txt");
    cmdWrite("a.txt", "original");
    cmdWrite("a.txt", string(TOTAL_BLOCKS * BLOCK_SIZE + 1, 'z'));
    check(textOf("a.txt") == "original", "old text still there");
    startTest("TEST 16: everything comes back after a restart");
    useSaveFile = true;
    saveFileName = "test_save.txt";
    remove(saveFileName.c_str());
    setupFileSystem();
    cmdMkdir("docs");
    cmdLock("docs", "pw");
    cmdCd("docs", "pw");
    cmdTouch("a.txt");
    cmdWrite("a.txt", "hello again");
    cmdChmod("a.txt", "ro");
    saveFileSystem();
    setupFileSystem();                         
    int docs = findInCurrent("docs");
    check(docs != -1 && inodes[docs].password == "pw", "locked folder came back with its password");
    cmdCd("docs", "pw");
    int a = findInCurrent("a.txt");
    check(a != -1 && textOf("a.txt") == "hello again", "file and its text came back");
    check(a != -1 && inodes[a].readOnly, "read-only came back");
    check(countFreeBlocks() == TOTAL_BLOCKS - 1, "its block is marked used again");
    check(realFileExists("test_root/docs/a.txt"), "real file is rebuilt in test_root");
    remove(saveFileName.c_str());
    useSaveFile = false;
    realDeleteAll();
    cout << "\nPASSED: " << passed << "   FAILED: " << failed << "\n";
    return 0;
}