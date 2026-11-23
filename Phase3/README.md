
# Rootrix VFS – Phase 3

This folder has the **final version** of our project. In Phase 2 we had 19 commands. In Phase 3 we added 6 more (so 25 total) and made the file system:

- **faster** with a cache
- **bigger** with a disk that grows
- **safer** with a journal and crash recovery
- **easier to look around** with a tree view and a search

We did **not** rewrite Phase 2. We built Phase 3 on top of it. Only two Phase 2 files needed small changes, and all our old Phase 2 tests still pass.

## How to run

```
g++ -std=c++11 phase3_main.cpp -o vfs3
vfs3            (Windows: vfs3.exe)
```

Tests:
```
g++ -std=c++11 phase3_tests.cpp -o tests3
tests3          → PASSED: 73   FAILED: 0
```

Tip: if you copy `vfs_save.txt` from the `phase2` folder into this folder, Phase 3 opens your Phase 2 files too.

## The 6 new commands

| Command | What it does | Made by |
|---|---|---|
| `health` | shows disk size, cache hits/misses and journal status | Aditya + Tushar |
| `logbook` | shows the journal | Tushar |
| `powercut <operation>` | pretends the power went off in the middle of something | Tushar |
| `restore` | fixes things after a powercut | Tushar |
| `layout` | shows all folders and files like a tree | Tanish |
| `seek <name>` | searches for a name in every folder | Tanish |

## What each of us did in Phase 3

**Aditya – cache and growing disk** (`phase3_aditya_lru_cache.hpp`)
- Made the `LRUCache` class. It keeps the last 4 blocks we read. If we read a file again, it comes from the cache (a "hit") instead of the disk.
- When the cache is full, the block that was used longest ago gets removed. That's what LRU means.
- The disk now starts with 64 blocks and doubles when it's full (64 → 128 → 256…) up to 1024. New blocks are added at the end, so old files don't get messed up.

**Tushar – journal and testing** (`phase3_tushar_journal.hpp`, `phase3_tests.cpp`)
- Made the `Journal` class. Before every change it writes `BEGIN`, and after it writes `COMMIT`.
- If there's a BEGIN without a COMMIT, it means something crashed. Then you can't change anything until you type `restore`. This works even after closing and opening the program again, because the journal is saved in `vfs_journal.txt`.
- Passwords never go into the journal.
- Wrote the Phase 3 tests (27 tests, 73 checks).

**Tanish – layout and seek** (`phase3_tanish_folder_explorer.hpp`, `phase3_main.cpp`)
- Made the `FolderExplorer` class. `layout` uses recursion: the function calls itself for every folder inside a folder.
- `seek` looks through all files and folders for a name.
- If a folder is locked, it shows `[locked]` and doesn't show what's inside.

**Rounak – Phase3Shell** (`phase3_rounak_cli.hpp`)
- Made `Phase3Shell`, which **inherits** from our Phase 2 `Shell`. So all 19 old commands just work, without copying any code.
- Added the 6 new command classes.
- It overrides `runCommand()` so that every changing command gets written in the journal.
- Also made locked folders show up empty in `vfs_root`, so nobody can open those files from File Explorer.

## Files in this folder

| File | Phase 3 status |
|---|---|
| `phase2_rounak_real_folder.hpp` | same as Phase 2 |
| `phase2_tanish_directory_tree.hpp` | same as Phase 2 |
| `phase2_tushar_metadata.hpp` | same as Phase 2 |
| `phase2_aditya_block_manager.hpp` | small changes (cache + growing disk) |
| `phase2_rounak_cli.hpp` | small changes (so Phase3Shell can inherit) |
| `phase3_aditya_lru_cache.hpp` | new |
| `phase3_tushar_journal.hpp` | new |
| `phase3_tanish_folder_explorer.hpp` | new |
| `phase3_rounak_cli.hpp` | new |
| `phase3_main.cpp` | new |
| `phase3_tests.cpp` | new |

## What we changed in the Phase 2 files

In **Aditya's block manager**:
- the disk can now hold up to 1024 blocks, but starts with 64 open
- added `growDiskFor()` to grow the disk, and `loadTextCached()` to read through the cache
- freed blocks also get removed from the cache
- `store`, `addto` and `clone` try to grow the disk before saying "disk is full"
- the save file now starts with `VFS3` and also saves the disk size (old `VFS2` files still open)

In **Rounak's CLI**:
- changed `private` to `protected` in `Shell`, so the child class can use it
- made the destructor `virtual`
- added a `virtual runCommand()` that Phase3Shell overrides

## OOP in Phase 3

Each of us made at least one class:
- `LRUCache` (Aditya), `Journal` (Tushar), `FolderExplorer` (Tanish): private data, public functions, constructors
- `Phase3Shell` (Rounak): inheritance, overriding and polymorphism

We also tried not to use STL in the new code. The new classes only use arrays and strings.

## Try this demo

```
makedir docs
goto docs
newfile a.txt
store a.txt hello world
readfile a.txt
readfile a.txt
health                → 1 miss, 1 hit
goto ..
newfile big.txt
store big.txt (paste a long text, more than 2048 characters)
diskmap               → "Disk grown to 128 blocks"
makedir secret
seal secret pw123
layout                → secret/  [locked]
powercut store a.txt
makedir test          → not allowed
logbook               → last BEGIN has no COMMIT
bye
```
Now open it again. It still remembers the crash. Type `restore`, then `makedir test` works.

## Things to know

- If a command fails its own check (like "already exists"), the journal still writes BEGIN and COMMIT for it. We kept it simple on purpose.
- `powercut` happens before anything changes, so `restore` only cancels the entry.
- The cache is only used by `readfile`.
- Passwords are saved as plain text in `vfs_save.txt`.
