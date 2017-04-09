# include <iostream>
# include <cmath>
# include <sstream>
# include <stdio.h>
# include <stdlib.h>
# include "ordered_tec.h"

# define DATATYPE double
int main(int argc,char **argv)
{
# ifdef __linux__
	std::cout << "os: Linus" << std::endl;
	system("rm -r test_04");
	system("mkdir test_04");
# else
	std::cout << "os: Windows" << std::endl;
	system("rmdir /s /q test_04");
	system("mkdir test_04");
# endif

	DATATYPE *x, *y, *z;
	size_t NI = 1000, NJ = 2000;
	try
	{
		x = new DATATYPE[NI*NJ];
		y = new DATATYPE[NI*NJ];
		z = new DATATYPE[NI*NJ];
	}
	catch (...)
	{
		std::cerr << "runtime_error: out of memery" << std::endl;
		return 0;
	}

	for (int j = 0; j != NJ; ++j)
	{
		for (int i = 0; i != NI; ++i)
		{
			x[i + j*NI] = j*0.01;
			y[i + j*NI] = i*0.01;
		}
	}

	ORDERED_TEC::TEC_FILE tecfile("file_g", "./test_04", "test_04_grid");
	tecfile.FileType=1;
	tecfile.Variables.push_back("x");
	tecfile.Variables.push_back("y");
	tecfile.Zones.push_back(ORDERED_TEC::TEC_ZONE("grid"));
	tecfile.Zones[0].Max[0]=NI;
	tecfile.Zones[0].Max[1]=NJ;
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::TEC_DATA(x));
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::TEC_DATA(y));
	tecfile.Zones[0].Skip[0]=10;
	tecfile.Zones[0].Skip[1]=10;
	tecfile.Zones[0].StrandId=-1;

	try
	{
		tecfile.set_echo_mode("simple", "none");
		tecfile.write_plt();
		tecfile.last_log.write_echo();
		tecfile.last_log.write_json();
		tecfile.last_log.write_xml();
	}
	catch(std::runtime_error err)
	{
		std::cerr<<"runtime_error: "<<err.what()<< std::endl;
	}

	tecfile.Title="test_04_solution";
	tecfile.FileType=2;
	tecfile.Variables.clear();
	tecfile.Variables.push_back("z");
	tecfile.Zones[0].Data.clear();
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::TEC_DATA(z));
	tecfile.Zones[0].StrandId=0;
	tecfile.set_echo_mode("simple", "none");
	std::ofstream of_j, of_x;
	of_j.open("test_04.json");
	if (!of_j)
	{
		std::cerr << "can not open file test_04.json" << std::endl;
		return 1;
	}
	of_x.open("test_04.xml");
	if (!of_x)
	{
		std::cerr << "can not open file test_04.xml" << std::endl;
		return 1;
	}
	of_j << "[" << std::endl;
	of_x << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
	of_x << "<Files>" << std::endl;
	for(int n=0;n!=30;++n)
	{
		for (int j = 0; j != NJ; ++j)
		{
			for (int i = 0; i != NI; ++i)
			{
				z[i + j*NI] = sin(x[i + j*NI]/2+n/5.0)+cos(y[i + j*NI]/2+n/5.0);
			}
		}
		std::stringstream ss;
		ss<<n;
		tecfile.FileName= std::string("file_s_")+ss.str();
		tecfile.Zones[0].ZoneName = std::string("s_") + ss.str();
		ss.str("");
		tecfile.Zones[0].SolutionTime = n;
		try
		{
			tecfile.write_plt();
			tecfile.last_log.write_json(1, of_j);
			tecfile.last_log.write_xml(1, of_x);
		}
		catch(std::runtime_error err)
		{
			std::cerr<<"runtime_error: "<<err.what()<< std::endl;
		}
		if (n != 30 - 1)
		{
			of_j << "\t," << std::endl;
		}
	}
	of_j << "]" << std::endl;
	of_x << "</Files>" << std::endl;
	of_j.close();
	of_x.close();

	delete [] x;
	delete [] y;
	delete [] z;
	return 0;
}