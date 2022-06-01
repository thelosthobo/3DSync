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

// https://stackoverflow.com/a/10467633
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y%m%d-%H%M%S", &tstruct);

    return buf;
}


int main(int argc, char** argv){
    std::string checkpointPath = "/mnt/c/3ds-sdmc-dev/3ds/Checkpoint";
    auto pathmap = findCheckpointSaves(checkpointPath);

    std::string timestamp = currentDateTime();
    std::cout << timestamp << std::endl;

    auto token = std::getenv("DROPBOX_TOKEN");
    std::string dropboxToken(token);
    Dropbox dropbox(dropboxToken);
    auto folder = dropbox.list("/sdmc/Nintendo 3DS/00000000000000000000000000000000/00000000000000000000000000000000/title/00040000");

    int i = 0;
    for (auto file : folder) {
        std::cout << file.name << std::endl;
        downloadCitraSaveToCheckpoint(
            &dropbox,
            timestamp,
            checkpointPath,
            pathmap,
            file.name,
            file.path_display
        );
        i++;
    }
}
