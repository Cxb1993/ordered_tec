# include "ordered_tec.h"

# include <string>
# include <vector>
# include <map>
# include <cstring>
# include <stdexcept>
# include <iostream>
# include <fstream>
# include <sstream>
# include <cstdio>
# include <ctime>

# define TEC_INT32_S 4
# define TEC_FLOAT32_S 4
# define TEC_FLOAT64_S 8

using namespace ORDERED_TEC;

void W_INT32(const INT32 &a, FILE *f)
{
	INT32 t = a;
	std::fwrite(&t, TEC_INT32_S, 1, f);
}

void W_FLOAT32(const FLOAT32 &a, FILE *f)
{
	FLOAT32 t = a;
	std::fwrite(&t, TEC_FLOAT32_S, 1, f);
}

void W_FLOAT64(const FLOAT64 &a, FILE *f)
{
	FLOAT64 t = a;
	std::fwrite(&t, TEC_FLOAT64_S, 1, f);
}

void W_STRING(const std::string &a, FILE *f)
{
	for (std::string::const_iterator i = a.begin(); i != a.end(); ++i)
	{
		W_INT32(*i, f);
	}
	W_INT32(0, f);
}

std::string get_time(const std::string &format = "%Y%m%dT%H%M%S")
{
	time_t time_c = std::time(NULL);
	struct tm tm_c;
# ifdef __linux__
	localtime_r(&time_c, &tm_c);
# else
	localtime_s(&tm_c, &time_c);
# endif
	char buf[100];
	std::strftime(buf, 100, format.c_str(), &tm_c);
	return buf;
}

FILE * openfile(const std::string &fullname, const char* mode)
{
# ifdef __linux__
	FILE *of;
	of = std::fopen(fullname.c_str(), mode);
	if (of == NULL)
	{
		throw std::runtime_error(std::string("can not open file ") + fullname);
	}
# else
	FILE *of;
	errno_t err = fopen_s(&of, fullname.c_str(), mode);
	if (err != 0)
	{
		throw std::runtime_error(std::string("can not open file ") + fullname);
	}
# endif
	return of;
}

TEC_FILE_LOG::TEC_FILE_LOG()
{
	FileType = -1;
	Size = 0;
	UsingTime = 0;
}

TEC_FILE_LOG::TEC_FILE_LOG(const TEC_FILE & file) : TEC_FILE_BASE(file) {}

void TEC_FILE_LOG::write_echo(std::ofstream &of)
{
	bool newfile = !of.is_open();
	if (newfile)
	{
		of.clear();
		of.open((FilePath + "/" + FileName + ".txt").c_str());
		if (!of)
		{
			throw std::runtime_error(std::string("can not open file ") + (FilePath + "/" + FileName + ".txt"));
		}
	}

	std::vector<TEC_ZONE_LOG>::iterator zi = Zones.begin();
	for (std::vector<std::string>::iterator i = Echo_Text.begin(); i != Echo_Text.end(); ++i)
	{
		if (i->compare("#ZONE#") == 0)
		{
			zi->write_echo(of);
			++zi;
		}
		else
		{
			of << i->c_str() << std::endl;
		}
	}

	if (newfile)
	{
		of.close();
	}
}

void TEC_FILE_LOG::write_json(int depth, std::ofstream &of)
{
	bool newfile = !of.is_open();
	if (newfile)
	{
		of.clear();
		of.open((FilePath + "/" + FileName + ".json").c_str());
		if (!of)
		{
			throw std::runtime_error(std::string("can not open file ") + (FilePath + "/" + FileName + ".json"));
		}
	}

	std::vector<TEC_ZONE_LOG>::iterator zi = Zones.begin();
	for (std::vector<std::string>::iterator i = Json_Text.begin(); i != Json_Text.end(); ++i)
	{
		if (i->compare("#ZONE#") == 0)
		{
			zi->write_json(depth + 2, of);
			++zi;
		}
		else
		{
			for (int j = 0; j != depth; j++)
			{
				of << "\t";
			}
			of << i->c_str() << std::endl;
		}
	}

	if (newfile)
	{
		of.close();
	}
}

void TEC_FILE_LOG::write_xml(int depth, std::ofstream &of)
{
	bool newfile = !of.is_open();
	if (newfile)
	{
		of.clear();
		of.open((FilePath + "/" + FileName + ".xml").c_str());
		if (!of)
		{
			throw std::runtime_error(std::string("can not open file ") + (FilePath + "/" + FileName + ".xml"));
		}
		of << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
	}

	std::vector<TEC_ZONE_LOG>::iterator zi = Zones.begin();
	for (std::vector<std::string>::iterator i = Xml_Text.begin(); i != Xml_Text.end(); ++i)
	{
		if (i->compare("#ZONE#") == 0)
		{
			zi->write_xml(depth + 2, of);
			++zi;
		}
		else
		{
			for (int j = 0; j != depth; j++)
			{
				of << "\t";
			}
			of << i->c_str() << std::endl;
		}
	}

	if (newfile)
	{
		of.close();
	}
}

