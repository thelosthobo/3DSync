#include "dropbox.h"
// #include <3ds.h>
#include <json-c/json.h>
#include <memory>
#include <iostream>

Dropbox::Dropbox(std::string token) : _token(token){
}

void Dropbox::upload(std::map<std::pair<std::string, std::string>, std::vector<std::string>> paths){
    for(auto item : paths){
        for(auto path : item.second){
            printf("Uploading %s\n", (item.first.first + path).c_str());
            FILE *file = fopen((item.first.first + path).c_str(), "rb");
            std::string args("Dropbox-API-Arg: {\"path\":\"/" + item.first.second + path + "\",\"mode\": \"add\",\"mute\": false,\"strict_conflict\": false}");
            std::string auth("Authorization: Bearer " + _token);
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, auth.c_str());
            headers = curl_slist_append(headers, args.c_str());
            headers = curl_slist_append(headers, "Content-Type: application/octet-stream");
            headers = curl_slist_append(headers, "Expect:");
            _curl.setURL(std::string("https://content.dropboxapi.com/2/files/upload"));
            _curl.setHeaders(headers);
            _curl.setReadData((void *)file);
            _curl.perform();
            fclose(file);
            printf("\n");
        }
    }
}

std::vector<std::string> Dropbox::list(std::string path) {
    std::vector<std::string> paths;

    std::string body("{\"path\":\"" + path + "\"}");
    std::cout << body << std::endl;
    std::string auth("Authorization: Bearer " + _token);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, auth.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Expect:");
    _curl.setURL(std::string("https://api.dropboxapi.com/2/files/list_folder"));
    _curl.setHeaders(headers);
    _curl.setBody(body.c_str());

    std::string httpData;
    _curl.setWriteData(&httpData);
    auto rescode = _curl.perform();

    auto http_code = _curl.getHTTPCode();
    std::cout << http_code << std::endl;


    if (http_code == 200) {
        std::cout << "http data: " << httpData << std::endl;

        struct json_object *parsed_json;
        struct json_object *entries;
        parsed_json = json_tokener_parse(httpData.c_str());
        json_object_object_get_ex(parsed_json, "entries", &entries);

        auto entries_obj = json_object_get_array(entries);
        int len = array_list_length(entries_obj);
        std::cout << "Num Dirs: " << len << std::endl;
        for (int i = 0; i < len; i++) {
            struct json_object *name;
            struct json_object *path_display;
            struct json_object *entry = (struct json_object *)(array_list_get_idx(entries_obj, i));
            json_object_object_get_ex(entry, "name", &name);
            json_object_object_get_ex(entry, "path_display", &path_display);
            // std::cout << json_object_get_string(name) << std::endl;
            // std::cout << json_object_get_string(path_display) << std::endl;
            paths.push_back(json_object_get_string(path_display));
        }
    }

    return paths;
}

void Dropbox::download(std::string path, std::string destPath) {
    printf("Downloading %s\n", (path).c_str());
    FILE *file = fopen((destPath).c_str(), "wb");
    std::string args("Dropbox-API-Arg: {\"path\":\"" + path + "\"}");
    std::string auth("Authorization: Bearer " + _token);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, auth.c_str());
    headers = curl_slist_append(headers, args.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/octet-stream");
    headers = curl_slist_append(headers, "Content-Length: 0");
    headers = curl_slist_append(headers, "Connection: close");
    _curl.setURL(std::string("https://content.dropboxapi.com/2/files/download"));
    _curl.setHeaders(headers);

    std::string headerData;
    _curl.setHeaderData(&headerData);
    _curl.setWriteFile(file);

    _curl.perform();

    auto http_code = _curl.getHTTPCode();
    std::cout << http_code << std::endl;

    fclose(file);
    printf("\n");
}
