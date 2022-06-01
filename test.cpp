#include <stdio.h>
#include <malloc.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>

#include <sys/stat.h>

#include "source/modules/dropbox.h"
#include "source/modules/citra.h"
#include "source/modules/time.h"


int main(int argc, char** argv){
    std::string checkpointPath = "/mnt/c/3ds-sdmc-dev/3ds/Checkpoint";

    auto token = std::getenv("DROPBOX_TOKEN");
    std::string dropboxToken(token);
    
    downloadCitraSaves(dropboxToken, checkpointPath);
}
