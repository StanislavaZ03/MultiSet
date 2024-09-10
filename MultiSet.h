#pragma once
class MultiSet
{
private:
	unsigned char* buckets = nullptr;
	size_t bytesCount = 0;
	size_t bitsPerNumber = 0;
	size_t maxCountForNumber = 0;
	size_t maxNumber = 0;

	void free();
	void copyFrom(const MultiSet& other);

public:
	explicit MultiSet(size_t n, size_t k);
	MultiSet(const MultiSet& other);
	MultiSet& operator=(const MultiSet& other);
	void printNumber(unsigned num, unsigned occurances) const;
	bool addNumber(int number);
	unsigned getCountOfOccurrences(unsigned num) const;
	void printSet() const;

	void serialize(const char* fileName) const;
	void deserialize(const char* fileName);
	MultiSet intersection(const MultiSet& other) const;
	MultiSet difference(const MultiSet& other) const;
	void complement();
	~MultiSet();

private:
	size_t getByteIndex(unsigned number) const;
	size_t getBitIndexInByte(unsigned number) const;
	void addNumberMultipleTimes(unsigned num, unsigned times);
	bool isInTwoBytes(int beg, int end) const;
	int findNumberOfBitsInAdjacentByte(int begIndex, int lastIndex) const;
};

