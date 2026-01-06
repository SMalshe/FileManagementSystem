// tests.cpp - Test file for the FileSystem class
// Tests all functions and exception handling

#include <iostream>
#include <string>
#include "FileSystem.h"

using namespace std;

int testsPassed = 0;
int testsFailed = 0;

// only prints if test fails
void check(string testName, string subtest, bool passed) {
    if (passed) {
        testsPassed++;
    } else {
        cout << "[FAIL] " << testName << " - " << subtest << "\n";
        testsFailed++;
    }
}

// TEST: Constructor
void testConstructor() {
    string test = "Constructor";
    FileSystem fs;
    check(test, "should start at root path /", fs.getCurrentPath() == "/");
}

// TEST: createFile
void testCreateFile() {
    string test = "createFile";
    FileSystem fs;

    // test 1: create new file should work
    bool worked = true;
    try {
        fs.createFile("test.txt");
    } catch (...) {
        worked = false;
    }
    check(test, "should not throw for new file", worked);

    // test 2: duplicate file should throw AlreadyExistsException
    bool threw = false;
    try {
        fs.createFile("test.txt");
    } catch (AlreadyExistsException& e) {
        threw = true;
    } catch (...) {}
    check(test, "should throw AlreadyExistsException for duplicate", threw);

    // test 3: file with content should work
    worked = true;
    try {
        fs.createFile("data.txt", "hello");
    } catch (...) {
        worked = false;
    }
    check(test, "should work with initial content", worked);

    // test 4: empty name should throw InvalidNameException
    threw = false;
    try {
        fs.createFile("");
    } catch (InvalidNameException& e) {
        threw = true;
    } catch (...) {}
    check(test, "should throw InvalidNameException for empty name", threw);

    // test 5: name with / should throw InvalidNameException
    threw = false;
    try {
        fs.createFile("bad/name.txt");
    } catch (InvalidNameException& e) {
        threw = true;
    } catch (...) {}
    check(test, "should throw InvalidNameException for name with /", threw);
}

// TEST: createDirectory
void testCreateDirectory() {
    string test = "createDirectory";
    FileSystem fs;

    // test 1: create new folder should work
    bool worked = true;
    try {
        fs.createDirectory("folder1");
    } catch (...) {
        worked = false;
    }
    check(test, "should not throw for new folder", worked);

    // test 2: duplicate folder should throw
    bool threw = false;
    try {
        fs.createDirectory("folder1");
    } catch (AlreadyExistsException& e) {
        threw = true;
    } catch (...) {}
    check(test, "should throw AlreadyExistsException for duplicate", threw);

    // test 3: empty name should throw
    threw = false;
    try {
        fs.createDirectory("");
    } catch (InvalidNameException& e) {
        threw = true;
    } catch (...) {}
    check(test, "should throw InvalidNameException for empty name", threw);
}

// TEST: changeDirectory
void testChangeDirectory() {
    string test = "changeDirectory";
    FileSystem fs;

    fs.createDirectory("myFolder");

    // test 1: cd into folder should work
    bool worked = true;
    try {
        fs.changeDirectory("myFolder");
    } catch (...) {
        worked = false;
    }
    check(test, "should not throw when entering folder", worked);

    // test 2: path should update
    check(test, "path should be /myFolder", fs.getCurrentPath() == "/myFolder");

    // test 3: cd .. should work
    worked = true;
    try {
        fs.changeDirectory("..");
    } catch (...) {
        worked = false;
    }
    check(test, "should not throw for ..", worked);

    // test 4: should be back at root
    check(test, "path should be / after ..", fs.getCurrentPath() == "/");

    // test 5: cd .. at root should throw
    bool threw = false;
    try {
        fs.changeDirectory("..");
    } catch (DirectoryNotFoundException& e) {
        threw = true;
    } catch (...) {}
    check(test, "should throw DirectoryNotFoundException for .. at root", threw);

    // test 6: cd to nonexistent should throw
    threw = false;
    try {
        fs.changeDirectory("nonexistent");
    } catch (DirectoryNotFoundException& e) {
        threw = true;
    } catch (...) {}
    check(test, "should throw DirectoryNotFoundException for missing folder", threw);

    // test 7: cd / should work
    fs.changeDirectory("myFolder");
    worked = true;
    try {
        fs.changeDirectory("/");
    } catch (...) {
        worked = false;
    }
    check(test, "should not throw for /", worked);
    check(test, "path should be / after /", fs.getCurrentPath() == "/");
}

