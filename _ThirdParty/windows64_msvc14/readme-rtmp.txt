


2����rtmpdump�ڵ����ݿ�����Ŀ¼�£�Ȼ��.c .h��ӵ�����


3��_CRT_SECURE_NO_WARNINGS


4��ȡ��Ԥ����ͷ�ļ�


5��getopt9


6��_WINSOCK_DEPRECATED_NO_WARNINGS


7�� strdup  -�������� _strdup


8:
#ifdef _MSC_VER	/* MSVC */

#define snprintf _snprintf

#define strcasecmp stricmp  ->>>> _stricmp
#define strncasecmp strnicmp ->>>> _strnicmp



9:#define EWOULDBLOCK	WSAETIMEDOUT	/* we don't use nonblocking, but we do use timeouts */
-->>>>>

#define EWOULDBLOCK_WSAETIMEDOUT	WSAETIMEDOUT



10: int32_t *ip;-->>>int32_t *ip = NULL;

11//extern FILE *netstackdump;
//extern FILE *netstackdump_read;



#define NO_CRYPTO
typedef long off_t;


