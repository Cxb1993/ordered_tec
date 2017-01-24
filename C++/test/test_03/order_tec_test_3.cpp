# include <iostream>
using namespace std;
# include "ordered_tec.h"

# define DATATYPE double
int main(int argc,char **argv)
{
	odt::TEC_FILE tecfile;
	odt::TEC_ZONE teczone;
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
	unsigned int echo = 11;

	cout<<"please input echo leval (a number such as 1,2,3 ...): "<<endl;
	cin>>echo;

	tecfile.FileName="test_03_g";
	tecfile.Title="test_03_grid";
	tecfile.FileType=1;
	tecfile.Variables.push_back("x");
	tecfile.Variables.push_back("y");
	teczone.ZoneName="grid";
	teczone.IMax=NI;
	teczone.JMax=NJ;
	teczone.Data.push_back(odt::DATA_P(x, odt::DATA_P::TEC_DOUBLE));
	teczone.Data.push_back(odt::DATA_P(y, odt::DATA_P::TEC_DOUBLE));
	teczone.ISkip=2;
	teczone.JSkip=3;
	teczone.IBegin=50;
	teczone.IEnd=50;
	teczone.JBegin=10;
	teczone.JEnd=10;
	try
	{
		ORDERED_TEC(tecfile,teczone,echo);
	}
	catch(std::runtime_error err)
	{
		cerr<<"runtime_error: "<<err.what()<<endl;
	}

	tecfile.FileName="test_03_s";
	tecfile.Title="test_03_solution";
	tecfile.FileType=2;
	tecfile.Variables.clear();
	tecfile.Variables.push_back("z");
	tecfile.Variables.push_back("w");
	teczone.ZoneName="solution";
	teczone.Data.clear();
	teczone.Data.push_back(odt::DATA_P(z, odt::DATA_P::TEC_DOUBLE));
	teczone.Data.push_back(odt::DATA_P(w, odt::DATA_P::TEC_DOUBLE));
	try
	{
		ORDERED_TEC(tecfile,teczone, echo);
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