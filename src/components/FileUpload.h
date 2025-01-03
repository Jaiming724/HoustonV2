#pragma once

#include "../pch.h"
#include "Component.h"
#include "../Util.h"

class FileUpload : public Component {
private:
    NFD::UniquePath outPath;
    std::unordered_map<std::string, std::string> map;
    httplib::Client client;

public:
    FileUpload(const char *name);

    ~FileUpload() override;

    void start() override;

    void render() override;

    void stop() override;

    std::string queryFiles();

    int uploadFiles(const std::string &filepath);

    int deleteFiles(const std::string &fileName);

    int flashFile(const std::string &fileName, std::string board);
};

