#include <stdio.h>
#include <malloc.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>

#include <sys/stat.h>

#include "modules/dropbox.h"
#include "modules/citra.h"
#include "modules/time.h"


int main(int argc, char** argv){
    std::string checkpointPath = "/mnt/c/3ds-sdmc-dev/3ds/Checkpoint";
    auto pathmap = findCheckpointSaves(checkpointPath);

    auto token = std::getenv("DROPBOX_TOKEN");
    std::string dropboxToken(token);
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
