#pragma once
#include "WQueue.h"
#include <atomic>
#include <chrono>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <thread>
#include <thread>

/*! \brief The Network Thread handles gathers infrormation from the camera
 */
class CameraThread {
  int device_id;        //!< The device id of the camera
  cv::VideoCapture cap; //!< Interface to collect video frams
  bool is_initialized;  //!< Is the interface started?

  wqueue<cv::Mat>& m_queue; //!< Queue to dump frame from the camera
  std::atomic<bool> m_stop; //!< Is the thread stopped?
  std::thread m_thread;     //!< Thread to run operations

public:
  /*! \brief Constructor to initialize Camera Thread Class
   *  \warning Default constructor is not used
   */
  CameraThread() = delete;

  /*! \brief Constructor to initialize Camera Thread Class
   *  \param [in] queue Queue to dump the camera frames to
   *  \param [in] dev_id Camera id
   */
  CameraThread(wqueue<cv::Mat>& queue, int dev_id)
      : m_queue(queue), m_stop(false), m_thread() {
    device_id = dev_id;
    initialize();
  }

  /*! \brief Function to stop the thread
  */
  void stop() {
    m_stop = true;
    m_thread.join();
  }

  /*! \brief Function to start the thread
  */
  void start() { m_thread = std::thread(&CameraThread::run, this); }

private:
  /*! \brief Function to check if the camera is initialized
  */
  void initialize() {
    cap.open(device_id);
    is_initialized = cap.isOpened();
  }

  /*! \brief Function that the thread runs
  */
  void run() {
    while(!is_initialized) {
      initialize();
    }
    for(int i = 0;; i++) {
      if(m_stop) {
        break;
      }
      cv::Mat frame;
      cap.read(frame);
      // check if we succeeded
      if(frame.empty()) {
        std::cerr << "ERROR! Blank frame grabbed" << std::endl;
        break;
      }
      m_queue.add(std::move(frame));
    }
  }
};
