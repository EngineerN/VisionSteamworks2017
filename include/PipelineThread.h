#pragma once
#include "GripPipeline.h"
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
class PipelineThread {
private:
  grip::GripPipeline pipeline; //!< Interface to pipeline code

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
      pipeline.process(*frame);
      std::vector<std::vector<cv::Point>>* contours =
        pipeline.getfilterContoursOutput();
      int contour_center_total = 0;
      int contour_count = 0;
      cv::Size s = (*frame).size();
      int width = s.width;
      int midwidth = width / 2;
      int min_dist_from_center = width / 4;
      int neg_min_dist_from_center = -1 * min_dist_from_center;
      bool in_center = false;
      for(auto& contour : *contours) {
        cv::Rect rect = cv::boundingRect(contour);
        int centerx = midwidth - rect.x + rect.width / 2;
        if(centerx < min_dist_from_center &&
           centerx > neg_min_dist_from_center) {
          contour_center_total += centerx;
          contour_count++;
          in_center = true;
        }
      }
      if(in_center) {
        m_queue_output.add(
          std::make_pair(true, contour_center_total / contour_count));
      }
      else {
        m_queue_output.add(std::make_pair(false, 0));
      }
    }
  }

public:
  /*! \brief Constructor to initialize Pipeline Thread Class
   *  \warning Default constructor is not used
   */
  PipelineThread() = delete;

  /*! \brief Constructor to initiailize Pipeline Thread Class
   *  \param [in] queue Input queue from the Camera Thread
   *  \param [in] queue_output Output queue to the Network Thread
   */
  PipelineThread(wqueue<cv::Mat>& queue,
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
  void start() { m_thread = std::thread(&PipelineThread::run, this); }
};
