/*
 * hdf5_toolkit.h
 *
 * Created on 20231223
 *   by Yukun Cheng
 *   cyk_phy@mail.ustc.edu.cn
 *
 */

#ifndef CPPTOOLKIT_HDF5_TOOLKIT_H_
#define CPPTOOLKIT_HDF5_TOOLKIT_H_

#include <iostream>
#include <vector>
#include <mutex>
#include <highfive/H5File.hpp>
#include <highfive/H5Group.hpp>
#include <xtensor-io/xhighfive.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xadapt.hpp>
//#include <xtensor/xview.hpp>
#ifdef slots
#undef slots
#define slots_undefed
#endif

#include <ATen/ATen.h>

#ifdef slots_undefed
#define slots Q_SLOTS
#endif

#include <CppToolkit/log.h>

//#include <torch/types.h>
//#include <torch\all.h>


namespace cpptoolkit {
//// This function template maps C++ types to corresponding HDF5 data types.
//// It is useful for operations where an HDF5 data type is required, and
//// you are working with native C++ types. This function helps to reduce
//// the manual effort of mapping and provides a more type-safe way of
//// handling HDF5 data types in C++.
//template <typename T>
//inline H5::DataType GetHDF5DataType() {
//  if constexpr (std::is_same_v<T, char>) {
//    return H5::PredType::NATIVE_CHAR;
//  } else if constexpr (std::is_same_v<T, unsigned char>) {
//    return H5::PredType::NATIVE_UCHAR;
//  } else if constexpr (std::is_same_v<T, short>) {
//    return H5::PredType::NATIVE_SHORT;
//  } else if constexpr (std::is_same_v<T, unsigned short>) {
//    return H5::PredType::NATIVE_USHORT;
//  } else if constexpr (std::is_same_v<T, int>) {
//    return H5::PredType::NATIVE_INT;
//  } else if constexpr (std::is_same_v<T, unsigned int>) {
//    return H5::PredType::NATIVE_UINT;
//  } else if constexpr (std::is_same_v<T, long>) {
//    return H5::PredType::NATIVE_LONG;
//  } else if constexpr (std::is_same_v<T, unsigned long>) {
//    return H5::PredType::NATIVE_ULONG;
//  } else if constexpr (std::is_same_v<T, long long>) {
//    return H5::PredType::NATIVE_LLONG;
//  } else if constexpr (std::is_same_v<T, unsigned long long>) {
//    return H5::PredType::NATIVE_ULLONG;
//  } else if constexpr (std::is_same_v<T, float>) {
//    return H5::PredType::NATIVE_FLOAT;
//  } else if constexpr (std::is_same_v<T, double>) {
//    return H5::PredType::NATIVE_DOUBLE;
//  } else {
//    static_assert(false, "Unsupported type for HDF5 data type mapping.");
//  }
//}

//template <typename T>
//inline xt::xarray<T> LoadDatasetToXtensor(const std::string& filename,
//                                          const std::string& datasetName) {
//  HighFive::File file(filename, HighFive::File::ReadOnly);
//  HighFive::DataSet dataset = file.getDataSet(datasetName);
//  std::vector<size_t> dims = dataset.getDimensions();
//  xt::xarray<T> array_data(dims);
//  array_data = xt::load_hdf5<xt::xarray<T>>(filename, datasetName);
//  return std::move(array_data);
//}

//template <typename T>
//inline xt::xarray<T> LoadDatasetToXtensor(const std::string& filename,
//                                          const std::string& datasetName) {
//  // 使用 xt::load_hdf5 直接从 HDF5 文件读取数据集
//  xt::xarray<T> array = xt::load_hdf5<T>(filename, datasetName);
//  return array;
//}
//template <typename T>
//inline xt::xarray<T> LoadDatasetToXtensor(const H5::H5File& file,
//                                          const std::string& datasetName) {
//  // Open H5 file and dataset
//  H5::DataSet dataset = file.openDataSet(datasetName);
//
//  // Get dim and shape
//  H5::DataSpace dataspace = dataset.getSpace();
//  int rank = dataspace.getSimpleExtentNdims();
//  // std::vector<hsize_t> dims(rank);
//  std::vector<hsize_t> dims(rank);
//  dataspace.getSimpleExtentDims(dims.data(), nullptr);
//
//  // Read data
//  xt::xarray<T> array(dims);
//  assert(dataset.getDataType() == GetHDF5DataType<T>());
//  // dataset.read(array.data(), H5::PredType::NativeTypeFor<T>());
//  dataset.read(array.data(), GetHDF5DataType<T>());
//
//  return std::move(array);
//}

template <typename _Type>
inline xt::xarray<_Type> TensorToXArray(const at::Tensor& tensor,
                                        c10::optional<c10::ScalarType> dtype) {
  if (tensor.dtype() != dtype) {
    LOG_WARN("Type not match when convert tensor to xarray.");
  }
  std::vector<size_t> shape;
  for (auto size : tensor.sizes().vec()) {
    shape.push_back(static_cast<size_t>(size));
  }
  xt::xarray<_Type> result(shape);
  auto options = at::TensorOptions().device(at::kCPU).dtype(dtype);
  at::Tensor data_flat = at::from_blob(result.data(), tensor.sizes(), options);
  data_flat.copy_(tensor);
  return result;
}

template <typename _Type>
inline std::vector<_Type> TensorToSTDVector(
    const at::Tensor& tensor, c10::optional<c10::ScalarType> dtype) {
  if (tensor.dtype() != dtype) {
    LOG_WARN("Type not match when convert tensor to std::vector.");
  }
  auto flat_tensor = tensor.flatten();
  size_t shape = flat_tensor.size(0);
  std::vector<_Type> result(shape);

  auto options = at::TensorOptions().device(at::kCPU).dtype(dtype);
  at::Tensor data_flat =
      at::from_blob(result.data(), flat_tensor.sizes(), options);
  data_flat.copy_(tensor);
  return result;
}

template <typename T_key>
inline T_key ConvertToTKey(const std::string& str) {
  // Convert string to T_key
  std::stringstream ss(str);
  T_key result;
  ss >> result;
  return result;
}

template <typename T_value, typename T_key>
inline std::map<T_key, xt::xarray<T_value>> LoadGroupToMap(
    const std::string& filename, const std::string& groupName) {
  std::map<T_key, xt::xarray<T_value>> result;
  HighFive::File file(filename, HighFive::File::ReadOnly);

  // Open the group
  HighFive::Group group = file.getGroup(groupName);

  // Get the number of objects in the group
  std::vector<std::string> objectList = group.listObjectNames();

  // Iterate over each object in the group
  for (const auto& datasetName : objectList) {
    // Convert datasetName to T_key type
    T_key key = ConvertToTKey<T_key>(datasetName);

    // Load the dataset into an xtensor array
    xt::xarray<T_value> array_value = xt::load_hdf5<xt::xarray<T_value>>(
        filename, groupName + "/" + datasetName);

    // Add to the result map
    result[key] = std::move(array_value);
  }

  return result;
}

inline xt::xarray<double> ConvertToXArray(const at::Tensor& tensor) {
  std::vector<size_t> shape;
  for (auto size : tensor.sizes().vec()) {
    shape.push_back(static_cast<size_t>(size));
  }
  xt::xarray<double> result(shape);
  auto options = at::TensorOptions().device(at::kCPU).dtype(at::kDouble);
  at::Tensor data_flat = at::from_blob(result.data(), tensor.sizes(), options);
  data_flat.copy_(tensor);
  return result;
}

inline xt::xarray<int> ConvertToXArray(
    const std::vector<std::pair<int, int>>& data) {
  xt::xarray<int> result(std::vector<size_t>{data.size(), 2});
  if (!data.empty()) {
    int i = 0;
    for (const auto& pair : data) {
      result.at(i, 0) = (pair.first);
      result.at(i, 1) = (pair.second);
      ++i;
    }
  }
  return result;
}
inline xt::xarray<int> ConvertToXArray(const std::map<int, int>& data) {
  xt::xarray<int> result(std::vector<size_t>{data.size(), 2});
  if (!data.empty()) {
    int i = 0;
    for (const auto& pair : data) {
      result.at(i, 0) = (pair.first);
      result.at(i, 1) = (pair.second);
      ++i;
    }
  }
  return result;
}
inline xt::xarray<int64_t> ConvertToXArray(const std::vector<int64_t>& data) {
  return xt::adapt(data, {data.size()});
}
inline xt::xarray<int> ConvertToXArray(const std::vector<int>& data) {
  return xt::adapt(data, {data.size()});
}
inline xt::xarray<int> ConvertToXArray(const int data) {
  return xt::xarray<int>({data});
}

template <typename __T>
inline void save_data_to_h5(HighFive::File& File, std::string group_name,
                            std::string dataset_name, const __T& data) {
  xt::dump(File, group_name + dataset_name, ConvertToXArray(data));
}

template <typename __T>
inline void save_data_to_h5(HighFive::File& File, std::string group_name,
                            std::string dataset_name,
                            const std::map<int, __T>& data) {
  for (auto pair : data) {
    xt::dump(File, group_name + dataset_name + "/" + std::to_string(pair.first),
             ConvertToXArray(pair.second));
  }
}
template <typename __T>
void save_data_to_h5(HighFive::File& File, std::string group_name,
                     std::string dataset_name,
                     const std::map<std::string, __T>& data) {
  for (auto pair : data) {
    xt::dump(File, group_name + dataset_name + "/" + (pair.first),
             ConvertToXArray(pair.second));
  }
}
template <typename __T>
inline void save_data_to_h5(HighFive::File& File, std::string group_name,
                            std::string dataset_name,
                            const std::vector<std::vector<__T>>& data) {
  int i = 0;
  for (auto elem : data) {
    save_data_to_h5(File, group_name + dataset_name + "/", std::to_string(i),
                    elem);
    ++i;
  }
}
template <typename __T>
inline void save_data_to_h5(HighFive::File& File, std::string group_name,
                            std::string dataset_name,
                            const xt::xarray<__T>& data) {
  xt::dump(File, group_name + dataset_name, data);
}



// template <typename T_value, typename T_key>
// inline std::map<T_key, xt::xarray<T_value>> LoadGroupToMap(
//     const H5::H5File& file, const std::string& groupName) {
//   std::map<T_key, xt::xarray<T_value>> result;
//
//   // Open the group
//   H5::Group group = file.openGroup(groupName);
//
//   // Get the number of objects in the group
//   hsize_t numObjects = group.getNumObjs();
//
//   // Iterate over each object in the group
//   for (hsize_t i = 0; i < numObjects; ++i) {
//     // Get the dataset name
//     std::string datasetName = group.getObjnameByIdx(i);
//
//     // Convert datasetName to T_key type
//     T_key key = ConvertToTKey<T_key>(datasetName);
//
//     // Load the dataset into an xtensor array
//     xt::xarray<T_value> array_value =
//         LoadDatasetToXtensor<T_value>(file, groupName + "/" + datasetName);
//
//     // Add to the result map
//     result[key] = std::move(array_value);
//   }
//
//   return result;
// }


/**  
* @brief Thread-safe HighFive file handler with global access control.  
* @note Ensure proper lifecycle management for file handling and mutex locks.  
*  
* Uses a Meyer's singleton mutex for C++11/14 compatibility.  
*/  
class SafeHighFiveFile {  
public:  
 /**  
  * @brief Construct a new thread-safe file handler  
  * @param filename Path to HDF5 file  
  * @param openFlags File open mode (e.g. HighFive::File::Create)  
  *    Available modes:  
  *    - HighFive::File::ReadOnly: Open an existing file in read-only mode.  
  *    - HighFive::File::ReadWrite: Open an existing file in read-write mode.  
  *    - HighFive::File::Truncate: Overwrite an existing file if it already exists.  
  *    - HighFive::File::Excl: Fail if the file already exists.  
  *    - HighFive::File::Debug: Open the file in debug mode.  
  *    - HighFive::File::Create: Create a non-existing file.  
  *    - HighFive::File::Overwrite: Common write mode (equivalent to Truncate).  
  *    - HighFive::File::OpenOrCreate: Open in read-write mode or create a new file if it does not exist.
  */  
 explicit SafeHighFiveFile(const std::string& filename, unsigned openFlags)
     : file_lock_(mutex_instance())  // Lock mutex first
 {
   file_ = std::make_unique<HighFive::File>(filename, openFlags);
 }
 // Prohibit copying  
 SafeHighFiveFile(const SafeHighFiveFile&) = delete;  
 SafeHighFiveFile& operator=(const SafeHighFiveFile&) = delete;  
 // Allow moving  
 SafeHighFiveFile(SafeHighFiveFile&&) = default;  
 SafeHighFiveFile& operator=(SafeHighFiveFile&&) = default;  
 /**  
  * @brief Get the underlying HighFive file object  
  */  
 HighFive::File& get() noexcept { return *file_; }  
 const HighFive::File& get() const noexcept { return *file_; }  

private:  
 // Meyer's singleton pattern for mutex initialization  
 static std::mutex& mutex_instance() {  
   static std::mutex mtx;  
   return mtx;  
 }  
 std::unique_lock<std::mutex> file_lock_;  // Lock managed by RAII  
 std::unique_ptr<HighFive::File> file_;    // File resource  
};

}  // namespace cpptoolkit

// #include <gtest/gtest.h>
//
// TEST(H5ToolkitTest, LoadDatasetToXtensor) {
//   H5::H5File
//   file("../../data/1.8/ImgStk001_dk001_w10_Dt230525_000000_1.8.0.h5",
//                   H5F_ACC_RDONLY);
//   auto array = cpptoolkit::LoadDatasetToXtensor<int16_t>(file, "/volume");
//   // Show xarray shape
//   std::cout << "Shape: [";
//   for (size_t i = 0; i < array.shape().size(); ++i) {
//     std::cout << array.shape()[i];
//     if (i < array.shape().size() - 1) {
//       std::cout << ", ";
//     }
//   }
//   std::cout << "]" << std::endl;
//
//   // Show xarray Contents
//   std::cout << "Contents:" << std::endl;
//   std::cout << array << std::endl;
//   file.close();
// }

#endif  // CPPTOOLKIT_HDF5_TOOLKIT_H_