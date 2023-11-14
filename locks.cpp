#include "locks.h"

namespace cpptoolkit {

  Locks::Locks(int number_of_locks)
    : kNumberOfLocks_(number_of_locks),
    mutex_(number_of_locks),
    cond_var_(number_of_locks),
    flag_(number_of_locks) {
  for (int i = 0; i < kNumberOfLocks_; i++) {
    flag_.at(i) = false;
  }
  return;
}

void Locks::Reset() {
  for (int i = 0; i < kNumberOfLocks_; i++) {
    flag_.at(i) = false;
  }
  return;
}

void Locks::NotifyAll() {
  for (int i = 0; i < kNumberOfLocks_; i++) {
    notify(i);
  }
}

Locks::~Locks(){
  // NotifyAll();
}

void Locks::Wait(std::unique_lock<std::mutex>& unique_lock, int index) {
  while (!flag_.at(index)) {
    cond_var_.at(index).wait(unique_lock);
  }
  flag_.at(index) = false;
}

SafeLockUp::SafeLockUp(Locks& locks, int lock_index)
  : kPtrLocks_(&locks),
    kLockIndex_(lock_index),
    unique_lock_(locks.mutex_.at(lock_index)) {
  flag_lockup_ = true;
  flag_notifyed_ = false;
  flag_waited_ = false;
  return;
}

SafeLockUp::~SafeLockUp() {
  if (flag_lockup_) {
    unlock();
  }
  if (!flag_waited_ && !flag_notifyed_) {
    notify();
  }
  return;
}

void SleepWaiter::WaitForCondition(
    std::function<void(std::unique_lock<std::mutex>&, uint64_t)> waitFunc) {
  uint64_t key = 0;
  {
    std::lock_guard<std::mutex> lock(mutex_for_flag_);
    key = key_count++;  // Increment the key count
    // Ensure the key is unique in the map
    while (map_flag_wake_.count(key) > 0) {
      key++;
    }
    map_flag_wake_[key] = false;  // Set the key's flag to false initially
  }

  // Temp mutex for condition variable
  std::mutex temp_mutex;
  std::unique_lock<std::mutex> temp_lk(temp_mutex);
  waitFunc(temp_lk, key);  // Invoke the waiting logic

  {
    std::lock_guard<std::mutex> lock(mutex_for_flag_);
    // Clean up: set the key's flag to true and remove it from the map
    map_flag_wake_[key] = true;
    map_flag_wake_.erase(key);
  }
}

} // namespace cpptoolkit