/**
 * @file main.cpp
 * @brief Main entry point for file management system with multiple interaction modes
 */

#include <iostream>
#include <string>
#include "FileSystem.h"

using namespace std;

/**
 * @brief Displays the main menu for mode selection
 */
void printMainMenu() {
    cout << "\n============================================\n";
    cout << "           FILE MANAGEMENT SYSTEM\n";
    cout << "============================================\n";
    cout << "MODE SELECTION:\n";
    cout << "  1. Intuitive Mode (Easy Commands)\n";
    cout << "  2. CLI Learning Mode (Learn Unix)\n";
    cout << "  3. Full CLI Mode (Real Unix Commands)\n";
    cout << "  4. Exit\n\n";
}

/**
 * @brief Displays available commands for Intuitive Mode
 */
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
    cout << "  tree               - Show visual tree diagram (static)\n";
    cout << "  livetree           - Enable/disable live updating tree\n";
    cout << "  report             - Show system statistics\n";
    cout << "  mode               - Switch mode\n";
    cout << "  exit               - Quit program\n\n";
}

/**
 * @brief Displays menu for CLI Learning Mode
 */
void printCLILearningMenu() {
    cout << "\n============================================\n";
    cout << "              CLI LEARNING MODE\n";
    cout << "============================================\n\n";
    cout << "What do you want to do? (enter number):\n";
    cout << "  1. See the contents of current folder\n";
    cout << "  2. Create a new folder\n";
    cout << "  3. Go into a folder\n";
    cout << "  4. Go back to parent folder\n";
    cout << "  5. Create a new file\n";
    cout << "  6. View a file's content\n";
    cout << "  7. Edit a file\n";
    cout << "  8. Delete a file or folder\n";
    cout << "  9. Find a file\n";
    cout << " 10. Show file details\n";
    cout << " 11. Show current location\n";
    cout << " 12. Show visual tree diagram (static)\n";
    cout << " 13. Enable/disable live updating tree\n";
    cout << " 14. Show statistics\n";
    cout << " 15. Switch to normal mode\n";
    cout << " 16. Exit\n\n";
}

/**
 * @brief Implements Intuitive Mode with simplified command interface
 * @param fs Reference to the FileSystem instance
 */
void normalMode(FileSystem& fs) {
    string command, arg1, arg2;
    printCommandMenu();

    while(true) {
        cout << "FileSystem:" << fs.getCurrentPath() << "> ";
        getline(cin, command);

        if(command.empty()) continue;

        size_t space = command.find(' ');
        string cmd = (space == string::npos) ? command : command.substr(0, space);
        string args = (space == string::npos) ? "" : command.substr(space + 1);

        if(cmd == "list") {
            fs.listDirectory();
        }
        else if(cmd == "createfolder") {
            if(args.empty()) cout << "Usage: createfolder [name]\n";
            else fs.createDirectory(args);
        }
        else if(cmd == "openfolder") {
            if(args.empty()) cout << "Usage: openfolder [name]\n";
            else fs.changeDirectory(args);
        }
        else if(cmd == "createfile") {
            if(args.empty()) cout << "Usage: createfile [name]\n";
            else fs.createFile(args);
        }
        else if(cmd == "editfile") {
            if(args.empty()) {
                cout << "Usage: editfile [name]\n";
            } else {
                cout << "Enter content (type 'END' on new line to finish):\n";
                string content = "", line;
                while(getline(cin, line)) {
                    if(line == "END") break;
                    content += line + "\n";
                }
                fs.writeFile(args, content);
            }
        }
        else if(cmd == "view") {
            if(args.empty()) cout << "Usage: view [name]\n";
            else fs.readFile(args);
        }
        else if(cmd == "delete") {
            if(args.empty()) cout << "Usage: delete [name]\n";
            else fs.deleteFile(args);
        }
        else if(cmd == "findfile") {
            if(args.empty()) cout << "Usage: findfile [name]\n";
            else fs.searchFile(args);
        }
        else if(cmd == "details") {
            if(args.empty()) cout << "Usage: details [name]\n";
            else fs.fileInfo(args);
        }
        else if(cmd == "where") {
            cout << fs.getCurrentPath() << "\n";
        }
        else if(cmd == "tree") {
            fs.displayTree();
        }
        else if(cmd == "livetree") {
            if(fs.isLiveTreeEnabled()) {
                fs.disableLiveTree();
            } else {
                fs.enableLiveTree();
            }
        }
        else if(cmd == "report") {
            fs.displayStats();
        }
        else if(cmd == "mode") {
            return;
        }
        else if(cmd == "exit") {
            cout << "Goodbye!\n";
            exit(0);
        }
        else {
            cout << "Unknown command. Type 'mode' to switch, 'exit' to quit.\n";
        }
    }
}

