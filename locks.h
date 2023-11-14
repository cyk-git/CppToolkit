/*
 * andor_camera_controller.h
 *
 * Created on 27 February 2022
 *   by Yukun Cheng
 *   cyk_phy@mail.ustc.edu.cn
 *
 * Locks is a class to help synchronize threads.
 *   A Locks object can store a mutex-condition_var-flag array 
 *   and provide some functions to lock\hang up\wake threads.
 *   SafeLockUp is a class to provide a safe way to use Locks.
 *   The way to use SafeLockUp is the same as unique_lock.
 *   Every time you want to lock up a mutex in the Locks, you
 *   should create a new object of SafeLockUp. When a new object 
 *   of SafeLockUp was created, it will lock up the corresponding
 *   mutex in Locks using unique_lock. Then you can use Wait() to
 *   hang up threads, use Unlock() to unlock mutex or use Notify()
 *   to send a signal to the corresponding condition_var in Locks.
 *   When the destructor was called, the object will call Unlock()
 *   if the mutex has not been unlocked and Notify() if the object
 *   has not called Notify() or Wait() before.
 */

#ifndef CBMI_CAMERA_LOCKS_H_
#define CBMI_CAMERA_LOCKS_H_

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&); \
  void operator=(const TypeName&)

#include <mutex>
#include <condition_variable>
#include <vector>
#include <map>
#include "log.h"

namespace cpptoolkit {

class Locks {
public:
  friend class SafeLockUp;
  explicit Locks(int number_of_locks = 1);
  ~Locks();

  void Reset();
  void signal_off(int index) { 
    flag_.at(index) = false;
  }

  void lockup(int index) {
    mutex_.at(index).lock();
  }

  // wait_then_signal_of
  void Wait(std::unique_lock<std::mutex>& unique_lock, int index);
  
  // signal_on
  void notify(int index) {
    flag_.at(index) = true;
    cond_var_.at(index).notify_one();
  }

  // signal_on_and_unlock
  void notify_and_unlock(int index) {
    flag_.at(index) = true;
    mutex_.at(index).unlock();
    cond_var_.at(index).notify_one();
  }
  void NotifyAll(); // signal_on_all

private:
  const int kNumberOfLocks_;

  std::vector<std::mutex> mutex_;
  std::vector<std::condition_variable> cond_var_;
  std::vector<bool> flag_;

};

class SafeLockUp {
public:
  explicit SafeLockUp(Locks& locks, int lock_index);
  ~SafeLockUp();

  void wait() { // wait_then_signal_of
    kPtrLocks_->Wait(unique_lock_, kLockIndex_);
    flag_waited_ = true;
  }
  
  void unlock() {
    unique_lock_.unlock();
    flag_lockup_ = false;
  }
  
  void notify() { // signal_on
    kPtrLocks_->notify(kLockIndex_);
    flag_notifyed_ = true;
  }
  
  void notify_and_unlock() { // signal_on_and_unlock
    kPtrLocks_->flag_.at(kLockIndex_) = true;
    unlock();
    notify();
  }

  void signal_off() {
    kPtrLocks_->signal_off(kLockIndex_);
    flag_waited_ = true;
  }

private:
  DISALLOW_COPY_AND_ASSIGN(SafeLockUp);
  Locks* kPtrLocks_;
  const int kLockIndex_;
  std::unique_lock<std::mutex> unique_lock_;
  //std::condition_variable& cond_var_;
  //bool& flag_in_locks_;
  bool flag_lockup_;
  bool flag_notifyed_;
  bool flag_waited_; // If Wait() has been called, it means this SafeLockUp is the receiver of condition variable signel. Then it is not nesserary to call Notify() in the destructor.
};

/**
 * @brief The SleepWaiter class provides a mechanism similar to the
 * std::this_thread::sleep_for() or an operating system's sleep function but
 * with the added ability to be woken up at any time. This is particularly
 * useful in multi-threaded scenarios where it is beneficial to be able to
 * respond to external events during a waiting period.
 *
 * The class is non-copyable and non-movable to ensure unique instances and
 * maintain thread safety.
 *
 * @note The sleep operations provided by this class allow for flexible thread
 * suspension and waking, making it ideal for multi-threaded applications that
 * need to respond quickly to specific events. However, this class does not
 * guarantee precise sleep durations due to the inherent nature of thread
 * scheduling and potential variability in the time it takes to wake up a
 * thread. External factors such as system load, thread contention, and
 * scheduling policy can all affect the actual sleep duration.
 *
 * Usage example:
 *
 *     SleepWaiter sleeper;
 *     // In one thread
 *     sleeper.sleep_for(5000); // Sleep for 5 seconds
 *     // In another thread
 *     sleeper.wake_up(); // Wake all sleeping threads prematurely
 */
class SleepWaiter {
 public:
  SleepWaiter()=default;
  ~SleepWaiter() { wake_up(); };

