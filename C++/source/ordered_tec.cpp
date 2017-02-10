# include "ordered_tec.h"

# include <string>
# include <vector>
# include <map>
# include <cstring>
# include <stdexcept>
# include <sstream>
# include <stdio.h>

# define S_INT32 4
# define S_FLOAT32 4
# define S_FLOAT64 8

# define S_FLOAT 4
# define S_DOUBLE 8
# define S_LONGINT 4
# define S_SHORTINT 2
# define S_BYTE 1

using namespace ORDERED_TEC;

void W_INT32(const INT32 &a, FILE *f)
{
	INT32 t = a;
	fwrite(&t, S_INT32, 1, f);
}

void W_FLOAT32(const FLOAT32 &a, FILE *f)
{
	FLOAT32 t = a;
	fwrite(&t, S_FLOAT32, 1, f);
}

void W_FLOAT64(const FLOAT64 &a, FILE *f)
{
	FLOAT64 t = a;
	fwrite(&t, S_FLOAT64, 1, f);
}

void W_STRING(const std::string &a, FILE *f)
{
	for (std::string::const_iterator i = a.begin(); i != a.end(); ++i)
	{
		W_INT32(*i, f);
	}
	W_INT32(0, f);
}

TEC_FILE::TEC_FILE()
{
	FilePath = "";
	FileName = "untitled_file";
	FileType = 0;
	Title = "untitled";
	echo_mode();
}

bool TEC_FILE::add_auxiliary_data(std::string name,std::string value)
{
	std::pair<std::map<std::string,std::string>::iterator,bool> ans;
	std::pair<std::string,std::string> temp(name,value);
	ans=Auxiliary.insert(temp);
	return ans.second;
}

bool TEC_FILE::add_auxiliary_data(std::string name,double value)
{
	std::stringstream ss;
	ss<<value;
	return add_auxiliary_data(name,ss.str());
}

void TEC_FILE::set_echo_mode(std::string file, std::string zone)
{
	try
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
	catch (...)
	{
		throw std::runtime_error("echo code wrong");
	}
}

void TEC_FILE::write_plt()
{
	wrtie_plt_pre();

	std::ios::sync_with_stdio(false);

	FILE *of;
	errno_t err = fopen_s(&of, (FilePath + "/" + FileName + ".plt").c_str(), "wb");
	if (err != 0)
	{
		throw std::runtime_error(std::string("cannot open file ") + (FileName + ".plt"));
	}

	if (echo.test(0))
	{
		printf("#### creat file %s.plt ####\n", FileName.c_str());
	}

	//I    HEADER SECTION
	write_plt_filehead(of);

	//EOHMARKER, value=357.0
	W_FLOAT32(357.0f, of);

	if (echo.test(3))
	{
		printf("-------------------------------------\n");
	}

	//II   DATA SECTION
	write_plt_data(of);

	if (echo.test(4))
	{
		double s_f = double(ftell(of)) / 1024 / 1024;
		printf("     file size: %.1fMB\n", s_f);
	}

	fclose(of);

	std::ios::sync_with_stdio(true);

	if (echo.test(1))
	{
		printf("#### save file %s.plt ####\n", FileName.c_str());
	}
}

