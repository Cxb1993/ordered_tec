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
	tecfile.Zones[0].Max[0]=NI;
	tecfile.Zones[0].Max[1]=NJ;
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::TEC_DATA(x));
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::TEC_DATA(y));
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::TEC_DATA(z));
	tecfile.Zones[0].add_auxiliary_data("Auxiliary1","Auxiliary_test_1");
	tecfile.Zones[0].add_auxiliary_data("Auxiliary2",3.14);

	tecfile.Zones.push_back(tecfile.Zones[0]);
	tecfile.Zones[1].Begin[0] = 50;
	tecfile.Zones[1].End[0] = 50;
	tecfile.Zones[1].Begin[1] = 10;
	tecfile.Zones[1].End[1] = 10;
	tecfile.Zones[1].Skip[0] = 2;
	tecfile.Zones[1].Skip[1] = 4;
	tecfile.Zones[1].ZoneName="B";
	tecfile.Zones[1].Data[2]= ORDERED_TEC::TEC_DATA(w);
	tecfile.Zones[1].Auxiliary.clear();
	tecfile.Zones[1].add_auxiliary_data("Auxiliary3","Auxiliary_test_1_2");
	tecfile.Zones[1].add_auxiliary_data("Auxiliary4",3.1415);

	tecfile.set_echo_mode("full", "full");

	try
	{
		tecfile.write_plt();
		tecfile.last_log.write_echo();
		tecfile.last_log.write_json();
		tecfile.last_log.write_xml();
		tecfile.last_log.Zones[0].write_echo();
		tecfile.last_log.Zones[0].write_json();
		tecfile.last_log.Zones[0].write_xml();
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