# ifndef ORDERED_TEC_HEAD
# define ORDERED_TEC_HEAD

# include <string>
# include <vector>
# include <map>
# include <stdexcept>
# include <bitset>
# include <iostream>

# define LOG_JSON

namespace ORDERED_TEC
{
	typedef int INT32;
	typedef float FLOAT32;
	typedef double FLOAT64;

	typedef signed int longint;
	typedef signed short int shortint;
	typedef signed char byte;

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

		//size, section, variable, file_end, file_head
		std::bitset<5> Echo_Mode;
	public:
		TEC_FILE();

		bool add_auxiliary_data(std::string name, std::string value);
		bool add_auxiliary_data(std::string name, double value);

		void set_echo_mode(std::string file, std::string zone);

		void write_plt(std::ostream &echo = std::cout);

		void write_log_json(FILE *of, int depth = 0) const;
		void write_log_json() const;

		void write_log_xml(FILE *of, int depth = 0) const;
		void write_log_xml() const;
	protected:
		void echo_mode(std::string echo = "default");

		void wrtie_plt_pre();
		void write_plt_filehead(FILE *of, std::ostream &echo = std::cout);
		void write_plt_data(FILE *of, std::ostream &echo = std::cout);
	};

	class TEC_ZONE
	{
	public:
		std::string ZoneName;

		INT32 StrandId;
		FLOAT64 SolutionTime;

		size_t IMax, JMax, KMax;
		std::vector<DATA_P> Data;
		size_t ISkip, JSkip, KSkip;
		size_t IBegin, IEnd, JBegin, JEnd, KBegin, KEnd;

		std::map<std::string, std::string> Auxiliary;

		//size, stdid & soltime, begin & end, skip, max_org, max_real, variable, zone_end, zone_head
		std::bitset<9> Echo_Mode;
	protected:
		INT32 Real_IMax, Real_JMax, Real_KMax;
		INT32 Real_Dim;
		bool noskip, noexc;
		bool needreal;
	public:
		TEC_ZONE();
		INT32 get_real_size(short o);
		bool add_auxiliary_data(std::string name, std::string value);
		bool add_auxiliary_data(std::string name, double value);
	protected:
		void echo_mode(std::string echo = "default");

		void gather_real_size();
		void make_buf();
		void realise_buf();

		void write_plt_zonehead(FILE *of) const;
		void write_plt_zonedata(FILE *of, std::ostream &echo = std::cout);

		void write_log_json_zone(FILE *of, int depth = 0) const;
		void write_log_xml_zone(FILE *of, int depth = 0) const;

		friend TEC_FILE;
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
	protected:
		byte * buf;
		longint file_pt;
	public:
		DATA_P();
		DATA_P(void * iDataP, TEC_TYPE itype);

		std::pair<FLOAT64, FLOAT64> minmax(size_t N) const;
		void write_data(FILE *of, size_t N);

		friend TEC_ZONE;
	};
}
# endif