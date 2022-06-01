#include <stdio.h>
#include <malloc.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>

#include <sys/stat.h>

#include "dropbox.h"

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
            std::string readpath(path + "/" + dirname);
            std::cout << readpath << std::endl;

            std::string gameCode = checkpointDirToCitraGameCode(dirname);
            if (gameCode != "") {
                pathmap[gameCode] = dirname;
                std::cout << gameCode << " : " << dirname << std::endl;;
            } else {
                std::cout << "Invalid game directory, skipping: " << dirname << std::endl;
            }
        }
    }

    return pathmap;
}

void downloadCitraSaveToCheckpoint(Dropbox *dropbox, std::string timestamp, std::string checkpointPath, std::map<std::string, std::string> pathmap, std::string gameCode, std::string fullPath) {
    std::string baseSaveDir = checkpointPath + "/saves";

    if (!pathmap.count(gameCode)) {
        std::cout << "Game code " << gameCode << " not found in local Checkpoint saves, skipping";
        return;
    }

    std::string gameDirname = pathmap[gameCode];
    std::string gameSaveDir = baseSaveDir + "/" + gameDirname;
    std::string destPath = gameSaveDir + "/Citra_" + timestamp;

    int status = mkdir(destPath.c_str(), 0777);
    if (status != 0) {
        std::cout << "Failed to create Checkpoint save dir " << destPath << ", skipping" << std::endl;
        return;
    }
    
    dropbox->download(fullPath + "/data/00000001/system",  destPath + "/system");
    dropbox->download(fullPath + "/data/00000001/user1", destPath + "/user1");
}
