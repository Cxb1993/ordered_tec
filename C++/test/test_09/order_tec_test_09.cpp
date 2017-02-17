# include <iostream>
# include <cmath>
using namespace std;
# include "ordered_tec.h"

template<typename T>
void set_data(T *data, ORDERED_TEC::TEC_FILE &tecfile)
{
	int NI = tecfile.Zones[0].IMax;
	int NJ = tecfile.Zones[0].JMax;
	for (int j = 0; j != NJ; ++j)
	{
		for (int i = 0; i != NI; ++i)
		{
			data[i + j*NI] = 10 * (sin(double(j) / 500) - cos(double(i) / 500));
		}
	}
	tecfile.Zones.push_back(tecfile.Zones[0]);
	try
	{
		(tecfile.Zones.end() - 1)->Data.push_back(ORDERED_TEC::DATA_P(data));
	}
	catch (std::runtime_error err)
	{
		cerr << "runtime_error: " << err.what() << endl;
		tecfile.Zones.pop_back();
		return;
	}
}

int main(int argc, char **argv)
{
	double *x, *y;
	signed char *p_byte;
	short int *p_shortint;
	long int *p_longint;
	int *p_int;
	float *p_float;
	double *p_double;

	size_t NI = 1000, NJ = 2000;
	try
	{
		x = new double[NI*NJ];
		y = new double[NI*NJ];
		p_byte = new signed char[NI*NJ];
		p_shortint = new short int[NI*NJ];
		p_longint = new long int[NI*NJ];
		p_int = new int[NI*NJ];
		p_float = new float[NI*NJ];
		p_double = new double[NI*NJ];
	}
	catch (...)
	{
		cerr << "runtime_error: out of memery" << endl;
		return 0;
	}
	for (int j = 0; j != NJ; ++j)
	{
		for (int i = 0; i != NI; ++i)
		{
			x[i + j*NI] = j;
			y[i + j*NI] = i;
		}
	}

	ORDERED_TEC::TEC_FILE tecfile("test_09");
	tecfile.Variables.push_back("x");
	tecfile.Variables.push_back("y");
	tecfile.Variables.push_back("z");
	tecfile.Zones.push_back(ORDERED_TEC::TEC_ZONE("test_09"));
	tecfile.Zones[0].IMax = NI;
	tecfile.Zones[0].JMax = NJ;
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(x));
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(y));

	cout << "signed char" << endl;
	set_data(p_byte, tecfile);
	cout << "short int" << endl;
	set_data(p_shortint, tecfile);
	cout << "long int" << endl;
	set_data(p_longint, tecfile);
	cout << "int" << endl;
	set_data(p_int, tecfile);
	cout << "float" << endl;
	set_data(p_float, tecfile);
	cout << "double" << endl;
	set_data(p_double, tecfile);

	tecfile.Zones.erase(tecfile.Zones.begin());
	tecfile.set_echo_mode("full", "full");

	try
	{
		tecfile.write_plt();
	}
	catch (std::runtime_error err)
	{
		cerr << "runtime_error: " << err.what() << endl;
	}

	try
	{
		tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(&NI));
	}
	catch (std::runtime_error err)
	{
		cerr << "runtime_error: " << err.what() << endl;
	}

	delete[] x;
	delete[] y;
	delete[] p_byte;
	delete[] p_shortint;
	delete[] p_longint;
	delete[] p_int;
	delete[] p_float;
	delete[] p_double;

	return 0;
}