/*
 * date_time.h
 *
 * Created on 11 March 2022
 *   by Yukun Cheng
 *   cyk_phy@mail.ustc.edu.cn
 *
 * DateTime is a class to get and output current time.
 */

#ifndef CBMI_CAMERA_DATE_TIME_H_
#define CBMI_CAMERA_DATE_TIME_H_

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&); \
  void operator=(const TypeName&)

#include <sys/timeb.h>
#include <stdint.h>
#include <chrono>
#include <string>
#include <fstream>

namespace cpptoolkit{

class DateTime{
public:
  DateTime();
  // ~DateTime();
  void GetLocalTime();

  int year() const { return year_; }
  int month() const { return month_; }
  int day() const { return day_; }
  int hour() const { return hour_; }
  int minute() const { return minute_; }
  int second() const { return second_; }
  int millisecond() const { return millisecond_; }
  const char* date_time() const { return date_time_; }
  const char* date_time_ms() const { return date_time_ms_; }

  //friend Timestamp;

private:
  struct timeb tb;
  int year_;
  int month_;
  int day_;
  int hour_;
  int minute_;
  int second_;
  int millisecond_;
  char date_time_[20];
  char date_time_ms_[24];

};

class StopWatch {
public:
  StopWatch() {
    start_time_point_ = std::chrono::high_resolution_clock::now();
  }
  void reset() {
    start_time_point_ = std::chrono::high_resolution_clock::now();
  }

  void sync(StopWatch source_stopwatch) {
    start_time_point_ = source_stopwatch.start_time_point_;
  }

  int64_t get_timestamp() {
    time_stamp_ = std::chrono::high_resolution_clock::now();
    return (time_stamp_ - start_time_point_).count();
  }

  double lap(double time_unit) {
    return get_timestamp() / time_unit;
  }

  double lap_ns() {
    return lap(kNanoSecond);
  }

  double lap_us() {
    return lap(kMicroSecond);
  }

  double lap_ms() {
    return lap(kMilliSecond);
  }

  double lap_s() {
    return lap(kSecond);
  }

  static int64_t get_count_freq_Hz() {
    return kSecond;
  }

  //static int64_t convert_time_unit(int64_t timestamp, int64_t time_unit) {
  //  return timestamp % time_unit;
  //}

  static std::string get_formated_time(int64_t timestamp) {
    char time_str[32] = { 0 };
    sprintf_s(time_str, "%02lld:%02lld:%02lld", (timestamp / kHour), (timestamp / kMinute % 60), (timestamp / kSecond % 60));
    return time_str;
  }

  const static int64_t kHour = 3600000000000;
  const static int64_t kMinute = 60000000000;
  const static int64_t kSecond = 1000000000;
  const static int64_t kMilliSecond = 1000000;
  const static int64_t kMicroSecond = 1000;
  const static int64_t kNanoSecond = 1;


private:
  std::chrono::high_resolution_clock::time_point start_time_point_;
  std::chrono::high_resolution_clock::time_point time_stamp_;
};

class StopWatchWithLog:public StopWatch{
public:
  StopWatchWithLog(std::wstring file_path_name);
  ~StopWatchWithLog();
  void write_timestamp(std::wstring info) {
    log_file_ << get_timestamp() << "," << info << std::endl;
  }

private:
  std::wofstream log_file_;
};


//class Timestamp {
//public:
//  explicit Timestamp(int64_t tick_frequency);
//  void ResetBaseTime();
//  void GetTime(int64_t tick_count);
//
//  int64_t tick_frequency() { return tick_frequency_; }
//  void set_tick_frequency(int64_t tick_frequency) { tick_frequency_ = tick_frequency; }
//
//  const char* date_time() const { return current_time_.date_time_; }
//  const char* date_time_ms() const { return current_time_.date_time_ms_; }
//
//private:
//  int64_t tick_frequency_;
//  DateTime base_time_;
//  DateTime current_time_;
//};

} // namespace cpptoolkit

#endif // CBMI_CAMERA_DATE_TIME_H_