/**
 * @brief Implements CLI Learning Mode with numbered menu and Unix command references
 * @param fs Reference to the FileSystem instance
 */
void cliLearningMode(FileSystem& fs) {
    string choice;
    string arg;

    while(true) {
        printCLILearningMenu();
        cout << "Enter option: ";
        getline(cin, choice);

        if(choice == "1") {
            cout << "\n--- Unix Command: ls ---\n";
            cout << "$ ls\n";
            fs.listDirectory();
        }
        else if(choice == "2") {
            cout << "\n--- Unix Command: mkdir ---\n";
            cout << "Enter folder name: ";
            getline(cin, arg);
            cout << "$ mkdir " << arg << "\n";
            fs.createDirectory(arg);
        }
        else if(choice == "3") {
            cout << "\n--- Unix Command: cd ---\n";
            cout << "Enter folder name: ";
            getline(cin, arg);
            cout << "$ cd " << arg << "\n";
            fs.changeDirectory(arg);
        }
        else if(choice == "4") {
            cout << "\n--- Unix Command: cd .. ---\n";
            cout << "$ cd ..\n";
            fs.changeDirectory("..");
        }
        else if(choice == "5") {
            cout << "\n--- Unix Command: touch ---\n";
            cout << "Enter file name: ";
            getline(cin, arg);
            cout << "$ touch " << arg << "\n";
            fs.createFile(arg);
        }
        else if(choice == "6") {
            cout << "\n--- Unix Command: cat ---\n";
            cout << "Enter file name: ";
            getline(cin, arg);
            cout << "$ cat " << arg << "\n";
            fs.readFile(arg);
        }
        else if(choice == "7") {
            cout << "\n--- Unix Command: nano ---\n";
            cout << "Enter file name: ";
            getline(cin, arg);
            cout << "$ nano " << arg << "\n";
            cout << "Enter content (type 'END' on new line to finish):\n";
            string content = "", line;
            while(getline(cin, line)) {
                if(line == "END") break;
                content += line + "\n";
            }
            fs.writeFile(arg, content);
        }
        else if(choice == "8") {
            cout << "\n--- Unix Command: rm ---\n";
            cout << "Enter file/folder name: ";
            getline(cin, arg);
            cout << "$ rm " << arg << "\n";
            fs.deleteFile(arg);
        }
        else if(choice == "9") {
            cout << "\n--- Unix Command: find ---\n";
            cout << "Enter search term: ";
            getline(cin, arg);
            cout << "$ find " << arg << "\n";
            fs.searchFile(arg);
        }
        else if(choice == "10") {
            cout << "\n--- Unix Command: stat ---\n";
            cout << "Enter file name: ";
            getline(cin, arg);
            cout << "$ stat " << arg << "\n";
            fs.fileInfo(arg);
        }
        else if(choice == "11") {
            cout << "\n--- Unix Command: pwd ---\n";
            cout << "$ pwd\n";
            cout << fs.getCurrentPath() << "\n";
        }
        else if(choice == "12") {
            cout << "\n--- Visual Tree Diagram ---\n";
            fs.displayTree();
        }
        else if(choice == "13") {
            if(fs.isLiveTreeEnabled()) {
                fs.disableLiveTree();
            } else {
                fs.enableLiveTree();
            }
        }
        else if(choice == "14") {
            cout << "\n--- Unix Command: info ---\n";
            cout << "$ info\n";
            fs.displayStats();
        }
        else if(choice == "15") {
            cout << "Switching to Normal Mode...\n";
            return;
        }
        else if(choice == "16") {
            cout << "Goodbye!\n";
            exit(0);
        }
        else {
            cout << "Invalid choice. Try again.\n";
        }
    }
}