// TEST: writeFile
void testWriteFile() {
    string test = "writeFile";
    FileSystem fs;

    fs.createFile("notes.txt");

    // test 1: write to existing file should work
    bool worked = true;
    try {
        fs.writeFile("notes.txt", "my note");
    } catch (...) {
        worked = false;
    }
    check(test, "should not throw for existing file", worked);

    // test 2: write to missing file should throw
    bool threw = false;
    try {
        fs.writeFile("missing.txt", "content");
    } catch (FileNotFoundException& e) {
        threw = true;
    } catch (...) {}
    check(test, "should throw FileNotFoundException for missing file", threw);

    // test 3: write to directory should throw
    fs.createDirectory("myDir");
    threw = false;
    try {
        fs.writeFile("myDir", "content");
    } catch (FileNotFoundException& e) {
        threw = true;
    } catch (...) {}
    check(test, "should throw FileNotFoundException for directory", threw);
}

// TEST: readFile
void testReadFile() {
    string test = "readFile";
    FileSystem fs;

    fs.createFile("readme.txt");
    fs.writeFile("readme.txt", "hello");

    // test 1: read existing file should work
    bool worked = true;
    try {
        fs.readFile("readme.txt");
    } catch (...) {
        worked = false;
    }
    check(test, "should not throw for existing file", worked);

    // test 2: read missing file should throw
    bool threw = false;
    try {
        fs.readFile("missing.txt");
    } catch (FileNotFoundException& e) {
        threw = true;
    } catch (...) {}
    check(test, "should throw FileNotFoundException for missing file", threw);

    // test 3: read directory should throw
    fs.createDirectory("aDir");
    threw = false;
    try {
        fs.readFile("aDir");
    } catch (FileNotFoundException& e) {
        threw = true;
    } catch (...) {}
    check(test, "should throw FileNotFoundException for directory", threw);
}

// TEST: deleteFile
void testDeleteFile() {
    string test = "deleteFile";
    FileSystem fs;

    fs.createFile("toDelete.txt");
    fs.createDirectory("emptyFolder");
    fs.createDirectory("fullFolder");
    fs.changeDirectory("fullFolder");
    fs.createFile("inside.txt");
    fs.changeDirectory("..");

    // test 1: delete file should work
    bool worked = true;
    try {
        fs.deleteFile("toDelete.txt");
    } catch (...) {
        worked = false;
    }
    check(test, "should not throw for file", worked);

    // test 2: delete empty folder should work
    worked = true;
    try {
        fs.deleteFile("emptyFolder");
    } catch (...) {
        worked = false;
    }
    check(test, "should not throw for empty folder", worked);

    // test 3: delete non-empty folder should throw
    bool threw = false;
    try {
        fs.deleteFile("fullFolder");
    } catch (DirectoryNotEmptyException& e) {
        threw = true;
    } catch (...) {}
    check(test, "should throw DirectoryNotEmptyException for non-empty folder", threw);

    // test 4: delete missing item should throw
    threw = false;
    try {
        fs.deleteFile("notHere");
    } catch (FileNotFoundException& e) {
        threw = true;
    } catch (...) {}
    check(test, "should throw FileNotFoundException for missing item", threw);
}

// TEST: searchFile
void testSearchFile() {
    string test = "searchFile";
    FileSystem fs;

    fs.createFile("report.txt");
    fs.createFile("report2.txt");
    fs.createDirectory("docs");
    fs.changeDirectory("docs");
    fs.createFile("report3.txt");
    fs.changeDirectory("..");

    // searchFile returns vector of results
    vector<string> results = fs.searchFile("report");
    check(test, "should find 3 files with 'report'", results.size() == 3);
}

// TEST: fileInfo
void testFileInfo() {
    string test = "fileInfo";
    FileSystem fs;

    fs.createFile("info.txt");
    fs.writeFile("info.txt", "content");
    fs.createDirectory("infoDir");

    // test 1: info for file should work
    bool worked = true;
    try {
        fs.fileInfo("info.txt");
    } catch (...) {
        worked = false;
    }
    check(test, "should not throw for file", worked);

    // test 2: info for directory should work
    worked = true;
    try {
        fs.fileInfo("infoDir");
    } catch (...) {
        worked = false;
    }
    check(test, "should not throw for directory", worked);

    // test 3: info for missing should throw
    bool threw = false;
    try {
        fs.fileInfo("missing");
    } catch (FileNotFoundException& e) {
        threw = true;
    } catch (...) {}
    check(test, "should throw FileNotFoundException for missing item", threw);
}

