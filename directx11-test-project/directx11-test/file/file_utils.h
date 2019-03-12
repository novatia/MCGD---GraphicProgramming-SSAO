#pragma once

#include <future>
#include <file/binary_file.h>

namespace xtest {
namespace file {

	std::future<BinaryFile> ReadBinaryFile(std::wstring filePath);

} //file
} //xtest

