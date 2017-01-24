# include <iostream>
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
	for (int j = 0; j != NJ; ++j)
	{
		for (int i = 0; i != NI; ++i)
		{
			x[i + j*NI] = j;
			y[i + j*NI] = i;
			z[i + j*NI] = 1 + i / 2 + j;
		}
	}
	unsigned int echo = 7;

	tecfile.Variables.push_back("x");
	tecfile.Variables.push_back("y");
	tecfile.Variables.push_back("z");
	teczone.IMax=NI;
	teczone.JMax=NJ;
	teczone.Data.push_back(DATA_P(x,DATA_P::TEC_DOUBLE));
	teczone.Data.push_back(DATA_P(y,DATA_P::TEC_DOUBLE));
	teczone.Data.push_back(DATA_P(z,DATA_P::TEC_DOUBLE));

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
	return 0;
}