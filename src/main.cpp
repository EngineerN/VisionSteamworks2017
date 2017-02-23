#include "CameraThread.h"
#include "Config.h"
#include "GearPipelineThread.h"
#include "NetworkThread.h"
#include "ShooterPipelineThread.h"
#include "WQueue.h"
#include <chrono>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <sys/time.h>
#include <thread>

int main() {
  //!< Setup the config file
  Config projectConfig;
  if(!projectConfig.readFile("config.json")) {
    exit(0);
  }

  if(!projectConfig.getShooterCameraEnable() &&
     !projectConfig.getGearCamera1Enable() &&
     !projectConfig.getGearCamera2Enable()) {
    std::cout << "No Camera configuration has been set. "
              << "Please enable a camera in config.json" << '\n';
  }

  //!< Initialize queues used
  wqueue<cv::Mat> shooterCameraQueue;
  wqueue<std::tuple<bool, float, float>> shooterPipelineQueue;
  wqueue<cv::Mat> gearCamera1Queue;
  wqueue<std::tuple<bool, float, float>> gearPipeline1Queue;
  wqueue<cv::Mat> gearCamera2Queue;
  wqueue<std::tuple<bool, float, float>> gearPipeline2Queue;

  //!< Declaration of Camera Thread
  CameraThread shooterCameraThread(shooterCameraQueue, projectConfig.getShooterCameraID());

  CameraThread gearCameraThread1(gearCamera1Queue, projectConfig.getGearCamera1ID());

  CameraThread gearCameraThread2(gearCamera2Queue, projectConfig.getGearCamera2ID());

  //!< Declaration of Pipeline Thread
  ShooterPipelineThread shooterPipelineThread(
    shooterCameraQueue, shooterPipelineQueue,
    projectConfig.getShooterCameraXPercentOffset(),
    projectConfig.getShooterCameraYPercentOffset(),
    projectConfig.getDebugEnable());

  GearPipelineThread gearPipelineThread1(
    gearCamera1Queue, gearPipeline1Queue, projectConfig.getGearCamera1XPercentOffset(),
    projectConfig.getGearCamera1YPercentOffset(),
    projectConfig.getDebugEnable());

  GearPipelineThread gearPipelineThread2(
    gearCamera2Queue, gearPipeline2Queue, projectConfig.getGearCamera2XPercentOffset(),
    projectConfig.getGearCamera2YPercentOffset(),
    projectConfig.getDebugEnable());

  //!< Declaration of Network Thread
  NetworkThread networkThread(
    shooterPipelineQueue, gearPipeline1Queue, gearPipeline2Queue, projectConfig.getFilterLength(),
    projectConfig.getIPAddress(), projectConfig.getIPPort(),
    projectConfig.getDebugEnable());

  //!< Start Threads
  if(projectConfig.getShooterCameraEnable()) {
    shooterCameraThread.start();
    shooterPipelineThread.start();
  }

  if(projectConfig.getGearCamera1Enable()) {
    gearCameraThread1.start();
    gearPipelineThread1.start();
  }

  if(projectConfig.getGearCamera2Enable()) {
    gearCameraThread1.start();
    gearPipelineThread2.start();
  }

  networkThread.start();

  // Ctrl-C to end program
  std::cout << "Enter Ctrl-C to end the program..." << '\n';
  while(1) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  exit(0);
}
