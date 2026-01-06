// FileSystem.h - Header file for our file system simulator
// This creates a fake file system in memory using a tree structure

#ifndef FILESYSTEM_H  // prevents including this file twice
#define FILESYSTEM_H  // marks that we've included it

#include <iostream>   // for cout and cin
#include <vector>     // for dynamic arrays
#include <string>     // for string type
#include <ctime>      // for timestamps
#include <stdexcept>  // for exceptions

using namespace std;  // so we don't have to type std:: everywhere

// Custom exception for file not found errors
class FileNotFoundException : public runtime_error {
public:
    FileNotFoundException(string name)
        : runtime_error("File not found: " + name) {}
};

// Custom exception for directory not found errors
class DirectoryNotFoundException : public runtime_error {
public:
    DirectoryNotFoundException(string name)
        : runtime_error("Directory not found: " + name) {}
};

// Custom exception for when file/directory already exists
class AlreadyExistsException : public runtime_error {
public:
    AlreadyExistsException(string name)
        : runtime_error("Already exists: " + name) {}
};

// Custom exception for directory not empty errors
class DirectoryNotEmptyException : public runtime_error {
public:
    DirectoryNotEmptyException(string name)
        : runtime_error("Directory not empty: " + name) {}
};

// Custom exception for invalid name errors
class InvalidNameException : public runtime_error {
public:
    InvalidNameException(string msg)
        : runtime_error("Invalid name: " + msg) {}
};

// This struct represents a single file or folder in our tree
struct FileNode {
    string name;              // name of the file/folder
    bool isDirectory;         // true = folder, false = file
    string content;           // stores text content (only for files)
    time_t createdTime;       // when it was created
    time_t modifiedTime;      // when it was last changed
    FileNode* parent;         // pointer to the parent folder
    vector<FileNode*> children;  // list of files/folders inside (only for directories)

    // Constructor - runs when we create a new FileNode
    FileNode(string n, bool isDir, FileNode* p = nullptr) {
        name = n;                    // set the name
        isDirectory = isDir;         // set if it's a folder or file
        content = "";                // start with empty content
        parent = p;                  // set parent pointer
        createdTime = time(0);       // set creation time to now
        modifiedTime = time(0);      // set modified time to now
    }

    // Destructor - runs when we delete a FileNode
    ~FileNode() {
        // loop through all children and delete them too
        for (int i = 0; i < children.size(); i++) {
            delete children[i];      // free up memory for each child
        }
    }
};

// This class manages the entire file system
class FileSystem {
private:
    FileNode* root;           // the top-level root folder
    FileNode* currentDir;     // which folder we're currently in

    // Helper function to validate a file/folder name
    void validateName(string name) {
        // check if name is empty
        if (name.length() == 0) {
            throw InvalidNameException("name cannot be empty");
        }
        // check for invalid characters
        if (name.find('/') != string::npos) {
            throw InvalidNameException("name cannot contain /");
        }
    }

public:
    // Constructor - sets up the file system with an empty root folder
    FileSystem() {
        root = new FileNode("root", true);  // create root folder
        currentDir = root;                   // start in root folder
    }

    // Destructor - cleans up all memory when we're done
    ~FileSystem() {
        delete root;  // this will recursively delete everything
    }

    // Creates a new file in the current folder
    // Throws: InvalidNameException, AlreadyExistsException
    void createFile(string fileName, string content = "") {
        // validate the file name
        validateName(fileName);

        // check if a file with this name already exists
        for (int i = 0; i < currentDir->children.size(); i++) {
            if (currentDir->children[i]->name == fileName) {
                throw AlreadyExistsException(fileName);
            }
        }

        // create the new file node
        FileNode* newFile = new FileNode(fileName, false, currentDir);
        newFile->content = content;                // set initial content
        currentDir->children.push_back(newFile);   // add to current folder
        cout << "File '" << fileName << "' created\n";
    }

