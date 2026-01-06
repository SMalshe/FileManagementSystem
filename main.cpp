// main.cpp - Entry point for the file management system
// This program simulates a file system with different modes to interact with it

#include <iostream>
#include <string>
#include "FileSystem.h"

using namespace std;

// Shows the main menu where user picks which mode they want
void printMainMenu() {
    cout << "MODE SELECTION:\n";
    cout << "  1. Intuitive Mode (Easy Commands)\n";
    cout << "  2. CLI Learning Mode (Learn Unix)\n";
    cout << "  3. Full CLI Mode (Real Unix Commands)\n";
    cout << "  4. Exit\n\n";
}

// Shows the list of commands available in intuitive mode
void printCommandMenu() {
    cout << "============================================\n";
    cout << "              INTUITIVE MODE\n";
    cout << "============================================\n";
    cout << "AVAILABLE COMMANDS:\n";
    cout << "  list               - List files in current directory\n";
    cout << "  createfolder [name] - Create a new folder\n";
    cout << "  openfolder [name]  - Open a folder (.. for parent)\n";
    cout << "  createfile [name]  - Create a new file\n";
    cout << "  editfile [name]    - Edit file content\n";
    cout << "  view [name]        - View file content\n";
    cout << "  delete [name]      - Delete file/folder\n";
    cout << "  findfile [name]    - Search for file by name\n";
    cout << "  details [name]     - Show file details\n";
    cout << "  where              - Show current directory path\n";
    cout << "  report             - Show system statistics\n";
    cout << "  mode               - Switch mode\n";
    cout << "  exit               - Quit program\n\n";
}

// Shows the menu for CLI learning mode
void printCLILearningMenu() {
    cout << "\n============================================\n";
    cout << "              CLI LEARNING MODE\n";
    cout << "============================================\n\n";
    cout << "What do you want to do? (enter number):\n";
    cout << "  1. See the contents of current folder\n";
    cout << "  2. Create a new folder\n";
    cout << "  3. Go into a folder (use .. for parent)\n";
    cout << "  4. Create a new file\n";
    cout << "  5. View a file's content\n";
    cout << "  6. Edit a file\n";
    cout << "  7. Delete a file or folder\n";
    cout << "  8. Find a file\n";
    cout << "  9. Show file details\n";
    cout << " 10. Show current location\n";
    cout << " 11. Show statistics\n";
    cout << " 12. Switch mode\n";
    cout << " 13. Exit\n\n";
}

