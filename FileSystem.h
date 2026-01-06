// FileSystem.h - Header file for our file system simulator
// This creates a fake file system in memory using a tree structure

#ifndef FILESYSTEM_H  // prevents including this file twice
#define FILESYSTEM_H  // marks that we've included it

#include <iostream>   // for cout and cin
#include <vector>     // for dynamic arrays
#include <string>     // for string type
#include <ctime>      // for timestamps
#include <fstream>    // for writing HTML file
#include <cstdlib>    // for system() to open browser

using namespace std;  // so we don't have to type std:: everywhere

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
    bool createFile(string fileName, string content = "") {
        // first check if a file with this name already exists
        for (int i = 0; i < currentDir->children.size(); i++) {
            if (currentDir->children[i]->name == fileName) {
                cout << "Error: File already exists\n";  // tell user it exists
                return false;                             // return failure
            }
        }

        // create the new file node
        FileNode* newFile = new FileNode(fileName, false, currentDir);
        newFile->content = content;                // set initial content
        currentDir->children.push_back(newFile);   // add to current folder
        cout << "File '" << fileName << "' created\n";  // confirm success
        return true;                                     // return success
    }

    // Creates a new folder in the current folder
    bool createDirectory(string dirName) {
        // check if folder already exists
        for (int i = 0; i < currentDir->children.size(); i++) {
            if (currentDir->children[i]->name == dirName) {
                cout << "Error: Directory already exists\n";
                return false;
            }
        }

        // create new folder node
        FileNode* newDir = new FileNode(dirName, true, currentDir);
        currentDir->children.push_back(newDir);  // add to current folder
        cout << "Directory '" << dirName << "' created\n";
        return true;
    }

    // Changes which folder we're currently in
    bool changeDirectory(string dirName) {
        // if user types ".." go to parent folder
        if (dirName == "..") {
            if (currentDir->parent != nullptr) {  // make sure we have a parent
                currentDir = currentDir->parent;  // move to parent
                cout << "Changed to parent directory\n";
                return true;
            } else {
                cout << "Already at root\n";  // can't go above root
                return false;
            }
        }

        // if user types "/" go to root
        if (dirName == "/") {
            currentDir = root;
            cout << "Changed to root\n";
            return true;
        }

        // otherwise look for the folder by name
        for (int i = 0; i < currentDir->children.size(); i++) {
            FileNode* child = currentDir->children[i];  // get each child
            // check if it matches and is a directory
            if (child->name == dirName && child->isDirectory) {
                currentDir = child;  // move into that folder
                cout << "Changed to directory '" << dirName << "'\n";
                return true;
            }
        }

        cout << "Error: Directory not found\n";  // couldn't find it
        return false;
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
    bool writeFile(string fileName, string content) {
        // search for the file in current folder
        for (int i = 0; i < currentDir->children.size(); i++) {
            FileNode* child = currentDir->children[i];
            // check if it matches and is a file (not folder)
            if (child->name == fileName && !child->isDirectory) {
                child->content = content;        // update the content
                child->modifiedTime = time(0);   // update modified time
                cout << "File '" << fileName << "' written (";
                cout << content.length() << " bytes)\n";
                return true;
            }
        }
        cout << "Error: File not found\n";
        return false;
    }

    // Reads and displays a file's content
    bool readFile(string fileName) {
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
                return true;
            }
        }
        cout << "Error: File not found\n";
        return false;
    }

    // Deletes a file or empty folder
    bool deleteFile(string fileName) {
        // search for the file/folder
        for (int i = 0; i < currentDir->children.size(); i++) {
            if (currentDir->children[i]->name == fileName) {
                FileNode* toDelete = currentDir->children[i];

                // if it's a folder, make sure it's empty
                if (toDelete->isDirectory && toDelete->children.size() > 0) {
                    cout << "Error: Directory not empty\n";
                    return false;
                }

                delete toDelete;  // free the memory
                // remove from the children vector
                currentDir->children.erase(currentDir->children.begin() + i);
                cout << "'" << fileName << "' deleted\n";
                return true;
            }
        }
        cout << "Error: File/Directory not found\n";
        return false;
    }

    // Searches for files by name in the whole file system
    void searchFile(string fileName) {
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
    }

    // Shows info about a file or folder
    bool fileInfo(string fileName) {
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
                return true;
            }
        }
        cout << "Error: File not found\n";
        return false;
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

    // Shows a visual tree of the file system
    void displayTree() {
        cout << "\n";
        cout << "============================================\n";
        cout << "       FILE SYSTEM TREE STRUCTURE\n";
        cout << "============================================\n";
        cout << "Current Path: " << getCurrentPath() << "\n";
        cout << "============================================\n";

        printTreeHelper(root, "", true);  // print starting from root

        cout << "============================================\n";
        cout << "\n";
    }

    // Opens a graphical tree view in the browser
    void openTreeGUI() {
        string filename = "/tmp/filesystem_tree.html";  // where to save HTML
        ofstream htmlFile(filename);                     // create file for writing

        // check if file opened successfully
        if (!htmlFile.is_open()) {
            cout << "Error: Could not create HTML file\n";
            return;
        }

        // write the HTML code
        htmlFile << "<!DOCTYPE html>\n";
        htmlFile << "<html>\n<head>\n";
        htmlFile << "<title>File System Tree</title>\n";

        // write CSS styles for the page
        htmlFile << "<style>\n";
        htmlFile << "body { font-family: monospace; background: #1e1e1e; color: #d4d4d4; padding: 20px; }\n";
        htmlFile << ".container { max-width: 900px; margin: 0 auto; background: #252526; padding: 20px; border-radius: 8px; }\n";
        htmlFile << "h1 { color: #4ec9b0; }\n";
        htmlFile << ".current-path { color: #dcdcaa; margin-bottom: 20px; }\n";
        htmlFile << ".tree { font-size: 14px; line-height: 1.6; }\n";
        htmlFile << ".folder { color: #4ec9b0; font-weight: bold; }\n";
        htmlFile << ".file { color: #ce9178; }\n";
        htmlFile << ".current { background: #264f78; padding: 2px 4px; }\n";
        htmlFile << ".size { color: #858585; font-size: 12px; }\n";
        htmlFile << ".tree-line { color: #569cd6; }\n";
        htmlFile << "</style>\n</head>\n<body>\n";

        // write the container and title
        htmlFile << "<div class=\"container\">\n";
        htmlFile << "<h1>File System Tree Structure</h1>\n";
        htmlFile << "<div class=\"current-path\">Current: " << getCurrentPath() << "</div>\n";
        htmlFile << "<div class=\"tree\">\n";

        // generate the tree HTML
        generateTreeHTML(root, "", htmlFile, true);

        // close HTML tags
        htmlFile << "</div>\n";
        htmlFile << "</div>\n";
        htmlFile << "</body>\n</html>\n";
        htmlFile.close();  // close the file

        // open the file in browser (works on Mac)
        string command = "open " + filename;
        system(command.c_str());  // run the command

        cout << "Tree view opened in browser\n";
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

    // Helper function to print the tree structure
    void printTreeHelper(FileNode* node, string prefix, bool isLast) {
        if (node == nullptr) return;  // safety check

        // print the connector line
        cout << prefix;
        if (prefix.length() > 0) {
            cout << "+-- ";
        }

        // print [DIR] or [FILE] with the name
        if (node->isDirectory) {
            cout << "[DIR] " << node->name;
            // mark if this is the current directory
            if (node == currentDir) {
                cout << " <-- you are here";
            }
        } else {
            cout << "[FILE] " << node->name;
            // show file size if it has content
            if (node->content.length() > 0) {
                cout << " (" << node->content.length() << " bytes)";
            }
        }
        cout << "\n";

        // print all children
        for (int i = 0; i < node->children.size(); i++) {
            // figure out what prefix to use for children
            string newPrefix = prefix;
            if (prefix.length() > 0) {
                newPrefix = newPrefix + "    ";  // indent
            }

            // check if this is the last child
            bool lastChild = (i == node->children.size() - 1);
            printTreeHelper(node->children[i], newPrefix, lastChild);
        }
    }

    // Helper function to generate HTML for the tree
    void generateTreeHTML(FileNode* node, string prefix, ofstream& htmlFile, bool isRoot) {
        if (node == nullptr) return;  // safety check

        // print connector
        string connector = "";
        if (!isRoot) {
            connector = "+-- ";
        }

        htmlFile << prefix << "<span class=\"tree-line\">" << connector << "</span>";

        // check if this is where we are
        bool isCurrent = (node == currentDir);

        // set class name based on type
        string className = "";
        string icon = "";
        if (node->isDirectory) {
            className = "folder";
            icon = "[DIR] ";
        } else {
            className = "file";
            icon = "[FILE] ";
        }

        // write the node with appropriate styling
        if (isCurrent) {
            htmlFile << "<span class=\"" << className << " current\">";
            htmlFile << icon << node->name << " *</span>";
        } else {
            htmlFile << "<span class=\"" << className << "\">";
            htmlFile << icon << node->name << "</span>";
        }

        // show file size
        if (!node->isDirectory && node->content.length() > 0) {
            htmlFile << " <span class=\"size\">(";
            htmlFile << node->content.length() << " bytes)</span>";
        }

        htmlFile << "<br>\n";

        // process children
        if (node->isDirectory && node->children.size() > 0) {
            // make new prefix with more indentation
            string newPrefix = prefix;
            if (!isRoot) {
                newPrefix = newPrefix + "&nbsp;&nbsp;&nbsp;&nbsp;";  // HTML spaces
            }

            // generate HTML for each child
            for (int i = 0; i < node->children.size(); i++) {
                generateTreeHTML(node->children[i], newPrefix, htmlFile, false);
            }
        }
    }
};

#endif  // end of include guard