    // Creates a new folder in the current folder
    // Throws: InvalidNameException, AlreadyExistsException
    void createDirectory(string dirName) {
        // validate the directory name
        validateName(dirName);

        // check if folder already exists
        for (int i = 0; i < currentDir->children.size(); i++) {
            if (currentDir->children[i]->name == dirName) {
                throw AlreadyExistsException(dirName);
            }
        }

        // create new folder node
        FileNode* newDir = new FileNode(dirName, true, currentDir);
        currentDir->children.push_back(newDir);  // add to current folder
        cout << "Directory '" << dirName << "' created\n";
    }

    // Changes which folder we're currently in
    // Throws: DirectoryNotFoundException
    void changeDirectory(string dirName) {
        // if user types ".." go to parent folder
        if (dirName == "..") {
            if (currentDir->parent != nullptr) {  // make sure we have a parent
                currentDir = currentDir->parent;  // move to parent
                cout << "Changed to parent directory\n";
                return;
            } else {
                throw DirectoryNotFoundException("..");
            }
        }

        // if user types "/" go to root
        if (dirName == "/") {
            currentDir = root;
            cout << "Changed to root\n";
            return;
        }

        // otherwise look for the folder by name
        for (int i = 0; i < currentDir->children.size(); i++) {
            FileNode* child = currentDir->children[i];  // get each child
            // check if it matches and is a directory
            if (child->name == dirName && child->isDirectory) {
                currentDir = child;  // move into that folder
                cout << "Changed to directory '" << dirName << "'\n";
                return;
            }
        }

        // if we get here, directory was not found
        throw DirectoryNotFoundException(dirName);
    }

    // Shows all files and folders in current directory
    void listDirectory() {
        cout << "\n--- Directory: " << getCurrentPath() << " ---\n";
        cout << "[DIR]  ..\n";  // show parent folder option
        cout << "[DIR]  .\n";   // show current folder option

        // check if folder is empty
        if (currentDir->children.size() == 0) {
            cout << "(empty)\n";
        } else {
            // loop through all children and print them
            for (int i = 0; i < currentDir->children.size(); i++) {
                FileNode* child = currentDir->children[i];

                // print [DIR] or [FILE] based on type
                if (child->isDirectory) {
                    cout << "[DIR]  ";
                } else {
                    cout << "[FILE] ";
                }

                cout << child->name;  // print the name

                // if it's a file with content, show the size
                if (!child->isDirectory && child->content.length() > 0) {
                    cout << " (" << child->content.length() << " bytes)";
                }
                cout << "\n";
            }
        }
        cout << "\n";
    }

    // Writes content to an existing file
    // Throws: FileNotFoundException
    void writeFile(string fileName, string content) {
        // search for the file in current folder
        for (int i = 0; i < currentDir->children.size(); i++) {
            FileNode* child = currentDir->children[i];
            // check if it matches and is a file (not folder)
            if (child->name == fileName && !child->isDirectory) {
                child->content = content;        // update the content
                child->modifiedTime = time(0);   // update modified time
                cout << "File '" << fileName << "' written (";
                cout << content.length() << " bytes)\n";
                return;
            }
        }
        // if we get here, file was not found
        throw FileNotFoundException(fileName);
    }

    // Reads and displays a file's content
    // Throws: FileNotFoundException
    string readFile(string fileName) {
        // search for the file
        for (int i = 0; i < currentDir->children.size(); i++) {
            FileNode* child = currentDir->children[i];
            if (child->name == fileName && !child->isDirectory) {
                cout << "\n--- Content of " << fileName << " ---\n";
                // check if file is empty
                if (child->content.length() == 0) {
                    cout << "(empty)";
                } else {
                    cout << child->content;
                }
                cout << "\n\n";
                return child->content;
            }
        }
        // if we get here, file was not found
        throw FileNotFoundException(fileName);
    }

