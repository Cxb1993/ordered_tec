# include <iostream>
# include <fstream>
# include <cmath>
using namespace std;
# include "ordered_tec.h"

# define DATATYPE double
void write(ORDERED_TEC::TEC_FILE &tecfile, ofstream &log)
{
	try
	{
		tecfile.write_plt(false);
		tecfile.last_log.write_echo(log);
	}
	catch (std::runtime_error err)
	{
		log << "runtime_error: " << err.what() << endl;
		log << tecfile.last_log.Error << endl;
	}
}

int main(int argc, char **argv)
{
	DATATYPE *x, *y, *z, *w;
	size_t NI = 500, NJ = 1000;
	try
	{
		x = new DATATYPE[NI*NJ];
		y = new DATATYPE[NI*NJ];
		z = new DATATYPE[NI*NJ];
		w = new DATATYPE[NI*NJ];
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
			z[i + j*NI] = 1 + i / 2 + j;
			w[i + j*NI] = i + j;
		}
	}

	ofstream log("test_06.txt");

	ORDERED_TEC::TEC_FILE tecfile("test_06", ".", "test_06");
	write(tecfile, log); log << endl;
	tecfile.Variables.push_back("x");
	tecfile.Variables.push_back("y");
	tecfile.Variables.push_back("z");
	write(tecfile, log); log << endl;
	tecfile.Zones.push_back(ORDERED_TEC::TEC_ZONE("A"));
	tecfile.Zones[0].Max[0] = NI;
	tecfile.Zones[0].Max[1] = NJ;
	write(tecfile, log); log << endl;
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::TEC_DATA(x));
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::TEC_DATA(y));
	write(tecfile, log); log << endl;
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::TEC_DATA(z));
	tecfile.Zones.push_back(tecfile.Zones[0]);
	tecfile.Zones[1].ZoneName = "B";
	tecfile.Zones[1].Data[2] = ORDERED_TEC::TEC_DATA();
	write(tecfile, log); log << endl;
	tecfile.Zones[1].Data[2] = ORDERED_TEC::TEC_DATA(w);
	write(tecfile, log); log << endl;

	log.close();

	delete[] x;
	delete[] y;
	delete[] z;
	delete[] w;
	return 0;
}