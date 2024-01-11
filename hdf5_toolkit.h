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
#include <highfive/H5File.hpp>
#include <highfive/H5Group.hpp>
#include <xtensor-io/xhighfive.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>

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