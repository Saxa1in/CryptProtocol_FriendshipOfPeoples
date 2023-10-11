#pragma once
#include <iostream>
#include <vector>
#include <string>

class GaluaField
{
private:
	//std::vector<unsigned char> Polynom;
	//const std::vector<unsigned char> basePolynom = { 1 };
	uint8_t Polynom;
	const uint8_t basePolynom = 195;
	const unsigned char Module = 2;
	const unsigned char polynomSz = 8;

public:
	GaluaField();
	GaluaField(uint8_t Init);
	unsigned char& at(const size_t Index);
	explicit operator uint8_t() const { return Polynom; }
	GaluaField& operator=(const GaluaField& operand) 
	{
		Polynom = operand.Polynom;
		return *this;
	}
	GaluaField operator>>(const size_t operand) const;
	GaluaField operator<<(const size_t operand) const;
	bool operator==(const GaluaField) const;
	bool operator!=(const GaluaField) const;
	GaluaField operator&(const uint8_t operand) const;
	GaluaField operator+(GaluaField operand) const;
	//GaluaField operator*(unsigned char operand) const; //Умножение многочлена на константу
	GaluaField operator*(uint8_t operand) const; //Попарное умножение элементов многочленов
	GaluaField operator*(const GaluaField operand) const; //Умножение многочлена на многочлен
	GaluaField operator%(const GaluaField operand) const;
	GaluaField pow(const GaluaField baseDeg, const unsigned char indDeg);
	
};

std::ostream& operator<<(std::ostream& out, GaluaField GFOut);