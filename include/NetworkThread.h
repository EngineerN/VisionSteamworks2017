#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define WSAGetLastError() (errno)
#define SOCKET_ERROR (-1)
#include <thread>
#include <atomic>
#include <iostream>
#include <chrono>
#include <thread>
#include <deque>
#include "WQueue.h"

class NetworkThread{
  wqueue<std::pair<bool,int>>& m_queue;
  int device_id;
  struct sockaddr_in si_other;
  std::atomic<bool> m_stop;
  std::thread m_thread;
  std::deque<std::pair<bool,int>> m_filter_queue;
  int slen;
  int s;
  int m_filter_length;

public:
  NetworkThread() = delete;
  virtual ~NetworkThread(void) {}
  NetworkThread(wqueue<std::pair<bool,int>>& queue_output, int filter_length, std::string ip_address, int port) : m_queue(queue_output), m_stop(), m_thread() {
    // start networking
    m_filter_length = filter_length;
    s = sizeof(si_other);
    slen = sizeof(si_other);

    //create socket
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
      std::cout << "sendto() failed with error code : " << WSAGetLastError() << std::endl;
      exit(EXIT_FAILURE);
    }
    int broadcastEnable = 1;
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    //setup address structure
    memset((char *)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(port);
    inet_aton(ip_address.c_str(), &si_other.sin_addr);
  }
  void stop() { m_stop = true; m_thread.join(); }
  void start() { m_thread = std::thread(&NetworkThread::run, this); }
  void run() {
    for (int i = 0;; i++) {
      std::unique_ptr<std::pair<bool,int>> temp = m_queue.remove();
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
        if (sendto(s, message.str().c_str(), message.str().size(), 0, (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
        {
          std::cout << "sendto() failed with error code : " << WSAGetLastError() << std::endl;
          //exit(EXIT_FAILURE);
        }
      }
    }
  }
};
