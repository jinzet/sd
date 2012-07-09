
#ifndef _MA60COMM_H_
#define _MA60COMM_H_

// ��ģ���ǽ��麣�ṩ�Ľӿڷ����������������õ�������ͨ��
// ����������ȷ���У�ԭ���������ʼ������û�����ӣ����һ����ⲿ�����ĺ�����

class CComm
{
public:
	CComm();
	~CComm();
private:

public: //�ӿ�

};

// �������ѧ������
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

//����˶���
class ma60Server : public ma60Math
{
	//�պ���
	int   IdtReset(int ic,int jj);
	int   IdtFreeze(int ii);
	int   IdtRun(int ii);
	void  IdtPutVal(char ptr[],int rid,int off,char type,int size);
	char* IdtGetVal(int rid,int off,char type,int size);
	int   DbmOpen(char* a,int i);
	int   DbmClose();
	int   GetDBMPoint2(char  *name,int* rid,int* off,char* type,int* size);
	//�����Լ������Ƶ��������
	void  close(int s);

	//��ʵ��
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

	//��������������
	void fmain (int argc, char    **argv);
};



#endif //_MA60COMM_H_