void TEC_FILE_LOG::gen_json()
{
	char buf[200];

	Json_Text.push_back("{");

	std::sprintf(buf, "\t\"FileName\" : \"%s\" ,", (FileName + ".plt").c_str()); Json_Text.push_back(buf);
	std::sprintf(buf, "\t\"FilePath\" : \"%s\" ,", FilePath.c_str()); Json_Text.push_back(buf);
	std::sprintf(buf, "\t\"Time\" : \"%s\" ,", Time_Begin.c_str()); Json_Text.push_back(buf);
	std::sprintf(buf, "\t\"UsingTime\" : \"%.5lf\" ,", UsingTime); Json_Text.push_back(buf);
	std::sprintf(buf, "\t\"Title\" : \"%s\" ,", Title.c_str()); Json_Text.push_back(buf);
	Json_Text.push_back("\t\"FileType_comment\" : \"0 = FULL, 1 = GRID, 2 = SOLUTION\" ,");
	std::sprintf(buf, "\t\"FileType\" : %i ,", FileType); Json_Text.push_back(buf);

	Json_Text.push_back("\t\"Variables\" : [ ");
	for (std::vector<std::string>::const_iterator i = Variables.begin(); i != Variables.end(); ++i)
	{
		std::sprintf(buf, "\"%s\"", i->c_str()); *(Json_Text.end() - 1) += buf;
		if (Variables.end() - i != 1)
		{
			*(Json_Text.end() - 1) += ", ";
		}
	}
	*(Json_Text.end() - 1) += " ] ,";

	if (Auxiliary.size() != 0)
	{
		Json_Text.push_back("\t\"Auxiliary\" : {");
		int j = 0;
		for (std::map<std::string, std::string>::const_iterator i = Auxiliary.begin(); i != Auxiliary.end(); ++i)
		{
			std::sprintf(buf, "\t\t\"%s\" : \"%s\"", i->first.c_str(), i->second.c_str()); Json_Text.push_back(buf);
			if (j != Auxiliary.size() - 1)
			{
				*(Json_Text.end() - 1) += ",";
			}
			++j;
		}
		Json_Text.push_back("\t} ,");
	}

	Json_Text.push_back("\t\"Zones\" : [");
	for (std::vector<TEC_ZONE_LOG>::iterator i = Zones.begin(); i != Zones.end(); ++i)
	{
		i->gen_json();
		Json_Text.push_back("#ZONE#");
		if (Zones.end() - i != 1)
		{
			Json_Text.push_back("\t\t,");
		}
	}
	Json_Text.push_back("\t]");
	Json_Text.push_back("}");
}

void TEC_FILE_LOG::gen_xml()
{
	char buf[200];

	std::sprintf(buf, "<File FileName=\"%s\">", FileName.c_str()); Xml_Text.push_back(buf);

	std::sprintf(buf, "\t<FileName>%s</FileName>", (FileName + ".plt").c_str()); Xml_Text.push_back(buf);
	std::sprintf(buf, "\t<FilePath>%s</FilePath>", FilePath.c_str()); Xml_Text.push_back(buf);
	std::sprintf(buf, "\t<Time>%s</Time>", Time_Begin.c_str()); Xml_Text.push_back(buf);
	std::sprintf(buf, "\t<UsingTime>%.5lf</UsingTime>", UsingTime); Xml_Text.push_back(buf);
	std::sprintf(buf, "\t<Title>%s</Title>", Title.c_str()); Xml_Text.push_back(buf);
	std::sprintf(buf, "\t<!--%s-->", "0 = FULL, 1 = GRID, 2 = SOLUTION"); Xml_Text.push_back(buf);
	std::sprintf(buf, "\t<FileType>%i</FileType>", FileType); Xml_Text.push_back(buf);

	Xml_Text.push_back("\t<Variables>");
	for (std::vector<std::string>::const_iterator i = Variables.begin(); i != Variables.end(); ++i)
	{
		std::sprintf(buf, " <I>%s</I>", i->c_str()); *(Xml_Text.end() - 1) += buf;
	}
	*(Xml_Text.end() - 1) += " </Variables>";

	if (Auxiliary.size() != 0)
	{
		Xml_Text.push_back("\t<Auxiliarys>");
		for (std::map<std::string, std::string>::const_iterator i = Auxiliary.begin(); i != Auxiliary.end(); ++i)
		{
			std::sprintf(buf, "\t\t<Auxiliary Name=\"%s\">%s</Auxiliary>", i->first.c_str(), i->second.c_str());
			Xml_Text.push_back(buf);
		}
		Xml_Text.push_back("\t</Auxiliarys>");
	}

	Xml_Text.push_back("\t<Zones>");
	for (std::vector<TEC_ZONE_LOG>::iterator i = Zones.begin(); i != Zones.end(); ++i)
	{
		i->gen_xml();
		Xml_Text.push_back("#ZONE#");
	}
	Xml_Text.push_back("\t</Zones>");

	Xml_Text.push_back("</File>");
}

TEC_ZONE_LOG::TEC_ZONE_LOG() {}

TEC_ZONE_LOG::TEC_ZONE_LOG(const TEC_ZONE & zone) : TEC_ZONE_BASE(zone) {}

void TEC_ZONE_LOG::write_echo(std::ofstream &of)
{
	bool newfile = !of.is_open();
	if (newfile)
	{
		of.clear();
		of.open(("./Zone_" + ZoneName + ".txt").c_str());
		if (!of)
		{
			throw std::runtime_error(std::string("can not open file ") + ("./Zone_" + ZoneName + ".txt"));
		}
	}

	for (std::vector<std::string>::iterator i = Echo_Text.begin(); i != Echo_Text.end(); ++i)
	{
		of << i->c_str() << std::endl;
	}

	if (newfile)
	{
		of.close();
	}
}

