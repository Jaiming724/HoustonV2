#pragma once

#include "../pch.h"
#include "Component.h"
#include "../Util.h"
#include "../services/Dispatcher.h"
#include "../services/consumer/QueueData.h"
#include "AlertPanel.h"

class FileUpload : public Component {
private:
    NFD::UniquePath outPath;
    std::unordered_map<std::string, std::string> map;
    httplib::Client client;
    NFD::Guard nfdGuard;
    nfdfilteritem_t filterItem[1] = {{"BinFile", "bin"}};
    bool shouldRefreshMap = false;
    AlertPanel *alertPanel;
public:
    FileUpload(const char *name, AlertPanel *alertPanel);

    ~FileUpload() override;

    void start() override;

    void render() override;

    void stop() override;

    std::string queryFiles();

    int uploadFiles(const std::string &filepath);

    int deleteFiles(const std::string &fileName);

    int flashFile(const std::string &fileName, const std::string &board);
};


