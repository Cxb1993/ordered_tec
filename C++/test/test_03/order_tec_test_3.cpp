# include <iostream>
# include <fstream>
# include <sstream>
using namespace std;
# include "ordered_tec.h"

# define DATATYPE double
int main(int argc,char **argv)
{
	DATATYPE *x, *y, *z, *w;
	size_t NI = 1000, NJ = 2000;
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

	ORDERED_TEC::TEC_FILE tecfile_grid("test_03_g", ".", "test_03_grid");
	tecfile_grid.FileType=1;
	tecfile_grid.Variables.push_back("x");
	tecfile_grid.Variables.push_back("y");
	tecfile_grid.Zones.push_back(ORDERED_TEC::TEC_ZONE("grid"));
	tecfile_grid.Zones[0].IMax=NI;
	tecfile_grid.Zones[0].JMax=NJ;
	tecfile_grid.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(x, ORDERED_TEC::DATA_P::TEC_DOUBLE));
	tecfile_grid.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(y, ORDERED_TEC::DATA_P::TEC_DOUBLE));
	tecfile_grid.Zones[0].ISkip=2;
	tecfile_grid.Zones[0].JSkip=3;
	tecfile_grid.Zones[0].IBegin=50;
	tecfile_grid.Zones[0].IEnd=50;
	tecfile_grid.Zones[0].JBegin=10;
	tecfile_grid.Zones[0].JEnd=10;
	tecfile_grid.set_echo_mode("full", "full");
	try
	{
		ofstream log("log.txt");
		tecfile_grid.write_plt(log);
		log.close();
	}
	catch(std::runtime_error err)
	{
		cerr<<"runtime_error: "<<err.what()<<endl;
	}

	ORDERED_TEC::TEC_FILE tecfile_solution("test_03_s", ".", "test_03_solution");
	tecfile_solution.FileType=2;
	tecfile_solution.Variables.push_back("z");
	tecfile_solution.Variables.push_back("w");
	tecfile_solution.Zones.push_back(tecfile_grid.Zones[0]);
	tecfile_solution.Zones[0].ZoneName="solution";
	tecfile_solution.Zones[0].Data.clear();
	tecfile_solution.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(z, ORDERED_TEC::DATA_P::TEC_DOUBLE));
	tecfile_solution.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(w, ORDERED_TEC::DATA_P::TEC_DOUBLE));
	tecfile_solution.set_echo_mode("full", "full");
	try
	{
		ostringstream log;
		tecfile_solution.write_plt(log);
		cout << log.str();
	}
	catch(std::runtime_error err)
	{
		cerr<<"runtime_error: "<<err.what()<<endl;
	}

	delete [] x;
	delete [] y;
	delete [] z;
	delete [] w;
	return 0;
}