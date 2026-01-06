// FileSystem.h - simulates a file system using a tree structure

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <stdexcept>

using namespace std;

// exception classes
class FileNotFoundException : public runtime_error {
public:
    FileNotFoundException(string name)
        : runtime_error("File not found: " + name) {}
};

class DirectoryNotFoundException : public runtime_error {
public:
    DirectoryNotFoundException(string name)
        : runtime_error("Directory not found: " + name) {}
};

class AlreadyExistsException : public runtime_error {
public:
    AlreadyExistsException(string name)
        : runtime_error("Already exists: " + name) {}
};

class DirectoryNotEmptyException : public runtime_error {
public:
    DirectoryNotEmptyException(string name)
        : runtime_error("Directory not empty: " + name) {}
};

class InvalidNameException : public runtime_error {
public:
    InvalidNameException(string msg)
        : runtime_error("Invalid name: " + msg) {}
};

// represents a single file or folder in the tree
struct FileNode {
    string name;
    bool isDirectory;
    string content;
    time_t createdTime;
    time_t modifiedTime;
    FileNode* parent;
    vector<FileNode*> children;

    FileNode(string n, bool isDir, FileNode* p = nullptr) {
        name = n;
        isDirectory = isDir;
        content = "";
        parent = p;
        createdTime = time(0);
        modifiedTime = time(0);
    }

    ~FileNode() {
        for (int i = 0; i < children.size(); i++) {
            delete children[i];
        }
    }
};

// manages the entire file system
class FileSystem {
private:
    FileNode* root;
    FileNode* currentDir;

    void validateName(string name) {
        if (name.length() == 0) {
            throw InvalidNameException("name cannot be empty");
        }
        if (name.find('/') != string::npos) {
            throw InvalidNameException("name cannot contain /");
        }
    }

public:
    FileSystem() {
        root = new FileNode("root", true);
        currentDir = root;
    }

    ~FileSystem() {
        delete root;
    }

    // creates a new file in the current folder
    void createFile(string fileName, string content = "") {
        validateName(fileName);

        for (int i = 0; i < currentDir->children.size(); i++) {
            if (currentDir->children[i]->name == fileName) {
                throw AlreadyExistsException(fileName);
            }
        }

        FileNode* newFile = new FileNode(fileName, false, currentDir);
        newFile->content = content;
        currentDir->children.push_back(newFile);
        cout << "File '" << fileName << "' created\n";
    }

    // creates a new folder in the current folder
    void createDirectory(string dirName) {
        validateName(dirName);

        for (int i = 0; i < currentDir->children.size(); i++) {
            if (currentDir->children[i]->name == dirName) {
                throw AlreadyExistsException(dirName);
            }
        }

        FileNode* newDir = new FileNode(dirName, true, currentDir);
        currentDir->children.push_back(newDir);
        cout << "Directory '" << dirName << "' created\n";
    }

    // changes which folder we're currently in
    void changeDirectory(string dirName) {
        if (dirName == "..") {
            if (currentDir->parent != nullptr) {
                currentDir = currentDir->parent;
                cout << "Changed to parent directory\n";
                return;
            } else {
                throw DirectoryNotFoundException("..");
            }
        }

        if (dirName == "/") {
            currentDir = root;
            cout << "Changed to root\n";
            return;
        }

        for (int i = 0; i < currentDir->children.size(); i++) {
            FileNode* child = currentDir->children[i];
            if (child->name == dirName && child->isDirectory) {
                currentDir = child;
                cout << "Changed to directory '" << dirName << "'\n";
                return;
            }
        }

        throw DirectoryNotFoundException(dirName);
    }

    // shows all files and folders in current directory
    void listDirectory() {
        cout << "\n--- Directory: " << getCurrentPath() << " ---\n";
        cout << "[DIR]  ..\n";
        cout << "[DIR]  .\n";

        if (currentDir->children.size() == 0) {
            cout << "(empty)\n";
        } else {
            for (int i = 0; i < currentDir->children.size(); i++) {
                FileNode* child = currentDir->children[i];

                if (child->isDirectory) {
                    cout << "[DIR]  ";
                } else {
                    cout << "[FILE] ";
                }

                cout << child->name;

                if (!child->isDirectory && child->content.length() > 0) {
                    cout << " (" << child->content.length() << " bytes)";
                }
                cout << "\n";
            }
        }
        cout << "\n";
    }

