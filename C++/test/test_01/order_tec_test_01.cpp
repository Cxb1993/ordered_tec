# include <iostream>
# include <cmath>
using namespace std;
# include "ordered_tec.h"

# define DATATYPE double
int main(int argc,char **argv)
{
	DATATYPE *x, *y, *z;
	size_t NI=1000,NJ=2000;
	try
	{
		x = new DATATYPE[NI*NJ];
		y = new DATATYPE[NI*NJ];
		z = new DATATYPE[NI*NJ];
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
			z[i + j*NI] = sin(x[i + j*NI]/500) - cos(y[i + j*NI] /500);
		}
	}

	ORDERED_TEC::TEC_FILE tecfile("test_01");
	tecfile.Variables.push_back("x");
	tecfile.Variables.push_back("y");
	tecfile.Variables.push_back("z");
	tecfile.Zones.push_back(ORDERED_TEC::TEC_ZONE("test_01"));
	tecfile.Zones[0].Max[0] = NI;
	tecfile.Zones[0].Max[1] = NJ;
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(x));
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(y));
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(z));
	try
	{
		tecfile.write_plt();
	}
	catch(std::runtime_error err)
	{
		cerr<<"runtime_error: "<<err.what()<<endl;
	}

	delete [] x;
	delete [] y;
	delete [] z;
	return 0;
}