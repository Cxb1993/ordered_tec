# include "ordered_tec.h"

# include <string>
# include <vector>
# include <map>
# include <cstring>
# include <stdexcept>
# include <iostream>
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
	struct tm *tm_c = std::localtime(&time_c);
	char buf[100];
	std::strftime(buf, 100, format.c_str(), tm_c);
	return buf;
}

TEC_FILE::TEC_FILE(const std::string &name, const std::string &path, const std::string &title)
{
	FilePath = path;
	FileName = name;
	FileType = 0;
	Title = title;

	echo_mode();

	Json_Depth = 0;
	Json_WriteFile = true;
	Json_File = NULL;

	Xml_Depth = 0;
	Xml_WriteFile = true;
	Xml_File = NULL;
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

void TEC_FILE::write_plt(std::ostream &echo)
{
	clock_t clock_begin = clock();
	Time = get_time();

	wrtie_plt_pre();

	std::ios::sync_with_stdio(false);

	FILE *of;
# ifdef __linux__
	of = fopen((FilePath + "/" + FileName + ".plt").c_str(), "wb");
	if (of == NULL)
	{
		throw std::runtime_error(std::string("cannot open file ") + (FileName + ".plt"));
	}
# else
	errno_t err = fopen_s(&of, (FilePath + "/" + FileName + ".plt").c_str(), "wb");
	if (err != 0)
	{
		throw std::runtime_error(std::string("cannot open file ") + (FileName + ".plt"));
	}
# endif
	if (Echo_Mode.test(0))
	{
		if (Echo_Mode.test(5))
		{
			echo << "[" << Time << "] ";
		}
		echo << "#### creat file " << FilePath + "/" + FileName << ".plt ####" << std::endl;
	}

	//I    HEADER SECTION
	write_plt_filehead(of, echo);

	//EOHMARKER, value=357.0
	W_FLOAT32(357.0f, of);

	if (Echo_Mode.test(3))
	{
		echo << "-------------------------------------" << std::endl;
	}

	//II   DATA SECTION
	write_plt_data(of, echo);

	if (Echo_Mode.test(4))
	{
		double s_f = double(ftell(of)) / 1024 / 1024;
		char buf[100];
		std::sprintf(buf, "     file size: %.1lfMB", s_f);
		echo << buf << std::endl;
	}

	fclose(of);

	UsingTime = double(clock() - clock_begin) / CLOCKS_PER_SEC;

	if (Echo_Mode.test(6))
	{
		char buf[100];
		std::sprintf(buf, "     using time : %.5lf s", UsingTime);
		echo << buf << std::endl;
	}
	
	if (Echo_Mode.test(1))
	{
		if (Echo_Mode.test(5))
		{
			echo << "[" << get_time() << "] ";
		}
		echo << "#### creat file " << FilePath + "/" + FileName << ".plt ####" << std::endl;
	}

	Json_Text.clear();
	log_json();
	Xml_Text.clear();
	log_xml();
	write_log();

	std::ios::sync_with_stdio(true);
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
		throw std::runtime_error("File(" + FileName + "): Variables is empty");
	}
	if (Zones.size() == 0)
	{
		throw std::runtime_error("File(" + FileName + "): Zones is empty");
	}
	for (std::vector<TEC_ZONE>::iterator i = Zones.begin(); i != Zones.end(); ++i)
	{
		try
		{
			i->wrtie_plt_pre_zone(*this);
		}
		catch (std::runtime_error &err)
		{
			throw std::runtime_error("File(" + FileName + ")." + err.what());
		}
	}
}

