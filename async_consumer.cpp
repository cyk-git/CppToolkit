#include "async_consumer.h"

void cpptoolkit::AsyncConsumer::ConsumerLoop() {
  while (flag_run_) {
    CoreLoop();
  }
  PostCoreLoop();
}

void cpptoolkit::AsyncConsumer::CoreLoop() { DefaultCoreLoop(); }

void cpptoolkit::AsyncConsumer::PostCoreLoop() { CleanUpBuffer(); }

void cpptoolkit::AsyncConsumer::DefaultCoreLoop() {
  try {
    SafeLockUp lock(lock_data_transfer_, 0);
    if (is_need_wait_for_data()) {
      lock.wait();
    } else {
      lock.signal_off();  // reset signal flag. 
    }
    if (!flag_run_) {
      return;
    }
    if (!is_data_buffer_empty()) {
      LoadDataForProcess();
    }
    lock.unlock();
    if (flag_run_) {
      ProcessData();
    }
  } catch (...) {
    HandleException(boost::current_exception());
  }
}

void cpptoolkit::AsyncConsumer::CleanUpBuffer() {
  while (!is_data_buffer_empty()) {
    try {
      LoadDataForProcess();
      ProcessData();
    } catch (...) {
      auto level = HandleException(boost::current_exception());
      if (level != ErrorLevel::E_WARNING) {
        ClearDataBuffer();
      }
    }
  }
}

void cpptoolkit::AsyncConsumer::PreGetData() {
  //Check Critical Exception
  // First check existed critical exceptions
  // Critical exceptions will call Close(), so the program won't pass Init check
  // Start() will clear all Critical exceptions
  if (!queue_critical_exception_ptr_.empty()) {
    boost::exception_ptr e = queue_critical_exception_ptr_.front();
    queue_critical_exception_ptr_.pop();
    boost::rethrow_exception(e);
  }
  // Check Init
  if (!flag_init_) {
    CPPTOOLKIT_THROW_EXCEPTION(std::logic_error("Logic Error! Please Init Consumer First!"),ErrorLevel::E_CRITICAL);
  }
}

void cpptoolkit::AsyncConsumer::PostGetData() {
  // Check start loop
  try {
    if (!flag_run_&&!flag_handling_error_) {
      Start();
    }
  } catch (...) {
    auto level = HandleException(boost::current_exception());
    if (level == ErrorLevel::E_CRITICAL) {
      boost::rethrow_exception(boost::current_exception());
    }
  }
}

cpptoolkit::ErrorLevel cpptoolkit::AsyncConsumer::HandleException(
    boost::exception_ptr e_ptr) {
  try {
    boost::rethrow_exception(e_ptr);
  } catch (const boost::exception& e) {
    const ErrorLevel* level = boost::get_error_info<cpptoolkit::error_level>(e);
    if (level) {
      switch (*level) {
        case ErrorLevel::E_WARNING:
          LOG_WARN(boost::diagnostic_information(e_ptr));
          handle_warning(e_ptr);
          return ErrorLevel::E_WARNING;

        case ErrorLevel::E_ERROR:
          LOG_ERROR(boost::diagnostic_information(e_ptr));
          handle_error(e_ptr);
          return ErrorLevel::E_ERROR;

        case ErrorLevel::E_CRITICAL:
          LOG_CRITICAL(boost::diagnostic_information(e_ptr));
          handle_critical(e_ptr);
          return ErrorLevel::E_CRITICAL;
      }
    }
  } catch (...) {
  }
  // default action
  LOG_ERROR(boost::diagnostic_information(e_ptr));
  handle_error(e_ptr);
  return ErrorLevel::E_UNKNOWN;
}

void cpptoolkit::AsyncConsumer::ProcessAsync() { 
  PreGetData();
  try {
    if (!flag_handling_error_) {
      SafeLockUp lock(lock_data_transfer_, 0);
      GetData();
      lock.notify_and_unlock();
    }
  } catch (...) {
    auto level = HandleException(boost::current_exception());
    if (level == ErrorLevel::E_CRITICAL) {
      boost::rethrow_exception(boost::current_exception());
    }
  }
  PostGetData();
}

cpptoolkit::AsyncConsumerTest::AsyncConsumerTest(int width, int height)
    : width_(width), height_(height) {}

void cpptoolkit::AsyncConsumerTest::ProcessDataAsync(
    std::unique_ptr<int[]> data_ptr) {
  PreGetData();
  try {
    if (!flag_handling_error_) {
      SafeLockUp lock(lock_data_transfer_, 0);
      GetData(std::move(data_ptr));
      lock.notify_and_unlock();
    }
  } catch (...) {
    auto level = HandleException(boost::current_exception());
    if (level == ErrorLevel::E_CRITICAL) {
      boost::rethrow_exception(boost::current_exception());
    }
  }
  PostGetData();
}

void cpptoolkit::AsyncConsumerTest::LoadDataForProcess() { 
  loaded_data_ = std::move(queue_data_buffer_.front());
  queue_data_buffer_.pop();
}

void cpptoolkit::AsyncConsumerTest::ProcessData() {
  LOG_INFO("[Consumer Process]First elem: {}", loaded_data_[0]);
}

void cpptoolkit::AsyncConsumerTest::ClearDataBuffer() {
  queue_data_buffer_ = std::queue<std::unique_ptr<int[]>>();
}

void cpptoolkit::AsyncConsumerTest::GetData(std::unique_ptr<int[]> data_ptr) {
  queue_data_buffer_.push(std::move(data_ptr));
  LOG_DEBUG("GetData Succeed! Queue size:{}", queue_data_buffer_.size());
}
