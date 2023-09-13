#include "date_time.h"
#include <time.h>
#include <cstdio>
#include <fstream>

namespace cpptoolkit {

DateTime::DateTime(){
  GetLocalTime();
}

void DateTime::GetLocalTime() {
  ftime(&tb);
  time_t& timep = tb.time;
  struct tm time_struct;

  localtime_s(&time_struct, &timep);
  year_ = time_struct.tm_year;
  month_ = time_struct.tm_mon;
  day_ = time_struct.tm_mday;
  hour_ = time_struct.tm_hour;
  minute_ = time_struct.tm_min;
  second_ = time_struct.tm_sec;
  millisecond_ = tb.millitm;
  strftime(date_time_, sizeof(date_time_), "%Y-%m-%d_%H-%M-%S", &time_struct);
  sprintf_s(date_time_ms_, "%s-%03d", date_time_,millisecond_);
  return;
}

//Timestamp::Timestamp(int64_t tick_frequency)
//    : tick_frequency_(tick_frequency),
//    base_time_(),
//    current_time_() {}
//
//void Timestamp::ResetBaseTime() {
//  base_time_.GetLocalTime();
//}
//
//void Timestamp::GetTime(int64_t tick_count) {
//  current_time_.hour_=base_time_.hour_+tick_count/(tick_frequency_*60*60)%24
//}

StopWatchWithLog::StopWatchWithLog(std::wstring file_path_name)
  : log_file_(file_path_name, std::ios::out | std::ios::app){
  // log_file_.open(file_path_name, std::ios::out | std::ios::app);
}

StopWatchWithLog::~StopWatchWithLog(){
  log_file_.close();
}

} // namespace cpptoolkit

