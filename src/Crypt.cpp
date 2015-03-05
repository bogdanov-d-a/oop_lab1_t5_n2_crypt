// Crypt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

typedef uint8_t byte_t;
const byte_t BYTE_MAX = UINT8_MAX;
typedef uint8_t bitPos_t;
const bitPos_t BYTE_BIT_COUNT = 8;
typedef bitPos_t byteBitPosRatio_t[BYTE_BIT_COUNT];
typedef byte_t(*convertByteFunc_t)(byte_t byte, byte_t key);

enum class returnCode
{
	SUCCESS,
	INPUT_ERROR,
	OUTPUT_ERROR,
	BAD_KEY_FORMAT,
	BAD_KEY_RANGE,
	BAD_USAGE
};

byte_t GetByteBitMask(bitPos_t pos)
{
	assert(pos < BYTE_BIT_COUNT);
	return ((byte_t)1 << pos);
}

byte_t MoveByteBits(byte_t byte, byteBitPosRatio_t const& ratio)
{
	byte_t result = 0;

	bitPos_t srcBitPos;
	byte_t srcBitMask;
	for (srcBitPos = 0, srcBitMask = 1; srcBitPos < BYTE_BIT_COUNT; ++srcBitPos, srcBitMask <<= 1)
	{
		if (byte & srcBitMask)
		{
			result |= GetByteBitMask(ratio[srcBitPos]);
		}
	}

	return result;
}

byte_t CryptByte(byte_t byte, byte_t key)
{
	return MoveByteBits(byte ^ key, { 2, 3, 4, 6, 7, 0, 1, 5 });
}

byte_t DecryptByte(byte_t byte, byte_t key)
{
	return (MoveByteBits(byte, { 5, 6, 0, 1, 2, 7, 3, 4 }) ^ key);
}

void PrintUsage(const char programName[])
{
	printf("Usage: %s [ crypt / decrypt ] <input file> <output file> <key>\n", programName);
}

returnCode ConvertFile(const char inFileName[], const char outFileName[], convertByteFunc_t func, byte_t key)
{
	FILE *inFile;

	if (fopen_s(&inFile, inFileName, "rb") == 0)
	{
		FILE *outFile;

		if (fopen_s(&outFile, outFileName, "wb") == 0)
		{
			int tmpByte;
			while ((tmpByte = fgetc(inFile)) != EOF)
			{
				fputc(func(tmpByte, key), outFile);
			}

			fclose(outFile);
			fclose(inFile);
			return returnCode::SUCCESS;
		}
		else
		{
			puts("Output file open error");
			fclose(inFile);
			return returnCode::OUTPUT_ERROR;
		}
	}
	else
	{
		puts("Input file open error");
		return returnCode::INPUT_ERROR;
	}
}

byte_t ParseByteStr(const char str[])
{
	int strNum = std::stoi(str);

	if (strNum >= 0 && strNum <= BYTE_MAX)
	{
		return strNum;
	}
	else
	{
		throw std::out_of_range("ParseByteStr argument number is out of range");
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc == 5)
	{
		try
		{
			byte_t key = ParseByteStr(argv[4]);

			if (strcmp(argv[1], "crypt") == 0)
			{
				return static_cast<int>(ConvertFile(argv[2], argv[3], CryptByte, key));
			}
			else if (strcmp(argv[1], "decrypt") == 0)
			{
				return static_cast<int>(ConvertFile(argv[2], argv[3], DecryptByte, key));
			}
			else
			{
				assert(argc > 0);
				PrintUsage(argv[0]);
				return static_cast<int>(returnCode::BAD_USAGE);
			}
		}
		catch (std::invalid_argument const& e)
		{
			(void)e;
			puts("Incorrect key format");
			return static_cast<int>(returnCode::BAD_KEY_FORMAT);
		}
		catch (std::out_of_range const& e)
		{
			(void)e;
			puts("Key value is out of range");
			return static_cast<int>(returnCode::BAD_KEY_RANGE);
		}
	}
	else
	{
		assert(argc > 0);
		PrintUsage(argv[0]);
		return static_cast<int>(returnCode::BAD_USAGE);
	}
}
