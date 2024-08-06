#include <iostream>
#include "lib/tinyxml2.h"

namespace xml_parser {

namespace {

using namespace tinyxml2;

const std::string kTraffics = "Traffics";
const std::string kCmdsList = "cmds_list";
const std::string kTContextCmd = "TContextCMD";
const std::string kTCont = "TCont";
const std::string kArgs = "Args";

const XMLElement *FoundTContextCMD(const XMLDocument &doc) {
  const XMLElement *traffics(doc.FirstChildElement(kTraffics.c_str()));
  if (traffics == nullptr) {
    throw std::runtime_error("No Traffics found");
  }

  const XMLElement *cmdsList(traffics->FirstChildElement(kCmdsList.c_str()));
  if (cmdsList == nullptr) {
    throw std::runtime_error("No cmds_list found");
  }

  const XMLElement *contextCmd(cmdsList->FirstChildElement(kTContextCmd.c_str()));
  if (contextCmd == nullptr) {
    throw std::runtime_error("No TContextCMD found");
  }

  return contextCmd;
}

std::string hexToAscii(const std::string &hex) {
  std::string result;
  for (size_t i = 0; i < hex.length(); i += 2) {
    std::string byteString = hex.substr(i, 2);
    char byte = static_cast<char>(strtol(byteString.c_str(), nullptr, 16));
    result += byte;
  }

  return result;
}

void RetrieveTConts(const XMLElement *contextCmd) {
  const XMLElement *args = contextCmd->FirstChildElement(kArgs.c_str());
  if (!args) {
    throw std::runtime_error("Not found args");
  }

  for (const XMLElement *tcont = args->FirstChildElement(kTCont.c_str()); tcont; tcont = tcont->NextSiblingElement(
          kTCont.c_str())) {
    const char *arg_name = tcont->Attribute("Name");
    const char *arg_storage_len = tcont->Attribute("StorageLen");
    const char *arg_type = tcont->Attribute("Type");
    const char *arg_data = tcont->Attribute("Data");

    if (!arg_name || !arg_type || !arg_data || !arg_storage_len) {
      throw std::runtime_error("Not found any parameters for TCont");
    }

    std::string name(arg_name);
    std::string type(arg_type);
    std::string data(arg_data);

    if (type != "A" && type != "H" && type != "N") {
      std::cout << "Incorrect type, skip TCont" << std::endl;
      continue;
    }
    if (type == "A") {
      if (std::strtol(arg_storage_len, nullptr, 10) * 2 != data.size()) {
        std::cout << "Skip " << arg_name << "..." << std::endl;
        continue;
      }
      data = hexToAscii(data);
    }
    std::cout << name << ": " << data << std::endl;
  }
}

}  // namespace

void parse(const std::string &filename) {
  XMLDocument doc;
  XMLError error = doc.LoadFile(filename.c_str());

  if (error != XML_SUCCESS) {
    std::cout << "Error: " << tinyxml2::XMLDocument::ErrorIDToName(error) << std::endl;
    return;
  }

  try {
    auto request = FoundTContextCMD(doc);

    std::cout << "Request:" << std::endl;
    const char *data = request->Attribute("Data");
    if (data) {
      std::cout << "Command: " << data << std::endl;
    } else {
      std::cout << "Not found data, skipping..." << std::endl;
    }

    RetrieveTConts(request);

    request = request->NextSiblingElement(kTContextCmd.c_str());
    if (!request) {
      std::cout << "Not found response data (second TContextCMD)" << std::endl;
      return;
    }

    std::cout << "Response:" << std::endl;
    data = request->Attribute("Data");
    if (data) {
      std::cout << "Command: " << data << std::endl;
    } else {
      std::cout << "Not found data, skipping..." << std::endl;
    }
    RetrieveTConts(request);
  } catch (const std::exception &e) {
    std::cout << "Incorrect behaviour: " << e.what();
  }
}

}  // namespace xml_parser

int main() {
  xml_parser::parse("../resources/BA_test.xml");
  return 0;
}