#include "MultiSet.h"
#include<iostream>
#include<fstream>

MultiSet::MultiSet(const MultiSet& other)
{
	copyFrom(other);
}

MultiSet& MultiSet::operator=(const MultiSet& other)
{
	if (this != &other)
	{
		free();
		copyFrom(other);
	}
	return *this;
}

void MultiSet::free()
{
	delete[] buckets;
	buckets = nullptr;
	bytesCount = bitsPerNumber = maxCountForNumber = maxNumber = 0;
}

void MultiSet::copyFrom(const MultiSet& other)
{
	maxNumber = other.maxNumber;
	bytesCount = other.bytesCount;
	bitsPerNumber = other.bitsPerNumber;
	maxCountForNumber = other.maxCountForNumber;

	for (int i = 0; i < bytesCount; ++i)
	{
		buckets[i] = other.buckets[i];
	}
}

MultiSet::~MultiSet()
{
	free();
}
 
MultiSet::MultiSet(size_t n, size_t k) 
{
	maxNumber = n;
	bitsPerNumber = k;
	maxCountForNumber = (1 << k) - 1;
	bytesCount = (n + 1) * k / 8 + ((n + 1) * k % 8 != 0);
	buckets = new unsigned char[bytesCount] { 0 };
}

size_t MultiSet::getByteIndex(unsigned number) const 
{
	return (number * bitsPerNumber) / 8;
}

size_t MultiSet::getBitIndexInByte(unsigned number) const
{
	return (number % 8) * bitsPerNumber; 
}

bool MultiSet::isInTwoBytes(int beg, int end) const
{
	if (getByteIndex(beg) != getByteIndex(end))
		return true;

	return false;
}

bool MultiSet::addNumber(int num)
{
	if (getCountOfOccurrences(num) == maxCountForNumber)
	{
		return false;
	}

	unsigned bucket = getByteIndex(num); 
	unsigned index = getBitIndexInByte(num); 
	uint8_t& currBucket = buckets[bucket];
	unsigned char mask = (1 << bitsPerNumber) - 1;

	if (index < 8)
	{
		mask <<= (8 - (index + bitsPerNumber));
	}
	else
	{
		mask <<= (index + bitsPerNumber - 8);
	}

	if (isInTwoBytes(index, index + bitsPerNumber - 1)) // зануляваме битовете в първия и втория байт
	{
		int bitsInSecond = findNumberOfBitsInAdjacentByte(index, index + bitsPerNumber - 1);
		currBucket &= ~(1 << bitsInSecond - 1);
		buckets[index+1] &= ~(1 << bitsInSecond - 1);
		unsigned char mask2 = getCountOfOccurrences(num) +1;
		unsigned char mask3 = mask2 >> bitsInSecond;
		mask2 << (8 - bitsInSecond);
		
		currBucket |= mask3;
		buckets[index + 1] |= mask2;
	}
	else
	{
		mask = ~mask;

		currBucket &= mask;
		unsigned char mask2 = getCountOfOccurrences(num) + 1;
		mask2 <<= findNumberOfBitsInAdjacentByte(index, index + bitsPerNumber - 1);
		currBucket |= mask2;
	}
	return true;
}

int MultiSet::findNumberOfBitsInAdjacentByte(int begIndex, int lastIndex) const
{
	int result = begIndex;
	while (isInTwoBytes(begIndex, lastIndex))
	{
		result++;
	}
	return lastIndex - result;
}