void TEC_ZONE_LOG::write_json(int depth, std::ofstream &of)
{
	bool newfile = !of.is_open();
	if (newfile)
	{
		of.clear();
		of.open(("./Zone_" + ZoneName + ".json").c_str());
		if (!of)
		{
			throw std::runtime_error(std::string("can not open file ") + ("./Zone_" + ZoneName + ".json"));
		}
	}

	for (std::vector<std::string>::iterator i = Json_Text.begin(); i != Json_Text.end(); ++i)
	{
		for (int j = 0; j != depth; j++)
		{
			of << "\t";
		}
		of << i->c_str() << std::endl;
	}

	if (newfile)
	{
		of.close();
	}
}

void TEC_ZONE_LOG::write_xml(int depth, std::ofstream &of)
{
	bool newfile = !of.is_open();
	if (newfile)
	{
		of.clear();
		of.open(("./Zone_" + ZoneName + ".xml").c_str());
		if (!of)
		{
			throw std::runtime_error(std::string("can not open file ") + ("./Zone_" + ZoneName + ".xml"));
		}
		of << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
	}

	for (std::vector<std::string>::iterator i = Xml_Text.begin(); i != Xml_Text.end(); ++i)
	{
		for (int j = 0; j != depth; j++)
		{
			of << "\t";
		}
		of << i->c_str() << std::endl;
	}

	if (newfile)
	{
		of.close();
	}
}

void TEC_ZONE_LOG::gen_json()
{
	char buf[200];

	Json_Text.push_back("{");

	std::sprintf(buf, "\t\"ZoneName\" : \"%s\" ,", ZoneName.c_str()); Json_Text.push_back(buf);
	std::sprintf(buf, "\t\"StrandId\" : %i ,", StrandId); Json_Text.push_back(buf);
	if (StrandId != -1)
	{
		std::sprintf(buf, "\t\"SolutionTime\" : %le ,", SolutionTime); Json_Text.push_back(buf);
	}
	std::sprintf(buf, "\t\"Real_Dim\" : %i ,", Real_Dim); Json_Text.push_back(buf);
	std::sprintf(buf, "\t\"Org_Max\" : [ %i, %i, %i ] ,", Max[0], Max[1], Max[2]); Json_Text.push_back(buf);
	std::sprintf(buf, "\t\"Skip\" : [ %i, %i, %i ] ,", Skip[0], Skip[1], Skip[2]); Json_Text.push_back(buf);
	std::sprintf(buf, "\t\"Begin\" : [ %i, %i, %i ] ,", Begin[0], Begin[1], Begin[2]); Json_Text.push_back(buf);
	std::sprintf(buf, "\t\"End\" : [ %i, %i, %i ] ,", End[0], End[1], End[2]); Json_Text.push_back(buf);
	std::sprintf(buf, "\t\"Real_Max\" : [ %i, %i, %i ] ,", Real_Max[0], Real_Max[1], Real_Max[2]); Json_Text.push_back(buf);

	if (Auxiliary.size() != 0)
	{
		Json_Text.push_back("\t\"Auxiliary\" : {");
		int j = 0;
		for (std::map<std::string, std::string>::const_iterator i = Auxiliary.begin(); i != Auxiliary.end(); ++i)
		{
			std::sprintf(buf, "\t\t\"%s\" : \"%s\"", i->first.c_str(), i->second.c_str()); Json_Text.push_back(buf);
			if (j != Auxiliary.size() - 1)
			{
				*(Json_Text.end() - 1) += ",";
			}
			++j;
		}
		Json_Text.push_back("\t} ,");
	}

	Json_Text.push_back("\t\"Data_type_comment\" : \"1=Float, 2=Double, 3=LongInt, 4=ShortInt, 5=Byte, 6=Bit\" ,");
	Json_Text.push_back("\t\"Data\" : [");
	for (std::vector<TEC_DATA_LOG>::const_iterator i = Data.begin(); i != Data.end(); ++i)
	{
		std::sprintf(buf, "\t\t{ \"type\":%i, \"size_i\":%zi, \"file_pt\":%li }", i->type, i->size, Data[i - Data.begin()].file_pt); Json_Text.push_back(buf);
		if (Data.end() - i != 1)
		{
			*(Json_Text.end() - 1) += ",";
		}
	}
	Json_Text.push_back("\t]");

	Json_Text.push_back("}");
}

