#pragma once

#include <QObject>
#include <QString>

#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/spdlog.h>

#include <mutex>

namespace cpptoolkit {

class LogSignalEmitter : public QObject {
  Q_OBJECT
 public:
  explicit LogSignalEmitter(QObject* parent = nullptr) : QObject(parent) {}

 signals:
  void logWarningSignal(const QString& message);
  void logErrorSignal(const QString& message);
};

template <typename Mutex>
class QtPopupSink : public spdlog::sinks::base_sink<Mutex> {
 public:
  explicit QtPopupSink(LogSignalEmitter* emitter) : signal_emitter_(emitter) {}
  LogSignalEmitter* get_emmiter() const { return signal_emitter_; }

 protected:
  void sink_it_(const spdlog::details::log_msg& msg) override {
    if (msg.level >= spdlog::level::warn) {
      // format log message
      spdlog::memory_buf_t formatted;
      spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
      std::string message = fmt::to_string(formatted);

      // emit signal to show popup
      if (msg.level >= spdlog::level::err) {
        emit signal_emitter_->logErrorSignal(QString::fromStdString(message));
      } else {
        emit signal_emitter_->logWarningSignal(QString::fromStdString(message));
      }
    }
  }

  void flush_() override {}

 private:
  LogSignalEmitter* signal_emitter_;
};

// specialized types for convenience
using QtPopupSink_mt = QtPopupSink<std::mutex>;  // thread safe version
using QtPopupSink_st =
    QtPopupSink<spdlog::details::null_mutex>;  // single thread version

inline void AddQPopupSink(
    /*QWidget *parent,*/
    LogSignalEmitter* emmiter,
    spdlog::level::level_enum sink_log_level = spdlog::level::info) {
  // Add QStatusBarSink to default logger
  auto default_logger = spdlog::default_logger();
  auto statusBarSink = std::make_shared<QtPopupSink_mt>(emmiter);
  statusBarSink->set_level(sink_log_level);
  statusBarSink->set_formatter(
      std::make_unique<spdlog::pattern_formatter>("[%l] %v"));
  default_logger->sinks().push_back(statusBarSink);

  // auto messageBoxSink =
  // std::make_shared<QMessageBoxSink<std::mutex>>(parent);
  // messageBoxSink->set_level(spdlog::level::info);
  // messageBoxSink->set_formatter(
  //     std::make_unique<spdlog::pattern_formatter>("[%H:%M:%S.%e]\n %v"));
  // default_logger->sinks().push_back(messageBoxSink);
}

inline void RemovePopupSink(/*QWidget *parent,*/ LogSignalEmitter* emmiter) {
  auto defaultLogger = spdlog::default_logger();
  // defaultLogger->sinks().erase(
  //     std::remove_if(
  //         defaultLogger->sinks().begin(), defaultLogger->sinks().end(),
  //         [parent](const std::shared_ptr<spdlog::sinks::sink> &sink) {
  //           auto ptr_q_status_bar_sink =
  //               dynamic_cast<QMessageBoxSink<std::mutex> *>(sink.get());
  //           if (ptr_q_status_bar_sink != nullptr) {
  //             return ptr_q_status_bar_sink->get_parent() == parent;
  //           }
  //           return false;
  //         }),
  //     defaultLogger->sinks().end());
  defaultLogger->sinks().erase(
      std::remove_if(
          defaultLogger->sinks().begin(), defaultLogger->sinks().end(),
          [emmiter](const std::shared_ptr<spdlog::sinks::sink>& sink) {
            QtPopupSink_mt* ptr_q_status_bar_sink =
                dynamic_cast<QtPopupSink_mt*>(sink.get());
            if (ptr_q_status_bar_sink != nullptr) {
              return ptr_q_status_bar_sink->get_emmiter() == emmiter;
            }
            return false;
          }),
      defaultLogger->sinks().end());
}


}  // namespace cpptoolkit 