/**
 * @brief Implements Full CLI Mode with Unix-like command interface
 * @param fs Reference to the FileSystem instance
 */
void fullCLIMode(FileSystem& fs) {
    string command, arg1, arg2;

    cout << "============================================\n";
    cout << "        FULL CLI MODE\n"; 
    cout << "     Use Real Unix Commands!\n";
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
    cout << "  tree               - Show visual tree diagram (static)\n";
    cout << "  livetree           - Enable/disable live updating tree\n";
    cout << "  info               - Show statistics\n";
    cout << "  mode               - Switch mode\n";
    cout << "  exit               - Quit\n\n";

    while(true) {
        cout << "$ ";
        getline(cin, command);

        if(command.empty()) continue;

        size_t space = command.find(' ');
        string cmd = (space == string::npos) ? command : command.substr(0, space);
        string args = (space == string::npos) ? "" : command.substr(space + 1);

        if(cmd == "ls") {
            fs.listDirectory();
        }
        else if(cmd == "mkdir") {
            if(args.empty()) cout << "mkdir: missing operand\n";
            else fs.createDirectory(args);
        }
        else if(cmd == "cd") {
            if(args.empty()) cout << "cd: missing operand\n";
            else fs.changeDirectory(args);
        }
        else if(cmd == "touch") {
            if(args.empty()) cout << "touch: missing operand\n";
            else fs.createFile(args);
        }
        else if(cmd == "cat") {
            if(args.empty()) cout << "cat: missing operand\n";
            else fs.readFile(args);
        }
        else if(cmd == "nano") {
            if(args.empty()) {
                cout << "nano: missing operand\n";
            } else {
                cout << "Enter content (type 'END' on new line to finish):\n";
                string content = "", line;
                while(getline(cin, line)) {
                    if(line == "END") break;
                    content += line + "\n";
                }
                fs.writeFile(args, content);
            }
        }
        else if(cmd == "rm") {
            if(args.empty()) cout << "rm: missing operand\n";
            else fs.deleteFile(args);
        }
        else if(cmd == "find") {
            if(args.empty()) cout << "find: missing operand\n";
            else fs.searchFile(args);
        }
        else if(cmd == "stat") {
            if(args.empty()) cout << "stat: missing operand\n";
            else fs.fileInfo(args);
        }
        else if(cmd == "pwd") {
            cout << fs.getCurrentPath() << "\n";
        }
        else if(cmd == "tree") {
            fs.displayTree();
        }
        else if(cmd == "livetree") {
            if(fs.isLiveTreeEnabled()) {
                fs.disableLiveTree();
            } else {
                fs.enableLiveTree();
            }
        }
        else if(cmd == "info") {
            fs.displayStats();
        }
        else if(cmd == "mode") {
            cout << "Switching mode...\n";
            return;
        }
        else if(cmd == "exit" || cmd == "quit") {
            cout << "Goodbye!\n";
            exit(0);
        }
        else if(cmd == "help") {
            cout << "AVAILABLE COMMANDS:\n";
            cout << "  ls, mkdir, cd, touch, cat, nano, rm, find, stat, pwd, info\n";
            cout << "  mode (switch), exit (quit)\n\n";
        }
        else {
            cout << "Command not found: " << cmd << "\n";
        }
    }
}

/**
 * @brief Main entry point
 * @return 0 on successful execution
 */
int main() {
    FileSystem fs;
    int choice;

    cout << "============================================\n";
    cout << "        FILE MANAGEMENT SYSTEM\n";
    cout << "============================================\n";

    while(true) {
        printMainMenu();
        cout << "Select mode: ";
        cin >> choice;
        cin.ignore();

        if(choice == 1) {
            normalMode(fs);
        }
        else if(choice == 2) {
            cliLearningMode(fs);
        }
        else if(choice == 3) {
            fullCLIMode(fs);
        }
        else if(choice == 4) {
            cout << "Goodbye!\n";
            break;
        }
        else {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
