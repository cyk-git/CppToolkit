/*
 * q_status_bar_sink.h
 *
 * Created on 20231028
 *   by Yukun Cheng
 *   cyk_phy@mail.ustc.edu.cn
 *
 */

#ifndef CPPTOOLKIT_Q_STATUS_BAR_SINK_H_
#define CPPTOOLKIT_Q_STATUS_BAR_SINK_H_

#include <spdlog/sinks/base_sink.h>

#include <QStatusBar>
#include <QMessageBox>

namespace cpptoolkit {
template <typename Mutex>
class QStatusBarSink : public spdlog::sinks::base_sink<Mutex> {
 public:
  QStatusBarSink(QStatusBar *statusBar) : m_statusBar(statusBar) {}
  QStatusBar *get_status_bar() { return m_statusBar; }

 protected:
  void sink_it_(const spdlog::details::log_msg &msg) override {
    spdlog::memory_buf_t formatted;
    spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
    m_statusBar->showMessage(QString::fromStdString(fmt::to_string(formatted)));
  }

  void flush_() override {
    // Nothing to flush in this example
  }

 private:
  QStatusBar *m_statusBar;
};

//template <typename Mutex>
//class QMessageBoxSink : public spdlog::sinks::base_sink<Mutex> {
// public:
//  QMessageBoxSink(QWidget *parent) : parent_(parent){}
//  QWidget *get_parent() { return parent_; }
//
// protected:
//  void sink_it_(const spdlog::details::log_msg &msg) override {
//    spdlog::memory_buf_t formatted;
//    spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
//    QMessageBox::critical(nullptr, QMessageBox::tr("Error"),
//                          QString::fromStdString(fmt::to_string(formatted)));
//  }
//
//  void flush_() override {
//    // Nothing to flush in this example
//  }
//
// private:
//  //QStatusBar *m_statusBar;
//  QWidget *parent_;
//};



inline void AddQStatusBarSink(
    /*QWidget *parent,*/
    QStatusBar *statusBar,
    spdlog::level::level_enum sink_log_level = spdlog::level::info) {
  // Add QStatusBarSink to default logger
  auto default_logger = spdlog::default_logger();
  auto statusBarSink = std::make_shared<QStatusBarSink<std::mutex>>(statusBar);
  statusBarSink->set_level(sink_log_level);
  statusBarSink->set_formatter(
      std::make_unique<spdlog::pattern_formatter>("[%l] %v"));
  default_logger->sinks().push_back(statusBarSink);

  //auto messageBoxSink = std::make_shared<QMessageBoxSink<std::mutex>>(parent);
  //messageBoxSink->set_level(spdlog::level::info);
  //messageBoxSink->set_formatter(
  //    std::make_unique<spdlog::pattern_formatter>("[%H:%M:%S.%e]\n %v"));
  //default_logger->sinks().push_back(messageBoxSink);
}

inline void RemoveStatusBarSink(/*QWidget *parent,*/ QStatusBar *statusBar) {
  auto defaultLogger = spdlog::default_logger();
  //defaultLogger->sinks().erase(
  //    std::remove_if(
  //        defaultLogger->sinks().begin(), defaultLogger->sinks().end(),
  //        [parent](const std::shared_ptr<spdlog::sinks::sink> &sink) {
  //          auto ptr_q_status_bar_sink =
  //              dynamic_cast<QMessageBoxSink<std::mutex> *>(sink.get());
  //          if (ptr_q_status_bar_sink != nullptr) {
  //            return ptr_q_status_bar_sink->get_parent() == parent;
  //          }
  //          return false;
  //        }),
  //    defaultLogger->sinks().end());
  defaultLogger->sinks().erase(
      std::remove_if(
          defaultLogger->sinks().begin(), defaultLogger->sinks().end(),
          [statusBar](const std::shared_ptr<spdlog::sinks::sink> &sink) {
            auto ptr_q_status_bar_sink =
                dynamic_cast<QStatusBarSink<std::mutex> *>(sink.get());
            if (ptr_q_status_bar_sink != nullptr) {
              return ptr_q_status_bar_sink->get_status_bar() == statusBar;
            }
            return false;
          }),
      defaultLogger->sinks().end());
}

}  // namespace cpptoolkit

#endif  // CPPTOOLKIT_Q_STATUS_BAR_SINK_H_