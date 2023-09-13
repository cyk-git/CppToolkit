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

} // namespace cpptoolkit
#endif // CPPTOOLKIT_LOG_H_
