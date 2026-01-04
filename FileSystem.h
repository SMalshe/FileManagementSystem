/**
 * @file FileSystem.h
 * @brief In-memory file system implementation using tree structure
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <cstdlib>

using namespace std;

/**
 * @struct FileNode
 * @brief Represents a file or directory node in the file system tree
 * 
 * Each node stores its name, type (file/directory), content (for files),
 * timestamps, parent pointer, and children (for directories).
 */
struct FileNode {
    string name;                    // Name of the file or directory
    bool isDirectory;               // True for directories, false for files
    string content;                 // File contents (empty for directories)
    time_t createdTime;             // Creation timestamp
    time_t modifiedTime;            // Last modification timestamp
    FileNode* parent;               // Pointer to parent directory (nullptr for root)
    vector<FileNode*> children;     // Child nodes (used only for directories)

    /**
     * @brief Constructs a new file or directory node
     * @param n Name of the file/directory
     * @param isDir True if directory, false if file
     * @param p Pointer to parent directory (nullptr for root)
     */
    FileNode(string n, bool isDir, FileNode* p = nullptr)
        : name(n), isDirectory(isDir), content(""), parent(p) {
        createdTime = time(0);
        modifiedTime = time(0);
    }

    /**
     * @brief Destructor that recursively deletes all children
     */
    ~FileNode() {
        for(auto child : children) {
            delete child;
        }
    }
};

/**
 * @class FileSystem
 * @brief Manages the file system tree structure and operations
 * 
 * Maintains a root directory, current working directory, and hash map
 * index for O(1) file lookups by path.
 */
class FileSystem {
private:
    FileNode* root;                              // Root directory node
    FileNode* currentDir;                        // Current working directory
    unordered_map<string, FileNode*> fileIndex;  // Path to FileNode mapping for fast lookup

public:
    /**
     * @brief Initializes file system with empty root directory
     */
    FileSystem() {
        root = new FileNode("root", true);
        currentDir = root;
        fileIndex["root"] = root;
    }

