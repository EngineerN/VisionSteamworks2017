#pragma once
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

/*! \brief WQueue is a simple work queue to transfer work between threads.
 */
template <typename T> class wqueue {
  std::queue<std::unique_ptr<T>> m_queue; //!< Queue used to store the data
  std::mutex m_mutex;                     //!< Mutex to lock the queue
  std::condition_variable m_cv; //!< Conditional variable for thread waiting

public:
  /*! \brief Add item to the queue
  *   \param [in] item Item to add to the queue
  */
  void add(T&& item) {
    std::unique_ptr<T> item_ptr = std::make_unique<T>(item);
    std::lock_guard<std::mutex> mutex_lock(m_mutex);
    m_queue.push(std::move(item_ptr));
    m_cv.notify_one();
  }

  /*! \brief Remove item from the queue
  *   \return Unique_ptr to item from the queue
  */
  std::unique_ptr<T> remove() {
    std::unique_lock<std::mutex> mutex_lock(m_mutex);
    while(m_queue.empty()) {
      m_cv.wait(mutex_lock);
    }
    std::unique_ptr<T> item = std::move(m_queue.front());
    m_queue.pop();
    return item;
  }

  /*! \brief Returns the size of the queue
  *   \return Size of the queue
  */
  std::size_t size() {
    std::unique_lock<std::mutex> mutex_lock(m_mutex);
    return m_queue.size();
  }

  /*! \brief Returns if the queue is empty
  *   \return Bool of if the queue is empty
  */
  bool empty() {
    std::unique_lock<std::mutex> mutex_lock(m_mutex);
    return m_queue.empty();
  }
};
