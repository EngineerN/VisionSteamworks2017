#include "CameraThread.h"
#include "Config.h"
#include "NetworkThread.h"
#include "ShooterPipelineThread.h"
#include "GearPipelineThread.h"
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
     !projectConfig.getGearCameraEnable()) {
    std::cout << "No Camera configuration has been set." 
              << "Please enable a camera in config.json"
              << '\n';
  }

  //!< Initialize queues used
  wqueue<cv::Mat> cameraQueue1;
  wqueue<std::tuple<bool, float, float>> pipelineQueue1;
  wqueue<cv::Mat> cameraQueue2;
  wqueue<std::tuple<bool, float, float>> pipelineQueue2;

  //!< Declaration of Camera Thread
  CameraThread cameraThread1(cameraQueue1, projectConfig.getShooterCameraID());
  
  CameraThread cameraThread2(cameraQueue2, projectConfig.getGearCameraID());

  //!< Declaration of Pipeline Thread
  ShooterPipelineThread pipelineThread1(cameraQueue1, pipelineQueue1, projectConfig.getShooterCameraXPercentOffset(), projectConfig.getShooterCameraYPercentOffset());
  
  GearPipelineThread pipelineThread2(cameraQueue2, pipelineQueue2, projectConfig.getGearCameraXPercentOffset(), projectConfig.getGearCameraYPercentOffset());

  //!< Declaration of Network Thread
  NetworkThread networkThread(pipelineQueue1, pipelineQueue2, projectConfig.getFilterLength(),
                              projectConfig.getIPAddress(), projectConfig.getIPPort());

  //!< Start Threads
  if(projectConfig.getShooterCameraEnable()) {
    cameraThread1.start();
    pipelineThread1.start();
  } 

  if(projectConfig.getGearCameraEnable()) {
    cameraThread2.start();
    pipelineThread2.start();
  }

  networkThread.start();

  // Ctrl-C to end program
  std::cout << "Enter Ctrl-C to end the program..." << '\n';
  while(1) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  exit(0);
}