    /**
     * @brief Creates a new file in the current directory
     * @param fileName Name of the file to create
     * @param content Optional initial content (default empty)
     * @return true on success, false if file already exists
     */
    bool createFile(string fileName, string content = "") {
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

    /**
     * @brief Creates a new directory in the current directory
     * @param dirName Name of the directory to create
     * @return true on success, false if directory already exists
     */
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

    /**
     * @brief Changes current working directory
     * @param dirName Target directory name, ".." for parent, or "/" for root
     * @return true on success, false if directory not found
     */
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

    /**
     * @brief Lists all files and directories in current directory
     * 
     * Displays formatted list with [DIR] and [FILE] prefixes, sorted
     * alphabetically. Shows file sizes for non-empty files
     */
    void listDirectory() {
        cout << "\n--- Directory: " << getCurrentPath() << " ---\n";
        cout << "[DIR]  ..\n";
        cout << "[DIR]  .\n";

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

    /**
     * @brief Writes content to an existing file, overwriting previous content
     * @param fileName Name of the file to write
     * @param content Content to write
     * @return true on success, false if file not found
     */
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

    /**
     * @brief Reads and displays file contents
     * @param fileName Name of the file to read
     * @return true on success, false if file not found
     */
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

    /**
     * @brief Deletes a file or empty directory
     * @param fileName Name of the file or directory to delete
     * @return true on success, false if not found or directory not empty
     */
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

    /**
     * @brief Searches for files by name throughout the file system
     * 
     * Performs recursive search from root, using substring matching.
     * Case-sensitive search.
     * 
     * @param fileName Search term to match against file names
     */
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

    /**
     * @brief Displays metadata for a file or directory
     * @param fileName Name of the file or directory
     * @return true on success, false if not found
     */
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

    /**
     * @brief Returns the full path of the current working directory
     * @return Path string (e.g., "/root/folder/subfolder")
     */
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

    /**
     * @brief Displays file system statistics
     * 
     * Shows total file count, directory count, total size in bytes,
     * and number of indexed entries.
     */
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

    /**
     * @brief Displays a visual tree diagram of the entire file system
     * 
     * Shows the hierarchical structure with a graphical UI including:
     * - ANSI color codes for directories (blue) and files (white/green)
     * - Box drawing characters for tree structure
     * - Current directory highlighting
     * - File size information
     */
    void displayTree() {
        cout << "\n";
        cout << "============================================================\n";
        cout << "           FILE SYSTEM TREE STRUCTURE\n";
        cout << "============================================================\n";
        cout << "Current Path: " << getCurrentPath() << "\n";
        cout << "============================================================\n";
        vector<bool> isLast;
        printTreeHelper(root, "", isLast, true);
        cout << "============================================================\n";
        cout << "\n";
    }

    /**
     * @brief Opens a GUI window showing the file system tree
     * 
     * Creates an HTML file with the tree structure and opens it in the browser
     */
    void openTreeGUI() {
        string filename = "/tmp/filesystem_tree.html";
        ofstream htmlFile(filename);
        
        if(!htmlFile.is_open()) {
            cout << "Error: Could not create HTML file\n";
            return;
        }

        // Write HTML header
        htmlFile << "<!DOCTYPE html>\n";
        htmlFile << "<html>\n<head>\n";
        htmlFile << "<title>File System Tree</title>\n";
        htmlFile << "<style>\n";
        htmlFile << "body { font-family: 'Courier New', monospace; background: #1e1e1e; color: #d4d4d4; padding: 20px; }\n";
        htmlFile << ".container { max-width: 900px; margin: 0 auto; background: #252526; padding: 20px; border-radius: 8px; }\n";
        htmlFile << "h1 { color: #4ec9b0; margin-bottom: 10px; }\n";
        htmlFile << ".current-path { color: #dcdcaa; margin-bottom: 20px; font-size: 14px; }\n";
        htmlFile << ".tree { font-size: 14px; line-height: 1.6; }\n";
        htmlFile << ".folder { color: #4ec9b0; font-weight: bold; }\n";
        htmlFile << ".file { color: #ce9178; }\n";
        htmlFile << ".current { background: #264f78; padding: 2px 4px; border-radius: 3px; }\n";
        htmlFile << ".size { color: #858585; font-size: 12px; }\n";
        htmlFile << ".tree-line { color: #569cd6; }\n";
        htmlFile << "</style>\n</head>\n<body>\n";
        htmlFile << "<div class=\"container\">\n";
        htmlFile << "<h1>File System Tree Structure</h1>\n";
        htmlFile << "<div class=\"current-path\">Current: " << getCurrentPath() << "</div>\n";
        htmlFile << "<div class=\"tree\">\n";

        // Generate tree HTML
        generateTreeHTML(root, "", htmlFile, true);

        htmlFile << "</div>\n";
        htmlFile << "</div>\n";
        htmlFile << "</body>\n</html>\n";
        htmlFile.close();

        // Open in browser (macOS command)
        string command = "open " + filename;
        system(command.c_str());
        
        cout << "✓ Tree view opened in browser\n";
    }

private:
    /**
     * @brief Recursive helper function for file search
     * @param node Current node to search
     * @param target Search term
     * @param results Vector to store matching paths
     * @param path Current path string being built
     */
    void searchHelper(FileNode* node, string target, vector<string>& results, string path) {
        if(node->name.find(target) != string::npos && !node->isDirectory) {
            results.push_back(path + node->name);
        }

        for(auto child : node->children) {
            searchHelper(child, target, results, path + node->name + "/");
        }
    }

    /**
     * @brief Recursively counts files, directories, and total size
     * @param node Starting node for count
     * @param files Reference to file counter (modified by function)
     * @param dirs Reference to directory counter (modified by function)
     * @param size Reference to size counter (modified by function)
     */
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

    /**
     * @brief Constructs full path for a file in current directory
     * @param fileName File name
     * @return Full path string
     */
    string getFullPath(string fileName) {
        return getCurrentPath() + "/" + fileName;
    }

    /**
     * @brief ANSI color code helper - resets formatting
     */
    string resetColor() {
        return "\033[0m";
    }

    /**
     * @brief ANSI color code helper - blue for directories
     */
    string dirColor() {
        return "\033[1;34m"; // Bold blue
    }

    /**
     * @brief ANSI color code helper - green for files
     */
    string fileColor() {
        return "\033[1;32m"; // Bold green
    }

    /**
     * @brief ANSI color code helper - yellow for current directory
     */
    string currentDirColor() {
        return "\033[1;33m"; // Bold yellow
    }

    /**
     * @brief ANSI color code helper - cyan for tree connectors
     */
    string treeColor() {
        return "\033[0;36m"; // Cyan
    }

    /**
     * @brief Recursive helper function to print tree structure with graphical formatting
     * @param node Current node to print
     * @param prefix Prefix string for tree formatting (indentation and connectors)
     * @param isLast Vector tracking which ancestors are last children
     * @param isRoot True if this is the root node
     */
    void printTreeHelper(FileNode* node, string prefix, vector<bool>& isLast, bool isRoot) {
        if(!node) return;

        // Determine connector and tree structure
        string connector = "";
        string colorConnector = treeColor();
        if(!isRoot) {
            if(isLast.back()) {
                connector = "+-- ";
            } else {
                connector = "+-- ";
            }
        }

        // Print prefix and connector with color
        cout << prefix << colorConnector << connector << resetColor();

        // Determine colors based on node type and current directory
        bool isCurrent = (node == currentDir);
        string nameColor = "";
        string icon = "";

        if(node->isDirectory) {
            nameColor = isCurrent ? currentDirColor() : dirColor();
            icon = "[DIR] ";
        } else {
            nameColor = fileColor();
            icon = "[FILE] ";
        }

        // Print node name with icon and color
        cout << nameColor << icon << node->name;
        
        // Add indicator for current directory
        if(isCurrent) {
            cout << " *";
        }
        cout << resetColor();

        // Show file size if it's a non-empty file
        if(!node->isDirectory && !node->content.empty()) {
            cout << " " << treeColor() << "(" << node->content.length() << " bytes)" << resetColor();
        }
        cout << resetColor() << "\n";

        // If this is a directory with children, recurse
        if(node->isDirectory && !node->children.empty()) {
            // Sort children: directories first, then files, both alphabetically
            vector<FileNode*> sorted = node->children;
            sort(sorted.begin(), sorted.end(),
                 [](FileNode* a, FileNode* b) {
                     if(a->isDirectory != b->isDirectory) {
                         return a->isDirectory > b->isDirectory; // Directories first
                     }
                     return a->name < b->name;
                 });

            // Update prefix for children with proper tree connectors
            string newPrefix = prefix;
            if(!isRoot) {
                if(isLast.back()) {
                    newPrefix += "    ";  // Space for last child
                } else {
                    newPrefix += treeColor() + "|   " + resetColor();  // Vertical line for non-last
                }
            }

            // Recursively print children
            for(size_t i = 0; i < sorted.size(); i++) {
                isLast.push_back(i == sorted.size() - 1);
                printTreeHelper(sorted[i], newPrefix, isLast, false);
                isLast.pop_back();
            }
        }
    }

    /**
     * @brief Helper function to generate HTML for tree structure
     * @param node Current node
     * @param prefix HTML prefix for indentation
     * @param htmlFile Output file stream
     * @param isRoot True if root node
     */
    void generateTreeHTML(FileNode* node, string prefix, ofstream& htmlFile, bool isRoot) {
        if(!node) return;

        string connector = "";
        if(!isRoot) {
            connector = "+-- ";
        }

        htmlFile << prefix << "<span class=\"tree-line\">" << connector << "</span>";

        bool isCurrent = (node == currentDir);
        string className = node->isDirectory ? "folder" : "file";
        string icon = node->isDirectory ? "[DIR] " : "[FILE] ";

        if(isCurrent) {
            htmlFile << "<span class=\"" << className << " current\">" << icon << node->name << " *</span>";
        } else {
            htmlFile << "<span class=\"" << className << "\">" << icon << node->name << "</span>";
        }

        if(!node->isDirectory && !node->content.empty()) {
            htmlFile << " <span class=\"size\">(" << node->content.length() << " bytes)</span>";
        }

        htmlFile << "<br>\n";

        if(node->isDirectory && !node->children.empty()) {
            vector<FileNode*> sorted = node->children;
            sort(sorted.begin(), sorted.end(),
                 [](FileNode* a, FileNode* b) {
                     if(a->isDirectory != b->isDirectory) {
                         return a->isDirectory > b->isDirectory;
                     }
                     return a->name < b->name;
                 });

            string newPrefix = prefix + (isRoot ? "" : "&nbsp;&nbsp;&nbsp;&nbsp;");

            for(auto child : sorted) {
                generateTreeHTML(child, newPrefix, htmlFile, false);
            }
        }
    }

public:
    /**
     * @brief Destructor that deletes root and all descendants
     */
    ~FileSystem() {
        delete root;
    }
};

#endif
