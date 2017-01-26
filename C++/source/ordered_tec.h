# ifndef ORDERED_TEC_HEAD
# define ORDERED_TEC_HEAD

# include <string>
# include <vector>
# include <map>
# include <stdexcept>

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
		std::string FileName;
		std::string Title;
		std::vector<std::string> Variables;
		INT32 FileType;

		std::vector<TEC_ZONE> Zones;

		std::map<std::string, std::string> Auxiliary;

	public:
		TEC_FILE();

		bool add_auxiliary_data(std::string name, std::string value);
		bool add_auxiliary_data(std::string name, double value);

		void write_plt(unsigned int echo);

	protected:
		void wrtie_plt_pre();
		void write_plt_filehead(FILE *of, unsigned int echo);
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
		void gather_real_size();
		void make_buf();
		void realise_buf();

		void write_plt_zonehead(FILE *of, unsigned int echo) const;
		void write_plt_zonedata(FILE *of, unsigned int echo);

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

	public:
		DATA_P();
		DATA_P(void * iDataP, TEC_TYPE itype);

		std::pair<FLOAT64, FLOAT64> minmax(size_t N) const;
		void write_data(FILE *of, size_t N) const;

		friend TEC_ZONE;
	};
}
# endif