# include <iostream>
# include <fstream>
# include <stdlib.h>
using namespace std;
# include "ordered_tec.h"

# define DATATYPE double
void get_size(ORDERED_TEC::TEC_FILE &tecfile, ostream &log)
{
	int IMax, JMax, KMax, Dim;
	try
	{
		IMax = tecfile.Zones[0].get_real_size(0);
		JMax = tecfile.Zones[0].get_real_size(1);
		KMax = tecfile.Zones[0].get_real_size(2);
		Dim = tecfile.Zones[0].get_real_size(3);
	}
	catch (std::runtime_error err)
	{
		log << "runtime_error: " << err.what() << endl;
		return;
	}
	log << "IMax: " << IMax << endl;
	log << "JMax: " << JMax << endl;
	log << "KMax: " << KMax << endl;
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
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(x, ORDERED_TEC::DATA_P::TEC_DOUBLE));
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(y, ORDERED_TEC::DATA_P::TEC_DOUBLE));
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(z, ORDERED_TEC::DATA_P::TEC_DOUBLE));

	ofstream log("test_07.txt");
	tecfile.Zones[0].IMax = 0;
	tecfile.Zones[0].JMax = 0;
	tecfile.Zones[0].KMax = 0;
	tecfile.Zones[0].ISkip = 0;
	tecfile.Zones[0].JSkip = 0;
	tecfile.Zones[0].KSkip = 0;
	tecfile.Zones[0].IBegin = 50;
	tecfile.Zones[0].IEnd = 50;
	tecfile.Zones[0].JBegin = 101;
	tecfile.Zones[0].JEnd = 100;
	tecfile.Zones[0].KBegin = 101;
	tecfile.Zones[0].KEnd = 50;

	get_size(tecfile, log); log << endl;
	tecfile.Zones[0].IMax = NI;
	get_size(tecfile, log); log << endl;
	tecfile.Zones[0].ISkip = 1;
	get_size(tecfile, log); log << endl;
	tecfile.Zones[0].IBegin = 49;
	tecfile.Zones[0].IEnd = 50;
	
	get_size(tecfile, log); log << endl;
	tecfile.Zones[0].JMax = NJ;
	get_size(tecfile, log); log << endl;
	tecfile.Zones[0].JSkip = 2;
	get_size(tecfile, log); log << endl;
	tecfile.Zones[0].JBegin = 10;
	tecfile.Zones[0].JEnd = 10;

	get_size(tecfile, log); log << endl;
	tecfile.Zones[0].KMax = 1;
	get_size(tecfile, log); log << endl;
	tecfile.Zones[0].KSkip = 1;
	get_size(tecfile, log); log << endl;
	tecfile.Zones[0].KBegin = 0;
	tecfile.Zones[0].KEnd = 0;

	get_size(tecfile, log); log << endl;
	
	tecfile.set_echo_mode("full", "full");

	try
	{
		tecfile.write_plt(log);
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