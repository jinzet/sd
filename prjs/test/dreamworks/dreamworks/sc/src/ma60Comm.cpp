
//////////////////////////
#include "windows.h"
//////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <unistd.h>
//#include <sys/time.h>
#include <sys/timeb.h>
//#include <sys/socket.h>
//#include <sys/filio.h>
//#include <netdb.h>
//#include <arpa/inet.h>
//#include <netinet/in.h>
#include <errno.h>
//#include <signal.h>
#include <math.h>
//#include <pwd.h>
//#include <curses.h>

//#define SOCK_ADDR               "192.168.5.255"
#define SOCK_ADDR               "172.16.5.10"
#define SEND_PORT               9701
#define RECV_PORT               9702

#define DATA_NUMBER_MAX         100
#define DATA_NAME_LENGTH_MAX    32
#define DATA_NUMBER_MAX1        40        /* set value count */
#define DATA_SIZE               sizeof(double)

#define READ_COMM_FLAG_ERR      1
#define READ_DATA_NUM_ERR       2
#define READ_DATA_NAME_LEN_ERR  3
#define READ_DATA_NAME_ERR      4
#define READ_SET_INDEX_ERR      5
#define READ_SET_VALUE_ERR      6
#define SET_VALUE_ERR           7
#define GET_DATA_VALUE_ERR      8
#define WRITE_DATA_VALUE_ERR    9

#define L1      0
#define I1      1
#define I2      2
#define I4      3
#define R4      4
#define R8      8


//////////////////////////
#include <winsock.h>
//////////////////////////

#include "ma60Comm.h"
#   pragma warning (push)
#	pragma warning( disable: 4996 )   //不安全的函数警告

/* GLOBAL DEFINE: byte swap flag */
#define NET_BYTE_FLAG           99





CComm::CComm()
{

}

CComm::~CComm()
{

}


float	ma60Math::f_swap (float value)
{

    if (isNetByteSwap) value = f_sun_ind(value);
    return(value);
}

int		ma60Math::i_swap (int value)
{
    if (isNetByteSwap) value = i_sun_ind (value);
    return(value);
}

short	ma60Math::s_swap (short value)
{
    if (isNetByteSwap) value = s_sun_ind (value);
    return(value);
}

double	ma60Math::d_swap (double value)
{
    if (isNetByteSwap) value = d_sun_ind (value);
    return(value);
}

float	ma60Math::f_sun_ind (float value)
{
   union{
      float f_val;
      char i_val[4];
   }change;
   char tmp_val[4];
   int i;

   change.f_val = value;

   tmp_val[0] = change.i_val[3];
   tmp_val[1] = change.i_val[2];
   tmp_val[2] = change.i_val[1];
   tmp_val[3] = change.i_val[0];

   for (i = 0; i < 4; i++)
       change.i_val[i] = tmp_val[i];

   return(change.f_val);
}

int		ma60Math::i_sun_ind (int value)
{
    union{
        int ii_val;
        char  i_val[4];
    }change;
    char tmp_val[4];
    int i;

    change.ii_val = value;
    tmp_val[0] = change.i_val[3];
    tmp_val[1] = change.i_val[2];
    tmp_val[2] = change.i_val[1];
    tmp_val[3] = change.i_val[0];
    for (i = 0; i < 4; i++)
        change.i_val[i] = tmp_val[i];
    return(change.ii_val);
}

short	ma60Math::s_sun_ind (short value)
{
    union{
        short s_val;
        char  i_val[2];
    }change;
    char tmp_val[2];
    int i;

    change.s_val = value;
    tmp_val[0] = change.i_val[1];
    tmp_val[1] = change.i_val[0];
    for (i = 0; i < 2; i++)
        change.i_val[i] = tmp_val[i];
    return(change.s_val);
}

