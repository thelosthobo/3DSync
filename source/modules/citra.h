#include <stdio.h>
#include <malloc.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>

#include <sys/stat.h>

#include "dropbox.h"

std::string getUpdateTimestampForCitraSave(std::string dropboxToken, std::string fullPath) {
    Dropbox dropbox(dropboxToken);
    auto results = dropbox.list_folder(fullPath + "/data/00000001");
    for (auto lr : results) {
        if (lr.name == "user1") {
            return lr.server_modified;
        }
    }
    return "";
}

std::string checkpointDirToCitraGameCode(std::string checkpointGameSaveDir) {
    if (checkpointGameSaveDir.rfind("0x", 0) == 0) {
        std::string gameCode = "0" + checkpointGameSaveDir.substr(2,5) + "00";
        for (auto &elem : gameCode) {
            elem = std::tolower(elem);
        }
        return gameCode;
    }
    return "";
}

std::map<std::string, std::string> findCheckpointSaves(std::string checkpointPath) {
    std::map<std::string, std::string> pathmap;

    std::string path(checkpointPath + "/saves");
    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(path.c_str())) != NULL){
        while((ent = readdir(dir)) != NULL){
            std::string dirname = ent->d_name;

            if (dirname == "." || dirname == "..") {
                continue;
            }

            std::string readpath(path + "/" + dirname);
            // std::cout << readpath << std::endl;

            std::string gameCode = checkpointDirToCitraGameCode(dirname);
            if (gameCode != "") {
                pathmap[gameCode] = dirname;
                // std::cout << gameCode << " : " << dirname << std::endl;;
            } else {
                std::cout << "Invalid game directory, skipping: " << dirname << std::endl;
            }
        }
    }

    return pathmap;
}

void downloadCitraSaveToCheckpoint(std::string dropboxToken, std::string timestamp, std::string checkpointPath, std::map<std::string, std::string> pathmap, std::string gameCode, std::string fullPath) {
    Dropbox dropbox(dropboxToken);

    std::string baseSaveDir = checkpointPath + "/saves";

    if (!pathmap.count(gameCode)) {
        std::cout << "Game code " << gameCode << " not found in local Checkpoint saves, skipping";
        return;
    }

    std::string gameDirname = pathmap[gameCode];
    std::string gameSaveDir = baseSaveDir + "/" + gameDirname;
    std::string destPath = gameSaveDir + "/Citra_" + timestamp;

    std::cout << "Citra save found for " << gameDirname << " with timestamp: " << timestamp << std::endl;

    struct stat info;

    if (stat(destPath.c_str(), &info) != 0) {
        // int status = mkdir(destPath.c_str(), 0777);
        // if (status != 0) {
        //     std::cout << "Failed to create Checkpoint save dir " << destPath << ", skipping" << std::endl;
        //     return;
        // }
        // std::cout << "Created Checkpoint save dir " << destPath << std::endl;
        // dropbox.download(fullPath + "/data/00000001/system",  destPath + "/system");
        // dropbox.download(fullPath + "/data/00000001/user1", destPath + "/user1");
    } else if (info.st_mode & !S_IFDIR) {
        std::cout << "File already exists at Checkpoint save dir, delete the file and try again:\n" << destPath << std::endl;
    } else {
        std::cout << "Checkpoint save dir already exists, skipping: " << destPath << std::endl;
    }
}

void downloadCitraSaves(std::string dropboxToken, std::string checkpointPath) {
    auto pathmap = findCheckpointSaves(checkpointPath);

    Dropbox dropbox(dropboxToken);
    auto folder = dropbox.list_folder("/sdmc/Nintendo 3DS/00000000000000000000000000000000/00000000000000000000000000000000/title/00040000");
    for (auto lr : folder) {
        std::string timestamp = getUpdateTimestampForCitraSave(dropboxToken, lr.path_display);
        if (timestamp == "") {
            std::cout << lr.name << ": Could not find timestamp, skipping" << std::endl; 
            continue;
        }

        downloadCitraSaveToCheckpoint(
            dropboxToken,
            timestamp,
            checkpointPath,
            pathmap,
            lr.name,
            lr.path_display
        );
    }
}