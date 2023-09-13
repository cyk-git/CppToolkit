#include "log.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include "date_time.h"

namespace cpptoolkit {

spdlog::filename_t GetLogFileName(spdlog::filename_t base_filename) {
  cpptoolkit::DateTime time;
#if defined(_WIN32) && defined(SPDLOG_WCHAR_FILENAMES)
  return to_wide_string(experiment_time.date_time()) + base_filename;
#else
  return time.date_time() + base_filename;
#endif

}

void InitLogger(
  spdlog::filename_t log_filepath,
  spdlog::filename_t log_filename,
  spdlog::level::level_enum console_log_level,
  spdlog::level::level_enum file_log_level,
  spdlog::level::level_enum logger_log_level,
  std::size_t max_file_size,
  std::size_t max_file_number) {
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(console_log_level);

  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_filepath + SPDLOG_FILENAME_T("/") + log_filename, max_file_size, max_file_number);
  file_sink->set_level(file_log_level);


  spdlog::sinks_init_list sinks_list = { console_sink, file_sink };
  auto logger = std::make_shared<spdlog::logger>("new_default", sinks_list);
  logger->set_level(logger_log_level);
  spdlog::set_default_logger(logger);
  spdlog::set_pattern("[%H:%M:%S.%e] [thread %t] [%^%l%$] %v");
  spdlog::flush_on(console_log_level);
}

std::string GetIdStr(std::thread* ptr_thread){
  std::stringstream str_stream;
  if (ptr_thread == nullptr) {
    return std::string("NULL");
  }
  str_stream << ptr_thread->get_id();
  return str_stream.str();
}

} // namespace cpptoolkit

