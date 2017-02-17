# include <iostream>
# include <sstream>
# include <fstream>
# include <cmath>
using namespace std;
# include "ordered_tec.h"

# define DATATYPE double

void write_plt(ORDERED_TEC::TEC_FILE & tecfile, ostream &log)
{
	try
	{
		tecfile.write_plt(log);
		double ut;
		istringstream tr(tecfile.get_log("usingtime"));
		tr >> ut;
		log << "speed: " 
			<< (tecfile.Zones[0].get_real_size(0)*tecfile.Zones[0].get_real_size(1)*tecfile.Zones[0].get_real_size(2)) / ut 
			<< " N/s"
			<< endl;
	}
	catch (std::runtime_error err)
	{
		cerr << "runtime_error: " << err.what() << endl;
	}
	cout << "----------------------" << endl;
}

int main(int argc, char **argv)
{
	DATATYPE *x, *y, *z;
	size_t NI = 3000, NJ = 6000;
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
			z[i + j*NI] = sin(x[i + j*NI] / 1500) - cos(y[i + j*NI] / 3000);
		}
	}

	ORDERED_TEC::TEC_FILE tecfile("test_08");
	tecfile.Variables.push_back("x");
	tecfile.Variables.push_back("y");
	tecfile.Variables.push_back("z");
	tecfile.Zones.push_back(ORDERED_TEC::TEC_ZONE("test_08"));
	tecfile.Zones[0].IMax = NI;
	tecfile.Zones[0].JMax = NJ;
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(x));
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(y));
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(z));
	tecfile.set_echo_mode("none", "001111000");

	ofstream log("test_08.txt");

	write_plt(tecfile,log);

	tecfile.Zones[0].JBegin = 200;
	tecfile.Zones[0].JEnd = 200;
	write_plt(tecfile, log);

	tecfile.Zones[0].IBegin = 100;
	tecfile.Zones[0].IEnd = 100;
	write_plt(tecfile, log);

	tecfile.Zones[0].JSkip = 3;
	write_plt(tecfile, log);

	tecfile.Zones[0].ISkip = 3;
	write_plt(tecfile, log);

	log.close();

	delete[] x;
	delete[] y;
	delete[] z;
	return 0;
}