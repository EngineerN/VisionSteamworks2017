#pragma once
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <string>

// for convenience
using json = nlohmann::json;

class Config {
private:
  int shooterCameraID;
  int gearCameraID;
  int filterLength;
  std::string ipAddress;
  int ipPort;
  bool shooterCameraEnable;
  bool gearCameraEnable;
  float gearCameraXPercentOffset;
  float shooterCameraXPercentOffset;
  float gearCameraYPercentOffset;
  float shooterCameraYPercentOffset;
  bool debugEnable;

public:
  bool readFile(std::string filename) {
    std::ifstream i(filename);
    json j;
    bool isValid = true;

    // Read in the file
    if(i.is_open()) {
      i >> j;
    }
    i.close();

    if(j.find("Debug_Enable") != j.end() && j["Debug_Enable"].is_boolean()) {
      // there is an entry with key "foo"
      debugEnable = j["Debug_Enable"];
    }
    else {
      // Set the default and invalidate config
      j["Debug_Enable"] = false;
      isValid = false;
    }

    if(j.find("Shooter_Camera_ID") != j.end() &&
       j["Shooter_Camera_ID"].is_number()) {
      // there is an entry with key "foo"
      shooterCameraID = j["Shooter_Camera_ID"];
    }
    else {
      // Set the default and invalidate config
      j["Shooter_Camera_ID"] = 0;
      isValid = false;
    }

    if(j.find("Shooter_Camera_X_Percent_Offset") != j.end() &&
       j["Shooter_Camera_X_Percent_Offset"].is_number()) {
      // there is an entry with key "foo"
      shooterCameraXPercentOffset = j["Shooter_Camera_X_Percent_Offset"];
    }
    else {
      // Set the default and invalidate config
      j["Shooter_Camera_X_Percent_Offset"] = 0.0f;
      isValid = false;
    }

    if(j.find("Shooter_Camera_Y_Percent_Offset") != j.end() &&
       j["Shooter_Camera_Y_Percent_Offset"].is_number()) {
      // there is an entry with key "foo"
      shooterCameraYPercentOffset = j["Shooter_Camera_Y_Percent_Offset"];
    }
    else {
      // Set the default and invalidate config
      j["Shooter_Camera_Y_Percent_Offset"] = 0.0f;
      isValid = false;
    }

    if(j.find("Shooter_Camera_Enable") != j.end() &&
       j["Shooter_Camera_Enable"].is_boolean()) {
      // there is an entry with key "foo"
      shooterCameraEnable = j["Shooter_Camera_Enable"];
    }
    else {
      // Set the default and invalidate config
      j["Shooter_Camera_Enable"] = false;
      isValid = false;
    }

    if(j.find("Gear_Camera_ID") != j.end() && j["Gear_Camera_ID"].is_number()) {
      // there is an entry with key "foo"
      gearCameraID = j["Gear_Camera_ID"];
    }
    else {
      // Set the default and invalidate config
      j["Gear_Camera_ID"] = 0;
      isValid = false;
    }

    if(j.find("Gear_Camera_X_Percent_Offset") != j.end() &&
       j["Gear_Camera_X_Percent_Offset"].is_number()) {
      // there is an entry with key "foo"
      gearCameraXPercentOffset = j["Gear_Camera_X_Percent_Offset"];
    }
    else {
      // Set the default and invalidate config
      j["Gear_Camera_X_Percent_Offset"] = 0.0f;
      isValid = false;
    }

    if(j.find("Gear_Camera_Y_Percent_Offset") != j.end() &&
       j["Gear_Camera_Y_Percent_Offset"].is_number()) {
      // there is an entry with key "foo"
      gearCameraYPercentOffset = j["Gear_Camera_Y_Percent_Offset"];
    }
    else {
      // Set the default and invalidate config
      j["Gear_Camera_Y_Percent_Offset"] = 0.0f;
      isValid = false;
    }

    if(j.find("Gear_Camera_Enable") != j.end() &&
       j["Gear_Camera_Enable"].is_boolean()) {
      // there is an entry with key "foo"
      gearCameraEnable = j["Gear_Camera_Enable"];
    }
    else {
      // Set the default and invalidate config
      j["Gear_Camera_Enable"] = false;
      isValid = false;
    }

    if(j.find("Filter_Length") != j.end() && j["Filter_Length"].is_number()) {
      // there is an entry with key "foo"
      filterLength = j["Filter_Length"];
    }
    else {
      // Set the default and invalidate config
      j["Filter_Length"] = 5;
      isValid = false;
    }

    if(j.find("IP_Address") != j.end() && j["IP_Address"].is_string()) {
      // there is an entry with key "foo"
      ipAddress = j["IP_Address"];
    }
    else {
      // Set the default and invalidate config
      j["IP_Address"] = "127.0.0.1";
      isValid = false;
    }

    if(j.find("IP_Port") != j.end() && j["IP_Port"].is_number()) {
      // there is an entry with key "foo"
      ipPort = j["IP_Port"];
    }
    else {
      // Set the default and invalidate config
      j["IP_Port"] = 100;
      isValid = false;
    }

    if(!isValid) {
      std::cout << "Config is not valid please look at " << filename
                << std::endl;
      // write prettified JSON to another file
      std::ofstream o(filename.c_str());
      o << std::setw(4) << j << std::endl;
      o.close();
    }
    return isValid;
  }

  int getShooterCameraID() { return shooterCameraID; }

  int getGearCameraID() { return gearCameraID; }

  int getFilterLength() { return filterLength; }

  std::string getIPAddress() { return ipAddress; }

  int getIPPort() { return ipPort; }

  bool getShooterCameraEnable() { return shooterCameraEnable; }

  bool getGearCameraEnable() { return gearCameraEnable; }

  int getShooterCameraXPercentOffset() { return shooterCameraXPercentOffset; }

  int getShooterCameraYPercentOffset() { return shooterCameraXPercentOffset; }

  int getGearCameraXPercentOffset() { return gearCameraXPercentOffset; }

  int getGearCameraYPercentOffset() { return gearCameraYPercentOffset; }

  bool getDebugEnable() { return debugEnable; }
};
