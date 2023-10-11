#include "GaluaField.h"

GaluaField::GaluaField()
{
	Polynom = 0;
}

GaluaField::GaluaField(const uint8_t Init)
{
	Polynom = Init;
}

unsigned char& GaluaField::at(const size_t Index)
{
	if (Index >= polynomSz) throw std::out_of_range("Invalid index");
	return reinterpret_cast<unsigned char*>(&Polynom)[Index];
}

GaluaField GaluaField::operator>>(const size_t operand) const 
{
	return Polynom >> operand;
}

GaluaField GaluaField::operator<<(const size_t operand) const
{
	return Polynom << operand;
}

bool GaluaField::operator==(const GaluaField operand) const 
{
	return (*this).Polynom == operand.Polynom;
}

bool GaluaField::operator!=(const GaluaField operand) const
{
	return (*this).Polynom != operand.Polynom;
}

GaluaField GaluaField::operator&(const uint8_t operand) const
{
	return GaluaField((*this).Polynom & operand);
}

GaluaField GaluaField::operator+(GaluaField operand) const
{
	return (*this).Polynom ^ operand.Polynom;
}

GaluaField GaluaField::operator*(uint8_t operand) const
{
	return (*this).Polynom * operand;
}

GaluaField GaluaField::operator*(GaluaField operand) const
{
	GaluaField oper1 = *this;
	GaluaField Itog;
	size_t kolvoSteps = 0;
	for (kolvoSteps = 0; kolvoSteps < polynomSz; ++kolvoSteps)
		if ((oper1 >> kolvoSteps) == 0) break;
	for (size_t i = 0; i < kolvoSteps; ++i)
	{
		Itog = Itog + (operand * (uint8_t)(oper1 & 1));
		operand = (operand << 1) + basePolynom * ((operand >> (polynomSz - 1)).Polynom);
		oper1 = oper1 >> 1;
	}
	return Itog;
}

std::ostream& operator<<(std::ostream& out, GaluaField GFOut)
{
	std::string strOut = "0+";
	for (size_t i = 0; i < 8; ++i)
	{
		if (GFOut == 0) break;
		if ((GFOut & 1)!=GaluaField(0)) strOut = strOut + "+x^" + std::to_string(i);
		GFOut = GFOut >> 1;
	}
	out << strOut;
	return out;
}