// TEST: getCurrentPath
void testGetCurrentPath() {
    string test = "getCurrentPath";
    FileSystem fs;

    check(test, "should return / at root", fs.getCurrentPath() == "/");

    fs.createDirectory("level1");
    fs.changeDirectory("level1");
    check(test, "should return /level1", fs.getCurrentPath() == "/level1");

    fs.createDirectory("level2");
    fs.changeDirectory("level2");
    check(test, "should return /level1/level2", fs.getCurrentPath() == "/level1/level2");
}

// TEST: displayStats
void testDisplayStats() {
    string test = "displayStats";
    FileSystem fs;

    fs.createFile("file1.txt");
    fs.writeFile("file1.txt", "hello");
    fs.createDirectory("dir1");

    bool worked = true;
    try {
        fs.displayStats();
    } catch (...) {
        worked = false;
    }
    check(test, "should not throw", worked);
}

// TEST: listDirectory
void testListDirectory() {
    string test = "listDirectory";
    FileSystem fs;

    fs.createFile("alpha.txt");
    fs.createDirectory("beta");

    bool worked = true;
    try {
        fs.listDirectory();
    } catch (...) {
        worked = false;
    }
    check(test, "should not throw", worked);
}

// TEST: nested directories
void testNestedDirectories() {
    string test = "Nested Directories";
    FileSystem fs;

    fs.createDirectory("a");
    fs.changeDirectory("a");
    fs.createDirectory("b");
    fs.changeDirectory("b");
    fs.createDirectory("c");
    fs.changeDirectory("c");

    check(test, "path should be /a/b/c", fs.getCurrentPath() == "/a/b/c");

    bool worked = true;
    try {
        fs.createFile("deep.txt");
    } catch (...) {
        worked = false;
    }
    check(test, "should create file in nested dir", worked);

    fs.changeDirectory("..");
    fs.changeDirectory("..");
    fs.changeDirectory("..");
    check(test, "should navigate back to /", fs.getCurrentPath() == "/");
}

// TEST: edge cases
void testEdgeCases() {
    string test = "Edge Cases";
    FileSystem fs;

    // file with spaces
    bool worked = true;
    try {
        fs.createFile("my file.txt");
    } catch (...) {
        worked = false;
    }
    check(test, "should allow spaces in name", worked);

    // file with underscores
    worked = true;
    try {
        fs.createFile("data_2024.log");
    } catch (...) {
        worked = false;
    }
    check(test, "should allow underscores in name", worked);

    // overwrite content
    fs.createFile("overwrite.txt");
    fs.writeFile("overwrite.txt", "first");
    worked = true;
    try {
        fs.writeFile("overwrite.txt", "second");
    } catch (...) {
        worked = false;
    }
    check(test, "should allow overwriting content", worked);

    // empty content
    fs.createFile("empty.txt");
    worked = true;
    try {
        fs.writeFile("empty.txt", "");
    } catch (...) {
        worked = false;
    }
    check(test, "should allow empty content", worked);
}

// TEST: exception messages
void testExceptionMessages() {
    string test = "Exception Messages";
    FileSystem fs;

    // FileNotFoundException should include filename
    try {
        fs.readFile("nonexistent.txt");
    } catch (FileNotFoundException& e) {
        string msg = e.what();
        check(test, "FileNotFoundException should include filename",
              msg.find("nonexistent.txt") != string::npos);
    } catch (...) {
        check(test, "should throw FileNotFoundException", false);
    }

    // AlreadyExistsException should include name
    fs.createFile("exists.txt");
    try {
        fs.createFile("exists.txt");
    } catch (AlreadyExistsException& e) {
        string msg = e.what();
        check(test, "AlreadyExistsException should include name",
              msg.find("exists.txt") != string::npos);
    } catch (...) {
        check(test, "should throw AlreadyExistsException", false);
    }
}

int main() {
    testConstructor();
    testCreateFile();
    testCreateDirectory();
    testChangeDirectory();
    testWriteFile();
    testReadFile();
    testDeleteFile();
    testSearchFile();
    testFileInfo();
    testGetCurrentPath();
    testDisplayStats();
    testListDirectory();
    testNestedDirectories();
    testEdgeCases();
    testExceptionMessages();

    cout << "\n============================================\n";
    cout << "      TEST SUMMARY\n";
    cout << "============================================\n";
    cout << "Tests Passed: " << testsPassed << "\n";
    cout << "Tests Failed: " << testsFailed << "\n";
    cout << "Total Tests:  " << (testsPassed + testsFailed) << "\n";

    if (testsFailed == 0) {
        cout << "\nAll tests passed!\n";
    } else {
        cout << "\nSome tests failed. See failures above.\n";
    }

    return 0;
}
