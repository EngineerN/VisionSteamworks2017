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

  //!< Initialize queues used
  wqueue<cv::Mat> cameraQueue1;
  wqueue<std::pair<bool, int>> pipelineQueue1;
  wqueue<cv::Mat> cameraQueue2;
  wqueue<std::pair<bool, int>> pipelineQueue2;

  //!< Declaration of Camera Thread
  CameraThread* cameraThread1 =
    new CameraThread(cameraQueue1, projectConfig.getShooterCameraID());
  
  CameraThread* cameraThread2 =
    new CameraThread(cameraQueue2, projectConfig.getGearCameraID());

  //!< Declaration of Pipeline Thread
  ShooterPipelineThread* pipelineThread1 =
    new ShooterPipelineThread(cameraQueue1, pipelineQueue1);
  
  GearPipelineThread* pipelineThread2 =
    new GearPipelineThread(cameraQueue2, pipelineQueue2);

  //!< Declaration of Network Thread
  NetworkThread* networkThread =
    new NetworkThread(pipelineQueue1, pipelineQueue2, projectConfig.getFilterLength(),
                      projectConfig.getIPAddress(), projectConfig.getIPPort());

  //!< Start Threads
  if(projectConfig.getShooterCameraEnable()) {
    cameraThread1->start();
    pipelineThread1->start();
  } 

  if(projectConfig.getGearCameraEnable()) {
    cameraThread2->start();
    pipelineThread2->start();
  }

  networkThread->start();

  // Ctrl-C to end program
  printf("Enter Ctrl-C to end the program...\n");
  while(1) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  exit(0);
}
