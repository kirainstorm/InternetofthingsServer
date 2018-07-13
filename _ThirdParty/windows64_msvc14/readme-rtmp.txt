


2：将rtmpdump内的内容拷贝到目录下，然后将.c .h添加到工程


3：_CRT_SECURE_NO_WARNINGS


4：取消预编译头文件


5：getopt9


6：_WINSOCK_DEPRECATED_NO_WARNINGS


7： strdup  -》》》》 _strdup


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


