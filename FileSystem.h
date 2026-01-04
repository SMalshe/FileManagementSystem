#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <ctime>
#include <algorithm>

using namespace std;

// Represents a file or directory in the system
struct FileNode {
    string name;
    bool isDirectory;
    string content;
    time_t createdTime;
    time_t modifiedTime;
    FileNode* parent;
    vector<FileNode*> children;  // For directories

    FileNode(string n, bool isDir, FileNode* p = nullptr)
        : name(n), isDirectory(isDir), content(""), parent(p) {
        createdTime = time(0);
        modifiedTime = time(0);
    }

    ~FileNode() {
        for(auto child : children) {
            delete child;
        }
    }
};

class FileSystem {
private:
    FileNode* root;
    FileNode* currentDir;
    unordered_map<string, FileNode*> fileIndex;  

public:
    FileSystem() {
        root = new FileNode("root", true);
        currentDir = root;
        fileIndex["root"] = root;
    }

    // Create a new file
    bool createFile(string fileName, string content = "") {
        // Check if file already exists (O(1) with hash table)
        string fullPath = getFullPath(fileName);
        if(fileIndex.find(fullPath) != fileIndex.end()) {
            cout << "Error: File already exists\n";
            return false;
        }

        FileNode* newFile = new FileNode(fileName, false, currentDir);
        newFile->content = content;
        currentDir->children.push_back(newFile);
        fileIndex[fullPath] = newFile;
        cout << "✓ File '" << fileName << "' created\n";
        return true;
    }

    // Create a new directory
    bool createDirectory(string dirName) {
        string fullPath = getFullPath(dirName);
        if(fileIndex.find(fullPath) != fileIndex.end()) {
            cout << "Error: Directory already exists\n";
            return false;
        }

        FileNode* newDir = new FileNode(dirName, true, currentDir);
        currentDir->children.push_back(newDir);
        fileIndex[fullPath] = newDir;
        cout << "✓ Directory '" << dirName << "' created\n";
        return true;
    }

    // Change directory
    bool changeDirectory(string dirName) {
        if(dirName == "..") {
            if(currentDir->parent) {
                currentDir = currentDir->parent;
                cout << "✓ Changed to parent directory\n";
                return true;
            } else {
                cout << "Already at root\n";
                return false;
            }
        }

        if(dirName == "/") {
            currentDir = root;
            cout << "✓ Changed to root\n";
            return true;
        }

        for(auto child : currentDir->children) {
            if(child->name == dirName && child->isDirectory) {
                currentDir = child;
                cout << "✓ Changed to directory '" << dirName << "'\n";
                return true;
            }
        }

        cout << "Error: Directory not found\n";
        return false;
    }

    // List files and directories (using vector traversal)
    void listDirectory() {
        cout << "\n--- Directory: " << getCurrentPath() << " ---\n";
        cout << "[DIR]  ..\n";
        cout << "[DIR]  .\n";

        // Sort children alphabetically
        vector<FileNode*> sorted = currentDir->children;
        sort(sorted.begin(), sorted.end(),
             [](FileNode* a, FileNode* b) { return a->name < b->name; });

        if(sorted.empty()) {
            cout << "(empty)\n";
        } else {
            for(auto child : sorted) {
                cout << (child->isDirectory ? "[DIR]  " : "[FILE] ");
                cout << child->name;
                if(!child->isDirectory && !child->content.empty()) {
                    cout << " (" << child->content.length() << " bytes)";
                }
                cout << "\n";
            }
        }
        cout << "\n";
    }

    // Write content to file
    bool writeFile(string fileName, string content) {
        for(auto child : currentDir->children) {
            if(child->name == fileName && !child->isDirectory) {
                child->content = content;
                child->modifiedTime = time(0);
                cout << "✓ File '" << fileName << "' written ("
                     << content.length() << " bytes)\n";
                return true;
            }
        }
        cout << "Error: File not found\n";
        return false;
    }

    // Read file content
    bool readFile(string fileName) {
        for(auto child : currentDir->children) {
            if(child->name == fileName && !child->isDirectory) {
                cout << "\n--- Content of " << fileName << " ---\n";
                cout << (child->content.empty() ? "(empty)" : child->content);
                cout << "\n\n";
                return true;
            }
        }
        cout << "Error: File not found\n";
        return false;
    }

    // Delete file or directory
    bool deleteFile(string fileName) {
        for(size_t i = 0; i < currentDir->children.size(); i++) {
            if(currentDir->children[i]->name == fileName) {
                if(currentDir->children[i]->isDirectory &&
                   !currentDir->children[i]->children.empty()) {
                    cout << "Error: Directory not empty\n";
                    return false;
                }
                delete currentDir->children[i];
                currentDir->children.erase(currentDir->children.begin() + i);
                cout << "✓ '" << fileName << "' deleted\n";
                return true;
            }
        }
        cout << "Error: File/Directory not found\n";
        return false;
    }

    // Search for file by name (uses vector linear search)
    void searchFile(string fileName) {
        cout << "Searching for '" << fileName << "'...\n";
        vector<string> results;
        searchHelper(root, fileName, results, "");

        if(results.empty()) {
            cout << "No files found\n";
        } else {
            for(auto path : results) {
                cout << "Found: " << path << "\n";
            }
        }
        cout << "\n";
    }

    // Display file info
    bool fileInfo(string fileName) {
        for(auto child : currentDir->children) {
            if(child->name == fileName) {
                cout << "\n--- File Info ---\n";
                cout << "Name: " << child->name << "\n";
                cout << "Type: " << (child->isDirectory ? "Directory" : "File") << "\n";
                cout << "Size: " << child->content.length() << " bytes\n";
                cout << "Created: " << ctime(&child->createdTime);
                cout << "Modified: " << ctime(&child->modifiedTime);
                cout << "\n";
                return true;
            }
        }
        cout << "Error: File not found\n";
        return false;
    }

    // Get current working directory path
    string getCurrentPath() {
        string path = "";
        vector<string> pathParts;
        FileNode* node = currentDir;

        while(node != root) {
            pathParts.push_back(node->name);
            node = node->parent;
        }

        path = "/";
        for(int i = pathParts.size() - 1; i >= 0; i--) {
            path += pathParts[i];
            if(i > 0) path += "/";
        }
        return path;
    }

    // Display file system statistics
    void displayStats() {
        int fileCount = 0, dirCount = 0, totalSize = 0;
        countStats(root, fileCount, dirCount, totalSize);

        cout << "\n--- File System Statistics ---\n";
        cout << "Total Files: " << fileCount << "\n";
        cout << "Total Directories: " << dirCount << "\n";
        cout << "Total Size: " << totalSize << " bytes\n";
        cout << "Indexed Files: " << fileIndex.size() << " (Hash Table)\n";
        cout << "\n";
    }

private:
    void searchHelper(FileNode* node, string target, vector<string>& results, string path) {
        if(node->name.find(target) != string::npos && !node->isDirectory) {
            results.push_back(path + node->name);
        }

        for(auto child : node->children) {
            searchHelper(child, target, results, path + node->name + "/");
        }
    }

    void countStats(FileNode* node, int& files, int& dirs, int& size) {
        if(node->isDirectory) {
            dirs++;
            for(auto child : node->children) {
                countStats(child, files, dirs, size);
            }
        } else {
            files++;
            size += node->content.length();
        }
    }

    string getFullPath(string fileName) {
        return getCurrentPath() + "/" + fileName;
    }

public:
    ~FileSystem() {
        delete root;
    }
};

#endif