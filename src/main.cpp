#include <sys/time.h>
#include "WQueue.h"
#include "CameraThread.h"
#include "PipelineThread.h"
#include "NetworkThread.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <thread>

int main(int argc, char const *argv[]){
  wqueue<cv::Mat>  cameraQueue1;
  wqueue<std::pair<bool,int>>  pipelineQueue1;
  CameraThread* cameraThread1 = new CameraThread(cameraQueue1, 0);
  cameraThread1->start();
  PipelineThread* pipelineThread1 = new PipelineThread(cameraQueue1, pipelineQueue1);
  pipelineThread1->start();
  NetworkThread* networkThread = new NetworkThread(pipelineQueue1, 7, "10.255.255.255", 4143);
  networkThread->start();

  // Ctrl-C to end program
  printf("Enter Ctrl-C to end the program...\n");
  while (1) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  exit(0);
}

