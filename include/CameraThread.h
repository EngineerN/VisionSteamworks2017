#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>
#include <iostream>
#include <chrono>
#include <thread>
#include "WQueue.h"

class CameraThread{
  wqueue<cv::Mat>& m_queue;
  int device_id;
  cv::VideoCapture cap;
  std::atomic<bool> m_stop;
  std::thread m_thread;
  bool is_initialized;

public:
  CameraThread() = delete;
  virtual ~CameraThread(void) {}
  CameraThread(wqueue<cv::Mat>& queue, int dev_id) : m_queue(queue), m_stop(), m_thread() {
    device_id = dev_id;
    is_initialized = false;
  }
  void stop() { m_stop = true; m_thread.join(); }
  void start() { m_thread = std::thread(&CameraThread::run, this); }
  void initialize() {
     cap.open(device_id);
     is_initialized = cap.isOpened();
  }
  void run() {
    while(!is_initialized) {
      initialize();
    }
    for (int i = 0;; i++) {
        cv::Mat frame;
        cap.read(frame);
        // check if we succeeded
        if (frame.empty()) {
            std::cerr << "ERROR! Blank frame grabbed" << std::endl;
            break;
        }
        m_queue.add(frame);
    }
  }
};
