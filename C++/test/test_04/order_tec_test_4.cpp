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
		tecfile.write_log_json();
		tecfile.write_log_xml();
	}
	catch(std::runtime_error err)
	{
		std::cerr<<"runtime_error: "<<err.what()<< std::endl;
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
	FILE *of_json, *of_xml;
	errno_t err;
	err = fopen_s(&of_json, "test_04.json", "wb");
	if (err != 0)
	{
		std::cerr << "cannot open file test_04.json" << std::endl;;
	}
	err = fopen_s(&of_xml, "test_04.xml", "wb");
	if (err != 0)
	{
		std::cerr << "cannot open file test_04.xml" << std::endl;;
	}
	fprintf(of_json, "[\n");
	fprintf(of_xml, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(of_xml, "<Files>\n");
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
		tecfile.FileName= std::string("./test_04/file_s_")+ss.str();
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
		tecfile.write_log_json(of_json, 1);
		if (n != 30 - 1)
		{
			fprintf(of_json, ",");
		}
		fprintf(of_json, "\n");
		tecfile.write_log_xml(of_xml, 1);
		fprintf(of_xml, "\n");
	}
	fprintf(of_json, "]");
	fprintf(of_xml, "</Files>");
	fclose(of_json);
	fclose(of_xml);

	delete [] x;
	delete [] y;
	delete [] z;
	return 0;
}