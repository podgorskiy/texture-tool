/*
 * Copyright 2019-2020 Stanislav Pidhorskyi. All rights reserved.
 * License: https://raw.githubusercontent.com/podgorskiy/bimpy/master/LICENSE.txt
 */

#include "common.h"

#include <PVRTexture.h>
#include <PVRTextureDefines.h>
#include <PVRTextureFormat.h>
#include <PVRTextureHeader.h>
#include <PVRTextureUtilities.h>
#include <PVRTextureVersion.h>

#include <pybind11/operators.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <memory>
#include <mutex>
#include <cstdio>
#include <sstream>
#ifdef __linux
#include <sys/stat.h>
#endif


int main()
{
	return 0;
}


PYBIND11_MODULE(_pypvrtex, m)
{
	m.doc() = "_pypvrtex";
//
//	py::enum_<Records::DataType>(m, "DataType", py::arithmetic())
//			.value("string", Records::DataType::DT_STRING)
//			.value("float32", Records::DataType::DT_FLOAT)
//			.value("int64", Records::DataType::DT_INT64)
//			.value("uint8", Records::DataType::DT_UINT8)
//			.export_values();
//
//	py::class_<RecordReader>(m, "RecordReader")
//			.def(py::init<fsal::File>())
//			.def(py::init<const std::string&>())
//			.def("read_record", [](RecordReader& self, size_t& offset)->py::object
//			{
//				PyBytesObject* bytesObject = nullptr;
//				{
//					py::gil_scoped_release release;
//
//					fsal::Status result = self.ReadRecord(offset, GetBytesAllocator(bytesObject));
//					if (!result.ok())
//					{
//						PyObject_Free(bytesObject);
//						throw runtime_error("Error reading record at offset %zd", offset);
//					}
//				}
//
//				return py::reinterpret_steal<py::object>((PyObject*)bytesObject);
//			})
//			.def("__iter__", [](py::object& self)->py::object
//			{
//				return self;
//			})
//			.def("__next__", [](RecordReader& self)->py::object
//			{
//				PyBytesObject* bytesObject = nullptr;
//				auto status = self.GetNext(GetBytesAllocator(bytesObject));
//				if (!status.ok())
//				{
//					if (status.state == fsal::Status::kEOF)
//					{
//						throw py::stop_iteration();
//					}
//					else
//					{
//						throw runtime_error("Error while iterating RecordReader at offset: %zd", self.offset());
//					}
//				}
//				return py::reinterpret_steal<py::object>((PyObject*)bytesObject);
//			});
//
//	py::class_<Records::RecordParser::FixedLenFeature>(m, "FixedLenFeature")
//			.def(py::init())
//			.def(py::init<std::vector<size_t>, Records::DataType>())
//			.def(py::init<std::vector<size_t>, Records::DataType, py::object>())
//			.def_readwrite("shape", &Records::RecordParser::FixedLenFeature::shape)
//			.def_readwrite("dtype", &Records::RecordParser::FixedLenFeature::dtype)
//			.def_readwrite("default_value", &Records::RecordParser::FixedLenFeature::default_value);
//
//	py::class_<Records::RecordParser>(m, "RecordParser")
//			.def(py::init<py::dict>())
//			.def(py::init<py::dict, bool>())
//			.def(py::init<py::dict, bool, int>())
//			.def("parse_single_example_inplace", &Records::RecordParser::ParseSingleExampleInplace)
//			.def("parse_single_example", &Records::RecordParser::ParseSingleExample)
//			.def("parse_example", &Records::RecordParser::ParseExample);
//
//	py::class_<RecordYielderBasic>(m, "RecordYielderBasic")
//			.def(py::init<std::vector<std::string>&>())
//			.def("__iter__", [](py::object& self)->py::object
//			{
//				return self;
//			})
//			.def("__next__", &RecordYielderBasic::GetNext, py::return_value_policy::take_ownership)
//	        .def("next_n", &RecordYielderBasic::GetNextN, py::return_value_policy::take_ownership);
//
//	py::class_<RecordYielderRandomized>(m, "RecordYielderRandomized")
//			.def(py::init<std::vector<std::string>&, int, uint64_t, int>())
//			.def("__iter__", [](py::object& self)->py::object
//			{
//				return self;
//			})
//			.def("__next__", &RecordYielderRandomized::GetNext, py::return_value_policy::take_ownership)
//			.def("next_n", &RecordYielderRandomized::GetNextN, py::return_value_policy::take_ownership);
//
//	py::class_<ParsedRecordYielderRandomized>(m, "ParsedRecordYielderRandomized")
//			.def(py::init<py::object, std::vector<std::string>&, int, uint64_t, int>())
//			.def("__iter__", [](py::object& self)->py::object
//			{
//				return self;
//			})
//			.def("__next__", &ParsedRecordYielderRandomized::GetNext, py::return_value_policy::take_ownership)
//			.def("next_n", &ParsedRecordYielderRandomized::GetNextN, py::return_value_policy::take_ownership);
//
//	m.def("open_as_bytes", [](const char* filename)
//	{
//		py::gil_scoped_release release;
//		fsal::StdFile tmp_std;
//		auto fp = openfile(filename, tmp_std);
//		return read_as_bytes(fp);
//	});
//
//	m.def("open_as_numpy_ubyte", [](const char* filename, py::object shape)
//	{
//		fsal::StdFile tmp_std;
//		fsal::File fp;
//		{
//			py::gil_scoped_release release;
//			fp = openfile(filename, tmp_std);
//		}
//		return read_as_numpy_ubyte(fp, shape);
//	},  py::arg("filename"),  py::arg("shape").none(true) = py::none());
//
//	m.def("read_jpg_as_numpy", [](const char* filename, bool use_turbo)
//	{
//		fsal::StdFile tmp_std;
//		fsal::File fp;
//		{
//			py::gil_scoped_release release;
//			fp = openfile(filename, tmp_std);
//		}
//		return read_jpg_as_numpy(fp, use_turbo);
//	},  py::arg("filename"),  py::arg("use_turbo") = false);
//
//	py::enum_<fsal::Mode>(m, "Mode", py::arithmetic())
//		.value("read", fsal::Mode::kRead)
//		.value("write", fsal::Mode::kWrite)
//		.value("append", fsal::Mode::kAppend)
//		.value("read_update", fsal::Mode::kReadUpdate)
//		.value("write_update", fsal::Mode::kWriteUpdate)
//		.value("append_update", fsal::Mode::kAppendUpdate)
//		.export_values();
//
//	py::class_<fsal::Location>(m, "Location")
//		.def(py::init<const char*>())
//		.def(py::init<const char*, fsal::Location::Options, fsal::PathType, fsal::LinkType>());
//
//	py::implicitly_convertible<const char*, fsal::Location>();
//
//	m.def("open_zip_archive", [](const char* filename)
//	{
//		fsal::FileSystem fs;
//		auto archive_file = fs.Open(filename);
//		auto zr = new fsal::ZipReader;
//		zr->OpenArchive(archive_file);
//		return zr;
//	});
//
//	py::class_<fsal::ArchiveReaderInterface> Archive(m, "Archive");
//		Archive.def("open", [](fsal::ArchiveReaderInterface& self, const std::string& filepath)->py::object{
//			fsal::File f;
//			{
//				py::gil_scoped_release release;
//				f = self.OpenFile(filepath);
//			}
//			if (f)
//			{
//				return py::cast(f);
//			}
//			else
//			{
//				return py::cast<py::none>(Py_None);
//			}
//		}, "Opens file")
//		.def("open_as_bytes", [](fsal::ArchiveReaderInterface& self, const std::string& filepath)->py::object
//		{
//			PyBytesObject* bytesObject = nullptr;
//			{
//				py::gil_scoped_release release;
//
//				void* result = self.OpenFile(filepath, GetBytesAllocator(bytesObject));
//				if (!result)
//				{
//					PyObject_Free(bytesObject);
//					throw runtime_error("Can't open file: %s", filepath.c_str());
//				}
//
//			}
//
//			return py::reinterpret_steal<py::object>((PyObject*)bytesObject);
//		})
//		.def("open_as_numpy_ubyte", [](fsal::ArchiveReaderInterface& self, const std::string& filepath, py::object _shape)
//		{
//			size_t size = 0;
//			std::vector<size_t> shape;
//			ndarray_uint8 data;
//			{
//				auto alloc = [&size, &data, &_shape, &shape](size_t s)
//				{
//					py::gil_scoped_acquire acquire;
//					size = s;
//					fix_shape(_shape, size, shape);
//					data = ndarray_uint8(shape);
//					void* ptr = data.request().ptr;
//					return ptr;
//				};
//				{
//					py::gil_scoped_release release;
//					void* result = self.OpenFile(filepath, alloc);
//					if (!result)
//					{
//						throw runtime_error("Can't open file: %s", filepath.c_str());
//					}
//				}
//			}
//			return data;
//		})
//		.def("read_jpg_as_numpy", [](fsal::ArchiveReaderInterface& self, const std::string& filepath, bool use_turbo)
//		{
//			size_t size = 0;
//			fsal::File f;
//			void* data;
//			{
//				py::gil_scoped_release release;
//				f = self.OpenFile(filepath);
//				if (!f)
//				{
//					throw runtime_error("Can't open file: %s", filepath.c_str());
//				}
//				size = f.GetSize();
//				data = f.GetDataPointer();
//			}
//			if (use_turbo)
//			{
//				return decode_jpeg_turbo(data, size);
//			}
//			else
//			{
//				return decode_jpeg_vanila(data, size);
//			}
//		},  py::arg("filename"),  py::arg("use_turbo") = false)
//		.def("exists", [](fsal::ArchiveReaderInterface& self, const std::string& filepath){
//			return self.Exists(filepath);
//		}, "Exists")
//		.def("list_directory", [](fsal::ArchiveReaderInterface& self, const std::string& filepath){
//			return self.Exists(filepath);
//		}, "ListDirectory");
//
//	py::class_<fsal::ZipReader>(m, "ZipReader", Archive)
//		.def(py::init());
//
//	py::class_<fsal::FileSystem>(m, "FileSystem")
//		.def(py::init())
//		.def("open", [](fsal::FileSystem& fs, const fsal::Location& location, fsal::Mode mode)->py::object{
//				fsal::File f = fs.Open(location, mode);
//				if (f)
//				{
//					return py::cast(f);
//				}
//				else
//				{
//					return py::cast<py::none>(Py_None);
//				}
//			}, py::arg("location"), py::arg("mode") = fsal::kRead, "Opens file")
//		.def("exists", &fsal::FileSystem::Exists, "Exists")
//		.def("rename", &fsal::FileSystem::Rename, "Rename")
//		.def("remove", &fsal::FileSystem::Remove, "Remove")
//		.def("create_directory", &fsal::FileSystem::CreateDirectory, "CreateDirectory")
//		.def("push_search_path", &fsal::FileSystem::PushSearchPath, "PushSearchPath")
//		.def("pop_search_path", &fsal::FileSystem::PopSearchPath, "PopSearchPath")
//		.def("clear_search_paths", &fsal::FileSystem::ClearSearchPaths, "ClearSearchPaths")
//		.def("mount_archive", &fsal::FileSystem::MountArchive, "AddArchive");
//
//	py::class_<fsal::File>(m, "File")
//			.def(py::init())
//			.def("read", [](fsal::File& self, ptrdiff_t size) -> py::object
//			{
//				size_t position = self.Tell();
//				size_t file_size = self.GetSize();
//				const char* ptr = (const char*)self.GetDataPointer();
//				if (size < 0)
//				{
//					size = file_size - position;
//				}
//				if (ptr)
//				{
//					self.Seek(position + size);
//					return py::bytes(ptr + position, size); // py::bytes copies data, because PyBytes_FromStringAndSize copies. No way around
//				}
//				else
//				{
//					PyBytesObject* bytesObject = (PyBytesObject *)PyObject_Malloc(offsetof(PyBytesObject, ob_sval) + size + 1);
//					PyObject_INIT_VAR(bytesObject, &PyBytes_Type, size);
//					bytesObject->ob_shash = -1;
//					bytesObject->ob_sval[size] = '\0';
//					self.Read((uint8_t*)bytesObject->ob_sval, size);
//					return py::reinterpret_steal<py::object>((PyObject*)bytesObject);
//				}
//			}, py::arg("size") = -1, py::return_value_policy::take_ownership)
//			.def("seek", [](fsal::File& self, ptrdiff_t offset, int origin){
//				self.Seek(offset, fsal::File::Origin(origin));
//				return self.Tell();
//			}, py::arg("offset"), py::arg("origin") = 0)
//			.def("tell", &fsal::File::Tell);
//
//	py::class_<fsal::Status>(m, "Status")
//			.def(py::init())
//			.def("__nonzero__", &fsal::Status::ok);
}
