# ifndef ORDERED_TEC_HEAD
# define ORDERED_TEC_HEAD

# include <string>
# include <vector>
# include <map>
# include <stdexcept>
# include <bitset>
# include <iostream>
# include <typeinfo>
# include <cstdio>

namespace ORDERED_TEC
{
	typedef int INT32;
	typedef float FLOAT32;
	typedef double FLOAT64;

	typedef signed long int longint;
	typedef signed short int shortint;
	typedef signed char byte;

	const size_t TEC_FLOAT_S = 4;
	const size_t TEC_DOUBLE_S = 8;
	const size_t TEC_LONGINT_S = 4;
	const size_t TEC_SHORTINT_S = 2;
	const size_t TEC_BYTE_S = 1;

	class TEC_FILE;
	class DATA_P;
	class TEC_ZONE;

	class TEC_FILE
	{
	public:
		std::string FilePath;
		std::string FileName;
		std::string Title;
		std::vector<std::string> Variables;
		INT32 FileType;

		std::vector<TEC_ZONE> Zones;

		std::map<std::string, std::string> Auxiliary;

		//usingtime, time, size, section, variable, file_end, file_head
		std::bitset<7> Echo_Mode;

		int Json_Depth;
		bool Json_WriteFile;
		FILE * Json_File;

		int Xml_Depth;
		bool Xml_WriteFile;
		FILE * Xml_File;

		std::string Json_Text;
		std::string Xml_Text;
		std::string Time;
		double UsingTime;
	public:
		TEC_FILE(std::string name = "untitled_file", std::string path = ".", std::string title = "untitled");

		bool add_auxiliary_data(std::string name, std::string value);
		bool add_auxiliary_data(std::string name, double value);

		void set_echo_mode(std::string file, std::string zone);

		void write_plt(std::ostream &echo = std::cout);
	protected:
		void echo_mode(std::string echo = "brief");

		void wrtie_plt_pre();
		void write_plt_filehead(FILE *of, std::ostream &echo = std::cout);
		void write_plt_data(FILE *of, std::ostream &echo = std::cout);

		void log_json();
		void log_xml();
		void write_log();
	};

	class TEC_ZONE
	{
	public:
		std::string ZoneName;

		INT32 StrandId;
		FLOAT64 SolutionTime;

		size_t Max[3];
		std::vector<DATA_P> Data;
		size_t Skip[3];
		size_t Begin[3];
		size_t End[3];

		std::map<std::string, std::string> Auxiliary;

		//size, stdid & soltime, begin & end, skip, max_org, max_real, variable, zone_end, zone_head
		std::bitset<9> Echo_Mode;
	protected:
		INT32 Real_Max[3];
		INT32 Real_Dim;
		bool noskip, noexc;
		bool needreal;
	public:
		TEC_ZONE(std::string name = "untitled_zone");
		const INT32 * get_real_size(std::string name = "realmax");
		bool add_auxiliary_data(std::string name, std::string value);
		bool add_auxiliary_data(std::string name, double value);
	protected:
		void echo_mode(std::string echo = "brief");

		void gather_real_size();
		void make_buf();
		void realise_buf();

		void wrtie_plt_pre_zone(const TEC_FILE &thisfile);
		void write_plt_zonehead(FILE *of) const;
		void write_plt_zonedata(FILE *of, const TEC_FILE &thisfile, std::ostream &echo = std::cout);

		void log_json_zone(std::string &Json_Text, int Json_Depth) const;
		void log_xml_zone(std::string &Xml_Text, int Xml_Depth) const;

		friend class TEC_FILE;
	};

	class DATA_P
	{
	public:
		enum TEC_TYPE {
			TEC_NULL,
			TEC_FLOAT = 1,
			TEC_DOUBLE,
			TEC_LONGINT,
			TEC_SHORTINT,
			TEC_BYTE
		};
	public:
		const void * DataP;
		TEC_TYPE type;
		size_t size;

		long int file_pt;
	protected:
		byte * buf;
	public:
		DATA_P();
		template<typename T> explicit DATA_P(T * iDataP);

	protected:
		std::pair<FLOAT64, FLOAT64> minmax(size_t N) const;

		friend class TEC_ZONE;
	};
}

template<typename T> ORDERED_TEC::DATA_P::DATA_P(T * iDataP)
{
	if (typeid(iDataP) == typeid(float *))
	{
		type = DATA_P::TEC_FLOAT;
	}
	else if (typeid(iDataP) == typeid(double *))
	{
		type = DATA_P::TEC_DOUBLE;
	}
	else if ((typeid(iDataP) == typeid(longint *) || typeid(iDataP) == typeid(int *)) && sizeof(T) == TEC_LONGINT_S)
	{
		type = DATA_P::TEC_LONGINT;
	}
	else if ((typeid(iDataP) == typeid(shortint *) || typeid(iDataP) == typeid(int *)) && sizeof(T) == TEC_SHORTINT_S)
	{
		type = DATA_P::TEC_SHORTINT;
	}
	else if (typeid(iDataP) == typeid(byte *))
	{
		type = DATA_P::TEC_BYTE;
	}
	else
	{
		char err[100];
		std::sprintf(err,"type [%s]*%zi is unsupported in Tecplot", typeid(T).name(),sizeof(T));
		throw(std::runtime_error(err));
	}
	DataP = iDataP;
	buf = NULL;
	size = sizeof(T);
}

# endif