unsigned MultiSet::getCountOfOccurrences(unsigned num) const
{
	unsigned bucket = getByteIndex(num); 
	unsigned index = getBitIndexInByte(num);
	unsigned char mask = (1 << bitsPerNumber) - 1;
	unsigned char currBucket = buckets[bucket];
	unsigned occurrences = 0;

	if (index < 8)
	{
		currBucket >>= (8 - (index + bitsPerNumber));
	}
	else
	{
		currBucket >>= (index + bitsPerNumber - 8);
	}

	if (isInTwoBytes(index, index + bitsPerNumber - 1))
	{
		currBucket <<= (findNumberOfBitsInAdjacentByte(index, index + bitsPerNumber - 1));
		currBucket &= mask;
		unsigned char currBucket2 = buckets[bucket + 1];
		currBucket2 >>= (8 - findNumberOfBitsInAdjacentByte(index, index + bitsPerNumber - 1));
		currBucket2 &= mask;
		currBucket |= currBucket2;
	}
	else
	{
		currBucket &= mask;
	}

	unsigned char mask2 = 1;

	for (unsigned i = 0; i < 8; ++i)
	{
		if ((currBucket & mask2) == mask2)
		{
			occurrences += (1 << i);
		}
		mask2 <<= 1;
	}

	return occurrences;
}

void MultiSet::printNumber(unsigned num, unsigned occurances) const
{
	for (unsigned i = 0; i < occurances; i++)
		std::cout << num << " ";
}

void MultiSet::printSet() const {
	std::cout << '{';
	for (unsigned i = 0; i < maxNumber; i++)
	{
		unsigned occurrences = getCountOfOccurrences(i);
		printNumber(i, occurrences);
	}
	std::cout << '}';
}

void MultiSet::serialize(const char* fileName) const
{
	std::ofstream file(fileName, std::ios::binary);

	if (!file.is_open()) 
	{
		std::cout << "Failed to open file for writing.\n";
		return;
	}
	file.write((const char*)&maxNumber, sizeof(maxNumber));
	file.write((const char*)&bytesCount, sizeof(bytesCount));
	file.write((const char*)&bitsPerNumber, sizeof(bitsPerNumber));
	file.write((const char*)&maxCountForNumber, sizeof(maxCountForNumber));
	file.write((const char*)buckets, bytesCount);
	file.close();
}

void MultiSet::deserialize(const char* fileName) 
{
	std::ifstream file(fileName, std::ios::binary);
	if (!file.is_open()) 
	{
		std::cout << "Failed to open file for reading.\n";
		return;
	}

	free();
	file.read((char*)&maxNumber, sizeof(maxNumber));
	file.read((char*)&bytesCount, sizeof(bytesCount));
	file.read((char*)&bitsPerNumber, sizeof(bitsPerNumber));
	file.read((char*)&maxCountForNumber, sizeof(maxCountForNumber));
	buckets = new unsigned char[bytesCount];
	file.read((char*)buckets, bytesCount);
	file.close();
}

MultiSet MultiSet::intersection(const MultiSet& other) const {
	MultiSet result(std::min(maxNumber, other.maxNumber), bitsPerNumber);
	for (unsigned i = 0; i < result.maxNumber; ++i) {
		unsigned occurrences1 = getCountOfOccurrences(i);
		unsigned occurrences2 = other.getCountOfOccurrences(i);
		unsigned minOccurrences = std::min(occurrences1, occurrences2);
		result.addNumberMultipleTimes(i, minOccurrences);
	}
	return result;
}

MultiSet MultiSet::difference(const MultiSet& other) const {
	MultiSet result(maxNumber, bitsPerNumber);
	for (unsigned i = 0; i < result.maxNumber; ++i) {
		unsigned occurrences1 = getCountOfOccurrences(i);
		unsigned occurrences2 = other.getCountOfOccurrences(i);
		unsigned difference = occurrences1 - occurrences2;
		if (difference > 0) {
			result.addNumberMultipleTimes(i, difference);
		}
	}
	return result;
}

void MultiSet::complement() {
	for (unsigned i = 0; i < maxNumber; ++i) {
		unsigned occurrences = getCountOfOccurrences(i);
		unsigned complementOccurrences = (1 << bitsPerNumber) - 1 - occurrences;
		addNumberMultipleTimes(i, complementOccurrences);
	}
}

void MultiSet::addNumberMultipleTimes(unsigned num, unsigned times) 
{
	if (times > maxCountForNumber)
		times = maxCountForNumber;

	for (unsigned i = 0; i < times; i++)
		addNumber(num);
}