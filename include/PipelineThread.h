#pragma once
#include "GripPipeline.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>
#include <iostream>
#include <utility>
#include "WQueue.h"

class PipelineThread{
  wqueue<cv::Mat>& m_queue;
  wqueue<std::pair<bool,int>>& m_queue_output;
  std::atomic<bool> m_stop;
  std::thread m_thread;
  grip::GripPipeline pipeline;

public:
  PipelineThread() = delete;
  virtual ~PipelineThread(void) {}
  PipelineThread(wqueue<cv::Mat>& queue, wqueue<std::pair<bool,int>>& queue_output) : m_queue(queue), m_queue_output(queue_output), m_stop(), m_thread() {
  }
  void stop() { m_stop = true; m_thread.join(); }
  void start() { m_thread = std::thread(&PipelineThread::run, this); }
  void run() {    
    for (int i = 0;; i++) {
        std::unique_ptr<cv::Mat> frame = m_queue.remove();
        pipeline.process(*frame);
        std::vector<std::vector<cv::Point> >* contours = pipeline.getfilterContoursOutput();
        int contour_center_total = 0;
        int contour_count = 0;
        cv::Rect rect;
        const int min_dist_from_center = 100;
        const int neg_min_dist_from_center = -100;
        bool in_center = false;
        for (auto& contour : *contours) {
              rect = cv::boundingRect(contour);
	      int centerx = 320 - rect.x + rect.width/2;
              if ( centerx < min_dist_from_center && centerx > neg_min_dist_from_center) {
                contour_center_total += centerx;
                contour_count++;
                in_center = true;
              }
        }
        if(in_center) {
          m_queue_output.add(std::make_pair(in_center, contour_center_total/contour_count));
        }
        else {
          m_queue_output.add(std::make_pair(false, 0));
        }
        
    }
  }
};
