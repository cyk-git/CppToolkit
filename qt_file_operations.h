/*
 * qt_file_operations.h
 *
 * Created on 19 March 2024
 *   by Yukun Cheng
 *   cyk_phy@mail.ustc.edu.cn
 *
 * QJsonSaveAndLoad is a utility class designed to simplify the process of
 *   saving and loading application data in JSON format. 
 */

#ifndef CPPTOOLKIT_QT_FILE_OPERATIONS_H_
#define CPPTOOLKIT_QT_FILE_OPERATIONS_H_

#include "log.h"
#include <string>
#include <utility>
#include <QDir>
#include <QString>
#include <QUuid>

namespace cpptoolkit {
inline void create_directory_if_needed(const QString& dir_path) {
  QDir qdir(dir_path);
  if (!qdir.exists()) {
    if (!qdir.mkpath(qdir.absolutePath())) {
      std::string path = dir_path.toStdString();
      LOG_ERROR("Failed to create directory: {}", path);
      throw std::runtime_error("Failed to create directory: " + path);
    }
  }
}

inline QString generate_temp_file_path(const QString& root_dir,
                             const QString& surfix = ".temp",
                             const QString& prefix = "") {
  QDir root_qdir(root_dir);
  QString temp_path = root_qdir.filePath(
      prefix + QUuid::createUuid().toString(QUuid::Id128) + surfix);
  return temp_path;
}

inline QString generate_file_path(const QString& root_dir,
                                    const QString& filename) {
  QDir root_qdir(root_dir);
  QString file_path = root_qdir.filePath(filename);
  return file_path;
}

inline void rename_temp_file(const QString& temp_path, const QString& file_path) {
  if (QFile::rename(temp_path, file_path)) {
    return;
  }

  { // handle rename failure
    std::string temp_path_std = temp_path.toStdString();
    std::string file_path_std = file_path.toStdString();
    LOG_WARN("Failed to rename temporary file to final file: {} to {}",
             temp_path_std, file_path_std);

    if (QFile::exists(temp_path) && !QFile::remove(temp_path)) {
      LOG_WARN("Failed to remove temporary file: {}", temp_path_std);
    }

    throw std::runtime_error("Failed to rename temporary file to final file: " +
                             temp_path_std + " to " + file_path_std);
  }
}

// Example usage of the above functions
//void save_file(const QString& dir_path, const QString& filename) { 
//  create_directory_if_needed(dir_path);
//  QString temp_path = generate_temp_file_path(dir_path);
//  QString file_path = generate_file_path(dir_path,filename);
//  //save
//  rename_temp_file(temp_path,file_path);
//}

}  // namespace cpptoolkit

#endif // CPPTOOLKIT_QT_FILE_OPERATIONS_H_