void TEC_ZONE_LOG::gen_xml()
{
	char buf[200];

	std::sprintf(buf, "<Zone ZoneName=\"%s\">", ZoneName.c_str()); Xml_Text.push_back(buf);

	std::sprintf(buf, "\t<ZoneName>%s</ZoneName>", ZoneName.c_str()); Xml_Text.push_back(buf);
	std::sprintf(buf, "\t<StrandId>%i</StrandId>", StrandId); Xml_Text.push_back(buf);
	if (StrandId != -1)
	{
		std::sprintf(buf, "\t<SolutionTime>%le</SolutionTime>", SolutionTime); Xml_Text.push_back(buf);
	}
	std::sprintf(buf, "\t<Real_Dim>%i</Real_Dim>", Real_Dim); Xml_Text.push_back(buf);
	std::sprintf(buf, "\t<Org_Max> <I>%i</I> <I>%i</I> <I>%i</I> </Org_Max>", Max[0], Max[1], Max[2]); Xml_Text.push_back(buf);
	std::sprintf(buf, "\t<Skip> <I>%i</I> <I>%i</I> <I>%i</I> </Skip>", Skip[0], Skip[1], Skip[2]); Xml_Text.push_back(buf);
	std::sprintf(buf, "\t<Begin> <I>%i</I> <I>%i</I> <I>%i</I> </Begin>", Begin[0], Begin[1], Begin[2]); Xml_Text.push_back(buf);
	std::sprintf(buf, "\t<End> <I>%i</I> <I>%i</I> <I>%i</I> </End>", End[0], End[1], End[2]); Xml_Text.push_back(buf);
	std::sprintf(buf, "\t<Real_Max> <I>%i</I> <I>%i</I> <I>%i</I> </Real_Max>", Real_Max[0], Real_Max[1], Real_Max[2]); Xml_Text.push_back(buf);

	if (Auxiliary.size() != 0)
	{
		Xml_Text.push_back("\t<Auxiliary>");
		for (std::map<std::string, std::string>::const_iterator i = Auxiliary.begin(); i != Auxiliary.end(); ++i)
		{
			std::sprintf(buf, "\t\t<Auxiliary Name=\"%s\">%s</Auxiliary>", i->first.c_str(), i->second.c_str());
			Xml_Text.push_back(buf);
		}
		Xml_Text.push_back("\t</Auxiliary>");
	}

	Xml_Text.push_back("\t<!--1=Float, 2=Double, 3=LongInt, 4=ShortInt, 5=Byte, 6=Bit-->");
	Xml_Text.push_back("\t<Datas>");
	for (std::vector<TEC_DATA_LOG>::iterator i = Data.begin(); i != Data.end(); ++i)
	{
		std::sprintf(buf, "\t\t<Data type=\"%i\" size_i=\"%zi\" file_pt=\"%li\"/>", i->type, i->size, Data[i - Data.begin()].file_pt);
		Xml_Text.push_back(buf);
	}
	Xml_Text.push_back("\t</Datas>");

	Xml_Text.push_back("</Zone>");
}

TEC_DATA_LOG::TEC_DATA_LOG() {}

TEC_DATA_LOG::TEC_DATA_LOG(const TEC_DATA & data) : TEC_DATA_BASE(data) {}

TEC_FILE::TEC_FILE(const std::string &name, const std::string &path, const std::string &title)
{
	FilePath = path;
	FileName = name;
	FileType = 0;
	Title = title;

	echo_mode();
}

bool TEC_FILE::add_auxiliary_data(const std::string &name,const std::string &value)
{
	std::pair<std::map<std::string,std::string>::iterator,bool> ans;
	std::pair<std::string,std::string> temp(name,value);
	ans=Auxiliary.insert(temp);
	return ans.second;
}

bool TEC_FILE::add_auxiliary_data(const std::string &name,const double &value)
{
	std::ostringstream ss;
	ss << value;
	return add_auxiliary_data(name,ss.str());
}

void TEC_FILE::set_echo_mode(const std::string &file, const std::string &zone)
{
	if (file.compare("leave") != 0)
	{
		echo_mode(file);
	}
	if (zone.compare("leave") != 0)
	{
		for (std::vector<TEC_ZONE>::iterator i = Zones.begin(); i != Zones.end(); ++i)
		{
			i->echo_mode(zone);
		}
	}
}

void TEC_FILE::write_plt(bool echo)
{
	last_log = TEC_FILE_LOG(*this);

	clock_t clock_begin = clock();
	last_log.Time_Begin = get_time();

	wrtie_plt_pre();

	FILE *of;
	try
	{
		of = openfile(FilePath + "/" + FileName + ".plt", "wb");
	}
	catch (std::runtime_error &err)
	{
		last_log.Error = err.what();
		throw std::runtime_error(err);
	}
	if (Echo_Mode.test(0))
	{
		if (Echo_Mode.test(5))
		{
			if (echo) std::printf("[%s] ", last_log.Time_Begin.c_str());
			last_log.Echo_Text.push_back("[" + last_log.Time_Begin + "] ");
		}
		if (echo) std::printf("#### creat file %s/%s.plt ####\n", FilePath.c_str(), FileName.c_str());
		if (Echo_Mode.test(5))
		{
			*(last_log.Echo_Text.end() - 1) += "#### creat file " + FilePath + "/" + FileName + ".plt ####";
		}
		else
		{
			last_log.Echo_Text.push_back("#### creat file " + FilePath + "/" + FileName + ".plt ####");
		}
	}

	//I    HEADER SECTION
	write_plt_head(of, echo);

	//EOHMARKER, value=357.0
	W_FLOAT32(357.0f, of);
	if (Echo_Mode.test(3))
	{
		if (echo) std::printf("-------------------------------------\n");
		last_log.Echo_Text.push_back("-------------------------------------");
	}

	//II   DATA SECTION
	write_plt_data(of, echo);

	long int pos = std::ftell(of);
	double s_f = double(pos) / 1024 / 1024;
	last_log.Size = s_f;
	if (Echo_Mode.test(4))
	{
		char buf[100];
		std::sprintf(buf, "     file size: %.1lf MB", s_f);
		if (echo) std::printf("%s\n", buf);
		last_log.Echo_Text.push_back(buf);
	}

	fclose(of);

	last_log.UsingTime = double(clock() - clock_begin) / CLOCKS_PER_SEC;
	if (Echo_Mode.test(6))
	{
		char buf[100];
		std::sprintf(buf, "     using time : %.5lf s", last_log.UsingTime);
		if (echo) std::printf("%s\n", buf);
		last_log.Echo_Text.push_back(buf);
	}
	
	last_log.Time_End = get_time();
	if (Echo_Mode.test(1))
	{
		if (Echo_Mode.test(5))
		{
			if (echo) std::printf("[%s] ", last_log.Time_End.c_str());
			last_log.Echo_Text.push_back("[" + last_log.Time_End + "] ");
		}
		if (echo) std::printf("#### creat file %s/%s.plt ####\n", FilePath.c_str(), FileName.c_str());
		if (Echo_Mode.test(5))
		{
			*(last_log.Echo_Text.end() - 1) += "#### creat file " + FilePath + "/" + FileName + ".plt ####";
		}
		else
		{
			last_log.Echo_Text.push_back("#### creat file " + FilePath + "/" + FileName + ".plt ####");
		}
	}

	last_log.gen_json();
	last_log.gen_xml();
}

