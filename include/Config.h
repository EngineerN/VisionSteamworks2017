#pragma once
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <string>

// for convenience
using json = nlohmann::json;

class Config {
private:
  bool debugEnable;

  int shooterCameraID;
  float shooterCameraXPercentOffset;
  float shooterCameraYPercentOffset;
  bool shooterCameraEnable;

  int gearCamera1ID;
  float gearCamera1XPercentOffset;
  float gearCamera1YPercentOffset;
  bool gearCamera1Enable;

  int gearCamera2ID;
  float gearCamera2XPercentOffset;
  float gearCamera2YPercentOffset;
  bool gearCamera2Enable;

  std::string ipAddress;
  int ipPort;
  int filterLength;


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

    if(j.find("Gear_Camera_1_ID") != j.end() && j["Gear_Camera_1_ID"].is_number()) {
      // there is an entry with key "foo"
      gearCamera1ID = j["Gear_Camera_1_ID"];
    }
    else {
      // Set the default and invalidate config
      j["Gear_Camera_1_ID"] = 1;
      isValid = false;
    }

    if(j.find("Gear_Camera_1_X_Percent_Offset") != j.end() &&
       j["Gear_Camera_1_X_Percent_Offset"].is_number()) {
      // there is an entry with key "foo"
      gearCamera1XPercentOffset = j["Gear_Camera_1_X_Percent_Offset"];
    }
    else {
      // Set the default and invalidate config
      j["Gear_Camera_1_X_Percent_Offset"] = 0.0f;
      isValid = false;
    }

    if(j.find("Gear_Camera_1_Y_Percent_Offset") != j.end() &&
       j["Gear_Camera_1_Y_Percent_Offset"].is_number()) {
      // there is an entry with key "foo"
      gearCamera1YPercentOffset = j["Gear_Camera_1_Y_Percent_Offset"];
    }
    else {
      // Set the default and invalidate config
      j["Gear_Camera_1_Y_Percent_Offset"] = 0.0f;
      isValid = false;
    }

    if(j.find("Gear_Camera_1_Enable") != j.end() &&
       j["Gear_Camera_1_Enable"].is_boolean()) {
      // there is an entry with key "foo"
      gearCamera1Enable = j["Gear_Camera_1_Enable"];
    }
    else {
      // Set the default and invalidate config
      j["Gear_Camera_1_Enable"] = false;
      isValid = false;
    }

    if(j.find("Gear_Camera_2_ID") != j.end() && j["Gear_Camera_2_ID"].is_number()) {
      // there is an entry with key "foo"
      gearCamera2ID = j["Gear_Camera_2_ID"];
    }
    else {
      // Set the default and invalidate config
      j["Gear_Camera_2_ID"] = 2;
      isValid = false;
    }

    if(j.find("Gear_Camera_2_X_Percent_Offset") != j.end() &&
       j["Gear_Camera_2_X_Percent_Offset"].is_number()) {
      // there is an entry with key "foo"
      gearCamera2XPercentOffset = j["Gear_Camera_2_X_Percent_Offset"];
    }
    else {
      // Set the default and invalidate config
      j["Gear_Camera_2_X_Percent_Offset"] = 0.0f;
      isValid = false;
    }

    if(j.find("Gear_Camera_2_Y_Percent_Offset") != j.end() &&
       j["Gear_Camera_2_Y_Percent_Offset"].is_number()) {
      // there is an entry with key "foo"
      gearCamera2YPercentOffset = j["Gear_Camera_2_Y_Percent_Offset"];
    }
    else {
      // Set the default and invalidate config
      j["Gear_Camera_2_Y_Percent_Offset"] = 0.0f;
      isValid = false;
    }

    if(j.find("Gear_Camera_2_Enable") != j.end() &&
       j["Gear_Camera_2_Enable"].is_boolean()) {
      // there is an entry with key "foo"
      gearCamera2Enable = j["Gear_Camera_2_Enable"];
    }
    else {
      // Set the default and invalidate config
      j["Gear_Camera_2_Enable"] = false;
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

  bool getDebugEnable() { return debugEnable; }

  int getShooterCameraID() { return shooterCameraID; }

  int getShooterCameraXPercentOffset() { return shooterCameraXPercentOffset; }

  int getShooterCameraYPercentOffset() { return shooterCameraXPercentOffset; }

  bool getShooterCameraEnable() { return shooterCameraEnable; }

  int getGearCamera1ID() { return gearCamera1ID; }

  int getGearCamera1XPercentOffset() { return gearCamera1XPercentOffset; }

  int getGearCamera1YPercentOffset() { return gearCamera1YPercentOffset; }

  bool getGearCamera1Enable() { return gearCamera1Enable; }

  int getGearCamera2ID() { return gearCamera2ID; }

  int getGearCamera2XPercentOffset() { return gearCamera2XPercentOffset; }

  int getGearCamera2YPercentOffset() { return gearCamera2YPercentOffset; }

  bool getGearCamera2Enable() { return gearCamera2Enable; }

  int getFilterLength() { return filterLength; }

  std::string getIPAddress() { return ipAddress; }

  int getIPPort() { return ipPort; }
  
};
