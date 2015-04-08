// Crypt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

const unsigned BYTE_BIT_COUNT = 8;
typedef unsigned ByteBitPosRatio[BYTE_BIT_COUNT];
typedef function<uint8_t(uint8_t)> ConvertByteFunc;

enum class ReturnCode
{
	SUCCESS,
	INPUT_ERROR,
	OUTPUT_ERROR,
	BAD_KEY_FORMAT,
	BAD_KEY_RANGE,
	BAD_USAGE
};

uint8_t GetByteBitMask(unsigned pos)
{
	assert(pos < BYTE_BIT_COUNT);
	return (static_cast<uint8_t>(1) << pos);
}

uint8_t MoveByteBits(uint8_t byte, ByteBitPosRatio const& ratio)
{
	uint8_t result = 0;

	unsigned srcBitPos;
	uint8_t srcBitMask;
	for (srcBitPos = 0, srcBitMask = 1; srcBitPos < BYTE_BIT_COUNT; ++srcBitPos, srcBitMask <<= 1)
	{
		if (byte & srcBitMask)
		{
			result |= GetByteBitMask(ratio[srcBitPos]);
		}
	}

	return result;
}

uint8_t CryptByte(uint8_t byte, uint8_t key)
{
	return MoveByteBits(byte ^ key, { 2, 3, 4, 6, 7, 0, 1, 5 });
}

uint8_t DecryptByte(uint8_t byte, uint8_t key)
{
	return (MoveByteBits(byte, { 5, 6, 0, 1, 2, 7, 3, 4 }) ^ key);
}

ReturnCode ConvertFile(const char inFileName[], const char outFileName[], ConvertByteFunc func)
{
	ifstream inFile(inFileName, ios_base::binary);
	if (!inFile.is_open())
	{
		cout << "Input file open error" << endl;
		return ReturnCode::INPUT_ERROR;
	}

	ofstream outFile(outFileName, ios_base::binary);
	if (!outFile.is_open())
	{
		cout << "Output file open error" << endl;
		return ReturnCode::OUTPUT_ERROR;
	}

	char curByte;
	while (inFile.get(curByte))
	{
		outFile.put(func(curByte));
	}
	return ReturnCode::SUCCESS;
}

ReturnCode ParseByteStr(const char str[], uint8_t &result)
{
	int strNum;
	try
	{
		strNum = stoi(str);
	}
	catch (invalid_argument const& e)
	{
		(void)e;
		cout << "Incorrect key format" << endl;
		return ReturnCode::BAD_KEY_FORMAT;
	}

	if (strNum < 0 || strNum > UINT8_MAX)
	{
		cout << "Key value is out of range" << endl;
		return ReturnCode::BAD_KEY_RANGE;
	}

	result = strNum;
	return ReturnCode::SUCCESS;
}

int _tmain(int argc, _TCHAR* argv[])
{
	assert(argc > 0);
	const string USAGE_HINT =
		"Usage: " + string(argv[0]) + " [ crypt / decrypt ] <input file> <output file> <key>";

	if (argc != 5)
	{
		cout << USAGE_HINT << endl;
		return static_cast<int>(ReturnCode::BAD_USAGE);
	}

	uint8_t key;
	{
		const ReturnCode parseResult = ParseByteStr(argv[4], key);
		if (parseResult != ReturnCode::SUCCESS)
		{
			return static_cast<int>(parseResult);
		}
	}

	ConvertByteFunc convFunc;
	if (strcmp(argv[1], "crypt") == 0)
	{
		convFunc = [key](uint8_t byte){ return CryptByte(byte, key); };
	}
	else if (strcmp(argv[1], "decrypt") == 0)
	{
		convFunc = [key](uint8_t byte){ return DecryptByte(byte, key); };
	}

	if (!convFunc)
	{
		cout << USAGE_HINT << endl;
		return static_cast<int>(ReturnCode::BAD_USAGE);
	}

	return static_cast<int>(ConvertFile(argv[2], argv[3], convFunc));
}
