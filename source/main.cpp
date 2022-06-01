#include <stdio.h>
#include <malloc.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <iostream>

#include <3ds.h>

#include <curl/curl.h>

#include "libs/inih/INIReader/INIReader.h"
#include "modules/dropbox.h"


#define DEBUG

std::vector<std::string> recurse_dir(std::string basepath, std::string additionalpath=""){
    std::vector<std::string> paths;
    DIR *dir;
    struct dirent *ent;
    std::string path(basepath + additionalpath);
    if((dir = opendir(path.c_str())) != NULL){
        while((ent = readdir(dir)) != NULL){
            std::string readpath(path + "/" + ent->d_name);
            std::vector<std::string> recurse = recurse_dir(basepath, additionalpath + "/" + ent->d_name);
            paths.insert(paths.end(), recurse.begin(), recurse.end());
        }
    } else {
        if(additionalpath != "") paths.push_back(additionalpath);
        else printf("Folder %s not found\n", basepath.c_str());
    }
    closedir(dir);
    return paths;
}

bool componentsInit(){
    bool result = true;
    gfxInitDefault();

    consoleInit(GFX_BOTTOM, NULL);
    printf(CONSOLE_RED "\n 3DSync " VERSION_STRING " by Kyraminol" CONSOLE_RESET);
    printf("\n\n\n\n\n\n  Sync your saves with another 3DS,\n   a PC or even a cloud.");
    printf("\n\n\n\n\n\n Commit: " CONSOLE_BLUE REVISION_STRING CONSOLE_RESET);

    consoleInit(GFX_TOP, NULL);
    printf("Initializing components...\n\n");
    
    APT_SetAppCpuTimeLimit(30);
    cfguInit();
    romfsInit();
    pxiDevInit();
    amInit();
    acInit();

    u32* socketBuffer = (u32*)memalign(0x1000, 0x100000);
    if(socketBuffer == NULL){printf("Failed to create socket buffer.\n"); result = false;}
    if(socInit(socketBuffer, 0x100000)){printf("socInit failed.\n"); result = false;}

    httpcInit(0);
    sslcInit(0);
    return result;
}

void componentsExit(){
    sslcExit();
    httpcExit();
    socExit();
    acExit();
    pxiDevExit();
    romfsExit();
    cfguExit();
    gfxExit();
}

std::string checkpointDirToCitraGameCode(std::string checkpointGameSaveDir) {
    if (checkpointGameSaveDir.rfind("0x", 0) == 0) {
        std::string gameCode = "0" + checkpointGameSaveDir.substr(2,5);
        for (auto &elem : gameCode) {
            elem = std::tolower(elem);
        }
        return gameCode;
    }
    return "";
}

std::map<std::string, std::string> findCheckpointSaves(std::string citraSdmcPath, std::string checkpointPath) {
    std::map<std::string, std::string> pathmap;
    
    std::string citraSaveBasePath("Nintendo 3DS/00000000000000000000000000000000/00000000000000000000000000000000/title/00040000");

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
                // std::string fullDropboxPath(citraSdmcPath + "/" + citraSaveBasePath +  "/" + gameCode + "00/data/00000001");
                // std::cout << fullDropboxPath  << std::endl;
            } else {
                std::cout << "Invalid game directory, skipping: " << dirname << std::endl;
            }
        }
    }

    return pathmap;
}


int main(int argc, char** argv){
    if(!componentsInit()) componentsExit();

    INIReader reader("/3ds/3DSync/3DSync.ini");

    if(reader.ParseError() < 0){
        printf("Can't load configuration\n");
    } else {
        std::string dropboxToken = reader.Get("Dropbox", "token", "");
        
        if(dropboxToken != ""){
            Dropbox dropbox(dropboxToken);
            std::map<std::string, std::string> values = reader.GetValues();
            std::map<std::pair<std::string, std::string>, std::vector<std::string>> paths;
            for(auto value : values){
                if(value.first.rfind("paths=", 0) == 0){
                    std::pair<std::string, std::string> key = std::make_pair(value.second, value.first.substr(6));
                    paths[key] = recurse_dir(value.second);
                }
            }

            auto folders = dropbox.list("/sdmc/Nintendo 3DS/00000000000000000000000000000000/00000000000000000000000000000000/title/00040000");

            for (auto folder : folders) {
                dropbox.download(folder + "/")
            }

            // TODO: Download files 

            // Upload to Dropbox
            // if((int)paths.size() > 0) dropbox.upload(paths);
        } else {
            printf("Can't load Dropbox token from 3DSync.ini\n");
        }

        // std::string citraSdmcPath = reader.Get("Dropbox", "CitraSDMCPath", "");

        // if (citraSdmcPath != "") {
        //     std::cout << "Citra sdmc path: " << citraSdmcPath << std::endl;
        //     printf("Downloading Citra saves from Dropbox...\n");

        //     // List local paths
        //     std::string checkpointPath = reader.Get("Paths", "Checkpoint", "");
        //     if (checkpointPath != "") {
        //         findCheckpointSaves(citraSdmcPath, checkpointPath);
        //     }

        //     printf("Finished downloading Citra saves.\n");
        // } else {
        //     printf("Citra sdmc path not present, skipping");
        // }
    }

    printf("\n\nPress START to exit...");
    while (aptMainLoop()){
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START) break;
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    componentsExit();
    return 0;
}