    // writes content to an existing file
    void writeFile(string fileName, string content) {
        for (int i = 0; i < currentDir->children.size(); i++) {
            FileNode* child = currentDir->children[i];
            if (child->name == fileName && !child->isDirectory) {
                child->content = content;
                child->modifiedTime = time(0);
                cout << "File '" << fileName << "' written (";
                cout << content.length() << " bytes)\n";
                return;
            }
        }
        throw FileNotFoundException(fileName);
    }

    // reads and returns a file's content
    string readFile(string fileName) {
        for (int i = 0; i < currentDir->children.size(); i++) {
            FileNode* child = currentDir->children[i];
            if (child->name == fileName && !child->isDirectory) {
                cout << "\n--- Content of " << fileName << " ---\n";
                if (child->content.length() == 0) {
                    cout << "(empty)";
                } else {
                    cout << child->content;
                }
                cout << "\n\n";
                return child->content;
            }
        }
        throw FileNotFoundException(fileName);
    }

    // deletes a file or empty folder
    void deleteFile(string fileName) {
        for (int i = 0; i < currentDir->children.size(); i++) {
            if (currentDir->children[i]->name == fileName) {
                FileNode* toDelete = currentDir->children[i];

                if (toDelete->isDirectory && toDelete->children.size() > 0) {
                    throw DirectoryNotEmptyException(fileName);
                }

                delete toDelete;
                currentDir->children.erase(currentDir->children.begin() + i);
                cout << "'" << fileName << "' deleted\n";
                return;
            }
        }
        throw FileNotFoundException(fileName);
    }

    // searches for files by name recursively from root
    vector<string> searchFile(string fileName) {
        cout << "Searching for '" << fileName << "'...\n";
        vector<string> results;
        searchHelper(root, fileName, results, "");

        if (results.size() == 0) {
            cout << "No files found\n";
        } else {
            for (int i = 0; i < results.size(); i++) {
                cout << "Found: " << results[i] << "\n";
            }
        }
        cout << "\n";
        return results;
    }

    // shows info about a file or folder
    void fileInfo(string fileName) {
        for (int i = 0; i < currentDir->children.size(); i++) {
            FileNode* child = currentDir->children[i];
            if (child->name == fileName) {
                cout << "\n--- File Info ---\n";
                cout << "Name: " << child->name << "\n";

                if (child->isDirectory) {
                    cout << "Type: Directory\n";
                } else {
                    cout << "Type: File\n";
                }

                cout << "Size: " << child->content.length() << " bytes\n";
                cout << "Created: " << ctime(&child->createdTime);
                cout << "Modified: " << ctime(&child->modifiedTime);
                cout << "\n";
                return;
            }
        }
        throw FileNotFoundException(fileName);
    }

    // builds the current path by going up through parents
    string getCurrentPath() {
        string path = "";
        vector<string> pathParts;
        FileNode* node = currentDir;

        while (node != root) {
            pathParts.push_back(node->name);
            node = node->parent;
        }

        path = "/";

        // reverse order since we collected from bottom up
        for (int i = pathParts.size() - 1; i >= 0; i--) {
            path = path + pathParts[i];
            if (i > 0) {
                path = path + "/";
            }
        }

        return path;
    }

    // shows statistics about the file system
    void displayStats() {
        int fileCount = 0;
        int dirCount = 0;
        int totalSize = 0;

        countStats(root, fileCount, dirCount, totalSize);

        cout << "\n--- File System Statistics ---\n";
        cout << "Total Files: " << fileCount << "\n";
        cout << "Total Directories: " << dirCount << "\n";
        cout << "Total Size: " << totalSize << " bytes\n";
        cout << "\n";
    }

private:
    // recursively searches for files matching target name
    void searchHelper(FileNode* node, string target, vector<string>& results, string path) {
        if (node->name.find(target) != string::npos && !node->isDirectory) {
            results.push_back(path + node->name);
        }

        for (int i = 0; i < node->children.size(); i++) {
            searchHelper(node->children[i], target, results, path + node->name + "/");
        }
    }

    // recursively counts files, dirs, and total size
    void countStats(FileNode* node, int& files, int& dirs, int& size) {
        if (node->isDirectory) {
            dirs++;
            for (int i = 0; i < node->children.size(); i++) {
                countStats(node->children[i], files, dirs, size);
            }
        } else {
            files++;
            size = size + node->content.length();
        }
    }
};

#endif