  SleepWaiter(const SleepWaiter&) = delete;
  SleepWaiter& operator=(const SleepWaiter&) = delete;
  SleepWaiter(SleepWaiter&&) = delete;
  SleepWaiter& operator=(SleepWaiter&&) = delete;

  //void sleep() {
  //  uint64_t key = 0;
  //  {
  //    std::lock_guard<std::mutex> lock(mutex_for_flag_);
  //    key = key_count++;
  //    // find an unexisted key
  //    while (map_flag_wake_.count(key) > 0) {
  //      key++;
  //    }
  //    map_flag_wake_[key] = false;
  //  }
  //  std::mutex temp_mutex;
  //  std::unique_lock<std::mutex> temp_lk(temp_mutex);
  //  cond_var_.wait(temp_lk, [this, key] { return map_flag_wake_[key]; });
  //  {
  //    std::lock_guard<std::mutex> lock(mutex_for_flag_);
  //    map_flag_wake_[key] = true;
  //    map_flag_wake_.erase(key);
  //  }
  //}

  //void sleep_for(uint64_t milliseconds) {
  //  uint64_t key = 0;
  //  {
  //    std::lock_guard<std::mutex> lock(mutex_for_flag_);
  //    key = key_count++;
  //    // find an unexisted key
  //    while (map_flag_wake_.count(key) > 0) {
  //      key++;
  //    }
  //    map_flag_wake_[key] = false;
  //  }
  //  std::mutex temp_mutex;
  //  std::unique_lock<std::mutex> temp_lk(temp_mutex);
  //  cond_var_.wait_for(temp_lk, std::chrono::milliseconds(milliseconds),
  //                     [this,key] { return map_flag_wake_[key]; });
  //  {
  //    std::lock_guard<std::mutex> lock(mutex_for_flag_);
  //    map_flag_wake_[key] = true;
  //    map_flag_wake_.erase(key);
  //  }
  //}

  //template <class _Clock, class _Duration>
  //void sleep_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) { 
  //  uint64_t key = 0;
  //  {
  //    std::lock_guard<std::mutex> lock(mutex_for_flag_);
  //    key = key_count++;
  //    // find an unexisted key
  //    while (map_flag_wake_.count(key) > 0) {
  //      key++;
  //    }
  //    map_flag_wake_[key] = false;
  //  }
  //  std::mutex temp_mutex;
  //  std::unique_lock<std::mutex> temp_lk(temp_mutex);
  //  cond_var_.wait_until(temp_lk, _Abs_time,
  //                     [this, key] { return map_flag_wake_[key]; });
  //  {
  //    std::lock_guard<std::mutex> lock(mutex_for_flag_);
  //    map_flag_wake_[key] = true;
  //    map_flag_wake_.erase(key);
  //  }
  //}

  // Sleep indefinitely until woken up
  void sleep() {
    WaitForCondition([this](std::unique_lock<std::mutex>& lk, uint64_t key) {
      cond_var_.wait(lk, [this, key] { return map_flag_wake_[key]; });
    });
  }

  // Sleep for a specified duration in milliseconds
  void sleep_for(uint64_t milliseconds) {
    WaitForCondition(
        [this, milliseconds](std::unique_lock<std::mutex>& lk, uint64_t key) {
          cond_var_.wait_for(lk, std::chrono::milliseconds(milliseconds),
                             [this, key] { return map_flag_wake_[key]; });
        });
  }

  // Sleep until a specified time point
  template <class _Clock, class _Duration>
  void sleep_until(
      const std::chrono::time_point<_Clock, _Duration>& _Abs_time) {
    WaitForCondition(
        [this, &_Abs_time](std::unique_lock<std::mutex>& lk, uint64_t key) {
          cond_var_.wait_until(lk, _Abs_time,
                               [this, key] { return map_flag_wake_[key]; });
        });
  }

  void wake_up() {
    {
      std::lock_guard<std::mutex> lock(mutex_for_flag_);
      for (auto& pair : map_flag_wake_) {
        pair.second = true;
      }
      cond_var_.notify_all();
    }
  }

 private:
  std::mutex mutex_for_flag_;
  std::condition_variable cond_var_;
  uint64_t key_count = 0;
  std::map<int,bool> map_flag_wake_;

  // Common function to handle key acquisition, setting, and cleanup
  void WaitForCondition(
      std::function<void(std::unique_lock<std::mutex>&, uint64_t)> waitFunc);
};

}

#endif // CBMI_CAMERA_LOCKS_H_