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
	tecfile.Zones[0].IMax=NI;
	tecfile.Zones[0].JMax=NJ;
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(x));
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(y));
	tecfile.Zones[0].ISkip=10;
	tecfile.Zones[0].JSkip=10;
	tecfile.Zones[0].StrandId=-1;
	tecfile.set_echo_mode("simple", "none");

	try
	{
		tecfile.write_plt();
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
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(z));
	tecfile.Zones[0].StrandId=0;
	tecfile.set_echo_mode("simple", "none");
	tecfile.Json_Depth = 1;
	tecfile.Xml_Depth = 1;
# ifdef __linux__
	tecfile.Json_File = fopen("test_04.json", "wb");
	if (tecfile.Json_File == NULL)
	{
		std::cerr << "cannot open file test_04.json" << std::endl;
	}
	tecfile.Xml_File = fopen("test_04.xml", "wb");
	if (tecfile.Xml_File == NULL)
	{
		std::cerr << "cannot open file test_04.xml" << std::endl;
	}
# else
	errno_t err;
	err = fopen_s(&tecfile.Json_File, "test_04.json", "wb");
	if (err != 0)
	{
		std::cerr << "cannot open file test_04.json" << std::endl;
	}
	err = fopen_s(&tecfile.Xml_File, "test_04.xml", "wb");
	if (err != 0)
	{
		std::cerr << "cannot open file test_04.xml" << std::endl;
	}
# endif
	fprintf(tecfile.Json_File, "[\n");
	fprintf(tecfile.Xml_File, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(tecfile.Xml_File, "<Files>\n");
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
		}
		catch(std::runtime_error err)
		{
			std::cerr<<"runtime_error: "<<err.what()<< std::endl;
		}
		if (n != 30 - 1)
		{
			fprintf(tecfile.Json_File, ",");
		}
		fprintf(tecfile.Json_File, "\n");
		fprintf(tecfile.Xml_File, "\n");
	}
	fprintf(tecfile.Json_File, "]\n");
	fprintf(tecfile.Xml_File, "</Files>\n");
	fclose(tecfile.Json_File);
	tecfile.Json_File = NULL;
	fclose(tecfile.Xml_File);
	tecfile.Xml_File = NULL;

	delete [] x;
	delete [] y;
	delete [] z;
	return 0;
}