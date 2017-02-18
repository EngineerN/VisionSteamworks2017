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
  wqueue<std::tuple<bool, int, int>>&
  m_queue_output;         //!< Output queue to the Network Thread
  std::atomic<bool> m_stop; //!< Is the thread stopped?
  std::thread m_thread;     //!< Thread to run operations
  int m_pixel_x_offset;
  int m_pixel_y_offset;

  /*! \brief Function that the thread runs
  */
  void run() {
    for(int i = 0;; i++) {
      if(m_stop) {
        break;
      }
      std::unique_ptr<cv::Mat> frame = m_queue.remove();
      pipeline.Process(*frame);
      std::vector<std::vector<cv::Point>>* contours =
        pipeline.GetFilterContoursOutput();
      int contour_x_total = 0;
      int contour_y_total = 0;
      int contour_count = 0;
      cv::Size s = (*frame).size();
      int midwidth = s.width / 2;
      for(auto& contour : *contours) {
        cv::Rect rect = cv::boundingRect(contour);
        int centerx = midwidth - rect.x + rect.width / 2;
        int centery = rect.y + rect.height / 2;
        contour_x_total += centerx;
        contour_y_total += centery;
        contour_count++;
      }
      if(contour_count) {
        m_queue_output.add(std::move(
          std::make_tuple(true, contour_x_total / contour_count, contour_y_total / contour_count)));
      }
      else {
        m_queue_output.add(std::move(std::make_tuple(false, 0, 0)));
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
                 wqueue<std::tuple<bool, int, int>>& queue_output,
                 int pixel_x_offset, int pixel_y_offset)
      : m_queue(queue), m_queue_output(queue_output), m_stop(false), m_thread(), m_pixel_x_offset(pixel_x_offset), m_pixel_y_offset(pixel_y_offset) {}

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
