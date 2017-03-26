# include <iostream>
# include <fstream>
# include <cmath>
using namespace std;
# include "ordered_tec.h"

# define DATATYPE double
void setecho_and_write(ORDERED_TEC::TEC_FILE &tecfile, ostream &log, string echo_file, string echo_zone)
{
	try
	{
		tecfile.set_echo_mode(echo_file, echo_zone);
		tecfile.write_plt(log);
	}
	catch (std::runtime_error err)
	{
		log << "runtime_error: " << err.what() << endl;
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

	ORDERED_TEC::TEC_FILE tecfile("test_05", ".", "test_05");
	tecfile.Variables.push_back("x");
	tecfile.Variables.push_back("y");
	tecfile.Variables.push_back("z");
	tecfile.Zones.push_back(ORDERED_TEC::TEC_ZONE("A"));
	tecfile.Zones[0].Max[0] = NI;
	tecfile.Zones[0].Max[1] = NJ;
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::TEC_DATA(x));
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::TEC_DATA(y));
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::TEC_DATA(z));
	tecfile.Zones[0].StrandId = 0;
	tecfile.Zones.push_back(tecfile.Zones[0]);
	tecfile.Zones[1].ZoneName = "B";
	tecfile.Zones[1].Data[2] = ORDERED_TEC::TEC_DATA(w);
	tecfile.Zones[1].StrandId = -1;
	
	vector<string> echo_mode;
	echo_mode.push_back("brief");
	echo_mode.push_back("full");
	echo_mode.push_back("simple");
	echo_mode.push_back("none");
	echo_mode.push_back("leave");

	ofstream log("test_05.txt");

	for (int i = 0; i != 7; ++i)
	{
		string mode;
		for (int k = 0; k != 7 - i - 1; ++k)
		{
			mode += '0';
		}
		mode += '1';
		for (int k = 0; k != i; ++k)
		{
			mode += '0';
		}
		log << mode << endl;
		setecho_and_write(tecfile, log, mode, "none");
		log << endl;
	}
	string mode = "0100011";
	log << mode << endl;
	setecho_and_write(tecfile, log, mode, "none");
	log << endl;

	for (int i = 0; i != 9; ++i)
	{
		string mode;
		for (int k = 0; k != 9 - i - 1; ++k)
		{
			mode += '0';
		}
		mode += '1';
		for (int k = 0; k != i; ++k)
		{
			mode += '0';
		}
		log << mode << endl;
		setecho_and_write(tecfile, log, "none", mode);
		log << endl;
	}

	log << "_wrong_code_" << endl;
	setecho_and_write(tecfile, log, mode, "_wrong_code_");
	log << endl;
	log << "_wrong_code_" << endl;
	setecho_and_write(tecfile, log, "_wrong_code_", mode);
	log << endl;

	for (vector<string>::const_iterator i = echo_mode.begin(); i != echo_mode.end(); ++i)
	{
		for (vector<string>::const_iterator j = echo_mode.begin(); j != echo_mode.end(); ++j)
		{
			log << "file: " << *i << endl;
			log << "zones: " << *j << endl;
			setecho_and_write(tecfile, log, *i, *j);
			log << endl;
		}
	}

	log.close();

	delete[] x;
	delete[] y;
	delete[] z;
	delete[] w;
	return 0;
}