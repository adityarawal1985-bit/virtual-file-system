# Rootrix Virtual File System 🗂️

Hi! This is our PBL project for the **Data Structure And Programming In C++** course at Graphic Era (Deemed to be University).

We are **Team Rootrix** (Team ID: DSCPP-III-2026-T123) and we made our own small file system in C++. It doesn't use Windows to save files directly. Instead it makes a "fake" disk in memory and manages files and folders on it, kind of like how a real operating system does it.

## Why we made this

We use files and folders every day, but we never really knew what happens inside when we save or delete something. So we decided to build one ourselves. Doing this helped us understand things like blocks, inodes, bitmaps and hashing much better than just reading about them.

## Our team

- **Tanish Naithani** (Team Lead): directory tree, `layout` and `seek` — [@tanishnaithani](https://github.com/tanishnaithani)
- **Aditya Rawal**: disk and blocks, cache, growing disk — [@adityarawal1985-bit](https://github.com/adityarawal1985-bit)
- **Tushar Singh**: hash index, journal, testing — [@singhtushar94365-cpu](https://github.com/singhtushar94365-cpu)
- **Rounak Tayal**: command line (OOP classes), Phase3Shell — [@rounaktayal-AIML](https://github.com/rounaktayal-AIML)

Our mentor: **Mr. Akshay Rajput**

## What our VFS can do

- Make folders and files, open them, rename, move, copy and delete
- Save text in files (the text is stored in small blocks on our fake disk)
- Search a file by its full path, or search a name in every folder
- Make a file read-only, or lock a folder with a password
- Remember everything even after you close the program
- Show all files in a real folder called `vfs_root`, so you can open them in File Explorer (locked folders stay empty there)

In Phase 3 we also added:
- a **cache**, so reading the same file again is faster
- a **disk that grows** when it gets full (64 blocks up to 1024)
- a **journal**, so we can show what happens when the "power goes off" and how we recover
- a **tree view** of all folders, and a search that looks everywhere

Small thing: `Rahul` and `rahul` count as the same name, just like in Windows.

## Folders in this repo

```
phase1/    our Phase 1 proposal (PPT and synopsis)
phase2/    our Phase 2 version (19 commands) - we kept it as it was
phase3/    our final version (25 commands)
diagram/   architecture diagram
report/    Phase 3 report
presentation/   Phase 3 PPT (LaTeX)
```

There is a separate README inside `phase3/` that explains Phase 3 in more detail.

## How to run it

You need g++ installed (on Windows we used MinGW).

1. Go into the `phase3` folder
2. Compile:
   ```
   g++ -std=c++11 phase3_main.cpp -o vfs3
   ```
3. Run:
   ```
   vfs3          (Windows: vfs3.exe)
   ```
4. Type `guide` to see all commands.

To run our tests:
```
g++ -std=c++11 phase3_tests.cpp -o tests3
tests3
```
You should see `PASSED: 73   FAILED: 0`.

Only compile the `.cpp` file. The `.hpp` files get included by themselves. If you want to start fresh, just delete `vfs_save.txt` and `vfs_journal.txt`.

## Commands

Our mentor told us not to use the normal Linux command names, so we gave every command its own name.

**From Phase 2:**

| Command | What it does |
|---|---|
| `makedir <name>` | make a folder |
| `newfile <name>` | make a file |
| `goto <name>` | go inside a folder (`goto ..` to go back) |
| `shift <name> <new>` | rename, or move into a folder |
| `setmode <name> ro/rw` | read-only or normal |
| `store <file> <text>` | write text in a file |
| `addto <file> <text>` | add text at the end |
| `readfile <file>` | show what's in a file |
| `clone <file> <new>` | copy a file |
| `diskmap` | show which blocks are used |
| `items` | list what's in this folder |
| `erase <name>` | delete a file or empty folder |
| `inode <name>` | show all details of a file |
| `pathfind <path>` | find a file by full path |
| `here` | show where you are |
| `seal <folder> <pass>` | lock a folder |
| `unseal <folder> <pass>` | unlock a folder |
| `guide` | show all commands |
| `bye` | exit |

**New in Phase 3:** `health`, `logbook`, `powercut`, `restore`, `layout`, `seek` (explained in `phase3/README.md`).

Here's a small example:
```
vfs:/> makedir docs
Folder created: docs (inode 1)
vfs:/> goto docs
vfs:/docs> newfile a.txt
File created: a.txt (inode 2)
vfs:/docs> store a.txt hello world
Wrote 11 bytes to a.txt
vfs:/docs> readfile a.txt
hello world
```

## How it works (simple version)

- Our disk is just a 2D array. Each **block** holds 32 characters.
- A **bitmap** (true/false array) tells us which blocks are free.
- Every file and folder has an **inode** that stores its name, size, parent folder and which blocks it uses.
- Folders don't use pointers. Each inode just remembers its parent's number, and that makes the tree.
- A **hash index** maps the full path (like `/docs/a.txt`) to the inode number, so finding a file is quick.
- After every command, everything is saved in `vfs_save.txt`.

Here's our Phase 3 diagram:

![architecture](diagram/phase3_architecture.png)

## What we used

- C++ (C++11), g++ / MinGW, VS Code, Git and GitHub
- Data structures: arrays, bitmap, tree, hashing, recursion, LRU
- OOP: classes, encapsulation, inheritance, virtual functions, polymorphism

## Testing

We wrote our own test programs. Phase 2 has 39 checks and Phase 3 has 73 checks, and all of them pass. The Phase 3 tests also run all the old Phase 2 tests again, to make sure we didn't break anything.

## Problems we faced

- Our old compiler didn't support `<filesystem>`, so we had to use older functions to make real folders.
- Windows Smart App Control blocked our exe on one laptop, so we ran it on another one.
- At first all files disappeared when we closed the program, so we added saving to a file.
- In Phase 3 the hardest part was adding new features without breaking Phase 2. Inheritance helped a lot here.

## What it can't do (yet)

- It only stores text, not images.
- The crash is only simulated, so `restore` just cancels the last entry. A real file system also saves old data to undo things.
- Passwords are saved as plain text in the save file.
- Only one user at a time.

Some things we'd like to add later: storing images, encrypting the save file, and maybe a simple GUI.

## Books and websites we used

- E. Balagurusamy, *Object Oriented Programming with C++*
- Yashavant Kanetkar, *Data Structures Through C++*
- GeeksforGeeks
- cppreference.com
- Microsoft Learn (NTFS overview)

---
Made by Team Rootrix 🙂
