#pragma once
#include "ShooterGripPipeline.h"
#include "WQueue.h"
#include <atomic>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <thread>
#include <utility>

/*! \brief The Pipeline Thread handles information from the Camera Thread, does
 * image processing and sends information to the Network Thread.
 */
class ShooterPipelineThread {
private:
  grip::ShooterGripPipeline pipeline; //!< Interface to pipeline code

  wqueue<cv::Mat>& m_queue; //!< Input queue from the Camera Thread
  wqueue<std::pair<bool, int>>&
    m_queue_output;         //!< Output queue to the Network Thread
  std::atomic<bool> m_stop; //!< Is the thread stopped?
  std::thread m_thread;     //!< Thread to run operations

  /*! \brief Function that the thread runs
  */
  void run() {
    for(int i = 0;; i++) {
      std::unique_ptr<cv::Mat> frame = m_queue.remove();
      pipeline.Process(*frame);
      std::vector<std::vector<cv::Point>>* contours =
        pipeline.GetFilterContoursOutput();
      int contour_center_total = 0;
      int contour_count = 0;
      cv::Size s = (*frame).size();
      int midwidth = s.width / 2;
      bool found_contour = false;
      for(auto& contour : *contours) {
        cv::Rect rect = cv::boundingRect(contour);
        int centerx = midwidth - rect.x + rect.width / 2;
        contour_center_total += centerx;
        contour_count++;
        found_contour = true;
      }
      if(found_contour) {
        m_queue_output.add(std::move(
          std::make_pair(true, contour_center_total / contour_count)));
      }
      else {
        m_queue_output.add(std::move(std::make_pair(false, 0)));
      }
    }
  }

public:
  /*! \brief Constructor to initialize Pipeline Thread Class
   *  \warning Default constructor is not used
   */
  ShooterPipelineThread() = delete;

  /*! \brief Constructor to initiailize Pipeline Thread Class
   *  \param [in] queue Input queue from the Camera Thread
   *  \param [in] queue_output Output queue to the Network Thread
   */
  ShooterPipelineThread(wqueue<cv::Mat>& queue,
                 wqueue<std::pair<bool, int>>& queue_output)
      : m_queue(queue), m_queue_output(queue_output), m_stop(), m_thread() {}

  /*! \brief Function to stop the thread
  */
  void stop() {
    m_stop = true;
    m_thread.join();
  }

  /*! \brief Function to start the thread
  */
  void start() { m_thread = std::thread(&ShooterPipelineThread::run, this); }
};
