/*
 * log.h
 *
 * Created on 18 November 2022
 *   by Yukun Cheng
 *   cyk_phy@mail.ustc.edu.cn
 *
 * For program logging.
 */

#ifndef CPPTOOLKIT_LOG_H_
#define CPPTOOLKIT_LOG_H_

#include <spdlog\spdlog.h>
#include <spdlog/fmt/bundled/ostream.h>
#include <spdlog/fmt/chrono.h>
#include <sstream>

namespace cpptoolkit {

#define LOG_TRACE spdlog::trace
#define LOG_DEBUG spdlog::debug
#define LOG_INFO spdlog::info
#define LOG_WARN spdlog::warn
#define LOG_ERROR spdlog::error
#define LOG_CRITICAL spdlog::critical

spdlog::filename_t GetLogFileName(spdlog::filename_t base_filename);

void InitLogger(
  spdlog::filename_t log_filepath = "logs",
  spdlog::filename_t log_filename = "default_log.txt",
  spdlog::level::level_enum console_log_level = spdlog::level::warn,
  spdlog::level::level_enum file_log_level = spdlog::level::trace,
  spdlog::level::level_enum logger_log_level = spdlog::level::trace,
  std::size_t max_file_size = 1024 * 1024 * 5,
  std::size_t max_file_number = 200);

std::string GetIdStr(std::thread* ptr_thread);

//template <typename T>
//inline std::string PreviewVector(const std::vector<T>& vec, int n = 5) {
//  std::stringstream ss;
//
//  // If the vector size is less than or equal to 2n, convert the entire vector
//  // to string
//  if (vec.size() <= 2 * n) {
//    for (const auto& item : vec) {
//      ss << item << " ";
//    }
//  } else {
//    // Add the first n elements
//    for (int i = 0; i < n; ++i) {
//      ss << vec[i] << " ";
//    }
//    ss << "... ";
//    // Add the last n elements
//    for (int i = vec.size() - n; i < vec.size(); ++i) {
//      ss << vec[i] << " ";
//    }
//  }
//
//  return ss.str();
//}

//class LogLevelGuard {
// public:
//  explicit LogLevelGuard(spdlog::level::level_enum new_level)
//      : logger_(spdlog::default_logger()), original_level_(logger_->level()) {
//    logger_->set_level(new_level);
//  }
//
//  LogLevelGuard(std::shared_ptr<spdlog::logger> logger,
//                spdlog::level::level_enum new_level)
//      : logger_(std::move(logger)), original_level_(logger_->level()) {
//    logger_->set_level(new_level);
//  }
//
//  ~LogLevelGuard() { logger_->set_level(original_level_); }
//
//  LogLevelGuard(const LogLevelGuard&) = delete;
//  LogLevelGuard& operator=(const LogLevelGuard&) = delete;
//
// private:
//  std::shared_ptr<spdlog::logger> logger_;
//  spdlog::level::level_enum original_level_;
//};


template <typename T>
std::string PreviewVector(
    const std::vector<T>& vec, int n = 5,
    std::function<std::string(const T&)> convert =
        [](const T& val) -> std::string {
      std::stringstream ss;
      ss << val;
      return ss.str();
    }) {
  std::stringstream ss;
  // If the vector size is less than or equal to 2n, convert the entire vector
  // to string
  if (vec.size() <= 2 * n) {
    for (const auto& item : vec) {
      ss << convert(item) << " ";
    }
  } else {
    // Add the first n elements
    for (int i = 0; i < n; ++i) {
      ss << convert(vec[i]) << " ";
    }
    ss << "... ";
    // Add the last n elements
    for (size_t i = vec.size() - n; i < vec.size(); ++i) {
      ss << convert(vec[i]) << " ";
    }
  }

  return ss.str();
}

//template <typename T>
//inline std::string ToStringUseSStream(T data) {
//  std::stringstream ss;
//  ss << data;
//  return ss.str();
//}

template <typename... Args>
inline std::string ToStringStream(Args&&... args) {
  std::stringstream ss;
  using expander = int[];
  (void)expander{0, (void(ss << std::forward<Args>(args)), 0)...};
  return ss.str();
}

} // namespace cpptoolkit
#endif // CPPTOOLKIT_LOG_H_
