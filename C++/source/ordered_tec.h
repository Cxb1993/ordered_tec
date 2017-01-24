# ifndef ORDERED_TEC_HEAD
# define ORDERED_TEC_HEAD

# include <string>
# include <vector>
# include <map>
# include <stdexcept>

typedef int INT32;
typedef float FLOAT32;
typedef double FLOAT64;

typedef signed int longint;
typedef signed short int shortint;
typedef signed char byte;

class TEC_FILE;
class DATA_P;
class TEC_ZONE;
void ORDERED_TEC(TEC_FILE tec_file,std::vector<TEC_ZONE> tec_zone,unsigned int echo);
void ORDERED_TEC(TEC_FILE tec_file,TEC_ZONE tec_zone,unsigned int echo);

class TEC_FILE
{
public:
	std::string FileName;
	std::string Title;
	std::vector<std::string> Variables;
	INT32 FileType;
	std::map<std::string,std::string> Auxiliary;

public:
	TEC_FILE();
	bool add_auxiliary_data(std::string name,std::string value);
	bool add_auxiliary_data(std::string name,double value);

	friend void ORDERED_TEC(TEC_FILE tec_file,std::vector<TEC_ZONE> tec_zone,unsigned int echo);
};

class DATA_P
{
public:
	enum TEC_TYPE {
		TEC_NULL,
		TEC_FLOAT=1,
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
	DATA_P(void * iDataP,TEC_TYPE itype);

	friend class TEC_ZONE;
	friend std::pair<FLOAT64,FLOAT64> minmax(DATA_P datap,size_t N);
};

class TEC_ZONE
{
public:
	std::string ZoneName;
	INT32 StrandId;
	FLOAT64 SolutionTime;
	size_t IMax,JMax,KMax;
	std::vector<DATA_P> Data;
	size_t ISkip,JSkip,KSkip;
	size_t IBegin,IEnd,JBegin,JEnd,KBegin,KEnd;
	std::map<std::string,std::string> Auxiliary;
protected:
	INT32 Real_IMax,Real_JMax,Real_KMax;
	bool noskip,noexc;
	bool needreal;
	
public:
	TEC_ZONE();
	INT32 get_realijk(short o);
	void real_ijk();
	bool add_auxiliary_data(std::string name,std::string value);
	bool add_auxiliary_data(std::string name,double value);
protected:
	void make_buf();
	void realise_buf();
	void write_data(const DATA_P &data,FILE *of) const;

	friend void ORDERED_TEC(TEC_FILE tec_file,std::vector<TEC_ZONE> tec_zone,unsigned int echo);
};

void ORDERED_TEC(TEC_FILE tec_file,std::vector<TEC_ZONE> tec_zone,unsigned int echo);
void ORDERED_TEC(TEC_FILE tec_file,TEC_ZONE tec_zone,unsigned int echo);

# endif