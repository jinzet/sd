
#ifndef _MA60COMM_H_
#define _MA60COMM_H_

// 此模块是将珠海提供的接口服务器程序进行整理得到，编译通过
// 但并不能正确运行，原因是网络初始化部分没有增加，并且还有外部依赖的函数。

class CComm
{
public:
	CComm();
	~CComm();
private:

public: //接口

};

// 整理出数学处理类
class ma60Math
{
public:
protected:
	char  isNetByteSwap;
	float f_swap (float value);
	int i_swap (int value);
	short s_swap (short value);
	double d_swap (double value);
	float f_sun_ind (float value);
	int i_sun_ind (int value);
	short s_sun_ind (short value);
	double d_sun_ind (double value);
};

//服务端对象
class ma60Server : public ma60Math
{
	//空函数
	int   IdtReset(int ic,int jj);
	int   IdtFreeze(int ii);
	int   IdtRun(int ii);
	void  IdtPutVal(char ptr[],int rid,int off,char type,int size);
	char* IdtGetVal(int rid,int off,char type,int size);
	int   DbmOpen(char* a,int i);
	int   DbmClose();
	int   GetDBMPoint2(char  *name,int* rid,int* off,char* type,int* size);
	//根据自己理解设计的替代函数
	void  close(int s);

	//有实现
	char  GetDBMValueType(char type,int size);
	int   GetValueLength(char addr_dbm);
	void SetDBMValue(double lval, char p_dbm, int p_rid, int p_off, char p_type, int p_size);

	int readn (int s, char *buf, int nbytes);
	int writen (int s, char *buf, int nbytes);

	bool socketInit ();
	int ServerProcessIo ();
	int ServerErr (char err_code, int s);
	int sock;
	
	struct sockaddr_in client;
public:
	ma60Server();
	~ma60Server();

	//主函数（改名）
	void fmain (int argc, char    **argv);
};



#endif //_MA60COMM_H_