void TEC_FILE::echo_mode(const std::string &iecho)
{
	std::string e_m = iecho;
	if (e_m.compare("brief")==0)
	{
		e_m = "0100111";
	}
	else if (iecho.compare("full")==0)
	{
		e_m = "1111111";
	}
	else if (iecho.compare("simple")==0)
	{
		e_m = "0100001";
	}
	else if (iecho.compare("none")==0)
	{
		e_m = "0000000";
	}
	try
	{
		Echo_Mode = std::bitset<7>(e_m);
	}
	catch (...)
	{
		throw std::runtime_error("File(" + FileName + "): echo code wrong");
	}
}

void TEC_FILE::wrtie_plt_pre()
{
	if (Variables.size() == 0)
	{
		last_log.Error = "File(" + FileName + "): Variables is empty";
		throw std::runtime_error("File(" + FileName + "): Variables is empty");
	}
	if (Zones.size() == 0)
	{
		last_log.Error = "File(" + FileName + "): Zones is empty";
		throw std::runtime_error("File(" + FileName + "): Zones is empty");
	}
	for (std::vector<TEC_ZONE>::iterator i = Zones.begin(); i != Zones.end(); ++i)
	{
		try
		{
			last_log.Zones.push_back(TEC_ZONE_LOG(*i));
			i->wrtie_plt_pre(*this, *(last_log.Zones.end()-1));
		}
		catch (std::runtime_error &err)
		{
			last_log.Error = "File(" + FileName + ")." + err.what();
			throw std::runtime_error("File(" + FileName + ")." + err.what());
		}
	}
}

void TEC_FILE::write_plt_head(FILE *of, bool echo)
{
	//I    HEADER SECTION
	//i    Magic number, Version number
	fwrite("#!TDV112", sizeof(char), 8, of);//8 Bytes, exact characters "#!TDV112". Version number follows the "V" and consumes the next 3 characters (for example: "V75", "V101")
	//ii   Integer value of 1
	W_INT32(1, of);//This is used to determine the byte order of the reader, relative to the writer
	//iii  Title and variable names
	W_INT32(FileType, of);//FileType 0 = FULL, 1 = GRID, 2 = SOLUTION
	W_STRING(Title, of);//The TITLE
	W_INT32(Variables.size(), of);//Number of variables (NumVar) in the datafile
	if (Echo_Mode.test(2))
	{
		if (echo) std::printf("     VAR:");
		last_log.Echo_Text.push_back("     VAR:");
	}
	for (std::vector<std::string>::const_iterator i = Variables.begin(); i != Variables.end(); ++i)
	{
		W_STRING(*i, of);//Variable names

		if (Echo_Mode.test(2))
		{
			if (echo) std::printf(" <%s>", i->c_str());
			*(last_log.Echo_Text.end() - 1) += " <" + *i + ">";
		}
	}
	if (Echo_Mode.test(2))
	{
		if (echo) std::printf("\n");
	}
	//iv   Zones
	for (std::vector<TEC_ZONE>::const_iterator i = Zones.begin(); i != Zones.end(); ++i)
	{
		i->write_plt_head(of);
	}
	//ix Dataset Auxiliary data
	for (std::map<std::string, std::string>::const_iterator i = Auxiliary.begin(); i != Auxiliary.end(); ++i)
	{
		W_FLOAT32(799.0f, of);//DataSetAux Marker
		W_STRING(i->first, of);//Text for Auxiliary "Name"
		W_INT32(0, of);//Auxiliary Value Format (Currently only allow 0=AuxDataType_String)
		W_STRING(i->second, of);//Text for Auxiliary "Value"
	}
}

void TEC_FILE::write_plt_data(FILE *of, bool echo)
{
	//II   DATA SECTION
	//i    For both ordered and fe zones
	for (std::vector<TEC_ZONE>::iterator i = Zones.begin(); i != Zones.end(); ++i)
	{
		if (i->Echo_Mode.test(0))
		{
			char buf[100];
			std::sprintf(buf, "--   write zone %i: %s   --", int(i - Zones.begin() + 1), i->ZoneName.c_str());
			if (echo) std::printf("%s\n", buf);
			last_log.Echo_Text.push_back(buf);
		}

		i->write_plt_data(of, *this, last_log.Zones[i- Zones.begin()], echo);
		last_log.Echo_Text.push_back("#ZONE#");

		if (i->Echo_Mode.test(1))
		{
			char buf[100];
			std::sprintf(buf, "--   write zone %i: %s   --", int(i - Zones.begin() + 1), i->ZoneName.c_str());
			if (echo) std::printf("%s\n", buf);
			last_log.Echo_Text.push_back(buf);
		}
	}
}

TEC_ZONE::TEC_ZONE(const std::string &name)
{
	ZoneName = name;
	StrandId = -1;
	SolutionTime = 0.0f;
	Max[0] = 1;
	Max[1] = 1;
	Max[2] = 1;
	Skip[0] = 1;
	Skip[1] = 1;
	Skip[2] = 1;
	Begin[0] = 0;
	End[0] = 0;
	Begin[1] = 0;
	End[1] = 0;
	Begin[2] = 0;
	End[2] = 0;
	noskip = true;
	noexc = true;
	needreal = false;
	echo_mode();
}