double	ma60Math::d_sun_ind (double value)
{
    union{
        double l_val;
        char  i_val[8];
    }change;
    char tmp_val[8];
    int i;

    change.l_val = value;
    tmp_val[0] = change.i_val[7];
    tmp_val[1] = change.i_val[6];
    tmp_val[2] = change.i_val[5];
    tmp_val[3] = change.i_val[4];

    tmp_val[4] = change.i_val[3];
    tmp_val[5] = change.i_val[2];
    tmp_val[6] = change.i_val[1];
    tmp_val[7] = change.i_val[0];
    for (i = 0; i < 8; i++)
        change.i_val[i] = tmp_val[i];
    return(change.l_val);
}

ma60Server::ma60Server()
{

}

ma60Server::~ma60Server()
{

}

int ma60Server::IdtReset(int ic,int jj)
{
	return 1;
}

int ma60Server::IdtFreeze(int ii)
{
	return 1;
}

int ma60Server::IdtRun(int ii)
{
	return 1;
}

void ma60Server::IdtPutVal(char ptr[],int rid,int off,char type,int size)
{
}

char* ma60Server::IdtGetVal(int rid,int off,char type,int size)
{
	return "a";
}

int ma60Server::DbmOpen(char* a,int i)
{
	return 1;
}
int ma60Server::DbmClose()
{
	return 1;
}
int ma60Server::GetDBMPoint2(char  *name,int* rid,int* off,char* type,int* size)
{
	return 1;
}

void ma60Server::close(int s)
{
	closesocket(s);
}


char ma60Server::GetDBMValueType(char p_type,int p_size)
{
    if (p_type == 'i') {
        if (p_size == 4)  return(I4); 
        if (p_size == 2)  return(I2);
        if (p_size == 1)  return(I1);
    }
    if (p_type == 'r') {
        if (p_size == 4) return(R4);
        if (p_size == 8) return(R8);
    }
    if (p_type == 'l') return(L1);
    return(L1);
}

int ma60Server::GetValueLength(char addr_dbm)
{
    if (addr_dbm == L1) return(sizeof(char));
    if (addr_dbm == I1) return(sizeof(char));
    if (addr_dbm == I2) return(sizeof(int));
    if (addr_dbm == I4) return(sizeof(int));
    if (addr_dbm == R4) return(sizeof(float));
    if (addr_dbm == R8) return(sizeof(double));
    return(sizeof(char));
}

void ma60Server::SetDBMValue(double lval, char p_dbm, int p_rid, int p_off, char p_type, int p_size)
{
    union {
        int ii;
        char cc;
        short ss;
        float ff;
        double dd;
        char ptr[8];
    }setval;

    if(p_dbm == L1) setval.cc = (char)lval;
    if(p_dbm == I1) setval.cc = (char)lval;
    if(p_dbm == I2) setval.ss = (short)lval;
    if(p_dbm == I4) setval.ii = (int)lval;
    if(p_dbm == R4) setval.ff = (float)lval;
    if(p_dbm == R8) setval.dd = (double)lval;
    printf ("value = %f, dbm = %d, size = %d\n", lval, p_dbm, p_size);
    if (p_size) IdtPutVal(setval.ptr, p_rid, p_off, p_type, p_size);
}

int ma60Server::readn (int s, char *buf, int nbytes)
{
    int nread, nleft;
    int client_len;

    client_len = sizeof (client);
    client.sin_port = htons(RECV_PORT);

    nleft = nbytes;
    while (nleft > 0) {
        nread = recvfrom (s, buf, nleft, 0,
                          (struct sockaddr *)&client, &client_len);
        if (nread < 0) {
            if (errno == EINTR) nread = 0;
            else return(-1);
        }
        else if (nread == 0) break;
        nleft -= nread;
        buf += nread;
    }

    printf ("recvfrom   : IP -%s,   Port - %d\n",
            inet_ntoa(client.sin_addr), ntohs(client.sin_port));
    return (nbytes-nleft);
}

