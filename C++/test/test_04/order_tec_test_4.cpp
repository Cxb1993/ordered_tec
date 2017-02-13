# include <iostream>
# include <cmath>
# include <sstream>
# include <stdlib.h>
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
	std::cout<<"os: Windows"<< std::endl;
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
	tecfile.FilePath = "./test_04";
	tecfile.FileName="file_g";
	tecfile.Title="test_04_grid";
	tecfile.FileType=1;
	tecfile.Variables.push_back("x");
	tecfile.Variables.push_back("y");
	tecfile.Zones.push_back(ORDERED_TEC::TEC_ZONE("grid"));
	tecfile.Zones[0].IMax=NI;
	tecfile.Zones[0].JMax=NJ;
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(x, ORDERED_TEC::DATA_P::TEC_DOUBLE));
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(y, ORDERED_TEC::DATA_P::TEC_DOUBLE));
	tecfile.Zones[0].ISkip=10;
	tecfile.Zones[0].JSkip=10;
	tecfile.Zones[0].StrandId=-1;

	tecfile.set_echo_mode("simple", "none");
	tecfile.Json_WriteFile = true;
	tecfile.Xml_WriteFile = true;

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
	tecfile.Zones[0].Data.push_back(ORDERED_TEC::DATA_P(z, ORDERED_TEC::DATA_P::TEC_DOUBLE));
	tecfile.Zones[0].StrandId=0;
	tecfile.set_echo_mode("simple", "none");
	tecfile.Json_Depth = 1;
	tecfile.Xml_Depth = 1;
	errno_t err;
	err = fopen_s(&tecfile.Json_File, "test_04.json", "wb");
	if (err != 0)
	{
		std::cerr << "cannot open file test_04.json" << std::endl;;
	}
	err = fopen_s(&tecfile.Xml_File, "test_04.xml", "wb");
	if (err != 0)
	{
		std::cerr << "cannot open file test_04.xml" << std::endl;;
	}
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
	fprintf(tecfile.Json_File, "]");
	fprintf(tecfile.Xml_File, "</Files>");
	fclose(tecfile.Json_File);
	fclose(tecfile.Xml_File);

	delete [] x;
	delete [] y;
	delete [] z;
	return 0;
}