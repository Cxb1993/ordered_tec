# include <iostream>
# include <stdlib.h>
using namespace std;
# include "ordered_tec.h"

# define DATATYPE double
int main(int argc,char **argv)
{
	DATATYPE *x, *y, *z, *w;
	size_t NI = 2000, NJ = 4000;
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

	ORDERED_TEC::TEC_FILE tecfile("test_02",".", "test_02");
	tecfile.Variables.push_back("x");
	tecfile.Variables.push_back("y");
	tecfile.Variables.push_back("z");
	tecfile.add_auxiliary_data("Auxiliary1","Auxiliary_test_1_ds");
	tecfile.add_auxiliary_data("Auxiliary2",3.14);

	tecfile.Zones.push_back(ORDERED_TEC::TEC_ZONE("A"));
	tecfile.Zones[0].IMax=NI;
	tecfile.Zones[0].JMax=NJ;
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(x, ORDERED_TEC::DATA_P::TEC_DOUBLE));
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(y, ORDERED_TEC::DATA_P::TEC_DOUBLE));
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(z, ORDERED_TEC::DATA_P::TEC_DOUBLE));
	tecfile.Zones[0].IBegin=50;
	tecfile.Zones[0].IEnd=50;
	tecfile.Zones[0].JBegin=10;
	tecfile.Zones[0].JEnd=10;
	tecfile.Zones[0].add_auxiliary_data("Auxiliary1","Auxiliary_test_1");
	tecfile.Zones[0].add_auxiliary_data("Auxiliary2",3.14);

	tecfile.Zones.push_back(tecfile.Zones[0]);
	tecfile.Zones[1].ZoneName="B";
	tecfile.Zones[1].Data[2]= ORDERED_TEC::DATA_P(w, ORDERED_TEC::DATA_P::TEC_DOUBLE);
	tecfile.Zones[1].Auxiliary.clear();
	tecfile.Zones[1].add_auxiliary_data("Auxiliary3","Auxiliary_test_1_2");
	tecfile.Zones[1].add_auxiliary_data("Auxiliary4",3.1415);

	tecfile.set_echo_mode("full", "full");

	try
	{
		tecfile.write_plt();
	}
	catch(std::runtime_error err)
	{
		cerr<<"runtime_error: "<<err.what()<<endl;
	}

	system("pause");
	cout << endl;
	cout << tecfile.get_log("xml") << endl;
	cout << tecfile.get_log("json") << endl;
	cout << tecfile.get_log("time") << endl;
	cout << tecfile.get_log("usingtime") << endl;
	cout << tecfile.get_log("wrong code") << endl;

	delete [] x;
	delete [] y;
	delete [] z;
	delete [] w;
	return 0;
}