void TEC_FILE::write_log_json(FILE *of, int depth) const
{
	for (int i = 0; i != depth; ++i) { fprintf(of, "\t"); }
	fprintf(of, "{\n");

	for (int i = 0; i != depth + 1; ++i) { fprintf(of, "\t"); }
	fprintf(of, "\"FileName\" : \"%s\" ,\n", (FileName + ".plt").c_str());
	for (int i = 0; i != depth + 1; ++i) { fprintf(of, "\t"); }
	fprintf(of, "\"Title\" : \"%s\" ,\n", Title.c_str());
	for (int i = 0; i != depth + 1; ++i) { fprintf(of, "\t"); }
	fprintf(of, "\"FileType_comment\" : \"%s\" ,\n", "0 = FULL, 1 = GRID, 2 = SOLUTION");
	for (int i = 0; i != depth + 1; ++i) { fprintf(of, "\t"); }
	fprintf(of, "\"FileType\" : %i ,\n", FileType);

	for (int i = 0; i != depth + 1; ++i) { fprintf(of, "\t"); }
	fprintf(of, "\"Variables\" : [ ");
	for (std::vector<std::string>::const_iterator i = Variables.begin(); i != Variables.end(); ++i)
	{
		fprintf(of, "\"%s\"", i->c_str());
		if (Variables.end() - i != 1)
		{
			fprintf(of, ", ");
		}
	}
	fprintf(of, " ] ,\n");

	if (Auxiliary.size() != 0)
	{
		for (int i = 0; i != depth + 1; ++i) { fprintf(of, "\t"); }
		fprintf(of, "\"Auxiliary\" : {\n");
		int j = 0;
		for (std::map<std::string, std::string>::const_iterator i = Auxiliary.begin(); i != Auxiliary.end(); ++i)
		{
			for (int i = 0; i != depth + 2; ++i) { fprintf(of, "\t"); }
			fprintf(of, "\"%s\" : \"%s\"", i->first.c_str(), i->second.c_str());
			if (j != Auxiliary.size() - 1)
			{
				fprintf(of, ",\n");
			}
			++j;
		}
		fprintf(of, "\n");
		for (int i = 0; i != depth + 1; ++i) { fprintf(of, "\t"); }
		fprintf(of, "} ,\n");
	}

	for (int i = 0; i != depth + 1; ++i) { fprintf(of, "\t"); }
	fprintf(of, "\"Zones\" : [\n");
	for (std::vector<TEC_ZONE>::const_iterator i = Zones.begin(); i != Zones.end(); ++i)
	{
		i->write_log_json_zone(of, depth);
		if (Zones.end() - i != 1)
		{
			fprintf(of, " ,\n");
		}
	}
	fprintf(of, "\n");
	for (int i = 0; i != depth + 1; ++i) { fprintf(of, "\t"); }
	fprintf(of, "]\n");

	for (int i = 0; i != depth; ++i) { fprintf(of, "\t"); }
	fprintf(of, "}");
}

void TEC_FILE::write_log_json() const
{
	FILE *of;
	errno_t err = fopen_s(&of, (FilePath + "/" + FileName + ".json").c_str(), "wb");
	if (err != 0)
	{
		throw std::runtime_error(std::string("cannot open file ") + (FileName + ".json"));
	}

	write_log_json(of);

	fclose(of);
}

void TEC_FILE::write_log_xml(FILE *of, int depth) const
{
	for (int i = 0; i != depth; ++i) { fprintf(of, "\t"); }
	fprintf(of, "<File FileName=\"%s\">\n", (FileName + ".plt").c_str());

	for (int i = 0; i != depth + 1; ++i) { fprintf(of, "\t"); }
	fprintf(of, "<Title>%s</Title>\n", Title.c_str());
	for (int i = 0; i != depth + 1; ++i) { fprintf(of, "\t"); }
	fprintf(of, "<!--%s-->\n", "0 = FULL, 1 = GRID, 2 = SOLUTION");
	for (int i = 0; i != depth + 1; ++i) { fprintf(of, "\t"); }
	fprintf(of, "<FileType>%i</FileType>\n", FileType);

	for (int i = 0; i != depth + 1; ++i) { fprintf(of, "\t"); }
	fprintf(of, "<Variables");
	for (std::vector<std::string>::const_iterator i = Variables.begin(); i != Variables.end(); ++i)
	{
		fprintf(of, " V%i=\"%s\"", i - Variables.begin() + 1, i->c_str());
	}
	fprintf(of, "/>\n");

	if (Auxiliary.size() != 0)
	{
		for (int i = 0; i != depth + 1; ++i) { fprintf(of, "\t"); }
		fprintf(of, "<Auxiliary>\n");
		for (std::map<std::string, std::string>::const_iterator i = Auxiliary.begin(); i != Auxiliary.end(); ++i)
		{
			for (int i = 0; i != depth + 2; ++i) { fprintf(of, "\t"); }
			fprintf(of, "<%s>%s</%s>\n", i->first.c_str(), i->second.c_str(), i->first.c_str());
		}
		for (int i = 0; i != depth + 1; ++i) { fprintf(of, "\t"); }
		fprintf(of, "</Auxiliary>\n");
	}

	for (int i = 0; i != depth + 1; ++i) { fprintf(of, "\t"); }
	fprintf(of, "<Zones>\n");
	for (std::vector<TEC_ZONE>::const_iterator i = Zones.begin(); i != Zones.end(); ++i)
	{
		i->write_log_xml_zone(of, depth);
		fprintf(of, "\n");
	}
	for (int i = 0; i != depth + 1; ++i) { fprintf(of, "\t"); }
	fprintf(of, "</Zones>\n");

	for (int i = 0; i != depth; ++i) { fprintf(of, "\t"); }
	fprintf(of, "</File>");
}