int ma60Server::writen (int s, char *buf, int nbytes)
{
    int nread, nleft;
    int client_len;

    client_len = sizeof (client);
    client.sin_port = htons(SEND_PORT);

    nleft = nbytes;
    while (nleft > 0) {
        nread = sendto (s, buf, nleft, 0, 
                        (struct sockaddr *)&client, client_len);
        if (nread < 0) {
            if (errno == EINTR) nread = 0;
            else return (-1);
        }
        nleft -= nread;
        buf += nread;
    }

    printf ("sendto   : IP -%s,   Port - %d\n",
            inet_ntoa(client.sin_addr), ntohs(client.sin_port));

    return (nbytes);
}





bool ma60Server::socketInit ()
{
    struct hostent *host;
    int /*optflag,*/ optval, optlen;
    struct sockaddr_in server;
    int server_len/*, client_len*/;

/* step 1: specify client IP(inlcude broadcast address) & PORT */
    if (!(host = gethostbyname(SOCK_ADDR))) {
        perror ("gethostbyname");
        //exit (1);
		return false;
    }
    client.sin_family      = AF_INET;
    memcpy (&client.sin_addr, host->h_addr, host->h_length);
    client.sin_port        = htons(SEND_PORT);
/* ------------------------------------------------------------*/

/* step 2: create socket and set NOBLOCK                       */
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror ("socket");
        //exit (2);
		return false;
    }

/**********************************************
    if (ioctl (sock, FIONBIO, &optflag) < 0) {
        perror ("ioctl");
        exit (3);
    }
***********************************************/
/* ------------------------------------------------------------*/

/* step 3: specify server ADDR & PORT and REUSE of address     */
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port        = htons(RECV_PORT);

    getsockopt (sock, SOL_SOCKET, SO_SNDBUF, (char *)&optval, &optlen);
    printf ("SO_SNDBUF: optlen = %d\toptval = %d\n", optlen, optval);
    optval = 1;
    optlen = sizeof(int);
    setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, optlen);
    setsockopt (sock, SOL_SOCKET, SO_BROADCAST, (const char *)&optval, optlen);
/* ------------------------------------------------------------*/

/* step 4: bind socket and get using PORT                      */
    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror ("bind");
        //exit (4);
		return false;
    }
    if (getsockname(sock, (struct sockaddr *)&server, &server_len) < 0) {
        perror ("getsockname");
        //exit (5);
		return false;
    }
/* ------------------------------------------------------------*/

    printf ("Serverdd using PORT %d\n", ntohs(server.sin_port));
	return true;
}


