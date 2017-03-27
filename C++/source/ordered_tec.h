# ifndef ORDERED_TEC_HEAD
# define ORDERED_TEC_HEAD

# include <string>
# include <vector>
# include <map>
# include <stdexcept>
# include <bitset>
# include <iostream>
# include <sstream>
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

	class TEC_FILE_BASE;
	class TEC_ZONE_BASE;
	class TEC_DATA_BASE;
	class TEC_FILE_LOG;
	class TEC_ZONE_LOG;
	class TEC_DATA_LOG;
	class TEC_FILE;
	class TEC_ZONE;
	class TEC_DATA;

	class TEC_FILE_BASE
	{
	public:
		std::string FilePath;
		std::string FileName;
		std::string Title;
		std::vector<std::string> Variables;
		INT32 FileType;
		std::map<std::string, std::string> Auxiliary;
	};

	class TEC_ZONE_BASE
	{
	public:
		std::string ZoneName;
		INT32 StrandId;
		FLOAT64 SolutionTime;
		size_t Max[3];
		INT32 Dim;
		size_t Skip[3];
		size_t Begin[3];
		size_t End[3];
		INT32 Real_Max[3];
		INT32 Real_Dim;
		bool noskip, noexc;
		std::map<std::string, std::string> Auxiliary;
	};

	class TEC_DATA_BASE
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
		TEC_TYPE type;
		size_t size;
		long int file_pt;
	};

	class TEC_FILE_LOG : public TEC_FILE_BASE
	{
	public:
		std::vector<TEC_ZONE_LOG> Zones;

		std::string Time_Begin;
		std::string Time_End;
		double UsingTime;
		double Size;
		std::vector<std::string> Error;
		std::vector<std::string> Echo_Text;
		std::vector<std::string> Json_Text;
		std::vector<std::string> Xml_Text;
	public:
		TEC_FILE_LOG();
		explicit TEC_FILE_LOG(const TEC_FILE &file);

		void write_json(int depth = 0, FILE *of = NULL);
		void write_xml(int depth = 0, FILE *of = NULL);
	};

	class TEC_ZONE_LOG : public TEC_ZONE_BASE
	{
	public:
		std::vector<TEC_DATA_LOG> Data;

		double Size;
		std::vector<std::string> Echo_Text;
		std::vector<std::string> Json_Text;
		std::vector<std::string> Xml_Text;
	public:
		TEC_ZONE_LOG();
		explicit TEC_ZONE_LOG(const TEC_ZONE &zone);

		void write_json(int depth = 0, FILE *of = NULL);
		void write_xml(int depth = 0, FILE *of = NULL);
	};

	class TEC_DATA_LOG : public TEC_DATA_BASE
	{
	public:
		TEC_DATA_LOG();
		explicit TEC_DATA_LOG(const TEC_DATA &data);
	};

	class TEC_FILE : public TEC_FILE_BASE
	{
	public:
		std::vector<TEC_ZONE> Zones;

		//usingtime, time, size, section, variable, file_end, file_head
		std::bitset<7> Echo_Mode;

		TEC_FILE_LOG last_log;
	public:
		TEC_FILE(const std::string &name = "untitled_file", const std::string &path = ".", const std::string &title = "untitled");

		bool add_auxiliary_data(const std::string &name, const std::string &value);
		bool add_auxiliary_data(const std::string &name, const double &value);

		void set_echo_mode(const std::string &file, const std::string &zone);

		void write_plt(std::ostream &echo = std::cout);
	protected:
		void echo_mode(const std::string &echo = "brief");

		void wrtie_plt_pre();
		void write_plt_filehead(FILE *of, std::ostream &echo = std::cout);
		void write_plt_data(FILE *of, std::ostream &echo = std::cout);

		void log_json();
		void log_xml();
	};

	class TEC_ZONE : public TEC_ZONE_BASE
	{
	public:
		std::vector<TEC_DATA> Data;

		//size, stdid & soltime, begin & end, skip, max_org, max_real, variable, zone_end, zone_head
		std::bitset<9> Echo_Mode;
	protected:
		INT32 Real_Max[3];
		INT32 Real_Dim;
		bool noskip, noexc;
		bool needreal;
	public:
		explicit TEC_ZONE(const std::string &name = "untitled_zone");
		const INT32 * get_real_size(const std::string &name = "realmax");
		bool add_auxiliary_data(const std::string &name, const std::string &value);
		bool add_auxiliary_data(const std::string &name, const double &value);
	protected:
		void echo_mode(const std::string &echo = "brief");

		void gather_real_size();
		void make_buf();
		void realise_buf();

		void wrtie_plt_pre_zone(const TEC_FILE &thisfile, TEC_ZONE_LOG &zone_log);
		void write_plt_zonehead(FILE *of) const;
		void write_plt_zonedata(FILE *of, const TEC_FILE &thisfile, std::ostream &echo = std::cout);

		void log_json_zone(TEC_ZONE_LOG &zone_log) const;
		void log_xml_zone(TEC_ZONE_LOG &zone_log) const;

		friend class TEC_FILE;
	};

	class TEC_DATA : public TEC_DATA_BASE
	{
	public:
		const void * DataP;
	protected:
		byte * buf;
	public:
		TEC_DATA();
		template<typename T> explicit TEC_DATA(T * iDataP);

	protected:
		std::pair<FLOAT64, FLOAT64> minmax(size_t N) const;

		friend class TEC_ZONE;
	};
}

template<typename T> ORDERED_TEC::TEC_DATA::TEC_DATA(T * iDataP)
{
	if (typeid(iDataP) == typeid(float *))
	{
		type = TEC_DATA::TEC_FLOAT;
	}
	else if (typeid(iDataP) == typeid(double *))
	{
		type = TEC_DATA::TEC_DOUBLE;
	}
	else if ((typeid(iDataP) == typeid(longint *) || typeid(iDataP) == typeid(int *)) && sizeof(T) == TEC_LONGINT_S)
	{
		type = TEC_DATA::TEC_LONGINT;
	}
	else if ((typeid(iDataP) == typeid(shortint *) || typeid(iDataP) == typeid(int *)) && sizeof(T) == TEC_SHORTINT_S)
	{
		type = TEC_DATA::TEC_SHORTINT;
	}
	else if (typeid(iDataP) == typeid(byte *))
	{
		type = TEC_DATA::TEC_BYTE;
	}
	else
	{
		std::ostringstream err;
		err << "type [" << typeid(T).name() << "]*" << sizeof(T) << " is unsupported in Tecplot";
		throw(std::runtime_error(err.str()));
	}
	DataP = iDataP;
	buf = NULL;
	size = sizeof(T);
}

# endif
