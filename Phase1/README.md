# Rootrix VFS – Phase 1 (Proposal)

Phase 1 was our **proposal** phase. We didn't have the full project yet. We picked our topic, studied how file systems work, planned what we want to build, and showed it to our mentor with a PPT and a synopsis.

Both files are in this folder:
- `Phase1_PPT.pdf` – our Phase 1 presentation
- `Phase1_Synopsis.pdf` – our project proposal / synopsis

## Our team

Team **Rootrix** — Team ID: DSCPP-III-2026-T123
Semester 3, sections G, H, ML2, DS2
Mentor: **Mr. Akshay Rajput** (3 interactions in Phase 1)

| Member | ID | What they took up in Phase 1 |
|---|---|---|
| Tanish Naithani (Team Lead) | 25100011477 | Directory tree |
| Aditya Rawal | 2510380204 | Architecture, block manager |
| Tushar Singh | 2510011704 | Testing, metadata |
| Rounak Tayal | 2510370114 | CLI, documentation |

## The problem

Every day we create, move and delete files, but we never see what happens inside. The operating system quietly decides where the data goes, which blocks are free, and how to recover if something goes wrong. Because all of this is hidden, it's hard to actually *learn* how a file system works.

## Our idea

Build a small **Virtual File System (VFS) in C++**. It runs inside our own program and manages its own files, folders and storage blocks, **without touching the computer's real file system**. Since we build every part ourselves, we can see exactly how it works.

It was also a good way to use what we studied in class:
- **Trees** for folders
- **Hashing** for fast search
- **Bitmaps** for free and used blocks
- **Classes** to keep each part (disk, files, cache, journal) separate

## Who it's for

- CS students learning data structures and operating systems
- Teachers, as a teaching aid
- Maybe later: small or embedded systems

## Objectives

1. Design and build a working VFS in C++
2. Support the basic file and folder operations (create, read, update, delete)
3. Use data structures (trees, hashing, bitmaps) and OOP (classes, inheritance)
4. Test it through the command line, including edge cases
5. Show how it helps in learning OS and data structures

**Main goal:** a working command-line VFS with file and folder operations, caching and journaling.

## What we planned to build

From our synopsis:
1. A virtual disk split into fixed-size blocks
2. A tree of files and folders
3. Create, read, write, append, truncate, copy, move, rename and delete
4. File details: size, permissions, timestamps, block locations
5. A bitmap for free and used blocks
6. Search by name, path and details
7. An LRU cache for frequently used blocks
8. Journaling and basic recovery
9. A file system checker (consistency check)
10. Snapshots (save and restore the whole state)
11. A command-line interface
12. Disk usage and performance info

## Planned architecture

```
CLI input  →  Command parser  →  File / Directory manager  →  Block manager (bitmap)  →  Virtual disk  →  Output
```

Main parts we planned: virtual disk, block manager, directory tree, file manager, cache (LRU), journal and CLI. Extra parts: file system checker, snapshot manager, and permissions with owner / group / others and timestamps.

How a command was supposed to flow: you type a command, it gets parsed, the file is found using the tree and hash table, the blocks are read or written, and the result is shown.

## Tech stack and method

- **Language:** C++
- **Storage:** in memory, saved to a file
- **IDE:** VS Code
- **Version control:** Git and GitHub
- **Method:** requirement analysis → system design → implementation → testing → evaluation

## What we finished in Phase 1

- Studied how file systems work
- Wrote down the requirements
- Designed the system architecture
- Started a first version of the disk / block module

## Roadmap we made

1. **Phase 1:** proposal and design ✅
2. **Phase 2:** development
3. **Phase 3:** final implementation

## So what happened to the plan?

Looking back after Phase 3, here's how our plan turned out:

| Planned in Phase 1 | What we did |
|---|---|
| Virtual disk with fixed-size blocks | ✅ Phase 2 (32-character blocks), and it grows in Phase 3 |
| Tree of files and folders | ✅ Phase 2 (each inode stores its parent) |
| Create, read, write, append, copy, move, rename, delete | ✅ Phase 2 |
| Size and block locations | ✅ Phase 2 (`inode` command) |
| Permissions | ✅ in a simpler way: read-only files and password-sealed folders. No owner/group/others. |
| Bitmap | ✅ Phase 2 (`diskmap`) |
| Search | ✅ by full path in Phase 2 (`pathfind`), by name in Phase 3 (`seek`) |
| LRU cache | ✅ Phase 3 |
| Journaling and recovery | ✅ Phase 3 (`logbook`, `powercut`, `restore`) |
| Command-line interface | ✅ Phase 2, with our own command names |
| Disk usage info | ✅ `diskmap` and `health` |
| Classes, inheritance, polymorphism | ✅ CLI in Phase 2, one class per member in Phase 3 |

Some things also changed from the plan because of our mentor's feedback:
- we gave every command its own name instead of Linux names
- we copy files into a real folder so they can be seen
- names ignore upper/lower case, like Windows

## References we used

1. E. Balagurusamy, *Object Oriented Programming with C++*, McGraw Hill Education
2. Y. Kanetkar, *Data Structures Through C*, BPB Publications
3. GeeksforGeeks – Data Structures: https://www.geeksforgeeks.org/data-structures/
4. Programiz – C++ Programming: https://www.programiz.com/cpp-programming