void TEC_FILE::write_log_xml() const
{
	FILE *of;
	errno_t err = fopen_s(&of, (FilePath + "/" + FileName + ".xml").c_str(), "wb");
	if (err != 0)
	{
		throw std::runtime_error(std::string("cannot open file ") + (FileName + ".xml"));
	}

	fprintf(of, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	write_log_xml(of);

	fclose(of);
}

void TEC_FILE::echo_mode(std::string iecho)
{
	if (iecho.compare("default")==0)
	{
		iecho = "00111";
	}
	else if (iecho.compare("full")==0)
	{
		iecho = "11111";
	}
	else if (iecho.compare("simple")==0)
	{
		iecho = "00001";
	}
	else if (iecho.compare("none")==0)
	{
		iecho = "00000";
	}
	echo = std::bitset<5>(iecho);
}

void TEC_FILE::wrtie_plt_pre()
{
	if (Variables.size() == 0)
	{
		throw std::runtime_error("tec_file.Variables is empty");
	}
	if (Zones.size() == 0)
	{
		throw std::runtime_error("tec_file.Zones is empty");
	}
	for (std::vector<TEC_ZONE>::iterator i = Zones.begin(); i != Zones.end(); ++i)
	{
		i->gather_real_size();
		if (i->Data.size() == 0)
		{
			throw std::runtime_error("one of zone.Data is empty");
		}
		if (i->Data.size() != Variables.size())
		{
			throw std::runtime_error("the size of zone.Data is not equal to the size of tec_file.Variables");
		}
		for (std::vector<DATA_P>::const_iterator j = i->Data.begin(); j != i->Data.end(); ++j)
		{
			if (j->DataP == NULL || j->type == 0 || j->size == 0)
			{
				throw std::runtime_error("one of Data is unset");
			}
		}
	}
}

void TEC_FILE::write_plt_filehead(FILE *of)
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
	if (echo.test(2))
	{
		printf("     VAR: ");
	}
	for (std::vector<std::string>::const_iterator i = Variables.begin(); i != Variables.end(); ++i)
	{
		W_STRING(*i, of);//Variable names

		if (echo.test(2))
		{
			printf("<%s> ", i->c_str());
		}
	}
	if (echo.test(2))
	{
		printf("\n");
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

void TEC_FILE::write_plt_data(FILE *of)
{
	//II   DATA SECTION
	//i    For both ordered and fe zones
	for (std::vector<TEC_ZONE>::iterator i = Zones.begin(); i != Zones.end(); ++i)
	{
		if (i->echo.test(0))
		{
			printf("--   write zone %i: %s   --\n", int(i - Zones.begin() + 1), i->ZoneName.c_str());
		}

		i->write_plt_zonedata(of);

		if (i->echo.test(1))
		{
			printf("--   write zone %i: %s   --\n", int(i - Zones.begin() + 1), i->ZoneName.c_str());
		}
	}
}

TEC_ZONE::TEC_ZONE()
{
	ZoneName = "untitled_zone";
	StrandId = -1;
	SolutionTime = 0.0f;
	IMax = 1;
	JMax = 1;
	KMax = 1;
	ISkip = 1;
	JSkip = 1;
	KSkip = 1;
	IBegin = 0;
	IEnd = 0;
	JBegin = 0;
	JEnd = 0;
	KBegin = 0;
	KEnd = 0;
	noskip = true;
	noexc = true;
	needreal = false;
	echo_mode();
}

INT32 TEC_ZONE::get_real_size(short o)
{
	gather_real_size();
	if (o == 0)
	{
		return Real_IMax;
	}
	else if (o == 1)
	{
		return Real_JMax;
	}
	else if (o == 2)
	{
		return Real_KMax;
	}
	else if (o == 3)
	{
		return Real_Dim;
	}
	else
	{
		throw std::runtime_error("out of range");
	}
}

bool TEC_ZONE::add_auxiliary_data(std::string name,std::string value)
{
	std::pair<std::map<std::string,std::string>::iterator,bool> ans;
	std::pair<std::string,std::string> temp(name,value);
	ans=Auxiliary.insert(temp);
	return ans.second;
}

bool TEC_ZONE::add_auxiliary_data(std::string name,double value)
{
	std::stringstream ss;
	ss<<value;
	return add_auxiliary_data(name,ss.str());
}

void TEC_ZONE::echo_mode(std::string iecho)
{
	if (iecho.compare("default")==0)
	{
		iecho = "000001001";
	}
	else if (iecho.compare("full")==0)
	{
		iecho = "111111111";
	}
	else if (iecho.compare("simple")==0)
	{
		iecho = "000000001";
	}
	else if (iecho.compare("none")==0)
	{
		iecho = "000000000";
	}
	echo = std::bitset<9>(iecho);
}

void TEC_ZONE::gather_real_size()
{
	if (IMax == 0)
	{
		throw std::runtime_error("zone.IMax connot be zeor");
	}
	if (IMax == 1 && (ISkip != 1 || IBegin != 0 || IEnd != 0))
	{
		throw std::runtime_error("zone.ISkip(or zone.IBegin or zone.IEnd) donnot need to set");
	}
	Real_IMax = (IMax - IBegin - IEnd) / ISkip;
	if ((IMax - IBegin - IEnd) % ISkip)
	{
		++Real_IMax;
	}

	if (JMax == 0)
	{
		throw std::runtime_error("zone.JMax connot be zeor");
	}
	if (JMax == 1 && (JSkip != 1 || JBegin != 0 || JEnd != 0))
	{
		throw std::runtime_error("zone.JSkip(or zone.JBegin or zone.JEnd) donnot need to set");
	}
	Real_JMax = (JMax - JBegin - JEnd) / JSkip;
	if ((JMax - JBegin - JEnd) % JSkip)
	{
		++Real_JMax;
	}

	if (KMax == 0)
	{
		throw std::runtime_error("zone.KMax connot be zeor");
	}
	if (KMax == 1 && (KSkip != 1 || KBegin != 0 || KEnd != 0))
	{
		throw std::runtime_error("zone.KSkip(or zone.KBegin or zone.KEnd) donnot need to set");
	}
	Real_KMax = (KMax - KBegin - KEnd) / KSkip;
	if ((KMax - KBegin - KEnd) % KSkip)
	{
		++Real_KMax;
	}

	if (Real_IMax == 0 || Real_JMax == 0 || Real_KMax == 0)
	{
		throw std::runtime_error("zone.Real_IMax(or zone.Real_JMax or zone.Real_KMax) is zero due to unreasonable set of Skip, Being or End");
	}

	Real_Dim = 3;
	if (Real_KMax == 1)
	{
		--Real_Dim;
		if (Real_JMax == 1)
		{
			--Real_Dim;
			if (Real_IMax == 1)
			{
				--Real_Dim;
			}
		}
	}

	noskip = ISkip == 1 && JSkip == 1 && KSkip == 1;
	noexc = IBegin == 0 && IEnd == 0 && JBegin == 0 && JEnd == 0 && KBegin == 0 && KEnd == 0;
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
			i->buf=new byte[Real_IMax*Real_JMax*Real_KMax*size];
			if(i->buf==NULL)
			{
				throw std::runtime_error("out of memory");
			}
			for(size_t sk=KBegin;sk<KMax-KEnd;sk+=KSkip)
			{
				for(size_t sj=JBegin;sj<JMax-JEnd;sj+=JSkip)
				{
					std::memcpy(i->buf+(Real_IMax*(sj-JBegin)/JSkip+Real_IMax*Real_JMax*(sk-KBegin)/KSkip)*size,
						        datap+(IBegin+IMax*sj+IMax*JMax*sk)*size, Real_IMax*size);
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
			i->buf=new byte[Real_IMax*Real_JMax*Real_KMax*size];
			if(i->buf==NULL)
			{
				throw std::runtime_error("out of memory");
			}
			for(size_t sk=KBegin;sk<KMax-KEnd;sk+=KSkip)
			{
				for(size_t sj=JBegin;sj<JMax-JEnd;sj+=JSkip)
				{
					for(size_t si=IBegin;si<IMax-IEnd;si+=ISkip)
					{
						std::memcpy(i->buf+((si-IBegin)/ISkip+Real_IMax*(sj-JBegin)/JSkip+Real_IMax*Real_JMax*(sk-KBegin)/KSkip)*size,
							        datap+(si+IMax*sj+IMax*JMax*sk)*size, size);
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
	W_INT32(Real_IMax, of);//IMax
	W_INT32(Real_JMax, of);//JMax
	W_INT32(Real_KMax, of);//KMax
	for (std::map<std::string, std::string>::const_iterator j = Auxiliary.begin(); j != Auxiliary.end(); ++j)
	{
		W_INT32(1, of);//Auxiliary name/value pair to follow
		W_STRING(j->first, of);//name string
		W_INT32(0, of);//Auxiliary Value Format (Currently only allow 0=AuxDataType_String)
		W_STRING(j->second, of);//Value string
	}
	W_INT32(0, of);//No more Auxiliary name/value pairs
}

void TEC_ZONE::write_plt_zonedata(FILE *of)
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
		std::pair<FLOAT64, FLOAT64> mm = j->minmax(Real_IMax*Real_JMax*Real_KMax);
		W_FLOAT64(mm.first, of);//Min value
		W_FLOAT64(mm.second, of);//Max value
	}

	if (echo.test(3))
	{
		printf("     IMax=%i", Real_IMax);
		if (Real_JMax != 1)
		{
			printf(" JMax=%i", Real_JMax);
			if (Real_KMax != 1)
			{
				printf(" KMax=%i", Real_KMax);
			}
		}
		printf(" Dim=%i", Real_Dim);
		printf("\n");
	}
	if (echo.test(4))
	{
		printf("     IMax_Org=%i JMax_Org=%i KMax_Org=%i\n", IMax, JMax, KMax);
	}
	if (echo.test(5))
	{
		printf("     ISkip=%i", int(ISkip));
		if (Real_JMax != 1)
		{
			printf(" JSkip=%i", int(JSkip));
			if (Real_KMax != 1)
			{
				printf(" KSkip=%i", int(KSkip));
			}
		}
		printf("\n");
	}
	if (echo.test(6))
	{
		printf("     IBegin=%i IEnd=%i\n", int(IBegin), int(IEnd));
		if (Real_JMax != 1)
		{
			printf("     JBegin=%i JEnd=%i\n", int(JBegin), int(JEnd));
			if (Real_KMax != 1)
			{
				printf("     KBegin=%i KEnd=%i\n", int(KBegin), int(KEnd));
			}
		}
	}
	if (echo.test(7) && StrandId != -1)
	{
		printf("     StrandId=%i SolutionTime=%f\n", StrandId, SolutionTime);
	}

	if (echo.test(2))
	{
		printf("..   write variables: ");
	}
	for (std::vector<DATA_P>::iterator j = Data.begin(); j != Data.end(); ++j)
	{
		if (echo.test(2))
		{
			printf("<%i> ", int(j - Data.begin() + 1));
		}
		j->write_data(of, Real_IMax*Real_JMax*Real_KMax);//Zone Data. Each variable is in data format as specified above
	}
	if (echo.test(2))
	{
		printf("  ..\n");
	}

	realise_buf();

	if (echo.test(8))
	{
		double s_z = double(ftell(of) - pos) / 1024 / 1024;
		printf("     zone size: %.1fMB\n", s_z);
	}
}

void TEC_ZONE::write_log_json_zone(FILE *of, int depth) const
{
	for (int i = 0; i != depth + 2; ++i) { fprintf(of, "\t"); }
	fprintf(of, "{\n");

	for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
	fprintf(of, "\"ZoneName\" : \"%s\" ,\n", ZoneName.c_str());
	for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
	fprintf(of, "\"StrandId\" : %i ,\n", StrandId);
	if (StrandId != -1)
	{
		for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
		fprintf(of, "\"SolutionTime\" : %lf ,\n", SolutionTime);
	}
	for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
	fprintf(of, "\"Max\" : [ %zi, %zi, %zi ] ,\n", IMax, JMax, KMax);
	for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
	fprintf(of, "\"Skip\" : [ %zi, %zi, %zi ] ,\n", ISkip, JSkip, KSkip);
	for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
	fprintf(of, "\"Begin\" : [ %zi, %zi, %zi ] ,\n", IBegin, JBegin, KBegin);
	for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
	fprintf(of, "\"End\" : [ %zi, %zi, %zi ] ,\n", IEnd, JEnd, KEnd);
	for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
	fprintf(of, "\"Real_Max\" : [ %zi, %zi, %zi ] ,\n", Real_IMax, Real_JMax, Real_KMax);

	if (Auxiliary.size() != 0)
	{
		for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
		fprintf(of, "\"Auxiliary\" : {\n");
		int j = 0;
		for (std::map<std::string, std::string>::const_iterator i = Auxiliary.begin(); i != Auxiliary.end(); ++i)
		{
			for (int i = 0; i != depth + 4; ++i) { fprintf(of, "\t"); }
			fprintf(of, "\"%s\" : \"%s\"", i->first.c_str(), i->second.c_str());
			if (j != Auxiliary.size() - 1)
			{
				fprintf(of, ",\n");
			}
			++j;
		}
		fprintf(of, "\n");
		for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
		fprintf(of, "} ,\n");
	}

	for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
	fprintf(of, "\"Data_type_comment\" : \"%s\" ,\n", "1=Float, 2=Double, 3=LongInt, 4=ShortInt, 5=Byte, 6=Bit");
	for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
	fprintf(of, "\"Data\" : [\n");
	for (std::vector<DATA_P>::const_iterator i = Data.begin(); i != Data.end(); ++i)
	{
		for (int i = 0; i != depth + 4; ++i) { fprintf(of, "\t"); }
		fprintf(of, "{ \"type\":%i, \"size_i\":%zi, \"file_pt\":%li }", i->type, i->size, i->file_pt);
		if (Data.end() - i != 1)
		{
			fprintf(of, ",\n");
		}
	}
	fprintf(of, "\n");
	for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
	fprintf(of, "]\n");

	for (int i = 0; i != depth + 2; ++i) { fprintf(of, "\t"); }
	fprintf(of, "}");
}

void TEC_ZONE::write_log_xml_zone(FILE *of, int depth) const
{
	for (int i = 0; i != depth + 2; ++i) { fprintf(of, "\t"); }
	fprintf(of, "<Zone>\n");

	for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
	fprintf(of, "<ZoneName>%s</ZoneName>\n", ZoneName.c_str());
	for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
	fprintf(of, "<StrandId>%i</StrandId>\n", StrandId);
	if (StrandId != -1)
	{
		for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
		fprintf(of, "<SolutionTime>%lf</SolutionTime>\n", SolutionTime);
	}
	for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
	fprintf(of, "<Max IMax=\"%zi\" JMax=\"%zi\" KMax=\"%zi\"/>\n", IMax, JMax, KMax);
	for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
	fprintf(of, "<Skip ISkip=\"%zi\" JSkip=\"%zi\" KSkip=\"%zi\"/>\n", ISkip, JSkip, KSkip);
	for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
	fprintf(of, "<Begin IBegin=\"%zi\" JBegin=\"%zi\" KBegin=\"%zi\"/>\n", IBegin, JBegin, KBegin);
	for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
	fprintf(of, "<End IEnd=\"%zi\" JEnd=\"%zi\" KEnd=\"%zi\"/>\n", IEnd, JEnd, KEnd);
	for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
	fprintf(of, "<Real_Max Real_IMax=\"%zi\" Real_JMax=\"%zi\" Real_KMax=\"%zi\"/>\n", Real_IMax, Real_JMax, Real_KMax);

	if (Auxiliary.size() != 0)
	{
		for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
		fprintf(of, "<Auxiliary>\n");
		for (std::map<std::string, std::string>::const_iterator i = Auxiliary.begin(); i != Auxiliary.end(); ++i)
		{
			for (int i = 0; i != depth + 4; ++i) { fprintf(of, "\t"); }
			fprintf(of, "<%s>%s</%s>\n", i->first.c_str(), i->second.c_str(), i->first.c_str());
		}
		for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
		fprintf(of, "</Auxiliary>\n");
	}

	for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
	fprintf(of, "<!--%s-->\n", "1=Float, 2=Double, 3=LongInt, 4=ShortInt, 5=Byte, 6=Bit");
	for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
	fprintf(of, "<Data>\n");
	for (std::vector<DATA_P>::const_iterator i = Data.begin(); i != Data.end(); ++i)
	{
		for (int i = 0; i != depth + 4; ++i) { fprintf(of, "\t"); }
		fprintf(of, "<data_%i type=\"%i\" size_i=\"%zi\" file_pt=\"%li\"/>\n", i - Data.begin() + 1, i->type, i->size, i->file_pt);
	}
	for (int i = 0; i != depth + 3; ++i) { fprintf(of, "\t"); }
	fprintf(of, "</Data>\n");

	for (int i = 0; i != depth + 2; ++i) { fprintf(of, "\t"); }
	fprintf(of, "</Zone>");
}

DATA_P::DATA_P()
{
	DataP = NULL;
	type = TEC_NULL;
	size = 0;
	buf = NULL;
}

DATA_P::DATA_P(void * iDataP, TEC_TYPE itype)
{
	DataP = iDataP;
	type = itype;
	switch (itype)
	{
	case TEC_FLOAT:
	{
		size = S_FLOAT;
		break;
	}
	case TEC_DOUBLE:
	{
		size = S_DOUBLE;
		break;
	}
	case TEC_LONGINT:
	{
		size = S_LONGINT;
		break;
	}
	case TEC_SHORTINT:
	{
		size = S_SHORTINT;
		break;
	}
	case TEC_BYTE:
	{
		size = S_BYTE;
		break;
	}
	}
	buf = NULL;
}

template<typename T>
void get_minmax(T *data, size_t N, T &min, T &max)
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

void DATA_P::write_data(FILE *of, size_t N)
{
	file_pt = ftell(of);
	fwrite((const byte*)buf, size, N, of);
}
