
#include "FileUpload.h"


void FileUpload::start() {
    shouldRefreshMap = true;
}

FileUpload::FileUpload(const char *name, AlertPanel *alertPanel) : Component(name), client("http://192.168.4.1") {
    this->alertPanel = alertPanel;
}

FileUpload::~FileUpload() {

}

static void printRes(const httplib::Result &res) {
    if (res) {
        if (res->status == 200 || res->status == 303) {
            // Print the response body
            std::cout << "Response received:\n" << res->body << std::endl;
        } else {
            std::cerr << "Request failed with status code: " << res->status << std::endl;
        }
    } else {
        std::cerr << "Error: " << httplib::to_string(res.error()) << std::endl;
    }
}

std::string FileUpload::queryFiles() {

    // Send a GET request to the "/files" endpoint
    auto res = client.Get("/files");
    printRes(res);
    alertPanel->alerts.push_back("Updated File List");
    return res->body;
}

int FileUpload::uploadFiles(const std::string &filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return -1;
    }

    std::ostringstream oss;
    oss << file.rdbuf();
    std::string file_content = oss.str();
    file.close();

    // Create an HTTP client

    // Send the POST request with the file
    auto res = client.Post(
            "/upload/" + Util::parseFileName(filepath),
            file_content,
            "application/octet-stream"
    );
    printRes(res);
    return res->status;
}

int FileUpload::flashFile(const std::string &fileName, const std::string &board) {
    auto res = client.Post("/flash/" + board + "/" + fileName, "", "text/plain");
    printRes(res);
    if (res->status != 200 && res->status != 303) {
        alertPanel->alerts.push_back("Failed to flash " + fileName);
    } else {
        alertPanel->alerts.push_back("Successfully flash " + fileName);
    }
    return res->status;
}

int FileUpload::deleteFiles(const std::string &fileName) {
    auto res = client.Post("/delete/" + fileName, "", "text/plain");
    printRes(res);
    return res->status;
}

void FileUpload::render() {
    ImGui::Begin("File Control Panel");


    if (outPath.get() == nullptr) {
        ImGui::Text("%s", "No file selected");
    } else {
        ImGui::Text("Select file at %s", outPath.get());
    }
    ImGui::SameLine();
    if (ImGui::Button("Select File")) {
        // Open a file dialog

        nfdresult_t result = NFD::OpenDialog(outPath, filterItem, 1);
        if (result == NFD_OKAY) {
            std::cout << "Success!" << std::endl << outPath.get() << std::endl;
        } else if (result == NFD_CANCEL) {
            std::cout << "User pressed cancel." << std::endl;
        } else {
            std::cout << "Error: " << NFD::GetError() << std::endl;
        }
    }
    ImGui::SameLine();

    if (ImGui::Button("Upload File")) {
        //TODO check for correct size, is .bin, correctly uploaded
        if (outPath.get() != nullptr) {
            uploadFiles(outPath.get());
            shouldRefreshMap = true;
        } else {
            std::cout << "File Path is null" << std::endl;
        }

    }

    if (ImGui::Button("Query File")) {
        shouldRefreshMap = true;
    }

    if (!map.empty()) {
        if (ImGui::BeginTable("FileTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            // Define table headers
            ImGui::TableSetupColumn("File Name");
            ImGui::TableSetupColumn("File Size");
            ImGui::TableSetupColumn("Flash Rear");
            ImGui::TableSetupColumn("Flash Front");
            ImGui::TableSetupColumn("Delete File");
            ImGui::TableHeadersRow();

            // Populate the table with data
            for (std::unordered_map<std::string, std::string>::const_iterator it = map.begin(); it != map.end(); ++it) {
                ImGui::TableNextRow();

                // Column: File Name
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", it->first.c_str());

                // Column: File Size
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s bytes", it->second.c_str());

                // Column: Flash Rear Button
                ImGui::TableSetColumnIndex(2);
                std::string flashRearButtonId = "Flash Rear##" + it->first;
                if (ImGui::Button(flashRearButtonId.c_str())) {
                    std::cout << "Flash Rear clicked for file: " << it->first << std::endl;

                    flashFileName = it->first;
                    flashBoard = "R";
                    shouldShowConfirmPopup = true;


                    //flashFile(it->first, "R");
                }

                // Column: Flash Front Button
                ImGui::TableSetColumnIndex(3);
                std::string flashFrontButtonId = "Flash Front##" + it->first;
                if (ImGui::Button(flashFrontButtonId.c_str())) {


                    flashFileName = it->first;
                    flashBoard = "F";
                    shouldShowConfirmPopup = true;
                    std::cout << "Flash Front clicked for file: " << it->first << std::endl;
                    //flashFile(it->first, "F");
                    //shouldRefreshMap = true;
                }
                ImGui::TableSetColumnIndex(4);
                std::string deleteButtonId = "Delete##" + it->first;
                if (ImGui::Button(deleteButtonId.c_str())) {
                    std::cout << "Delete file: " << it->first << std::endl;
                    deleteFiles(it->first);
                    shouldRefreshMap = true;
                }
            }

            ImGui::EndTable();
        }
    }
    if (shouldShowConfirmPopup) {
        memset(confirmStrBuf, 0, 64);
        ImGui::OpenPopup("flashConfirm");
        shouldShowConfirmPopup = false;
    }
//    if (ImGui::IsPopupOpen("flashConfirm")) {
//        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(1, 1));
//        //ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Always);
//    }
    if (ImGui::BeginPopupModal("flashConfirm", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        std::string confirmStr = flashBoard + ":" + flashFileName;
        std::string label = "Please type " + confirmStr + " to confirm";

        ImGui::PushFont(roboto24Font);
        //ImGui::Text("%s", label.data());
        //ImGui::InputTextMultiline("##FlashConfirmInput", confirmStrBuf, 64);
        ImGui::InputText(label.data(), confirmStrBuf, 64);

        if (ImGui::Button("Confirm")) {
            std::cout << confirmStr << std::endl;
            std::cout << std::string(confirmStrBuf) << std::endl;
            if (std::strncmp(confirmStrBuf, confirmStr.c_str(), confirmStr.size()) == 0) {
                alertPanel->alerts.push_back("Flashing: " + flashFileName);
                flashFile(flashFileName, flashBoard);

            } else {
                std::cout << "fail to confirm " + confirmStr << std::endl;
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();
        ImGui::PopFont();
        ImGui::EndPopup();
    }

    if (shouldRefreshMap) {
        map = Util::parseKeyValuePairs(queryFiles());
        shouldRefreshMap = false;
    }
    ImGui::End();
}

void FileUpload::stop() {
    shouldRefreshMap = false;
    map.clear();
    flashFileName.clear();
    flashBoard.clear();
    Component::stop();
}

