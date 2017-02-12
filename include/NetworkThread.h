#pragma once
#include "WQueue.h"
#include <arpa/inet.h>
#include <atomic>
#include <chrono>
#include <deque>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <thread>
#define WSAGetLastError() (errno)
#define SOCKET_ERROR (-1)

/*! \brief The Network Thread handles information from the Pipeline Thread,
 * filters it and sends
 *  information over the network.
 */
class NetworkThread {
  struct sockaddr_in si_other; //!< Network Socket information
  int slen;                    //!< Network Socket information
  int s;                       //!< Network Socket information

  std::deque<std::pair<bool, int>> m_filter_queue; //!< Filter Queue
  int m_filter_length;                             //!< Max queue length

  wqueue<std::pair<bool, int>>& m_queue; //!< Network Queue
  std::atomic<bool> m_stop;              //!< Is the thread stopped?
  std::thread m_thread;                  //!< Thread to run operations

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
  NetworkThread(wqueue<std::pair<bool, int>>& queue, int filter_length,
                std::string ip_address, int port)
      : m_queue(queue), m_stop(), m_thread() {
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

private:
  /*! \brief Function that the thread runs
  */
  void run() {
    for(int i = 0;; i++) {
      std::unique_ptr<std::pair<bool, int>> temp = m_queue.remove();
      m_filter_queue.push_back(*temp);
      if(m_filter_queue.size() > m_filter_length) {
        m_filter_queue.pop_front();
      }
      bool in_center = false;
      int average = 0;
      for(auto it = m_filter_queue.begin(); it != m_filter_queue.end(); it++) {
        std::cout << (*it).first << " " << (*it).second << std::endl;
        if(!(*it).first) {
          continue;
        }
        else {
          average += (*it).second;
          in_center = true;
        }
      }
      average /= m_filter_length;

      {
        std::stringstream message;
        message << std::to_string(average) << " " << std::to_string(in_center);
        std::cout << "sending " << message.str() << std::endl;
        if(sendto(s, message.str().c_str(), message.str().size(), 0,
                  (struct sockaddr*)&si_other, slen) == SOCKET_ERROR) {
          std::cout << "sendto() failed with error code : " << WSAGetLastError()
                    << std::endl;
          // exit(EXIT_FAILURE);
        }
      }
    }
  }
};
