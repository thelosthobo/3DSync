#include <stdio.h>
#include <malloc.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>

#include "modules/dropbox.h"

int main(int argc, char** argv){
    auto token = std::getenv("DROPBOX_TOKEN");
    std::string dropboxToken(token);
    Dropbox dropbox(dropboxToken);
    auto folder = dropbox.list("/sdmc/Nintendo 3DS/00000000000000000000000000000000/00000000000000000000000000000000/title/00040000");

    int i = 0;
    for (auto file : folder) {
        std::cout << file << std::endl;
        dropbox.download(file + "/data/00000001/system", std::to_string(i) + "_system");
        dropbox.download(file + "/data/00000001/user1", "user1");
        i++;
    }
}