int ma60Server::ServerProcessIo ()
{
    char  comm_flag;    
    int   data_num, data_name_len, net_flag, data_len, len, offset; 
    char  data_name[DATA_NUMBER_MAX * (DATA_NAME_LENGTH_MAX + 1)];
    char  *data_name_p;
    char  p_type, pt_dbm;
    int   p_rid, p_off, p_size;
    char  data_addr_type[DATA_NUMBER_MAX];    
    int   data_addr_rid[DATA_NUMBER_MAX];
    int   data_addr_off[DATA_NUMBER_MAX];
    int   data_addr_size[DATA_NUMBER_MAX];
    char  data_addr_dbm[DATA_NUMBER_MAX];

    int   set_data_num, set_data_len, ic;
    char  sdata_addr_type[DATA_NUMBER_MAX1];    
    int   sdata_addr_rid[DATA_NUMBER_MAX1];
    int   sdata_addr_off[DATA_NUMBER_MAX1];
    int   sdata_addr_size[DATA_NUMBER_MAX1];
    char  sdata_addr_dbm[DATA_NUMBER_MAX1];

    char  *val;
    int   i;
    //char  cval;
    double lval;
    int   num;
    char  buf[DATA_NUMBER_MAX*8];
    struct timeb begin, end;
    union {
      char   cc;
      short  ss;
      int    ii;
      float  ff;
      double dd;
    } value[DATA_NUMBER_MAX];         /* value */

 
    while (1) {
        /** read communication flag **/
        if (readn (sock, &comm_flag, 1) != 1)
            return READ_COMM_FLAG_ERR;

        printf ("comm flag: %d\n", comm_flag);
        ftime (&begin);
        switch (comm_flag) {
            case 0:  /**** connect flag ****/
                /** read net byte flag {big-endian little-endian} **/
                if (readn (sock, (char *)&net_flag, 4) != 4)
                    return READ_DATA_NUM_ERR;
                fprintf (stderr, "net flag: %d", net_flag);
                isNetByteSwap = (net_flag == NET_BYTE_FLAG) ? 0 : 1;
                if (isNetByteSwap == 1) 
                    fprintf (stderr, " ==> %d  OK!\n", i_swap(net_flag));
                else
                    fprintf (stderr, " OK!\n");
                break;
                 
            case 1:  /** GET real-time sample data **/
            case 3:  /** SET operate data **/
                /** read data number **/
                if (readn (sock, (char *)&num, 4) != 4)
                    return READ_DATA_NUM_ERR;
                num = i_swap (num);
                printf ("data number: %d\n", num);
                if (comm_flag == 1) data_num = num;
                else set_data_num = num;
                
                /** read data name **/
                if (readn (sock, (char *)&data_name_len, 4) != 4)
                    return READ_DATA_NAME_LEN_ERR;
                data_name_len = i_swap (data_name_len);
                printf ("data name length: %d\n", data_name_len);
                
                if (readn (sock, data_name, data_name_len) != data_name_len)
                    return READ_DATA_NAME_ERR;
                printf ("data name: %s\n", data_name);
                
                /** get asca address and data type by data name **/
                DbmOpen (getenv("HOME"), 1);
                data_name_p    = strtok (data_name, ","); 
                for (i = 0; i < num; i ++) {
                    printf ("name[%d]: %s\n", i, data_name_p);
                    if (GetDBMPoint2 (data_name_p,
                                      &p_rid, &p_off, &p_type, &p_size)) {
                        p_size = 0;
                        printf ("[%d]:%s isn't in database\n",i,data_name_p);
                    }
                    else {
                        printf ("type[%d] = %c\n", i, p_type);
                    }
                    if (comm_flag == 1) {
                        data_addr_rid[i] = p_rid;
                        data_addr_off[i] = p_off;
                        data_addr_type[i] = p_type;
                        data_addr_size[i] = p_size;
                        data_addr_dbm[i] = GetDBMValueType(p_type, p_size);
                    }
                    else {
                        sdata_addr_rid[i] = p_rid;
                        sdata_addr_off[i] = p_off;
                        sdata_addr_type[i] = p_type;
                        sdata_addr_size[i] = p_size;
                        sdata_addr_dbm[i] = GetDBMValueType(p_type, p_size);
                    }
                    data_name_p    = strtok (0, ","); 
                }
                DbmClose();
                 
                memset (buf, 0, sizeof(buf));
                offset = 0; 
                len = 0;
                for (i = 0; i < num; i ++) {
                    pt_dbm = (comm_flag == 1) ? 
                             data_addr_dbm[i] : sdata_addr_dbm[i];
                    printf ("DBM type[%d] : %d\n", i, pt_dbm);
                    memcpy (buf+offset, &pt_dbm, sizeof(char));
                    offset += sizeof(char);
                    len += GetValueLength(pt_dbm);
                }
                
                if (comm_flag == 1) data_len = len;
                if (comm_flag == 3) set_data_len = set_data_num*sizeof(double);

                if (comm_flag == 3) break;

                printf ("data len: %d\n", len);
                len = i_swap (len);
                if (writen (sock, (char *)&len, 4) != 4)
                    return WRITE_DATA_VALUE_ERR;
                
                /** data type **/
                if (writen (sock, buf, num) != num)
                    return WRITE_DATA_VALUE_ERR;
                break;
                
            case 2:  /***** get data value from database *****/
                memset (buf, 0, sizeof(buf));
                offset = 0;
                for (i = 0; i < data_num; i ++) {
                    val = IdtGetVal (data_addr_rid[i],
                                     data_addr_off[i],
                                     data_addr_type[i],
                                     data_addr_size[i]);
                    if (val== NULL) {
                        value[i].cc = 0;
                        printf ("[%d]:no find data value\n", i);
                        memcpy (buf+offset, &(value[i].cc), sizeof(char));
                        offset += sizeof(char);
                        continue;
                    }

                    switch (data_addr_dbm[i]) {
                    case L1:
                         value[i].cc = *(char *)(val);
                         printf ("L1: value[%d] = %d\n", i, value[i].cc);
                         memcpy (buf+offset, &(value[i].cc), sizeof(char));
                         offset += sizeof(char);
                    break;
                    case I1:
                         value[i].cc = *(char *)(val);
                         printf ("I1: value[%d] = %d\n", i, value[i].cc);
                         memcpy (buf+offset, &(value[i].cc), sizeof(char));
                         offset += sizeof(char);
                    break;
                    case I2:
                         value[i].ss = *(short *)(val);
                         printf ("I2: value[%d] = %d\n", i, value[i].ss);
                         value[i].ss = i_swap (value[i].ss);
                         memcpy (buf+offset, &(value[i].ss), sizeof(short));
                         offset += sizeof(short);
                    break;
                    case I4:
                         value[i].ii = *(int *)(val);
                         printf ("I4: value[%d] = %d\n", i, value[i].ii);
                         value[i].ii = i_swap (value[i].ii);
                         memcpy (buf+offset, &(value[i].ii), sizeof(int));
                         offset += sizeof(int);
                    break;
                    case R4:
                         value[i].ff = *(float *)(val);
                         printf ("R4: value[%d] = %f\n", i, value[i].ff);
                         value[i].ff = f_swap (value[i].ff);
                         memcpy (buf+offset, &(value[i].ff), sizeof(float));
                         offset += sizeof(float);
                    break;
                    case R8:
                         value[i].dd = *(double *)(val);
                         printf ("R8: value[%d] = %lf\n", i, value[i].dd);
                         value[i].dd = d_swap (value[i].dd);
                         memcpy (buf+offset, &(value[i].dd), sizeof(double));
                         offset += sizeof(double);
                    break;
                    default: 
                         value[i].cc = 0;
                         printf ("[%d]:no find data type\n", i);
                         memcpy (buf+offset, &(value[i].cc), sizeof(char));
                         offset += sizeof(char);
                    break;
                    }
                }

                /** data value **/
                if (writen (sock, buf, data_len) != data_len)
                    return WRITE_DATA_VALUE_ERR;
                break;
            
            case 4:  /***** set data value to database *****/
                /** data value **/
                if (readn (sock, buf, set_data_len) != set_data_len)
                    return GET_DATA_VALUE_ERR;

                offset = 0;
                for (i = 0; i < set_data_num; i ++) {
                    memcpy (&lval, buf+offset, DATA_SIZE); /* set value */
                    lval = d_swap (lval);
                    offset += DATA_SIZE;
                    SetDBMValue (lval, sdata_addr_dbm[i],
                                       sdata_addr_rid[i],
                                       sdata_addr_off[i],
                                       sdata_addr_type[i],
                                       sdata_addr_size[i]);
                }
                break;

            case 5:  /***** SET    SINGLE POINT VALUE *****/
            case 6:  /***** SEARCH SINGLE POINT MESSAGE *****/
            case 7:  /***** SEARCH SINGLE POINT VALUE *****/
                offset = 0;
                len = DATA_NAME_LENGTH_MAX;
                if (comm_flag == 5) len += DATA_SIZE;
                if (readn (sock, buf, len) != len)
                    return GET_DATA_VALUE_ERR;
                memcpy (data_name, buf+offset, DATA_NAME_LENGTH_MAX); /* name */
                offset += DATA_NAME_LENGTH_MAX;
                if (comm_flag == 5) {
                    memcpy (&lval, buf+offset, DATA_SIZE);
                    lval = d_swap (lval);
                }

                /** get asca address and data type by data name **/
                DbmOpen (getenv("HOME"), 1);
                printf ("name: %s\n", data_name);
                if (GetDBMPoint2 (data_name,
                                  &p_rid, &p_off, &p_type, &p_size)) {
                    p_size = 0;
                    printf ("%s isn't in database\n", data_name);
                }
                else {
                    printf ("type = %c\n", p_type);
                    pt_dbm = GetDBMValueType(p_type, p_size);
                }
                DbmClose();

                if (comm_flag == 5) {
                    SetDBMValue (lval, pt_dbm, p_rid, p_off, p_type, p_size);
                }
                if (comm_flag == 6) {
                    offset = 0;
                    memcpy (buf+offset, &pt_dbm, sizeof(char));
                    offset += sizeof(char);
                    p_rid = i_swap (p_rid);
                    memcpy (buf+offset, &p_rid, sizeof(int));
                    offset += sizeof(int);
                    p_off = i_swap (p_off);
                    memcpy (buf+offset, &p_off, sizeof(int));
                    offset += sizeof(int);
                    memcpy (buf+offset, &p_type, sizeof(char));
                    offset += sizeof(char);
                    p_size = i_swap (p_size);
                    memcpy (buf+offset, &p_size, sizeof(int));
                    offset += sizeof(int);
                    if (writen (sock, buf, offset) != offset)
                        return WRITE_DATA_VALUE_ERR;
                }
                if (comm_flag == 7) {
                    offset = 0;
                    memcpy (buf+offset, &pt_dbm, sizeof(char));
                    offset += sizeof(char);

                    val = IdtGetVal (p_rid, p_off, p_type, p_size);
                    if (val== NULL) value[0].dd = 0;
                    else {
                        if(pt_dbm==L1) value[0].dd = (double)(*(char *)(val));
                        if(pt_dbm==I1) value[0].dd = (double)(*(char *)(val));
                        if(pt_dbm==I2) value[0].dd = (double)(*(int *)(val));
                        if(pt_dbm==I4) value[0].dd = (double)(*(int *)(val));
                        if(pt_dbm==R4) value[0].dd = (double)(*(float *)(val));
                        if(pt_dbm==R8) value[0].dd = (double)(*(double *)(val));
                    }
                    value[0].dd = d_swap (value[0].dd);
                    memcpy (buf+offset, &(value[0].dd), sizeof(double));
                    offset += sizeof(double);
                    if (writen (sock, buf, offset) != offset)
                        return WRITE_DATA_VALUE_ERR;
                }
                break;

            case 11:  /***** Idt Run *****/
                IdtRun (1);
                break;
            case 12:  /***** Idt Freeze *****/
                IdtFreeze (1);
                break;
            case 13:  /***** Idt Reset *****/
                /** read ic **/
                if (readn (sock, (char *)&ic, 4) != 4)
                    return READ_DATA_NUM_ERR;
                ic = i_swap (ic);
                printf ("set IC value: %d\n", ic);
                IdtReset (ic, 1);
                break;
            default:
                continue;
        }
        ftime (&end);
        printf ("flag #%d cost time: %d.%d\n", 
                 comm_flag,
                 end.time - begin.time,
                 end.millitm - begin.millitm);
/*
        napms (5);
*/
    }
}


int ma60Server::ServerErr (char err_code, int s)
{
    char    *err_str[] = {  "read comminucation flag",
                            "read data number",
                            "read data name length",
                            "send data name",
                            "write data flag",
                            "read set index",
                            "read set value",
                            "set value",
                            "get data value",
                            "write data value" };

    perror (err_str[err_code]);
    close (s);
    shutdown (s, 2);
    return 0;
}

void ma60Server::fmain (int argc, char    **argv)
{
	int    sock=0;
    int    err_code;

    /** ASCA **/
    //if (IdtAttShm() == -1) {
    //    printf("CANNOT ATTACH IDT SHARE MEMORY\n");
    //    exit(0);
    //}

    /** signal handle **/
    //signaln();
    if(!socketInit())
		return;

    err_code = ServerProcessIo ();
    ServerErr (err_code, sock);
}


#  pragma warning (pop)