# include <iostream>
# include <cmath>
# include <sstream>
# include <stdlib.h>
using namespace std;
# include "ordered_tec.h"

# define DATATYPE double
int main(int argc,char **argv)
{
	ORDERED_TEC::TEC_FILE tecfile;
	ORDERED_TEC::TEC_ZONE teczone;
	size_t NI=1000,NJ=2000;
	DATATYPE *x=new DATATYPE[NI*NJ];
	DATATYPE *y=new DATATYPE[NI*NJ];
	DATATYPE *z=new DATATYPE[NI*NJ];
	
# ifdef __linux__
	cout<<"os: Linus"<<endl;
	system("rm -r test_04");
	system("mkdir test_04");
# else
	cout<<"os: Windows"<<endl;
	system("rmdir /s /q test_04");
	system("mkdir test_04");
# endif

	for (int j = 0; j != NJ; ++j)
	{
		for (int i = 0; i != NI; ++i)
		{
			x[i + j*NI] = j*0.01;
			y[i + j*NI] = i*0.01;
		}
	}
	tecfile.FileName="./test_04/file_g";
	tecfile.Title="test_04_grid";
	tecfile.FileType=1;
	tecfile.Variables.push_back("x");
	tecfile.Variables.push_back("y");
	teczone.ZoneName="grid";
	teczone.IMax=NI;
	teczone.JMax=NJ;
	teczone.Data.push_back(ORDERED_TEC::DATA_P(x, ORDERED_TEC::DATA_P::TEC_DOUBLE));
	teczone.Data.push_back(ORDERED_TEC::DATA_P(y, ORDERED_TEC::DATA_P::TEC_DOUBLE));
	teczone.ISkip=10;
	teczone.JSkip=10;
	teczone.StrandId=-1;
	tecfile.Zones.push_back(teczone);

	tecfile.set_echo_mode("00001", "none");

	try
	{
		tecfile.write_plt();
	}
	catch(std::runtime_error err)
	{
		cerr<<"runtime_error: "<<err.what()<<endl;
	}

	tecfile.Title="test_04_solution";
	tecfile.FileType=2;
	tecfile.Variables.clear();
	tecfile.Variables.push_back("z");
	teczone.Data.clear();
	teczone.Data.push_back(ORDERED_TEC::DATA_P(z, ORDERED_TEC::DATA_P::TEC_DOUBLE));
	teczone.StrandId=0;
	tecfile.Zones.clear();
	tecfile.Zones.push_back(teczone);
	tecfile.set_echo_mode("00001", "none");
	for(int n=0;n!=30;++n)
	{
		for (int j = 0; j != NJ; ++j)
		{
			for (int i = 0; i != NI; ++i)
			{
				z[i + j*NI] = sin(x[i + j*NI]/2+n/5.0)+cos(y[i + j*NI]/2+n/5.0);
			}
		}
		stringstream ss;
		ss<<n;
		tecfile.FileName=string("./test_04/file_s_")+ss.str();
		tecfile.Zones[0].ZoneName = string("s_") + ss.str();
		ss.str("");
		tecfile.Zones[0].SolutionTime = n;
		try
		{
			tecfile.write_plt();
		}
		catch(std::runtime_error err)
		{
			cerr<<"runtime_error: "<<err.what()<<endl;
		}
	}
	

	delete [] x;
	delete [] y;
	delete [] z;
	return 0;
}