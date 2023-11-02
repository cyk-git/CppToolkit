/*
 * handle_exception.h
 *
 * Created on 20230910
 *   by Yukun Cheng
 *   cyk_phy@mail.ustc.edu.cn
 *
 * Handle exceptions by its level
 */

#ifndef CPPTOOLKIT_HANDLE_EXCEPTION_H_
#define CPPTOOLKIT_HANDLE_EXCEPTION_H_

#include <boost\exception\all.hpp>
#include <boost\throw_exception.hpp>
#include <cassert>

#include "log.h"

namespace cpptoolkit {
enum class ErrorLevel { E_WARNING, E_ERROR, E_CRITICAL,E_UNKNOWN };

typedef boost::error_info<struct tag_error_level, ErrorLevel> error_level;

enum class HandleStatus { CONTINUE, STOP, RETHROW };

typedef void (*HandlerFunc)(boost::exception_ptr);

#define PROCESS_HANDLE_STATUS(status, exception) \
  switch (status) {                              \
    case HandleStatus::CONTINUE:                 \
      break;                                     \
    case HandleStatus::STOP:                     \
      return;                                    \
    case HandleStatus::RETHROW:                  \
      BOOST_THROW_EXCEPTION(exception);          \
  }

inline HandleStatus handle_exception(boost::exception_ptr e_ptr,
                              HandlerFunc handle_warning = nullptr,
                              HandlerFunc handle_error = nullptr,
                              HandlerFunc handle_critical = nullptr) {
  try {
    boost::rethrow_exception(e_ptr);
  } catch (const boost::exception& e) {
    const ErrorLevel* level = boost::get_error_info<error_level>(e);
    if (level) {
      switch (*level) {
        case ErrorLevel::E_WARNING:
          LOG_WARN(boost::diagnostic_information(e_ptr));
          if (handle_warning) handle_warning(e_ptr);
          return HandleStatus::CONTINUE;

        case ErrorLevel::E_ERROR:
          LOG_ERROR(boost::diagnostic_information(e_ptr));
          if (handle_error) handle_error(e_ptr);
          return HandleStatus::STOP;

        case ErrorLevel::E_CRITICAL:
          LOG_CRITICAL(boost::diagnostic_information(e_ptr));
          if (handle_critical) handle_critical(e_ptr);
          return HandleStatus::RETHROW;
      }
    }
  } catch (...) {
  }
  // default action
  LOG_ERROR(boost::diagnostic_information(e_ptr));
  if (handle_error) handle_error(e_ptr);
  return HandleStatus::STOP;
}

template <typename T>
void throw_exception(const T& exception, ErrorLevel level,
                     boost::source_location const& loc) {
  boost::throw_exception(
      boost::enable_error_info(exception) << error_level(level), loc);
}

inline void LogMessage(const ErrorLevel& level, const std::string& message) {
  switch (level) {
    case ErrorLevel::E_WARNING:
      LOG_WARN(message);
      break;
    case ErrorLevel::E_UNKNOWN:
    case ErrorLevel::E_ERROR:
      LOG_ERROR(message);
      break;
    case ErrorLevel::E_CRITICAL:
      LOG_CRITICAL(message);
      break;
    default:
      assert(false && "Unknown ErrorLevel");
  }
}


}  // namespace cpptoolkit

#define CPPTOOLKIT_THROW_EXCEPTION(x,level) \
  ::cpptoolkit::throw_exception(x, level, BOOST_CURRENT_LOCATION)

#define CPPTOOLKIT_CHECK_API_ERRORCODE(errorCode, noErrorCode, operationName, \
                                       level)                                 \
  do {                                                                        \
    auto e_code = (errorCode);                                                \
    if (e_code != noErrorCode) {                                              \
      std::string errorMsg =                                                  \
          std::string(operationName) +                                        \
          " returned an error: " + std::to_string(e_code);                    \
      ::cpptoolkit::LogMessage((level), errorMsg);                            \
      CPPTOOLKIT_THROW_EXCEPTION(std::runtime_error(errorMsg), level);        \
    }                                                                         \
  } while (0)


#endif  // CPPTOOLKIT_HANDLE_EXCEPTION_H_