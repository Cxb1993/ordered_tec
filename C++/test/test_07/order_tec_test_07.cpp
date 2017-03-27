# include <iostream>
# include <fstream>
# include <stdlib.h>
using namespace std;
# include "ordered_tec.h"

# define DATATYPE double
void get_size(ORDERED_TEC::TEC_FILE &tecfile, ostream &log)
{
	int Max[3], Dim;
	try
	{
		Max[0] = tecfile.Zones[0].get_real_size()[0];
		Max[1] = tecfile.Zones[0].get_real_size()[1];
		Max[2] = tecfile.Zones[0].get_real_size()[2];
		Dim = *tecfile.Zones[0].get_real_size("realdim");
	}
	catch (std::runtime_error err)
	{
		log << "runtime_error: " << err.what() << endl;
		return;
	}
	log << "Max[0]: " << Max[0] << endl;
	log << "Max[1]: " << Max[1] << endl;
	log << "Max[2]: " << Max[2] << endl;
	log << "Dim: " << Dim << endl;
}

int main(int argc, char **argv)
{
	DATATYPE *x, *y, *z, *w;
	size_t NI = 100, NJ = 200;
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

	ORDERED_TEC::TEC_FILE tecfile("test_07", ".", "test_07");
	tecfile.Variables.push_back("x");
	tecfile.Variables.push_back("y");
	tecfile.Variables.push_back("z");
	tecfile.Zones.push_back(ORDERED_TEC::TEC_ZONE("A"));
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::TEC_DATA(x));
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::TEC_DATA(y));
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::TEC_DATA(z));

	ofstream log("test_07.txt");
	tecfile.Zones[0].Max[0] = 0;
	tecfile.Zones[0].Max[1] = 0;
	tecfile.Zones[0].Max[2] = 0;
	tecfile.Zones[0].Skip[0] = 0;
	tecfile.Zones[0].Skip[1] = 0;
	tecfile.Zones[0].Skip[2] = 0;
	tecfile.Zones[0].Begin[0] = 50;
	tecfile.Zones[0].End[0] = 50;
	tecfile.Zones[0].Begin[1] = 101;
	tecfile.Zones[0].End[1] = 100;
	tecfile.Zones[0].Begin[2] = 101;
	tecfile.Zones[0].End[2] = 50;

	get_size(tecfile, log); log << endl;
	tecfile.Zones[0].Max[0] = NI;
	get_size(tecfile, log); log << endl;
	tecfile.Zones[0].Skip[0] = 1;
	get_size(tecfile, log); log << endl;
	tecfile.Zones[0].Begin[0] = 49;
	tecfile.Zones[0].End[0] = 50;
	
	get_size(tecfile, log); log << endl;
	tecfile.Zones[0].Max[1] = NJ;
	get_size(tecfile, log); log << endl;
	tecfile.Zones[0].Skip[1] = 2;
	get_size(tecfile, log); log << endl;
	tecfile.Zones[0].Begin[1] = 10;
	tecfile.Zones[0].End[1] = 10;

	get_size(tecfile, log); log << endl;
	tecfile.Zones[0].Max[2] = 1;
	get_size(tecfile, log); log << endl;
	tecfile.Zones[0].Skip[2] = 1;
	get_size(tecfile, log); log << endl;
	tecfile.Zones[0].Begin[2] = 0;
	tecfile.Zones[0].End[2] = 0;

	get_size(tecfile, log); log << endl;
	
	tecfile.set_echo_mode("full", "full");

	try
	{
		tecfile.write_plt(false);
		tecfile.last_log.write_echo(log);
	}
	catch (std::runtime_error err)
	{
		cerr << "runtime_error: " << err.what() << endl;
		log.close();
	}

	log.close();

	delete[] x;
	delete[] y;
	delete[] z;
	delete[] w;
	return 0;
}