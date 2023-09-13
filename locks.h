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

namespace cpptoolkit {

class Locks {
public:
  friend class SafeLockUp;
  explicit Locks(int number_of_locks = 1);
  ~Locks();

  void Reset();

  void lockup(int index) {
    mutex_.at(index).lock();
  }

  void Wait(std::unique_lock<std::mutex>& unique_lock, int index);
  
  void notify(int index) {
    flag_.at(index) = true;
    cond_var_.at(index).notify_one();
  }

  void notify_and_unlock(int index) {
    flag_.at(index) = true;
    mutex_.at(index).unlock();
    cond_var_.at(index).notify_one();
  }
  void NotifyAll();

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

  void wait() {
    kPtrLocks_->Wait(unique_lock_, kLockIndex_);
    flag_waited_ = true;
  }
  
  void unlock() {
    unique_lock_.unlock();
    flag_lockup_ = false;
  }
  
  void notify() {
    kPtrLocks_->notify(kLockIndex_);
    flag_notifyed_ = true;
  }
  
  void notify_and_unlock() {
    kPtrLocks_->flag_.at(kLockIndex_) = true;
    unlock();
    notify();
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

}

#endif // CBMI_CAMERA_LOCKS_H_