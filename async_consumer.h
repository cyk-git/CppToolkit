/*
 * async_consumer.h
 *
 * Created on 20230910
 *   by Yukun Cheng
 *   cyk_phy@mail.ustc.edu.cn
 *
 * AsyncConsumer is a interface for consumer threads.
 */

#ifndef CPPTOOLKIT_ASYNC_CONSUMER_H_
#define CPPTOOLKIT_ASYNC_CONSUMER_H_

#include <boost\exception\all.hpp>
#include <boost\lockfree\queue.hpp>
#include <memory>
#include "handle_exception.h"
#include <queue>
#include "log.h"
#include "locks.h"

namespace cpptoolkit {
class AsyncConsumer {
 public:
  AsyncConsumer() = default;
  virtual ~AsyncConsumer() { Close(); };
  AsyncConsumer(const AsyncConsumer&) = delete;
  AsyncConsumer& operator=(const AsyncConsumer&) = delete;
  virtual void Init() { flag_init_ = true; }

 protected:
  bool flag_init_ = false;
  bool flag_run_ = false;
  std::unique_ptr<std::thread> th_loop_;
  Locks lock_data_transfer_;
  
  virtual void Start() { start_loop(); }
  void start_loop() {
    if (flag_run_ == false) {
      // Clear queue_critical_exception_ptr_
      queue_critical_exception_ptr_ = std::queue<boost::exception_ptr>();
      flag_run_ = true;
      th_loop_ = std::make_unique<std::thread>(&AsyncConsumer::ConsumerLoop, this);
    }
  }
  //void consumer_thread_function() { ConsumerLoop(); }
  virtual void ConsumerLoop();
  virtual void CoreLoop();
  virtual void PostCoreLoop();
  void DefaultCoreLoop();
  void CleanUpBuffer();
  void stop_loop() {
    if (flag_run_ == true) {
      flag_run_ = false;
      lock_data_transfer_.NotifyAll();
      if (th_loop_ && th_loop_->joinable()) {
        th_loop_->join();
      }
    }
  }
  virtual void Close() {
    stop_loop();
    flag_init_ = false;
  };

  virtual void LoadDataForProcess() = 0;
  virtual void ProcessData() = 0;
  virtual void ClearDataBuffer() = 0;
  virtual bool is_need_wait_for_data() = 0;
  virtual bool is_data_buffer_empty() = 0;
  bool never_wait_for_data() { return false; }

  void PreGetData();
  void PostGetData();

  //Handle exception
  bool flag_handling_error_ = false;
  std::queue<boost::exception_ptr> queue_critical_exception_ptr_;
  virtual void handle_warning(boost::exception_ptr) {}
  virtual void handle_error(boost::exception_ptr) {
    flag_handling_error_ = true;
    stop_loop();
    flag_handling_error_ = false;
  }
  virtual void handle_critical(boost::exception_ptr e_ptr) {
    flag_handling_error_ = true;
    queue_critical_exception_ptr_.push(e_ptr);
    Close();
    flag_handling_error_ = false;
  }
  virtual ErrorLevel HandleException(boost::exception_ptr e_ptr);

 private:
  // The following functions are only used to demonstrate the structure of
  // asynchronous call functions and do not have actual functionality
  void ProcessAsync();
  void GetData(){};
};

class AsyncConsumerTest : public AsyncConsumer {
 public:
  AsyncConsumerTest(int width, int height);
  virtual ~AsyncConsumerTest() { Close(); };
  AsyncConsumerTest(const AsyncConsumerTest&) = delete;
  AsyncConsumerTest& operator=(const AsyncConsumerTest&) = delete;

  void ProcessDataAsync(std::unique_ptr<int[]> data_ptr);

 protected:
  std::queue<std::unique_ptr<int[]>> queue_data_buffer_;
  std::unique_ptr<int[]> loaded_data_;
  int width_;
  int height_;

  virtual void ConsumerLoop() { LOG_DEBUG("test"); }

  virtual void LoadDataForProcess();
  virtual void ProcessData();
  virtual void ClearDataBuffer();
  virtual bool is_need_wait_for_data() { return queue_data_buffer_.empty(); }
  virtual bool is_data_buffer_empty() { return queue_data_buffer_.empty(); }

 private:
  void GetData(std::unique_ptr<int[]> data_ptr);

};

}  // namespace cpptoolkit

#endif  // CPPTOOLKIT_ASYNC_CONSUMER_H_