
/************************************************************************************
*
*		AUTHOR:		Mark Tripoli
*		DATE:		28 - JAN - 2018
*		LICENSE:	MIT License
*		
*		Purpose:	This library is geared towards TCP/IP operations within
*					NonStop Guardian enviornments. Some of the code may need to be
*					tweaked, as this was geared towards compilation on a C++ compiler.
*					
*		Notes:		When utilizing this library, please bear in mind, except for 
*					initialization, you will be operating through the TCP structure
*					when making calls. It is similar to working with object-based code.
*					
*		PS:			Sorry if you don't like my "java-like" naming style.
*
*		REVISIONS:
*		VERSION		DATE	     COMMENTS
*		-------    ------       ---------------------------------------------------
*		1.0.0	  1/28/18		Initial Release 
*************************************************************************************/

#ifndef _IPCONNH_INCLUDE_

#ifdef __TANDEM
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <socket.h>
#include <netdb.h>
#include <inet.h>
#include <time.h>
#include <stdint.h>
#include <cextdecs>
#include <route.h>
#include <if.h>
#include <in.h>
#include <in6.h>
#include <ioctl.h>
#else
/* fill in what you would like here....*/
#endif


/* Type Definitions */
typedef char			TCP_IPADDR;
typedef char			TCP_PROC_NAME;
typedef unsigned short	TCP_PORT;
typedef char			ERROR_MESSAGE[128];
/* These ones below you may already have, 
*  feel free to remove if you need to
*  if using C99 --> #include<stdbool.h> */
typedef enum { FAIL, SUCCESS } BOOLEAN;



/***************************************************************
*
*	Name:		TCP_CONNECTION_INFO
*	Type:		struct
*	Purpose:	Contains all the specific information
*				which will be used during TCP/IP operations
*				Most of these you will set initially, but
*				you are free to change throughout your process
*				as needed.
*
***************************************************************/
typedef struct tcp_connection_info
{
	TCP_IPADDR			ipaddr;
	TCP_PORT			port;
	TCP_PROC_NAME		process_name;
	int					*sock;
	long				sockaddr_len;
	int					flags;
	int					queue_len;
	long				tag;
	struct sockaddr_in	*sockaddr;
	int					sock_shutdown_how;
} TCP_CONNECTION_INFO;

/***************************************************************
*
*	Name:		TCP
*	Type:		struct
*	Purpose:	Contains all the function pointers which will 
*				be utilized throughout this library. It also
*				contains a pointer to the TCP_CONNECTION_INFO
*				structure, so you can operate solely from the 
*				TCP structure pointer.
*
***************************************************************/
typedef	struct _tcp
{
	TCP_CONNECTION_INFO				*tcp_connect;
	void(*set_proc)					(TCP_PROC_NAME);
	int(*get_sock)					(TCP_CONNECTION_INFO *, int, int, int);
	int(*get_sock_nw)				(TCP_CONNECTION_INFO *, int, int, int, int);
	int(*set_bind)					(TCP_CONNECTION_INFO *);
	int(*set_bind_nw)				(TCP_CONNECTION_INFO *);
	int(*make_connect)				(TCP_CONNECTION_INFO *);
	int(*make_connect_nw)			(TCP_CONNECTION_INFO *);
	int(*set_listen)				(TCP_CONNECTION_INFO *);
	int(*new_accept)				(TCP_CONNECTION_INFO *, int*);
	int(*new_accept_nw)				(TCP_CONNECTION_INFO *);
	int(*new_accept_nw1)			(TCP_CONNECTION_INFO *);
	int(*new_accept_nw2)			(TCP_CONNECTION_INFO *);
	int(*new_accept_nw3)			(TCP_CONNECTION_INFO *, struct sockaddr *);
	int(*new_send)					(TCP_CONNECTION_INFO *, char*, int);
	int(*new_send_nw)				(TCP_CONNECTION_INFO *, char*, int);
	int(*new_recv)					(TCP_CONNECTION_INFO *, char*, int);
	int(*new_recv_nw)				(TCP_CONNECTION_INFO *, char*, int);
	int(*shutdown_sock)				(TCP_CONNECTION_INFO *, int);
	int(*shutdown_sock_nw)			(TCP_CONNECTION_INFO *, int);
	int(*close_sock)				(TCP_CONNECTION_INFO *);
	int(*get_sock_name)				(TCP_CONNECTION_INFO *);
	int(*get_sock_name_nw)			(TCP_CONNECTION_INFO *);
	void(*clean_conn_info)			(TCP_CONNECTION_INFO *);
	void(*set_addtionals)			(TCP_CONNECTION_INFO *, int, int, long, long);
	void(*set_sockaddr)				(TCP_CONNECTION_INFO *, short);
} TCP;

/**********************************************************
*		Function Prototype Definition(s)
**********************************************************/
TCP *intialize_tcp ( void );

enum
{
	INFO = 0,
	WARNING = 1,
	ERROR = 2
};

#endif // !_IPCONNH_INCLUDE_