const INT32 * TEC_ZONE::get_real_size(const std::string &name)
{
	gather_real_size();
	if (name.compare("realmax") == 0)
	{
		return Real_Max;
	}
	else if (name.compare("realdim") == 0)
	{
		return &Real_Dim;
	}
	else
	{
		throw std::out_of_range("get_real_size : size code wrong");
	}
}

bool TEC_ZONE::add_auxiliary_data(const std::string &name, const std::string &value)
{
	std::pair<std::map<std::string,std::string>::iterator,bool> ans;
	std::pair<std::string,std::string> temp(name,value);
	ans=Auxiliary.insert(temp);
	return ans.second;
}

bool TEC_ZONE::add_auxiliary_data(const std::string &name, const double &value)
{
	std::stringstream ss;
	ss<<value;
	return add_auxiliary_data(name,ss.str());
}

void TEC_ZONE::echo_mode(const std::string &iecho)
{
	std::string e_m = iecho;
	if (e_m.compare("brief")==0)
	{
		e_m = "000001001";
	}
	else if (e_m.compare("full")==0)
	{
		e_m = "111111111";
	}
	else if (e_m.compare("simple")==0)
	{
		e_m = "000000001";
	}
	else if (e_m.compare("none")==0)
	{
		e_m = "000000000";
	}
	try
	{
		Echo_Mode = std::bitset<9>(e_m);
	}
	catch (...)
	{
		throw std::runtime_error("Zone(" + ZoneName + "): echo code wrong");
	}
}

void TEC_ZONE::gather_real_size()
{
	for (int i = 0; i != 3; ++i)
	{
		char index[3] = { 'I','J','K' };
		if (Max[i] == 0)
		{
			throw std::runtime_error("Zone(" + ZoneName + "): " + index[i] + "Max connot be zero");
		}
		if (Skip[i] == 0)
		{
			throw std::runtime_error("Zone(" + ZoneName + "): " + index[i] + "Skip must be positive");
		}
		if (Begin[i] + End[i] >= Max[i])
		{
			throw std::runtime_error("Zone(" + ZoneName + "): sum of " + index[i] + "Begin and " + index[i] + "End is not smaller than " + index[i] + "Max");
		}
	}

	Dim = 3;
	for (int i = 2; i != -1; --i)
	{
		if (Max[i] == 1)
		{
			--Dim;
		}
	}

	Real_Dim = 3;
	noskip = true;
	noexc = true;
	for (int i = 2; i != -1; --i)
	{
		Real_Max[i] = (Max[i] - Begin[i] - End[i]) / Skip[i];
		if ((Max[i] - Begin[i] - End[i]) % Skip[i])
		{
			++Real_Max[i];
		}
		if (Real_Max[i] == 1)
		{
			--Real_Dim;
		}
		noskip = noskip && Skip[i]==1;
		noexc = noexc && Begin[i] == 0 && End[i] == 0;
	}
}

void TEC_ZONE::make_buf()
{
	if(noskip && noexc)
	{
		for(std::vector<TEC_DATA>::iterator i=Data.begin();i!=Data.end();++i)
		{
			i->buf=(byte *)i->DataP;
		}
		needreal = false;
	}
	else if(noskip)
	{
		for(std::vector<TEC_DATA>::iterator i=Data.begin();i!=Data.end();++i)
		{
			size_t size=i->size;
			byte * datap=(byte *)i->DataP;
			try
			{
				i->buf = new byte[Real_Max[0]*Real_Max[1]*Real_Max[2]*size];
			}
			catch (...)
			{
				throw std::runtime_error("out of memory");
			}
			for(INT32 sk=Begin[2];sk<Max[2]-End[2];sk+=Skip[2])
			{
				for(INT32 sj=Begin[1];sj<Max[1]-End[1];sj+=Skip[1])
				{
					std::memcpy(i->buf+(Real_Max[0]*(sj-Begin[1])/Skip[1]+Real_Max[0]*Real_Max[1]*(sk-Begin[2])/Skip[2])*size,
						        datap+(Begin[0]+Max[0]*sj+Max[0]*Max[1]*sk)*size, Real_Max[0]*size);
				}
			}
		}
		needreal = true;
	}
	else
	{
		for(std::vector<TEC_DATA>::iterator i=Data.begin();i!=Data.end();++i)
		{
			size_t size=i->size;
			byte * datap=(byte *)i->DataP;
			try
			{
				i->buf = new byte[Real_Max[0]*Real_Max[1]*Real_Max[2]*size];
			}
			catch (...)
			{
				throw std::runtime_error("out of memory");
			}
			for(INT32 sk=Begin[2];sk<Max[2]-End[2];sk+=Skip[2])
			{
				for(INT32 sj=Begin[1];sj<Max[1]-End[1];sj+=Skip[1])
				{
					for(INT32 si=Begin[0];si<Max[0]-End[0];si+=Skip[0])
					{
						std::memcpy(i->buf+((si-Begin[0])/Skip[0]+Real_Max[0]*(sj-Begin[1])/Skip[1]+Real_Max[0]*Real_Max[1]*(sk-Begin[2])/Skip[2])*size,
							        datap+(si+Max[0]*sj+Max[0]*Max[1]*sk)*size, size);
					}
				}
			}
		}
		needreal = true;
	}
}

