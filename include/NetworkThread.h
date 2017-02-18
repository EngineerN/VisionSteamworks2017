#pragma once
#include "WQueue.h"
#include <arpa/inet.h>
#include <atomic>
#include <chrono>
#include <deque>
#include <iostream>
#include <netinet/in.h>
#include <tuple>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <sys/time.h>
#define WSAGetLastError() (errno)
#define SOCKET_ERROR (-1)

/*! \brief The Network Thread handles information from the Pipeline Thread,
 * filters it and sends
 *  information over the network.
 */
class NetworkThread {
private:
  struct sockaddr_in si_other; //!< Network Socket information
  int slen;                    //!< Network Socket information
  int s;                       //!< Network Socket information

  std::deque<std::tuple<bool, float, float>> m_filter_queue1; //!< Camera 1 Filter Queue
  std::deque<std::tuple<bool, float, float>> m_filter_queue2; //!< Camera 2 Filter Queue
  unsigned short m_filter_length;                  //!< Max queue length

  wqueue<std::tuple<bool, float, float>>& m_queue1; //!< Network Queue
  wqueue<std::tuple<bool, float, float>>& m_queue2; //!< Network Queue
  std::atomic<bool> m_stop;              //!< Is the thread stopped?
  std::thread m_thread;                  //!< Thread to run operations

  std::tuple<bool, float, float> filter(std::deque<std::tuple<bool, float, float>> filter_queue) {
    bool in_center = false;
    float average_x = 0;
    float average_y = 0;
    for(auto it = filter_queue.begin(); it != filter_queue.end(); it++) {
      if(!std::get<0>((*it))) {
        continue;
      }
      else {
        average_x += std::get<1>((*it));
        average_y += std::get<2>((*it));
        in_center = true;
      }
    }
    average_x /= (float)m_filter_length;
    average_y /= (float)m_filter_length;
    return std::make_tuple(std::move(in_center), std::move(average_x), std::move(average_y));
  }


  /*! \brief Function that the thread runs
  */
  void run() {
    for(int i = 0;; i++) {
      if(m_stop) {
        break;
      }
      if(!m_queue1.empty()) {
        auto temp1 = m_queue1.remove();
        m_filter_queue1.push_back(*temp1);
        if(m_filter_queue1.size() > m_filter_length) {
          m_filter_queue1.pop_front();
        }
      }

      if(!m_queue2.empty()) {
        auto temp2 = m_queue2.remove();
        m_filter_queue2.push_back(*temp2);
        if(m_filter_queue2.size() > m_filter_length) {
          m_filter_queue2.pop_front();
        }
      }
      
      std::tuple<bool, float, float> avg1, avg2;

      avg1 = filter(m_filter_queue1);
      avg2 = filter(m_filter_queue2);

      {
        std::stringstream message; 
        message << std::to_string((int)(std::get<1>(avg1) * 100.0f)) << " "
                << std::to_string((int)(std::get<2>(avg1) * 100.0f)) << " "
                << std::to_string((int)(std::get<1>(avg2) * 100.0f)) << " "
                << std::to_string((int)(std::get<2>(avg2) * 100.0f));
        std::cout << "sending " << message.str() << std::endl;
        if(sendto(s, message.str().c_str(), message.str().size(), 0,
                  (struct sockaddr*)&si_other, slen) == SOCKET_ERROR) {
          std::cout << "sendto() failed with error code : " << WSAGetLastError()
                    << std::endl;
          // exit(EXIT_FAILURE);
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
  }

public:
  /*! \brief Constructor to initialize Network Thread Class
   *  \warning Default constructor is not used
   */
  NetworkThread() = delete;

  /*! \brief Constructor to initialize Network Thread Class
   *  \param [in] queue Queue from the Pipeline Thread
   *  \param [in] filter_length Length of the filter used by the Network Thread
   *  \param [in] ip_address UDP Address to send the network packets
   *  \param [in] port UDP Port to send network packets accross
   */
  NetworkThread(wqueue<std::tuple<bool, float, float>>& queue1,
                wqueue<std::tuple<bool, float, float>>& queue2, 
                int filter_length, std::string ip_address, int port)
      : m_queue1(queue1), m_queue2(queue2), m_stop(false), m_thread() {
    // start networking
    m_filter_length = filter_length;
    s = sizeof(si_other);
    slen = sizeof(si_other);

    // create socket
    if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
      std::cout << "sendto() failed with error code : " << WSAGetLastError()
                << std::endl;
      exit(EXIT_FAILURE);
    }
    int broadcastEnable = 1;
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, &broadcastEnable,
               sizeof(broadcastEnable));

    // setup address structure
    memset((char*)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(port);
    inet_aton(ip_address.c_str(), &si_other.sin_addr);
  }

  /*! \brief Function to stop the thread
  */
  void stop() {
    m_stop = true;
    m_thread.join();
  }

  /*! \brief Function to start the thread
  */
  void start() { m_thread = std::thread(&NetworkThread::run, this); }
};
