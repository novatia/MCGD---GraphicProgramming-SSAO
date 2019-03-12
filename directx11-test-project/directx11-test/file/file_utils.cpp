#include "stdafx.h"
#include "file_utils.h"
#include <iostream>
#include <fstream>

using xtest::file::BinaryFile;


std::future<BinaryFile> xtest::file::ReadBinaryFile(std::wstring filePath)
{
	return std::async(std::launch::async, [filePath]()
	{
		// open the file at the end
		std::ifstream fileStream(filePath, std::ios::binary | std::ios::in | std::ios::ate);
		XTEST_ASSERT(fileStream.is_open(), L"unable to open the file:'%s'", filePath.c_str());

		std::streampos byteSize = fileStream.tellg(); // extract the byte size
		std::unique_ptr<char[]> byteArray(new char[byteSize]);

		// go back to the start of the file
		fileStream.seekg(0, std::ios::beg);
		fileStream.read(byteArray.get(), byteSize);
		fileStream.close();

		BinaryFile binaryFile(std::move(byteArray), byteSize);
		return binaryFile;
	});
}