void TEC_ZONE::realise_buf()
{
	if (needreal)
	{
		for (std::vector<TEC_DATA>::iterator i = Data.begin(); i != Data.end(); ++i)
		{
			delete[] i->buf;
		}
		needreal = false;
	}
	else
	{
		for (std::vector<TEC_DATA>::iterator i = Data.begin(); i != Data.end(); ++i)
		{
			i->buf = NULL;
		}
	}
}

void TEC_ZONE::wrtie_plt_pre(const TEC_FILE &thisfile, TEC_ZONE_LOG &zone_log)
{
	gather_real_size();
	zone_log.Dim = Dim;
	zone_log.Real_Dim = Real_Dim;
	zone_log.Real_Max[0] = Real_Max[0];
	zone_log.Real_Max[1] = Real_Max[1];
	zone_log.Real_Max[2] = Real_Max[2];
	zone_log.noskip = noskip;
	zone_log.noexc = noexc;
	
	if (Data.size() == 0)
	{
		throw std::runtime_error("Zone(" + ZoneName + "): Data is empty");
	}
	if (Data.size() != thisfile.Variables.size())
	{
		throw std::runtime_error("Zone(" + ZoneName + "): the size of Data is not equal to the size of tec_file.Variables");
	}
	for (std::vector<TEC_DATA>::const_iterator i = Data.begin(); i != Data.end(); ++i)
	{
		zone_log.Data.push_back(TEC_DATA_LOG(*i));
		if (i->DataP == NULL || i->type == 0 || i->size == 0)
		{
			throw std::runtime_error("Zone(" + ZoneName + ").Variable(" + thisfile.Variables[i - Data.begin()] + "): Data is unset");
		}
	}
}

void TEC_ZONE::write_plt_head(FILE *of) const
{
	W_FLOAT32(299.0f, of);//Zone marker. Value = 299.0
	W_STRING(ZoneName, of);//Zone name
	W_INT32(-1, of);//ParentZone
	W_INT32(StrandId, of);//StrandID
	W_FLOAT64(SolutionTime, of);//Solution time
	W_INT32(-1, of);//Not used. Set to -1
	W_INT32(0, of);//ZoneType 0=ORDERED
	W_INT32(0, of);//Specify Var Location. 0 = Do not specify, all data is located at the nodes
	W_INT32(0, of);//Are raw local 1-to-1 face neighbors supplied? (0=FALSE 1=TRUE) ORDERED and FELINESEG zones must specify 0
	W_INT32(0, of);//Number of miscellaneous user-defined face neighbor connections (value >= 0)
	W_INT32(Real_Max[0], of);//Max[0]
	W_INT32(Real_Max[1], of);//Max[1]
	W_INT32(Real_Max[2], of);//Max[2]
	for (std::map<std::string, std::string>::const_iterator j = Auxiliary.begin(); j != Auxiliary.end(); ++j)
	{
		W_INT32(1, of);//Auxiliary name/value pair to follow
		W_STRING(j->first, of);//name string
		W_INT32(0, of);//Auxiliary Value Format (Currently only allow 0=AuxDataType_String)
		W_STRING(j->second, of);//Value string
	}
	W_INT32(0, of);//No more Auxiliary name/value pairs
}

