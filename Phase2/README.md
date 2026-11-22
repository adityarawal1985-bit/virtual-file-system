# Rootrix VFS – Phase 2

This folder has our **Phase 2** version. This is where we built the main part of our file system. It has **19 commands**, and we kept this folder exactly as we showed it in Phase 2. (The final version with 25 commands is in the `phase3` folder.)

In Phase 2 our VFS can:
- make folders and files, go inside folders, rename, move, copy and delete
- save text in files (stored in blocks on our fake disk)
- find a file by its full path using a hash index
- make files read-only and lock folders with a password
- remember everything after closing the program
- show all files in a real folder called `vfs_root`

## How to run

```
g++ -std=c++11 phase2_main_integration.cpp -o vfs
vfs             (Windows: vfs.exe)
```

Type `guide` to see all the commands.

Tests:
```
g++ -std=c++11 phase2_tushar_tests.cpp -o tests
tests           → PASSED: 39   FAILED: 0
```

Only compile the `.cpp` file, the `.hpp` files get included automatically. To start fresh, delete `vfs_save.txt`.

## The 19 commands

Our mentor asked us not to use normal Linux names like `mkdir` or `cd`, so we gave every command its own name.

| Command | What it does | Made by |
|---|---|---|
| `makedir <name>` | make a folder | Tanish |
| `newfile <name>` | make a file | Tanish |
| `goto <name> [password]` | go inside a folder (`goto ..` back, `goto /` root) | Tanish |
| `shift <name> <new>` | rename, or move into a folder | Tanish |
| `setmode <name> ro/rw` | make read-only or normal again | Tanish |
| `store <file> <text>` | write text in a file | Aditya |
| `addto <file> <text>` | add text at the end | Aditya |
| `readfile <file>` | show what's in a file | Aditya |
| `clone <file> <new>` | copy a file | Aditya |
| `diskmap` | show which blocks are used | Aditya |
| `items` | list what's in this folder | Tushar |
| `erase <name> [password]` | delete a file or an empty folder | Tushar |
| `inode <name>` | show all details of a file or folder | Tushar |
| `pathfind <full path>` | find a file by its full path | Tushar |
| `here` | show where you are | Rounak |
| `seal <folder> <password>` | lock a folder | Rounak |
| `unseal <folder> <password>` | unlock a folder | Rounak |
| `guide` | show all commands | Rounak |
| `bye` | exit | Rounak |

Example:
```
vfs:/> makedir docs
Folder created: docs (inode 1)
vfs:/> goto docs
vfs:/docs> newfile a.txt
File created: a.txt (inode 2)
vfs:/docs> store a.txt hello world
Wrote 11 bytes to a.txt
vfs:/docs> items
[FILE]   a.txt   (inode 2, 11 bytes, blocks: 0)
```

## How it works

**The disk:** it's just a 2D array, `disk[64][32]`. That means 64 blocks, and each block holds 32 characters. So the whole disk can hold 2048 characters.

**Blocks:** when you save text, character number *i* goes into block *i/32* at position *i%32*. So "hello world" (11 characters) fits in 1 block, but a 45-character text needs 2 blocks.

**Bitmap:** `blockUsed[64]` is a true/false array. `true` means the block is used, `false` means it's free. `diskmap` shows it as 1s and 0s.

**Inodes:** every file and folder gets an inode. It's a small record with the name, whether it's a folder, its parent folder, its size, its blocks, read-only, and password. We keep all inodes in one array, `inodes[32]`. The inode number is just the position in the array, and the root folder `/` is always inode 0.

**Folder tree:** we didn't use pointers. Each inode just stores its parent's number. To see what's inside a folder, we look for all inodes with that folder as parent. To get the full path, we keep going up through the parents.

**Hash index:** an `unordered_map` that maps a full path like `/docs/a.txt` to its inode number. `pathfind` uses it to find a file in one step.

**Saving:** after every command, everything gets written to `vfs_save.txt` (format `VFS2`). When the program starts, it reads this file back.

**Real folder:** every file is also copied to `vfs_root` on your computer, so you can see it in File Explorer. The real data is still in our fake disk though.

**One rule we followed everywhere:** check first, change later. If anything is wrong (no such file, read-only, locked, disk full), the command stops and nothing changes.

## Files in this folder

| File | Made by | What's in it |
|---|---|---|
| `phase2_rounak_real_folder.hpp` | Rounak | making real files in `vfs_root` |
| `phase2_tanish_directory_tree.hpp` | Tanish | inode table, folders, `makedir`, `goto`, `shift`, `setmode` |
| `phase2_aditya_block_manager.hpp` | Aditya | disk, bitmap, `store`, `readfile`, `clone`, `diskmap`, saving and loading |
| `phase2_tushar_metadata.hpp` | Tushar | hash index, `items`, `erase`, `inode`, `pathfind` |
| `phase2_rounak_cli.hpp` | Rounak | reads commands, `Command` classes and `Shell` class |
| `phase2_main_integration.cpp` | Tanish | the main program |
| `phase2_tushar_tests.cpp` | Tushar | our tests |

The files include each other in this order:
```
real_folder → directory_tree → block_manager → metadata → cli → main
```

## OOP in Phase 2

Rounak's CLI is made with classes:
- `Command` is an abstract class with a pure virtual `execute()`
- `NamedCommand`, `TextCommand`, `TwoNameCommand` do the common checks (like "did you give a name?"), and every command inherits from them
- there's one class for each of the 19 commands
- `Shell` keeps all the command objects in a map and calls `execute()` on the right one (polymorphism)

The other modules use simple functions and arrays, because our mentor asked us to keep the code easy to dry run.

## Testing

Our test file has 16 tests with 39 checks, and all of them pass. They test things like writing and reading back, files bigger than one block, append, copy, rename and move, delete freeing blocks, read-only, locked folders, the hash index, `Rahul` vs `rahul`, disk full, and saving and loading after a restart.

## Things to know

- The disk is only 2048 characters, so a bigger file gets "disk is full". (We fixed this in Phase 3, where the disk can grow.)
- `erase` only deletes empty folders.
- A locked folder's files can still be seen in `vfs_root` from File Explorer. (Also fixed in Phase 3.)
- Only one line of text can be stored, and passwords are saved as plain text in `vfs_save.txt`.