// Intuitive mode - uses simple english commands
void normalMode(FileSystem& fs) {
    string input;
    string command;
    string argument;

    printCommandMenu();

    while (true) {
        cout << "FileSystem:" << fs.getCurrentPath() << "> ";
        getline(cin, input);

        if (input.length() == 0) {
            continue;
        }

        // split input into command and argument
        int spacePos = input.find(' ');
        if (spacePos == string::npos) {
            command = input;
            argument = "";
        } else {
            command = input.substr(0, spacePos);
            argument = input.substr(spacePos + 1);
        }

        // handle commands with try-catch for exceptions
        try {
            if (command == "list") {
                fs.listDirectory();
            }
            else if (command == "createfolder") {
                if (argument == "") {
                    cout << "Usage: createfolder [name]\n";
                } else {
                    fs.createDirectory(argument);
                }
            }
            else if (command == "openfolder") {
                if (argument == "") {
                    cout << "Usage: openfolder [name]\n";
                } else {
                    fs.changeDirectory(argument);
                }
            }
            else if (command == "createfile") {
                if (argument == "") {
                    cout << "Usage: createfile [name]\n";
                } else {
                    fs.createFile(argument);
                }
            }
            else if (command == "editfile") {
                if (argument == "") {
                    cout << "Usage: editfile [name]\n";
                } else {
                    cout << "Enter content (type 'END' on new line to finish):\n";
                    string content = "";
                    string line;
                    while (getline(cin, line)) {
                        if (line == "END") {
                            break;
                        }
                        content = content + line + "\n";
                    }
                    fs.writeFile(argument, content);
                }
            }
            else if (command == "view") {
                if (argument == "") {
                    cout << "Usage: view [name]\n";
                } else {
                    fs.readFile(argument);
                }
            }
            else if (command == "delete") {
                if (argument == "") {
                    cout << "Usage: delete [name]\n";
                } else {
                    fs.deleteFile(argument);
                }
            }
            else if (command == "findfile") {
                if (argument == "") {
                    cout << "Usage: findfile [name]\n";
                } else {
                    fs.searchFile(argument);
                }
            }
            else if (command == "details") {
                if (argument == "") {
                    cout << "Usage: details [name]\n";
                } else {
                    fs.fileInfo(argument);
                }
            }
            else if (command == "where") {
                cout << fs.getCurrentPath() << "\n";
            }
            else if (command == "report") {
                fs.displayStats();
            }
            else if (command == "mode") {
                return;
            }
            else if (command == "exit") {
                cout << "Goodbye!\n";
                exit(0);
            }
            else {
                cout << "Unknown command. Type 'mode' to switch, 'exit' to quit.\n";
            }
        }
        catch (exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
}

// CLI learning mode - teaches unix commands
void cliLearningMode(FileSystem& fs) {
    string choice;
    string arg;

    while (true) {
        printCLILearningMenu();
        cout << "Enter option: ";
        getline(cin, choice);

        try {
            if (choice == "1") {
                cout << "\n--- Unix Command: ls ---\n";
                cout << "$ ls\n";
                fs.listDirectory();
            }
            else if (choice == "2") {
                cout << "\n--- Unix Command: mkdir ---\n";
                cout << "Enter folder name: ";
                getline(cin, arg);
                cout << "$ mkdir " << arg << "\n";
                fs.createDirectory(arg);
            }
            else if (choice == "3") {
                cout << "\n--- Unix Command: cd ---\n";
                cout << "Enter folder name (use .. for parent): ";
                getline(cin, arg);
                cout << "$ cd " << arg << "\n";
                fs.changeDirectory(arg);
            }
            else if (choice == "4") {
                cout << "\n--- Unix Command: touch ---\n";
                cout << "Enter file name: ";
                getline(cin, arg);
                cout << "$ touch " << arg << "\n";
                fs.createFile(arg);
            }
            else if (choice == "5") {
                cout << "\n--- Unix Command: cat ---\n";
                cout << "Enter file name: ";
                getline(cin, arg);
                cout << "$ cat " << arg << "\n";
                fs.readFile(arg);
            }
            else if (choice == "6") {
                cout << "\n--- Unix Command: nano ---\n";
                cout << "Enter file name: ";
                getline(cin, arg);
                cout << "$ nano " << arg << "\n";
                cout << "Enter content (type 'END' on new line to finish):\n";
                string content = "";
                string line;
                while (getline(cin, line)) {
                    if (line == "END") {
                        break;
                    }
                    content = content + line + "\n";
                }
                fs.writeFile(arg, content);
            }
            else if (choice == "7") {
                cout << "\n--- Unix Command: rm ---\n";
                cout << "Enter file/folder name: ";
                getline(cin, arg);
                cout << "$ rm " << arg << "\n";
                fs.deleteFile(arg);
            }
            else if (choice == "8") {
                cout << "\n--- Unix Command: find ---\n";
                cout << "Enter search term: ";
                getline(cin, arg);
                cout << "$ find " << arg << "\n";
                fs.searchFile(arg);
            }
            else if (choice == "9") {
                cout << "\n--- Unix Command: stat ---\n";
                cout << "Enter file name: ";
                getline(cin, arg);
                cout << "$ stat " << arg << "\n";
                fs.fileInfo(arg);
            }
            else if (choice == "10") {
                cout << "\n--- Unix Command: pwd ---\n";
                cout << "$ pwd\n";
                cout << fs.getCurrentPath() << "\n";
            }
            else if (choice == "11") {
                cout << "\n--- Statistics ---\n";
                fs.displayStats();
            }
            else if (choice == "12") {
                cout << "Switching mode...\n";
                return;
            }
            else if (choice == "13") {
                cout << "Goodbye!\n";
                exit(0);
            }
            else {
                cout << "Invalid choice. Try again.\n";
            }
        }
        catch (exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
}

// Full CLI mode - uses real unix-style commands
void fullCLIMode(FileSystem& fs) {
    string input;
    string command;
    string argument;

    cout << "============================================\n";
    cout << "             FULL CLI MODE\n";
    cout << "        Use Real Unix Commands!\n";
    cout << "============================================\n";
    cout << "AVAILABLE COMMANDS:\n";
    cout << "  ls                 - List directory\n";
    cout << "  mkdir [name]       - Create folder\n";
    cout << "  cd [name]          - Change directory (.. for parent)\n";
    cout << "  touch [name]       - Create file\n";
    cout << "  cat [name]         - View file\n";
    cout << "  nano [name]        - Edit file\n";
    cout << "  rm [name]          - Delete file/folder\n";
    cout << "  find [name]        - Search for file\n";
    cout << "  stat [name]        - Show file details\n";
    cout << "  pwd                - Show current path\n";
    cout << "  info               - Show statistics\n";
    cout << "  mode               - Switch mode\n";
    cout << "  exit               - Quit\n\n";

    while (true) {
        cout << "$ ";
        getline(cin, input);

        if (input.length() == 0) {
            continue;
        }

        // split into command and argument
        int spacePos = input.find(' ');
        if (spacePos == string::npos) {
            command = input;
            argument = "";
        } else {
            command = input.substr(0, spacePos);
            argument = input.substr(spacePos + 1);
        }

        try {
            if (command == "ls") {
                fs.listDirectory();
            }
            else if (command == "mkdir") {
                if (argument == "") {
                    cout << "mkdir: missing operand\n";
                } else {
                    fs.createDirectory(argument);
                }
            }
            else if (command == "cd") {
                if (argument == "") {
                    cout << "cd: missing operand\n";
                } else {
                    fs.changeDirectory(argument);
                }
            }
            else if (command == "touch") {
                if (argument == "") {
                    cout << "touch: missing operand\n";
                } else {
                    fs.createFile(argument);
                }
            }
            else if (command == "cat") {
                if (argument == "") {
                    cout << "cat: missing operand\n";
                } else {
                    fs.readFile(argument);
                }
            }
            else if (command == "nano") {
                if (argument == "") {
                    cout << "nano: missing operand\n";
                } else {
                    cout << "Enter content (type 'END' on new line to finish):\n";
                    string content = "";
                    string line;
                    while (getline(cin, line)) {
                        if (line == "END") {
                            break;
                        }
                        content = content + line + "\n";
                    }
                    fs.writeFile(argument, content);
                }
            }
            else if (command == "rm") {
                if (argument == "") {
                    cout << "rm: missing operand\n";
                } else {
                    fs.deleteFile(argument);
                }
            }
            else if (command == "find") {
                if (argument == "") {
                    cout << "find: missing operand\n";
                } else {
                    fs.searchFile(argument);
                }
            }
            else if (command == "stat") {
                if (argument == "") {
                    cout << "stat: missing operand\n";
                } else {
                    fs.fileInfo(argument);
                }
            }
            else if (command == "pwd") {
                cout << fs.getCurrentPath() << "\n";
            }
            else if (command == "info") {
                fs.displayStats();
            }
            else if (command == "mode") {
                cout << "Switching mode...\n";
                return;
            }
            else if (command == "exit" || command == "quit") {
                cout << "Goodbye!\n";
                exit(0);
            }
            else if (command == "help") {
                cout << "Commands: ls, mkdir, cd, touch, cat, nano, rm, find, stat, pwd, info\n";
                cout << "Use 'mode' to switch modes, 'exit' to quit\n\n";
            }
            else {
                cout << "Command not found: " << command << "\n";
            }
        }
        catch (exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
}

// Main function - where the program starts
int main() {
    FileSystem fs;
    int choice;

    cout << "============================================\n";
    cout << "        FILE MANAGEMENT SYSTEM\n";
    cout << "============================================\n";

    while (true) {
        printMainMenu();
        cout << "Select mode: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            normalMode(fs);
        }
        else if (choice == 2) {
            cliLearningMode(fs);
        }
        else if (choice == 3) {
            fullCLIMode(fs);
        }
        else if (choice == 4) {
            cout << "Goodbye!\n";
            break;
        }
        else {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
