#include "CameraThread.h"
#include "Config.h"
#include "NetworkThread.h"
#include "PipelineThread.h"
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

  //!< Start the Camera Thread
  CameraThread* cameraThread1 =
    new CameraThread(cameraQueue1, projectConfig.getShooterCameraID());
  cameraThread1->start();

  //!< Start the Pipeline Thread
  PipelineThread* pipelineThread1 =
    new PipelineThread(cameraQueue1, pipelineQueue1);
  pipelineThread1->start();

  //!< Start the Network Thread
  NetworkThread* networkThread =
    new NetworkThread(pipelineQueue1, projectConfig.getFilterLength(),
                      projectConfig.getIPAddress(), projectConfig.getIPPort());
  networkThread->start();

  // Ctrl-C to end program
  printf("Enter Ctrl-C to end the program...\n");
  while(1) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  exit(0);
}
