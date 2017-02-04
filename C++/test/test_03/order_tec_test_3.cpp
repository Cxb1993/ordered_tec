# include <iostream>
using namespace std;
# include "ordered_tec.h"

# define DATATYPE double
int main(int argc,char **argv)
{
	ORDERED_TEC::TEC_FILE tecfile_grid, tecfile_solution;
	ORDERED_TEC::TEC_ZONE teczone;
	size_t NI=1000,NJ=2000;
	DATATYPE *x=new DATATYPE[NI*NJ];
	DATATYPE *y=new DATATYPE[NI*NJ];
	DATATYPE *z=new DATATYPE[NI*NJ];
	DATATYPE *w=new DATATYPE[NI*NJ];
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

	string echo_file, echo_zone;
	cout << "please input file echo leval (default/full/simple/none): " << endl;
	cin >> echo_file;
	cout << "please input zone echo leval (default/full/simple/none): " << endl;
	cin >> echo_zone;

	tecfile_grid.FileName="test_03_g";
	tecfile_grid.Title="test_03_grid";
	tecfile_grid.FileType=1;
	tecfile_grid.Variables.push_back("x");
	tecfile_grid.Variables.push_back("y");
	teczone.ZoneName="grid";
	teczone.IMax=NI;
	teczone.JMax=NJ;
	teczone.Data.push_back(ORDERED_TEC::DATA_P(x, ORDERED_TEC::DATA_P::TEC_DOUBLE));
	teczone.Data.push_back(ORDERED_TEC::DATA_P(y, ORDERED_TEC::DATA_P::TEC_DOUBLE));
	teczone.ISkip=2;
	teczone.JSkip=3;
	teczone.IBegin=50;
	teczone.IEnd=50;
	teczone.JBegin=10;
	teczone.JEnd=10;
	tecfile_grid.Zones.push_back(teczone);

	try
	{
		tecfile_grid.set_echo_mode(echo_file, echo_zone);
	}
	catch (std::runtime_error err)
	{
		cerr << "runtime_error: " << err.what() << endl;
	}

	try
	{
		tecfile_grid.write_plt();
		tecfile_grid.write_log_json();
		tecfile_grid.write_log_xml();
	}
	catch(std::runtime_error err)
	{
		cerr<<"runtime_error: "<<err.what()<<endl;
	}

	tecfile_solution.FileName="test_03_s";
	tecfile_solution.Title="test_03_solution";
	tecfile_solution.FileType=2;
	tecfile_solution.Variables.push_back("z");
	tecfile_solution.Variables.push_back("w");
	teczone.ZoneName="solution";
	teczone.Data.clear();
	teczone.Data.push_back(ORDERED_TEC::DATA_P(z, ORDERED_TEC::DATA_P::TEC_DOUBLE));
	teczone.Data.push_back(ORDERED_TEC::DATA_P(w, ORDERED_TEC::DATA_P::TEC_DOUBLE));
	tecfile_solution.Zones.push_back(teczone);

	try
	{
		tecfile_solution.set_echo_mode(echo_file, echo_zone);
	}
	catch (std::runtime_error err)
	{
		cerr << "runtime_error: " << err.what() << endl;
	}

	try
	{
		tecfile_solution.write_plt();
		tecfile_solution.write_log_json();
		tecfile_solution.write_log_xml();
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