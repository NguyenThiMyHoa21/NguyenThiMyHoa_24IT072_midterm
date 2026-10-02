# Midterm Project - Implement ls(1)

## 1. Student Information

- Name: Nguyen Thi My Hoa
- Student ID: 24IT072
- Course: Advanced Programming in the UNIX Environment
- Operating System: NetBSD/amd64

## 2. Project Description

This project implements a simplified version of the UNIX ls(1) command on NetBSD.

The program is organized into multiple C source files and a shared header file. The modular structure makes the program easier to develop, test, maintain, and extend.

## 3. Supported Options

| Option | Description |
|--------|-------------|
| -A | Show hidden files except . and .. |
| -a | Show all files including . and .. |
| -c | Use change time |
| -d | List directories as plain files |
| -F | Classify file types |
| -f | Do not sort output |
| -h | Display human-readable sizes |
| -i | Display inode numbers |
| -k | Display sizes in kilobytes |
| -l | Use long listing format |
| -n | Display numeric user and group IDs |
| -q | Replace non-printable characters with ? |
| -R | List directories recursively |
| -r | Reverse sorting order |
| -S | Sort by file size |
| -s | Display allocated blocks |
| -t | Sort by modification time |
| -u | Use access time |
| -w | Display non-printable characters as raw characters |

## 4. Project Structure

midterm_ls/
  include/ls.h
  src/main.c
  src/options.c
  src/util.c
  src/sort.c
  src/format.c
  src/list.c
  .gitignore
  Makefile
  README.md

## 5. Source Files

- main.c: Program entry point and operand processing.
- options.c: Command-line option parsing.
- util.c: Utility functions and file classification.
- sort.c: Sorting functions.
- format.c: Output formatting and file information display.
- list.c: Directory reading and listing.
- ls.h: Shared structures and function declarations.
- Makefile: Build configuration.
- .gitignore: Prevents object files and the executable from being committed.

## 6. Compilation

Build the project with:

    make

The executable ls will be generated after a successful build.

## 7. Clean Build Files

To remove object files and the executable:

    make clean

## 8. Usage Examples

Run the program:

    ./ls

Show all files:

    ./ls -a

Show hidden files except . and ..:

    ./ls -A

Long listing format:

    ./ls -l

Display inode numbers:

    ./ls -i

Sort by file size:

    ./ls -S

Sort by time:

    ./ls -t

Reverse sorting order:

    ./ls -r

Recursive listing:

    ./ls -R

List a directory as a plain file:

    ./ls -d src

## 9. Testing

The program was tested with the supported options and with multiple file and directory operands.

Compilation uses:

    -Wall -Wextra -Werror -std=c11

The program was also tested on NetBSD/amd64.

## 10. GitHub Repository

https://github.com/NguyenThiMyHoa21/NguyenThiMyHoa_24IT072_midterm

## 11. Development Environment

- Operating System: NetBSD/amd64
- Programming Language: C
- Compiler: cc
- Build Tool: Make
- Version Control: Git
- Repository Hosting: GitHub

## 12. Reference

The implementation is based on the ls(1) manual provided for the midterm project.
