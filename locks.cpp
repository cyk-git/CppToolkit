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

}