    // Deletes a file or empty folder
    // Throws: FileNotFoundException, DirectoryNotEmptyException
    void deleteFile(string fileName) {
        // search for the file/folder
        for (int i = 0; i < currentDir->children.size(); i++) {
            if (currentDir->children[i]->name == fileName) {
                FileNode* toDelete = currentDir->children[i];

                // if it's a folder, make sure it's empty
                if (toDelete->isDirectory && toDelete->children.size() > 0) {
                    throw DirectoryNotEmptyException(fileName);
                }

                delete toDelete;  // free the memory
                // remove from the children vector
                currentDir->children.erase(currentDir->children.begin() + i);
                cout << "'" << fileName << "' deleted\n";
                return;
            }
        }
        // if we get here, file was not found
        throw FileNotFoundException(fileName);
    }

    // Searches for files by name in the whole file system
    // Returns a vector of paths to matching files
    vector<string> searchFile(string fileName) {
        cout << "Searching for '" << fileName << "'...\n";
        vector<string> results;              // store paths of found files
        searchHelper(root, fileName, results, "");  // start search from root

        if (results.size() == 0) {
            cout << "No files found\n";
        } else {
            // print all found files
            for (int i = 0; i < results.size(); i++) {
                cout << "Found: " << results[i] << "\n";
            }
        }
        cout << "\n";
        return results;
    }

    // Shows info about a file or folder
    // Throws: FileNotFoundException
    void fileInfo(string fileName) {
        // search for it in current folder
        for (int i = 0; i < currentDir->children.size(); i++) {
            FileNode* child = currentDir->children[i];
            if (child->name == fileName) {
                cout << "\n--- File Info ---\n";
                cout << "Name: " << child->name << "\n";

                // show type
                if (child->isDirectory) {
                    cout << "Type: Directory\n";
                } else {
                    cout << "Type: File\n";
                }

                cout << "Size: " << child->content.length() << " bytes\n";
                cout << "Created: " << ctime(&child->createdTime);   // ctime converts to readable format
                cout << "Modified: " << ctime(&child->modifiedTime);
                cout << "\n";
                return;
            }
        }
        // if we get here, file was not found
        throw FileNotFoundException(fileName);
    }

    // Returns the current folder path as a string
    string getCurrentPath() {
        string path = "";                    // build path here
        vector<string> pathParts;            // store folder names
        FileNode* node = currentDir;         // start from current folder

        // go up the tree and collect folder names
        while (node != root) {
            pathParts.push_back(node->name);  // add name to list
            node = node->parent;               // move to parent
        }

        path = "/";  // start with root slash

        // build path from collected names (in reverse order)
        for (int i = pathParts.size() - 1; i >= 0; i--) {
            path = path + pathParts[i];  // add folder name
            if (i > 0) {
                path = path + "/";       // add slash between folders
            }
        }

        return path;
    }

    // Shows statistics about the file system
    void displayStats() {
        int fileCount = 0;    // count of files
        int dirCount = 0;     // count of directories
        int totalSize = 0;    // total size in bytes

        countStats(root, fileCount, dirCount, totalSize);  // count everything

        cout << "\n--- File System Statistics ---\n";
        cout << "Total Files: " << fileCount << "\n";
        cout << "Total Directories: " << dirCount << "\n";
        cout << "Total Size: " << totalSize << " bytes\n";
        cout << "\n";
    }

private:
    // Helper function to recursively search for files
    void searchHelper(FileNode* node, string target, vector<string>& results, string path) {
        // check if this node's name contains what we're looking for
        // and it's a file (not a folder)
        if (node->name.find(target) != string::npos && !node->isDirectory) {
            results.push_back(path + node->name);  // add to results
        }

        // search in all children
        for (int i = 0; i < node->children.size(); i++) {
            // recursive call with updated path
            searchHelper(node->children[i], target, results, path + node->name + "/");
        }
    }

    // Helper function to count files, folders, and total size
    void countStats(FileNode* node, int& files, int& dirs, int& size) {
        if (node->isDirectory) {
            dirs++;  // count this directory
            // count all children
            for (int i = 0; i < node->children.size(); i++) {
                countStats(node->children[i], files, dirs, size);
            }
        } else {
            files++;                          // count this file
            size = size + node->content.length();  // add its size
        }
    }
};

#endif  // end of include guard