void TEC_ZONE::write_plt_data(FILE *of, const TEC_FILE &thisfile, TEC_ZONE_LOG &zone_log, bool echo)
{
	long int pos_b, pos_e;
	pos_b = std::ftell(of);
	W_FLOAT32(299.0f, of);//Zone marker Value = 299.0
	for (std::vector<TEC_DATA>::const_iterator j = Data.begin(); j != Data.end(); ++j)
	{
		W_INT32(j->type, of);//Variable data format
	}
	W_INT32(0, of);//Has passive variables: 0 = no
	W_INT32(0, of);//Has variable sharing 0 = no
	W_INT32(-1, of);//Zero based zone number to share connectivity list with (-1 = no sharing)

	make_buf();

	for (std::vector<TEC_DATA>::const_iterator j = Data.begin(); j != Data.end(); ++j)
	{
		std::pair<FLOAT64, FLOAT64> mm = j->minmax(Real_Max[0]*Real_Max[1]*Real_Max[2]);
		W_FLOAT64(mm.first, of);//Min value
		W_FLOAT64(mm.second, of);//Max value
	}

	if (Echo_Mode.test(3))
	{
		char buf[100];
		std::sprintf(buf, "     Dim = %i   Real_Max = [", Real_Dim);
		if (echo) std::printf("%s", buf);
		zone_log.Echo_Text.push_back(buf);
		for (int dd = 0; dd != Real_Dim; ++dd)
		{
			std::sprintf(buf, " %i", Real_Max[dd]);
			if (echo) std::printf("%s", buf);
			*(zone_log.Echo_Text.end() - 1) += buf;
		}
		if (echo) std::printf(" ]\n");
		*(zone_log.Echo_Text.end() - 1) += " ]";
	}
	if (Echo_Mode.test(4))
	{
		char buf[100];
		if (echo) std::printf("     Org_Max = [");
		zone_log.Echo_Text.push_back("     Org_Max = [");
		for (int dd = 0; dd != 3; ++dd)
		{
			std::sprintf(buf, " %i", Max[dd]);
			if (echo) std::printf("%s", buf);
			*(zone_log.Echo_Text.end() - 1) += buf;
		}
		if (echo) std::printf(" ]\n");
		*(zone_log.Echo_Text.end() - 1) += " ]";
	}
	if (Echo_Mode.test(5))
	{
		char buf[100];
		if (echo) std::printf("     Skip = [");
		zone_log.Echo_Text.push_back("     Skip = [");
		for (int dd = 0; dd != 3; ++dd)
		{
			std::sprintf(buf, " %i", Skip[dd]);
			if (echo) std::printf("%s", buf);
			*(zone_log.Echo_Text.end() - 1) += buf;
		}
		if (echo) std::printf(" ]\n");
		*(zone_log.Echo_Text.end() - 1) += " ]";
	}
	if (Echo_Mode.test(6))
	{
		char buf[100];
		if (echo) std::printf("     Begin = [");
		zone_log.Echo_Text.push_back("     Begin = [");
		for (int dd = 0; dd != 3; ++dd)
		{
			std::sprintf(buf, " %i", Begin[dd]);
			if (echo) std::printf("%s", buf);
			*(zone_log.Echo_Text.end() - 1) += buf;
		}
		if (echo) std::printf(" ]");
		*(zone_log.Echo_Text.end() - 1) += " ]";

		if (echo) std::printf("   End = [");
		*(zone_log.Echo_Text.end() - 1) += " End = [";
		for (int dd = 0; dd != 3; ++dd)
		{
			std::sprintf(buf, " %i", End[dd]);
			if (echo) std::printf("%s", buf);
			*(zone_log.Echo_Text.end() - 1) += buf;
		}
		if (echo) std::printf(" ]\n");
		*(zone_log.Echo_Text.end() - 1) += " ]";
	}
	if (Echo_Mode.test(7) && StrandId != -1)
	{
		char buf[100];
		std::sprintf(buf, "     StrandId = %i SolutionTime = %le", StrandId, SolutionTime);
		if (echo) std::printf("%s\n", buf);
		zone_log.Echo_Text.push_back(buf);
	}

	if (Echo_Mode.test(2))
	{
		if (echo) std::printf("     write variables:");
		zone_log.Echo_Text.push_back("     write variables:");
	}
	for (std::vector<TEC_DATA>::iterator j = Data.begin(); j != Data.end(); ++j)
	{
		zone_log.Data[j- Data.begin()].file_pt = std::ftell(of);
		fwrite((const byte*)(j->buf), j->size, Real_Max[0]*Real_Max[1]*Real_Max[2], of);//Zone Data. Each variable is in data format as specified above
		if (Echo_Mode.test(2))
		{
			if (echo) std::printf(" <%s>", thisfile.Variables[j - Data.begin()].c_str());
			*(zone_log.Echo_Text.end() - 1) += (" <" + thisfile.Variables[j - Data.begin()] + ">");
		}
	}
	if (Echo_Mode.test(2))
	{
		if (echo) std::printf("\n");
	}

	realise_buf();

	pos_e = std::ftell(of);
	double s_z = double(pos_e - pos_b) / 1024 / 1024;
	zone_log.Size = s_z;
	if (Echo_Mode.test(8))
	{
		char buf[100];
		std::sprintf(buf, "     zone size: %.1lfMB", s_z);
		if (echo) std::printf("%s\n", buf);
		zone_log.Echo_Text.push_back(buf);
	}
}

TEC_DATA::TEC_DATA()
{
	DataP = NULL;
	type = TEC_NULL;
	size = 0;
	buf = NULL;
}

template<typename T>
void get_minmax(const T *data, size_t N, T &min, T &max)
{
	min = data[0];
	max = data[0];
	T loc_min, loc_max;
	size_t n;
	if (N % 2)
	{
		n = (N - 1) / 2;
	}
	else
	{
		n = N / 2;
	}
	for (size_t i = 0; i != n; ++i)
	{
		if (data[2 * i]>data[2 * i + 1])
		{
			loc_min = data[2 * i + 1];
			loc_max = data[2 * i];
		}
		else
		{
			loc_min = data[2 * i];
			loc_max = data[2 * i + 1];
		}
		if (loc_min<min)
		{
			min = loc_min;
		}
		if (loc_max>max)
		{
			max = loc_max;
		}
	}
	if (N % 2)
	{
		if (data[N - 1]<min)
		{
			min = data[N - 1];
		}
		else if (data[N - 1]>max)
		{
			max = data[N - 1];
		}
	}
}

std::pair<FLOAT64, FLOAT64> TEC_DATA::minmax(size_t N) const
{
	std::pair<FLOAT64, FLOAT64> ans;
	switch (type)
	{
	case TEC_DATA::TEC_FLOAT:
	{
		FLOAT32 min, max;
		FLOAT32 *data = (FLOAT32 *)buf;
		get_minmax<FLOAT32>(data, N, min, max);
		ans.first = min;
		ans.second = max;
		break;
	}
	case TEC_DATA::TEC_DOUBLE:
	{
		FLOAT64 min, max;
		FLOAT64 *data = (FLOAT64 *)buf;
		get_minmax<FLOAT64>(data, N, min, max);
		ans.first = min;
		ans.second = max;
		break;
	}
	case TEC_DATA::TEC_LONGINT:
	{
		longint min, max;
		longint *data = (longint *)buf;
		get_minmax<longint>(data, N, min, max);
		ans.first = min;
		ans.second = max;
		break;
	}
	case TEC_DATA::TEC_SHORTINT:
	{
		shortint min, max;
		shortint *data = (shortint *)buf;
		get_minmax<shortint>(data, N, min, max);
		ans.first = min;
		ans.second = max;
		break;
	}
	case TEC_DATA::TEC_BYTE:
	{
		byte min, max;
		byte *data = (byte *)buf;
		get_minmax<byte>(data, N, min, max);
		ans.first = min;
		ans.second = max;
		break;
	}
	default:
	{
		throw std::runtime_error("data type error");
	}
	}
	return ans;
}
