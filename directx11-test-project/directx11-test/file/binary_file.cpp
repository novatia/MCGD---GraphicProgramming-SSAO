#include "stdafx.h"
#include "binary_file.h"

using xtest::file::BinaryFile;


BinaryFile::BinaryFile()
	: m_byteArray(nullptr)
	, m_byteSize(0)
{}


BinaryFile::BinaryFile(BinaryFile&& other)
	: m_byteArray(std::move(other.m_byteArray))
	, m_byteSize(std::move(other.m_byteSize))
{
	other.m_byteArray = nullptr;
	other.m_byteSize = 0;
}


BinaryFile& BinaryFile::operator=(BinaryFile&& other)
{
	std::swap(m_byteArray, other.m_byteArray);
	std::swap(m_byteSize, other.m_byteSize);
	return *this;
}


BinaryFile::BinaryFile(std::unique_ptr<char[]> byteArray, uint64 size)
	: m_byteArray(std::move(byteArray))
	, m_byteSize(size)
{}


BinaryFile::~BinaryFile()
{}


const char* BinaryFile::Data() const
{
	return m_byteArray.get();
}


uint64 BinaryFile::ByteSize() const
{
	return m_byteSize;
}
