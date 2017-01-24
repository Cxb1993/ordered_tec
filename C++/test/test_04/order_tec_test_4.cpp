# include <iostream>
# include <cmath>
# include <sstream>
# include <stdlib.h>
using namespace std;
# include "ordered_tec.h"

# define DATATYPE double
int main(int argc,char **argv)
{
	TEC_FILE tecfile;
	TEC_ZONE teczone;
	size_t NI=1000,NJ=2000;
	DATATYPE *x=new DATATYPE[NI*NJ];
	DATATYPE *y=new DATATYPE[NI*NJ];
	DATATYPE *z=new DATATYPE[NI*NJ];
	unsigned int echo = 1;
	
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
	teczone.Data.push_back(DATA_P(x,DATA_P::TEC_DOUBLE));
	teczone.Data.push_back(DATA_P(y,DATA_P::TEC_DOUBLE));
	teczone.ISkip=10;
	teczone.JSkip=10;
	teczone.StrandId=-1;
	try
	{
		ORDERED_TEC(tecfile,teczone,echo);
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
	teczone.Data.push_back(DATA_P(z,DATA_P::TEC_DOUBLE));
	teczone.StrandId=0;
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
		teczone.ZoneName=string("s_")+ss.str();
		ss.str("");
		teczone.SolutionTime=n;
		try
		{
			ORDERED_TEC(tecfile,teczone,echo);
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