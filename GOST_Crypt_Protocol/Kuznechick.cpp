#include "Kuznechick.h"

Kuznechick::Kuznechick(const uint8_t Data[], const uint8_t Key[])
{
	iterConsts = new GaluaField*[KEYSZ];
	for (size_t i = 0; i < KEYSZ; ++i) iterConsts[i] = new GaluaField[BLOCKSZ];
	for (size_t i = 0; i < KEYSZ; ++i)
	{
		iterConsts[i][15] = i + 1;
		kuznechick_L(iterConsts[i]);
	}
	userKey = new GaluaField[KEYSZ];
	roundKeys = new GaluaField* [ROUNDKEYSSZ];
	for (size_t i = 0; i < ROUNDKEYSSZ; ++i) roundKeys[i] = new GaluaField[BLOCKSZ];
	blockData = new GaluaField[BLOCKSZ];
	for (size_t i = 0; i < BLOCKSZ; ++i)
		blockData[i] = Data[i];
	for (size_t i = 0; i < KEYSZ; ++i)
		roundKeys[i>>4][i%BLOCKSZ] = Key[i];
	kuznechick_exp_keys();
}

Kuznechick::~Kuznechick()
{
	delete[] iterConsts;
	delete[] userKey;
	delete[] roundKeys;
	delete[] blockData;
}

int Kuznechick::kuznechick_S(GaluaField*& Data)
{
	for (size_t i = 0; i < BLOCKSZ; ++i)
		Data[i] = STable[(uint8_t) Data[i]];
	return 0;
}

int Kuznechick::kuznechick_revS(GaluaField*& Data)
{
	for (size_t i = 0; i < BLOCKSZ; ++i)
		Data[i] = revSTable[(uint8_t)Data[i]];
	return 0;

}

int Kuznechick::kuznechick_X(const GaluaField* const Data1, const GaluaField* const Data2, GaluaField*& outData)
{
	for (size_t i = 0; i < BLOCKSZ; ++i)
		outData[i] = Data1[i] + Data2[i];
	return 0;
}

int Kuznechick::kuznechick_R(const GaluaField* const Data, GaluaField& outData)
{
	outData = 0;
	for (size_t i = 0; i < BLOCKSZ; ++i)
		outData = outData + (Data[i] * COEFFS[i]);
	return 0;
}

int Kuznechick::kuznechick_L(GaluaField*& Data)
{
	for (size_t i = 0; i < BLOCKSZ; ++i)
	{
		GaluaField firstByte(0);
		kuznechick_R(Data, firstByte);
		for (size_t j = BLOCKSZ - 1; j > 0; --j) Data[j] = Data[j - 1];
		Data[0] = firstByte;
	}
	return 0;
}

int kuznechick_revR(GaluaField*& Data)
{
	return 0;
}

int Kuznechick::kuznechick_revL(GaluaField*& Data)
{
	for (size_t i = 0; i < BLOCKSZ; ++i)
	{
		GaluaField lastByte(0);
		lastByte = Data[0];
		for (size_t j = 0; j < BLOCKSZ; ++j) Data[j] = Data[j + 1];
		Data[BLOCKSZ - 1] = lastByte;
		lastByte = 0;
		kuznechick_R(Data, lastByte);
		Data[BLOCKSZ - 1] = lastByte;
	}
	return 0;
}

int Kuznechick::kuznechick_F(const GaluaField* const inBlock, const GaluaField*const inKey, GaluaField*& fOut)
{
	kuznechick_X(inBlock, inKey, fOut);
	kuznechick_S(fOut);
	kuznechick_L(fOut);
	return 0;
}

int Kuznechick::kuznechick_revF(const GaluaField* const inBlock, const GaluaField* const inKey, GaluaField*& fOut)
{
	kuznechick_X(fOut, inKey, fOut);
	kuznechick_revL(fOut);
	kuznechick_revS(fOut);
	return 0;
}

int Kuznechick::kuznechick_exp_keys()
{
	for (size_t i = 0; i < (ROUNDKEYSSZ - 2); i = i + 2)
	{
		for (size_t j = 0; j < BLOCKSZ; ++j)
		{
			roundKeys[i + 2][j] = roundKeys[i][j];
			roundKeys[i + 3][j] = roundKeys[i + 1][j];
		}
		
		for (size_t j = 0; j < POLYDEG; ++j)
		{
			GaluaField* tmp;
			tmp = new GaluaField[16];
			kuznechick_F(roundKeys[i + 2 + (j % 2)], iterConsts[(i << 2) + j], tmp);
			kuznechick_X(roundKeys[i + 2 + ((j + 1) % 2)], tmp, roundKeys[i + 2 + ((j+1) % 2)]);
			delete[] tmp;
			
		}
	}
	return 0;
}

std::string Kuznechick::kuznechick_crypt()
{
	for (size_t i = 0; i < ROUNDKEYSSZ-1; ++i)
		kuznechick_F(blockData, roundKeys[i], blockData);
	kuznechick_X(blockData, roundKeys[ROUNDKEYSSZ - 1], blockData);
	std::string strOut = "";
	for (size_t i = 0; i < BLOCKSZ; ++i)
		strOut = strOut + std::to_string((uint8_t)blockData[i])+" ";
	return strOut;
}

std::string Kuznechick::kuznechick_decrypt()
{
	for (size_t i = ROUNDKEYSSZ; i > 1; --i)
		kuznechick_revF(blockData, roundKeys[i - 1], blockData);
	kuznechick_X(blockData, roundKeys[0], blockData);
	std::string strOut = "";
	for (size_t i = 0; i < BLOCKSZ; ++i)
		strOut = strOut + std::to_string((uint8_t)blockData[i]) + " ";
	return strOut;
}