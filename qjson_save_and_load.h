/*
 * qjson_save_and_load.h
 *
 * Created on 19 March 2024
 *   by Yukun Cheng
 *   cyk_phy@mail.ustc.edu.cn
 *
 * QJsonSaveAndLoad is a utility class designed to simplify the process of
 *   saving and loading application data in JSON format. 
 */

#ifndef CPPTOOLKIT_QT_JSON_SAVE_AND_LOAD_H_
#define CPPTOOLKIT_QT_JSON_SAVE_AND_LOAD_H_

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtConcurrent/QtConcurrent>

namespace cpptoolkit {
inline void SaveQJsonObject(const QJsonObject &rootObj,
                            const QString &fileName) {
  QJsonDocument doc(rootObj);
  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(nullptr, QObject::tr("Error"),
                          QObject::tr("Failed to open the file for writing."));
    return;
  }

  if (file.write(doc.toJson()) == -1) {
    QMessageBox::critical(nullptr, QObject::tr("Error"),
                          QObject::tr("Failed to write to the file."));
  }
}

inline QJsonObject LoadQJsonObject(const QString &fileName) {
  QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(nullptr, QObject::tr("Error"),
                          QObject::tr("Failed to open the file."));
    return QJsonObject();
  }

  QByteArray data = file.readAll();
  QJsonDocument doc = QJsonDocument::fromJson(data);
  if (doc.isNull()) {
    QMessageBox::critical(nullptr, QObject::tr("Error"),
                          QObject::tr("Failed to parse the file as JSON."));
    return QJsonObject();
  }

  return doc.object();
}

inline QString BrowseToSaveJsonFile(
    const QJsonObject &rootObj, const QString &suffix = QString(".json"),
    QWidget *parent = nullptr, const QString &caption = QString(),
    const QString &default_dir = QString(), const QString &filter = QString(),
    QString *selectedFilter = nullptr,
    QFileDialog::Options options = QFileDialog::Options()) {
  // Check if the default preset directory exists, create it if it doesn't
  QDir dir(default_dir);
  if (!dir.exists()) {
    if (!dir.mkpath(".")) {  // Attempt to create the directory and its parents
      QMessageBox::critical(
          nullptr, QObject::tr("Error"),
          QObject::tr("Failed to create the default directory: %1")
              .arg(default_dir));
      return QString();  // Exit the function as the directory couldn't be created
    }
  }
  // Open a file save dialog with a specific filter for .waveblueprint files
  QString fileName =
      QFileDialog::getSaveFileName(parent,
                                   caption,      // Dialog title
                                   default_dir,  // Initial directory
                                   filter,       // File type filter
                                   selectedFilter, options);

  // Check if the user has canceled the dialog
  if (fileName.isEmpty()) {
    return QString();  // Exit the function as there's no file to save
  }

  // Ensure the file name has the correct .waveblueprint suffix
  if (!fileName.endsWith(suffix)) {
    fileName.append(suffix);
  }

  try {
    // Attempt to save the blueprint to the specified file
    SaveQJsonObject(rootObj, fileName);
    return fileName;
  } catch (const std::exception &e) {
    // Handle standard exceptions (e.g., file write errors)
    QMessageBox::critical(nullptr, QObject::tr("Error"),
                          QObject::tr("Failed to save the file: %1")
                              .arg(e.what())  // Show the error message
    );
  } catch (...) {
    // Catch-all for any other unexpected exceptions
    QMessageBox::critical(
        nullptr, QObject::tr("Error"),
        QObject::tr("An unknown error occurred while saving the file."));
  }
  return QString();
}



} // namespace cpptoolkit

#endif // CPPTOOLKIT_QT_JSON_SAVE_AND_LOAD_H_