void TEC_FILE::write_plt_filehead(FILE *of, std::ostream &echo)
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
		echo << "     VAR: ";
	}
	for (std::vector<std::string>::const_iterator i = Variables.begin(); i != Variables.end(); ++i)
	{
		W_STRING(*i, of);//Variable names

		if (Echo_Mode.test(2))
		{
			echo << "<" << *i << "> ";
		}
	}
	if (Echo_Mode.test(2))
	{
		echo << std::endl;
	}
	//iv   Zones
	for (std::vector<TEC_ZONE>::const_iterator i = Zones.begin(); i != Zones.end(); ++i)
	{
		i->write_plt_zonehead(of);
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

void TEC_FILE::write_plt_data(FILE *of, std::ostream &echo)
{
	//II   DATA SECTION
	//i    For both ordered and fe zones
	for (std::vector<TEC_ZONE>::iterator i = Zones.begin(); i != Zones.end(); ++i)
	{
		if (i->Echo_Mode.test(0))
		{
			char buf[100];
			std::sprintf(buf, "--   write zone %i: %s   --", int(i - Zones.begin() + 1), i->ZoneName.c_str());
			echo << buf << std::endl;
		}

		i->write_plt_zonedata(of, *this, echo);

		if (i->Echo_Mode.test(1))
		{
			char buf[100];
			std::sprintf(buf, "--   write zone %i: %s   --", int(i - Zones.begin() + 1), i->ZoneName.c_str());
			echo << buf << std::endl;
		}
	}
}

void TEC_FILE::log_json()
{
	char buf[200];

	for (int ii = 0; ii != Json_Depth; ++ii) { Json_Text += '\t'; }
	Json_Text += "{\n";

	for (int ii = 0; ii != Json_Depth + 2; ++ii) { Json_Text += '\t'; }
	std::sprintf(buf, "\"FileName\" : \"%s\" ,\n", (FileName + ".plt").c_str()); Json_Text += buf;
	for (int ii = 0; ii != Json_Depth + 2; ++ii) { Json_Text += '\t'; }
	std::sprintf(buf, "\"FilePath\" : \"%s\" ,\n", FilePath.c_str()); Json_Text += buf;
	for (int ii = 0; ii != Json_Depth + 2; ++ii) { Json_Text += '\t'; }
	std::sprintf(buf, "\"Time\" : \"%s\" ,\n", Time.c_str()); Json_Text += buf;
	for (int ii = 0; ii != Json_Depth + 2; ++ii) { Json_Text += '\t'; }
	std::sprintf(buf, "\"UsingTime\" : \"%.5lf\" ,\n", UsingTime); Json_Text += buf;
	for (int ii = 0; ii != Json_Depth + 2; ++ii) { Json_Text += '\t'; }
	std::sprintf(buf, "\"Title\" : \"%s\" ,\n", Title.c_str()); Json_Text += buf;
	for (int ii = 0; ii != Json_Depth + 2; ++ii) { Json_Text += '\t'; }
	Json_Text += "\"FileType_comment\" : \"0 = FULL, 1 = GRID, 2 = SOLUTION\" ,\n";
	for (int ii = 0; ii != Json_Depth + 2; ++ii) { Json_Text += '\t'; }
	std::sprintf(buf, "\"FileType\" : %i ,\n", FileType); Json_Text += buf;

	for (int ii = 0; ii != Json_Depth + 2; ++ii) { Json_Text += '\t'; }
	Json_Text += "\"Variables\" : [ ";
	for (std::vector<std::string>::const_iterator i = Variables.begin(); i != Variables.end(); ++i)
	{
		std::sprintf(buf, "\"%s\"", i->c_str()); Json_Text += buf;
		if (Variables.end() - i != 1)
		{
			Json_Text += ", ";
		}
	}
	Json_Text += " ] ,\n";

	if (Auxiliary.size() != 0)
	{
		for (int ii = 0; ii != Json_Depth + 2; ++ii) { Json_Text += '\t'; }
		Json_Text += "\"Auxiliary\" : {\n";
		int j = 0;
		for (std::map<std::string, std::string>::const_iterator i = Auxiliary.begin(); i != Auxiliary.end(); ++i)
		{
			for (int ii = 0; ii != Json_Depth + 2; ++ii) { Json_Text += '\t'; }
			std::sprintf(buf, "\"%s\" : \"%s\"", i->first.c_str(), i->second.c_str()); Json_Text += buf;
			if (j != Auxiliary.size() - 1)
			{
				Json_Text += ",\n";
			}
			++j;
		}
		Json_Text += "\n";
		for (int ii = 0; ii != Json_Depth + 2; ++ii) { Json_Text += '\t'; }
		Json_Text += "} ,\n";
	}

	for (int ii = 0; ii != Json_Depth + 2; ++ii) { Json_Text += '\t'; }
	Json_Text += "\"Zones\" : [\n";
	for (std::vector<TEC_ZONE>::const_iterator i = Zones.begin(); i != Zones.end(); ++i)
	{
		i->log_json_zone(Json_Text, Json_Depth);
		if (Zones.end() - i != 1)
		{
			Json_Text += " ,\n";
		}
	}
	Json_Text += "\n";
	for (int ii = 0; ii != Json_Depth + 2; ++ii) { Json_Text += '\t'; }
	Json_Text += "]\n";

	for (int ii = 0; ii != Json_Depth; ++ii) { Json_Text += '\t'; }
	Json_Text += "}";
}

void TEC_FILE::log_xml()
{
	char buf[200];

	for (int ii = 0; ii != Xml_Depth; ++ii) { Xml_Text += '\t'; }
	std::sprintf(buf, "<File FileName=\"%s\">\n", FileName.c_str()); Xml_Text += buf;

	for (int ii = 0; ii != Xml_Depth + 1; ++ii) { Xml_Text += '\t'; }
	std::sprintf(buf, "<FileName>%s</FileName>\n", (FileName + ".plt").c_str()); Xml_Text += buf;
	for (int ii = 0; ii != Xml_Depth + 1; ++ii) { Xml_Text += '\t'; }
	std::sprintf(buf, "<FilePath>%s</FilePath>\n", FilePath.c_str()); Xml_Text += buf;
	for (int ii = 0; ii != Xml_Depth + 1; ++ii) { Xml_Text += '\t'; }
	std::sprintf(buf, "<Time>%s</Time>\n", Time.c_str()); Xml_Text += buf;
	for (int ii = 0; ii != Xml_Depth + 1; ++ii) { Xml_Text += '\t'; }
	std::sprintf(buf, "<UsingTime>%.5lf</UsingTime>\n", UsingTime); Xml_Text += buf;
	for (int ii = 0; ii != Xml_Depth + 1; ++ii) { Xml_Text += '\t'; }
	std::sprintf(buf, "<Title>%s</Title>\n", Title.c_str()); Xml_Text += buf;
	for (int ii = 0; ii != Xml_Depth + 1; ++ii) { Xml_Text += '\t'; }
	std::sprintf(buf, "<!--%s-->\n", "0 = FULL, 1 = GRID, 2 = SOLUTION"); Xml_Text += buf;
	for (int ii = 0; ii != Xml_Depth + 1; ++ii) { Xml_Text += '\t'; }
	std::sprintf(buf, "<FileType>%i</FileType>\n", FileType); Xml_Text += buf;

	for (int ii = 0; ii != Xml_Depth + 1; ++ii) { Xml_Text += '\t'; }
	Xml_Text += "<Variables";
	for (std::vector<std::string>::const_iterator i = Variables.begin(); i != Variables.end(); ++i)
	{
		std::sprintf(buf, " V%zi=\"%s\"", i - Variables.begin() + 1, i->c_str()); Xml_Text += buf;
	}
	Xml_Text += "/>\n";

	if (Auxiliary.size() != 0)
	{
		for (int ii = 0; ii != Xml_Depth + 1; ++ii) { Xml_Text += '\t'; }
		Xml_Text += "<Auxiliary>\n";
		for (std::map<std::string, std::string>::const_iterator i = Auxiliary.begin(); i != Auxiliary.end(); ++i)
		{
			for (int ii = 0; ii != Xml_Depth + 2; ++ii) { Xml_Text += '\t'; }
			std::sprintf(buf, "<%s>%s</%s>\n", i->first.c_str(), i->second.c_str(), i->first.c_str()); Xml_Text += buf;
		}
		for (int ii = 0; ii != Xml_Depth + 1; ++ii) { Xml_Text += '\t'; }
		Xml_Text += "</Auxiliary>\n";
	}

	for (int ii = 0; ii != Xml_Depth + 1; ++ii) { Xml_Text += '\t'; }
	Xml_Text += "<Zones>\n";
	for (std::vector<TEC_ZONE>::const_iterator i = Zones.begin(); i != Zones.end(); ++i)
	{
		i->log_xml_zone(Xml_Text, Xml_Depth);
		Xml_Text += "\n";
	}
	for (int ii = 0; ii != Xml_Depth + 1; ++ii) { Xml_Text += '\t'; }
	Xml_Text += "</Zones>\n";

	for (int ii = 0; ii != Xml_Depth; ++ii) { Xml_Text += '\t'; }
	Xml_Text += "</File>";
}

void TEC_FILE::write_log()
{
	if (Json_WriteFile)
	{
		if (Json_File == NULL)
		{
# ifdef __linux__
			Json_File = fopen((FilePath + "/" + FileName + ".json").c_str(), "w");
			if (Json_File == 0)
			{
				throw std::runtime_error(std::string("cannot open file ") + (FileName + ".json"));
			}
# else
			errno_t err = fopen_s(&Json_File, (FilePath + "/" + FileName + ".json").c_str(), "w");
			if (err != 0)
			{
				throw std::runtime_error(std::string("cannot open file ") + (FileName + ".json"));
			}
# endif
			std::fprintf(Json_File, "%s", Json_Text.c_str());
			fclose(Json_File);
			Json_File = NULL;
		}
		else
		{
			std::fprintf(Json_File, "%s", Json_Text.c_str());
		}
	}

	if (Xml_WriteFile)
	{
		if (Xml_File == NULL)
		{
# ifdef __linux__
			Xml_File = fopen((FilePath + "/" + FileName + ".xml").c_str(), "w");
			if (Xml_File == NULL)
			{
				throw std::runtime_error(std::string("cannot open file ") + (FileName + ".xml"));
			}
# else
			errno_t err = fopen_s(&Xml_File, (FilePath + "/" + FileName + ".xml").c_str(), "w");
			if (err != 0)
			{
				throw std::runtime_error(std::string("cannot open file ") + (FileName + ".xml"));
			}
# endif
			std::fprintf(Xml_File, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
			std::fprintf(Xml_File, "%s", Xml_Text.c_str());
			fclose(Xml_File);
			Xml_File = NULL;
		}
		else
		{
			std::fprintf(Xml_File, "%s", Xml_Text.c_str());
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
		for(std::vector<DATA_P>::iterator i=Data.begin();i!=Data.end();++i)
		{
			i->buf=(byte *)i->DataP;
		}
		needreal = false;
	}
	else if(noskip)
	{
		for(std::vector<DATA_P>::iterator i=Data.begin();i!=Data.end();++i)
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
			for(size_t sk=Begin[2];sk<Max[2]-End[2];sk+=Skip[2])
			{
				for(size_t sj=Begin[1];sj<Max[1]-End[1];sj+=Skip[1])
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
		for(std::vector<DATA_P>::iterator i=Data.begin();i!=Data.end();++i)
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
			for(size_t sk=Begin[2];sk<Max[2]-End[2];sk+=Skip[2])
			{
				for(size_t sj=Begin[1];sj<Max[1]-End[1];sj+=Skip[1])
				{
					for(size_t si=Begin[0];si<Max[0]-End[0];si+=Skip[0])
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
		for (std::vector<DATA_P>::iterator i = Data.begin(); i != Data.end(); ++i)
		{
			delete[] i->buf;
		}
		needreal = false;
	}
	else
	{
		for (std::vector<DATA_P>::iterator i = Data.begin(); i != Data.end(); ++i)
		{
			i->buf = NULL;
		}
	}
}

void TEC_ZONE::wrtie_plt_pre_zone(const TEC_FILE &thisfile)
{
	gather_real_size();
	if (Data.size() == 0)
	{
		throw std::runtime_error("Zone(" + ZoneName + "): Data is empty");
	}
	if (Data.size() != thisfile.Variables.size())
	{
		throw std::runtime_error("Zone(" + ZoneName + "): the size of Data is not equal to the size of tec_file.Variables");
	}
	for (std::vector<DATA_P>::const_iterator i = Data.begin(); i != Data.end(); ++i)
	{
		if (i->DataP == NULL || i->type == 0 || i->size == 0)
		{
			throw std::runtime_error("Zone(" + ZoneName + ").Variable(" + thisfile.Variables[i - Data.begin()] + "): Data is unset");
		}
	}
}

void TEC_ZONE::write_plt_zonehead(FILE *of) const
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

void TEC_ZONE::write_plt_zonedata(FILE *of, const TEC_FILE &thisfile, std::ostream &echo)
{
	longint pos = ftell(of);
	W_FLOAT32(299.0f, of);//Zone marker Value = 299.0
	for (std::vector<DATA_P>::const_iterator j = Data.begin(); j != Data.end(); ++j)
	{
		W_INT32(j->type, of);//Variable data format
	}
	W_INT32(0, of);//Has passive variables: 0 = no
	W_INT32(0, of);//Has variable sharing 0 = no
	W_INT32(-1, of);//Zero based zone number to share connectivity list with (-1 = no sharing)

	make_buf();

	for (std::vector<DATA_P>::const_iterator j = Data.begin(); j != Data.end(); ++j)
	{
		std::pair<FLOAT64, FLOAT64> mm = j->minmax(Real_Max[0]*Real_Max[1]*Real_Max[2]);
		W_FLOAT64(mm.first, of);//Min value
		W_FLOAT64(mm.second, of);//Max value
	}

	if (Echo_Mode.test(3))
	{
		echo << "     Dim = " << Real_Dim << " Real_Max=[";
		for (int dd = 0; dd != Real_Dim; ++dd)
		{
			echo << " " << Real_Max[dd];
		}
		echo << " ]" << std::endl;
	}
	if (Echo_Mode.test(4))
	{
		echo << "     Org_Max = [";
		for (int dd = 0; dd != 3; ++dd)
		{
			echo << " " << Max[dd];
		}
		echo << " ]" << std::endl;
	}
	if (Echo_Mode.test(5))
	{
		echo << "     Skip = [";
		for (int dd = 0; dd != Real_Dim; ++dd)
		{
			echo << " " << Skip[dd];
		}
		echo << " ]" << std::endl;
	}
	if (Echo_Mode.test(6))
	{
		echo << "     Begin = [";
		for (int dd = 0; dd != Real_Dim; ++dd)
		{
			echo << " " << Begin[dd];
		}
		echo << " ]";

		echo << " End = [";
		for (int dd = 0; dd != Real_Dim; ++dd)
		{
			echo << " " << End[dd];
		}
		echo << " ]" << std::endl;
	}
	if (Echo_Mode.test(7) && StrandId != -1)
	{
		char buf[100];
		std::sprintf(buf, "     StrandId = %i SolutionTime = %le", StrandId, SolutionTime);
		echo << buf << std::endl;
	}

	if (Echo_Mode.test(2))
	{
		echo << "     write variables: ";
	}
	for (std::vector<DATA_P>::iterator j = Data.begin(); j != Data.end(); ++j)
	{
		j->file_pt = std::ftell(of);
		fwrite((const byte*)(j->buf), j->size, Real_Max[0]*Real_Max[1]*Real_Max[2], of);//Zone Data. Each variable is in data format as specified above
		if (Echo_Mode.test(2))
		{
			echo << "<" << thisfile.Variables[j - Data.begin()] << "> ";
		}
	}
	if (Echo_Mode.test(2))
	{
		echo << std::endl;
	}

	realise_buf();

	if (Echo_Mode.test(8))
	{
		double s_z = double(ftell(of) - pos) / 1024 / 1024;
		char buf[100];
		std::sprintf(buf, "     zone size: %.1lfMB", s_z);
		echo << buf << std::endl;
	}
}

void TEC_ZONE::log_json_zone(std::string &Json_Text, int Json_Depth) const
{
	char buf[200];

	for (int ii = 0; ii != Json_Depth + 2; ++ii) { Json_Text += '\t'; }
	Json_Text += "{\n";

	for (int ii = 0; ii != Json_Depth + 3; ++ii) { Json_Text += '\t'; }
	std::sprintf(buf, "\"ZoneName\" : \"%s\" ,\n", ZoneName.c_str()); Json_Text += buf;
	for (int ii = 0; ii != Json_Depth + 3; ++ii) { Json_Text += '\t'; }
	std::sprintf(buf, "\"StrandId\" : %i ,\n", StrandId); Json_Text += buf;
	if (StrandId != -1)
	{
		for (int ii = 0; ii != Json_Depth + 3; ++ii) { Json_Text += '\t'; }
		std::sprintf(buf, "\"SolutionTime\" : %le ,\n", SolutionTime); Json_Text += buf;
	}
	for (int ii = 0; ii != Json_Depth + 3; ++ii) { Json_Text += '\t'; }
	std::sprintf(buf, "\"Real_Dim\" : %i ,\n", Real_Dim); Json_Text += buf;
	for (int ii = 0; ii != Json_Depth + 3; ++ii) { Json_Text += '\t'; }
	std::sprintf(buf, "\"Org_Max\" : [ %zi, %zi, %zi ] ,\n", Max[0], Max[1], Max[2]); Json_Text += buf;
	for (int ii = 0; ii != Json_Depth + 3; ++ii) { Json_Text += '\t'; }
	std::sprintf(buf, "\"Skip\" : [ %zi, %zi, %zi ] ,\n", Skip[0], Skip[1], Skip[2]); Json_Text += buf;
	for (int ii = 0; ii != Json_Depth + 3; ++ii) { Json_Text += '\t'; }
	std::sprintf(buf, "\"Begin\" : [ %zi, %zi, %zi ] ,\n", Begin[0], Begin[1], Begin[2]); Json_Text += buf;
	for (int ii = 0; ii != Json_Depth + 3; ++ii) { Json_Text += '\t'; }
	std::sprintf(buf, "\"End\" : [ %zi, %zi, %zi ] ,\n", End[0], End[1], End[2]); Json_Text += buf;
	for (int ii = 0; ii != Json_Depth + 3; ++ii) { Json_Text += '\t'; }
	std::sprintf(buf, "\"Real_Max\" : [ %i, %i, %i ] ,\n", Real_Max[0], Real_Max[1], Real_Max[2]); Json_Text += buf;

	if (Auxiliary.size() != 0)
	{
		for (int ii = 0; ii != Json_Depth + 3; ++ii) { Json_Text += '\t'; }
		Json_Text += "\"Auxiliary\" : {\n";
		int j = 0;
		for (std::map<std::string, std::string>::const_iterator i = Auxiliary.begin(); i != Auxiliary.end(); ++i)
		{
			for (int ii = 0; ii != Json_Depth + 4; ++ii) { Json_Text += '\t'; }
			std::sprintf(buf, "\"%s\" : \"%s\"", i->first.c_str(), i->second.c_str()); Json_Text += buf;
			if (j != Auxiliary.size() - 1)
			{
				Json_Text += ",\n";
			}
			++j;
		}
		Json_Text += "\n";
		for (int ii = 0; ii != Json_Depth + 3; ++ii) { Json_Text += '\t'; }
		Json_Text += "} ,\n";
	}

	for (int ii = 0; ii != Json_Depth + 3; ++ii) { Json_Text += '\t'; }
	Json_Text += "\"Data_type_comment\" : \"1=Float, 2=Double, 3=LongInt, 4=ShortInt, 5=Byte, 6=Bit\" ,\n";
	for (int ii = 0; ii != Json_Depth + 3; ++ii) { Json_Text += '\t'; }
	Json_Text += "\"Data\" : [\n";
	for (std::vector<DATA_P>::const_iterator i = Data.begin(); i != Data.end(); ++i)
	{
		for (int ii = 0; ii != Json_Depth + 4; ++ii) { Json_Text += '\t'; }
		std::sprintf(buf, "{ \"type\":%i, \"size_i\":%zi, \"file_pt\":%li }", i->type, i->size, i->file_pt); Json_Text += buf;
		if (Data.end() - i != 1)
		{
			Json_Text += ",\n";
		}
	}
	Json_Text += "\n";
	for (int ii = 0; ii != Json_Depth + 3; ++ii) { Json_Text += '\t'; }
	Json_Text += "]\n";

	for (int ii = 0; ii != Json_Depth + 2; ++ii) { Json_Text += '\t'; }
	Json_Text += "}";
}

void TEC_ZONE::log_xml_zone(std::string &Xml_Text, int Xml_Depth) const
{
	char buf[200];

	for (int ii = 0; ii != Xml_Depth + 2; ++ii) { Xml_Text += '\t'; }
	std::sprintf(buf, "<Zone ZoneName=\"%s\">\n", ZoneName.c_str()); Xml_Text += buf;

	for (int ii = 0; ii != Xml_Depth + 3; ++ii) { Xml_Text += '\t'; }
	std::sprintf(buf, "<ZoneName>%s</ZoneName>\n", ZoneName.c_str()); Xml_Text += buf;
	for (int ii = 0; ii != Xml_Depth + 3; ++ii) { Xml_Text += '\t'; }
	std::sprintf(buf, "<StrandId>%i</StrandId>\n", StrandId); Xml_Text += buf;
	if (StrandId != -1)
	{
		for (int ii = 0; ii != Xml_Depth + 3; ++ii) { Xml_Text += '\t'; }
		std::sprintf(buf, "<SolutionTime>%le</SolutionTime>\n", SolutionTime); Xml_Text += buf;
	}
	for (int ii = 0; ii != Xml_Depth + 3; ++ii) { Xml_Text += '\t'; }
	std::sprintf(buf, "<Real_Dim>%i</Real_Dim>\n", Real_Dim); Xml_Text += buf;
	for (int ii = 0; ii != Xml_Depth + 3; ++ii) { Xml_Text += '\t'; }
	std::sprintf(buf, "<Org_Max I=\"%zi\" J=\"%zi\" K=\"%zi\"/>\n", Max[0], Max[1], Max[2]); Xml_Text += buf;
	for (int ii = 0; ii != Xml_Depth + 3; ++ii) { Xml_Text += '\t'; }
	std::sprintf(buf, "<Skip I=\"%zi\" J=\"%zi\" K=\"%zi\"/>\n", Skip[0], Skip[1], Skip[2]); Xml_Text += buf;
	for (int ii = 0; ii != Xml_Depth + 3; ++ii) { Xml_Text += '\t'; }
	std::sprintf(buf, "<Begin I=\"%zi\" J=\"%zi\" K=\"%zi\"/>\n", Begin[0], Begin[1], Begin[2]); Xml_Text += buf;
	for (int ii = 0; ii != Xml_Depth + 3; ++ii) { Xml_Text += '\t'; }
	std::sprintf(buf, "<End I=\"%zi\" J=\"%zi\" K=\"%zi\"/>\n", End[0], End[1], End[2]); Xml_Text += buf;
	for (int ii = 0; ii != Xml_Depth + 3; ++ii) { Xml_Text += '\t'; }
	std::sprintf(buf, "<Real_Max I=\"%i\" J=\"%i\" K=\"%i\"/>\n", Real_Max[0], Real_Max[1], Real_Max[2]); Xml_Text += buf;

	if (Auxiliary.size() != 0)
	{
		for (int ii = 0; ii != Xml_Depth + 3; ++ii) { Xml_Text += '\t'; }
		Xml_Text += "<Auxiliary>\n";
		for (std::map<std::string, std::string>::const_iterator i = Auxiliary.begin(); i != Auxiliary.end(); ++i)
		{
			for (int ii = 0; ii != Xml_Depth + 4; ++ii) { Xml_Text += '\t'; }
			std::sprintf(buf, "<%s>%s</%s>\n", i->first.c_str(), i->second.c_str(), i->first.c_str());
			Xml_Text += buf;
		}
		for (int ii = 0; ii != Xml_Depth + 3; ++ii) { Xml_Text += '\t'; }
		Xml_Text += "</Auxiliary>\n";
	}

	for (int ii = 0; ii != Xml_Depth + 3; ++ii) { Xml_Text += '\t'; }
	Xml_Text += "<!--1=Float, 2=Double, 3=LongInt, 4=ShortInt, 5=Byte, 6=Bit-->\n";
	for (int ii = 0; ii != Xml_Depth + 3; ++ii) { Xml_Text += '\t'; }
	Xml_Text += "<Datas>\n";
	for (std::vector<DATA_P>::const_iterator i = Data.begin(); i != Data.end(); ++i)
	{
		for (int ii = 0; ii != Xml_Depth + 4; ++ii) { Xml_Text += '\t'; }
		std::sprintf(buf, "<Data type=\"%i\" size_i=\"%zi\" file_pt=\"%li\"/>\n", i->type, i->size, i->file_pt);
		Xml_Text += buf;
	}
	for (int ii = 0; ii != Xml_Depth + 3; ++ii) { Xml_Text += '\t'; }
	Xml_Text += "</Datas>\n";

	for (int ii = 0; ii != Xml_Depth + 2; ++ii) { Xml_Text += '\t'; }
	Xml_Text += "</Zone>";
}

DATA_P::DATA_P()
{
	DataP = NULL;
	type = TEC_NULL;
	size = 0;
	buf = NULL;
	file_pt = 0;
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

std::pair<FLOAT64, FLOAT64> DATA_P::minmax(size_t N) const
{
	std::pair<FLOAT64, FLOAT64> ans;
	switch (type)
	{
	case DATA_P::TEC_FLOAT:
	{
		FLOAT32 min, max;
		FLOAT32 *data = (FLOAT32 *)buf;
		get_minmax<FLOAT32>(data, N, min, max);
		ans.first = min;
		ans.second = max;
		break;
	}
	case DATA_P::TEC_DOUBLE:
	{
		FLOAT64 min, max;
		FLOAT64 *data = (FLOAT64 *)buf;
		get_minmax<FLOAT64>(data, N, min, max);
		ans.first = min;
		ans.second = max;
		break;
	}
	case DATA_P::TEC_LONGINT:
	{
		longint min, max;
		longint *data = (longint *)buf;
		get_minmax<longint>(data, N, min, max);
		ans.first = min;
		ans.second = max;
		break;
	}
	case DATA_P::TEC_SHORTINT:
	{
		shortint min, max;
		shortint *data = (shortint *)buf;
		get_minmax<shortint>(data, N, min, max);
		ans.first = min;
		ans.second = max;
		break;
	}
	case DATA_P::TEC_BYTE:
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
