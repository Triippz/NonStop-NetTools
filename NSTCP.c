/************************************************************************************
*		FILE:		"nstcp.c"
*
*		AUTHOR:		Mark Tripoli
*		DATE:		28 - JAN - 2018
*		LICENSE:	MIT License
*
*		Purpose:	This library is geared towards TCP/IP operations within
*					NonStop Guardian enviornments. Some of the code may need to be
*					tweaked, as this was geared towards compilation on a C++ compiler.
*
*		Notes:		Add your Trace Entrances/Exits and Debug Checks
*					If you are reading in your IPs, ports, etc. I would add that 
*					routine here, and call it whenever your process starts.
*
*		
*
*		REVISIONS:
*		VERSION		DATE	     COMMENTS
*		-------    ------       ---------------------------------------------------
*		1.0.0	  1/28/18		Initial Release
*************************************************************************************/

#ifdef __TANDEM
#include "=nstcph"
#else
#include "nstcp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************************
*						FUNCTION PROTOTYPES AND DEFINITIONS
***************************************************************************************/

	/* Add them here if you make some new routines */

/***************************************************************
*
* NAME:                           Set_Proc
*
* FUNCTION:                 Sets TCPIP Process Address
*
* NOTE:                     ex. $ZB27D, $ZB26C
*
* RETURNS:                         nothing
***************************************************************/
static void Set_Proc( TCP_PROC_NAME process_name )
{
	socket_set_inet_name(process_name);
}


/*************************************************************************
*
* NAME:                           Get_Sock
*
* FUNCTION:             Builds the sockaddr structure. The sockaddr structure
*                       holds all information required to perform operations
*                       w/ a client/server. It holds the IP, Port, Addr family
*                       in a network readible format.
*
* NOTE:            ex. of address_family : AF_INET, PF_INET, etc.
*
* RETURNS:                         nothing
*************************************************************************/
static void Set_SockAddr ( TCP_CONNECTION_INFO *connection, short address_family )
{
	/* allocate memory for the socket address structure */
	connection->sockaddr = malloc(sizeof(connection->sockaddr));
	/* zero it out */
	memset(connection->sockaddr, 0, sizeof(*connection->sockaddr));
	/* sometimes sa_data fills with junk which makes the server refuse the connection,*/
	/* so to be safe we zero it out                                                   */
	memset(connection->sockaddr->sa_data, '\0', sizeof(connection->sockaddr->sa_data));
	/* here is where we set the values in the structure into a network readable format*/
	connection->sockaddr->sin_family = address_family;
	connection->sockaddr->sin_port = htons(connection->port);
	connection->sockaddr->sin_addr.s_addr = inet_addr(connection->ipaddr);
}

/*******************************************************************
*
* NAME:                                 NewSocket
*
* FUNCTION:                         Creates a new Socket/FD
*
* NOTE:
*
* RETURNS:                              int - socket #
*******************************************************************/
static int Get_Sock (TCP_CONNECTION_INFO *connection, int address_family, int socket_type, int protocol)
{
	int socket_num;

	/* Don't forge to call the freeing proc when done */
	memset(&connection->sock, '\0', sizeof(*connection->sock));
	connection->sock = (int *)malloc(sizeof(int));

	socket_num = socket(address_family
		, socket_type
		, protocol);

	return socket_num;
}

/*******************************************************************
*
* NAME:                              Get_Sock_NW
*
* FUNCTION:                    Creates a new NOWAIT Socket/FD
*
* NOTE:
*
* RETURNS:                              int
*******************************************************************/
static int Get_Sock_NW( TCP_CONNECTION_INFO *connection, int address_family, int socket_type, int protocol, int sync)
{
	int socket_num;

	socket_num = socket_nw(address_family
		, socket_type
		, protocol
		, connection->flags
		, sync);

	return socket_num;
}

/*******************************************************************
*
* NAME:                                Set_Bind
*
* FUNCTION:                   associates a socket with a specific
*                             local internet address and port
*                             This is primarily a server func but
*                             is optional on most client connections
*
* NOTE:
*
* RETURNS:                              int
*******************************************************************/
static int Set_Bind (TCP_CONNECTION_INFO *connection)
{
	int status;

	status = bind(*connection->sock
		, (struct sockaddr *) connection->sockaddr
		, connection->sockaddr_len);

	return status;
}


/*******************************************************************
*
* NAME:                                 Set_Bind_NW
*
* FUNCTION:                   associates a socket with a specific
*                             local internet address and port
*                             This is primarily a server func but
*                             is optional on most client connections
*
*                             This is a NOWAIT call
*
* RETURNS:                              int
*******************************************************************/
static int Set_Bind_NW ( TCP_CONNECTION_INFO *connection )
{
	int status;

	status = bind_nw(*connection->sock
		, (struct sockaddr *) connection->sockaddr
		, connection->sockaddr_len
		, connection->tag);

	return status;
}


/*******************************************************************
*
* NAME:                                 Make_Connect
*
* FUNCTION:                 Connects to an already created socket
*
* NOTE:                     Must be called AFTER newSocket()
*
* RETURNS:                              int
*******************************************************************/
static int Make_Connect ( TCP_CONNECTION_INFO *connection )
{
	int status;

	/* we have to set sa_data, if not, seems like junk fills it
	* , which makes the server refuse the connection */
	memset ( connection->sockaddr->sa_data
		   , '\0'
		   , sizeof(connection->sockaddr->sa_data));

	status = connect ( *connection->sock
					 , ( struct sockaddr *) connection->sockaddr
					 , sizeof ( *connection->sockaddr ) );

	return status;
}

/*******************************************************************
*
* NAME:                             Make_Connect_NW
*
* FUNCTION:             Connects to a NOWAITED socket
*
* NOTE:                 Must be called AFTER newSocket_nw()
*
* RETURNS:                              int
*******************************************************************/
static int Make_Connect_NW ( TCP_CONNECTION_INFO *connection )
{
	int status;

	/* we have to set sa_data, if not, seems like junk fills it, which makes the server refuse the connection */
	memset ( connection->sockaddr->sa_data
		   , '\0'
		   , sizeof( connection->sockaddr->sa_data ) );

	status = connect_nw ( *connection->sock
						, (struct sockaddr *) &connection->sockaddr
						, connection->sockaddr_len
						, connection->tag );

	return status;
}


/*******************************************************************
*
* NAME:                                 Set_Listen
*
* FUNCTION:                 A server based func. Listens
*                           for incoming connections
*
* NOTE:
*
* RETURNS:                              int
* *******************************************************************/
static int Set_Listen (TCP_CONNECTION_INFO *connection )
{
	int status;

	status = listen ( *connection->sock
					, connection->queue_len );

	return status;
}

/*******************************************************************
*
* NAME:                                 New_Accept
*
* FUNCTION:                 A server based func. Accepts
*                           incoming connections.
*
* NOTE:
*
* RETURNS:                              int
* *******************************************************************/
static int New_Accept ( TCP_CONNECTION_INFO *connection, int *from_len_ptr )
{
	int status;

	status = accept ( *connection->sock
					, ( struct sockaddr * ) connection->sockaddr
					, from_len_ptr );

	return status;
}

/*******************************************************************
*
* NAME:                              New_Accept_NW
*
* FUNCTION:                 A server based func. Accepts
*                           incoming NOWAITED connections
*
* NOTE:
*
* RETURNS:                              int
* *******************************************************************/
static int New_Accept_NW ( TCP_CONNECTION_INFO *connection )
{
	int status;

	status = accept_nw ( *connection->sock
					   , ( struct sockaddr * ) connection->sockaddr
					   , &connection->sockaddr_len
					   , connection->tag );

	return status;
}

/*********************************************************************
*
* NAME:                           New_Accept_NW1
*
* FUNCTION:             A server based func. Accepts
*                       incoming NOWAITED connections.
*                       Can be used in place of newAccept_nw
*                       and newAccept.
*
* NOTE:
*
* RETURNS:                              int
* *******************************************************************/
static int New_Accept_NW1 ( TCP_CONNECTION_INFO *connection )
{
	int status;

	status = accept_nw1 ( *connection->sock
						, ( struct sockaddr * ) connection->sockaddr
						, &connection->sockaddr_len
						, connection->tag
						, ( short ) connection->queue_len);

	return status;
}

/*********************************************************************
*
* NAME:                                 New_Accept_NW2
*
* FUNCTION:
*
* NOTE:
*
* RETURNS:                              int
* *******************************************************************/
static int New_Accept_NW2 ( TCP_CONNECTION_INFO *connection )
{
	int status;

	status = accept_nw2 ( *connection->sock
						, ( struct sockaddr * ) connection->sockaddr
						, connection->tag );

	return status;
}

/*********************************************************************
*
* NAME:                                 New_Accept_NW3
*
* FUNCTION:
*
* NOTE:
*
* RETURNS:                              int
* *******************************************************************/
static int New_Accept_NW3 (TCP_CONNECTION_INFO *connection, struct sockaddr *me_ptr )
{
	int status;

	status = accept_nw3 ( *connection->sock
						, ( struct sockaddr * ) connection->sockaddr
						, me_ptr
						, connection->tag );

	return status;
}

/**********************************************************************
*
* NAME:                                 New_Send
*
* FUNCTION:                     Sends data on a connected
*                               socket
*
* NOTE:
*
* RETURNS:                              int
* *******************************************************************/
static int New_Send (TCP_CONNECTION_INFO *connection, char *buffer_ptr, int buffer_length )
{
	int status;

	status = send ( *connection->sock
				  , buffer_ptr
				  , buffer_length
				  , connection->flags );

	return status;
}

/*******************************************************************************
*
* NAME:                                 New_Send_NW
*
* FUNCTION:                     This is a NOWAIT operation.
*                               Sends data on a connected socket
*
* NOTE:
*
* RETURNS:                              int
* *****************************************************************************/
static int New_Send_NW ( TCP_CONNECTION_INFO *connection, char *buffer_ptr, int buffer_length )
{
	int status;

	status = send_nw ( *connection->sock
					 , buffer_ptr
					 , buffer_length
					 , connection->flags
					 , connection->tag );

	return status;
}

/*********************************************************************************
*
* NAME:                                 New_Recv
*
* FUNCTION:                     Receives data on a connected socket
*
* NOTE:
*
* RETURNS:                              int
* *******************************************************************************/
static int New_Recv (TCP_CONNECTION_INFO *connection, char *buffer_ptr, int buff_length )
{
	int status;

	status = recv ( *connection->sock
				  , buffer_ptr
				  , buff_length
				  , connection->flags );

	return status;
}

/*******************************************************************************
*
* NAME:                                 New_Recv_NW
*
* FUNCTION:                 This is a NOWAIT operation.
*                           Receives data on a connected socket.
*
* NOTE:
*
* RETURNS:                              int
* *****************************************************************************/
static int New_Recv_NW (TCP_CONNECTION_INFO *connection, char *buffer_ptr, int length )
{
	int status;

	status = recv_nw ( *connection->sock
					 , buffer_ptr
					 , length
					 , connection->flags
					 , connection->tag );

	return status;
}

/*********************************************************************************
*
* NAME:                                 Shutdown_Sock
*
* FUNCTION:             shuts down data transfer, partially
*                       or completely, on an actively connected
*                       TCP socket
*
* NOTE:                 for the "how"
*                       0 = stops recieving data over socket
*                       1 = stops sending data over socket
*                       3 = stops sends and recieves over socket
*
* RETURNS:                              int
* *******************************************************************************/
static int Shutdown_Sock (TCP_CONNECTION_INFO *connection, int how )
{
	int status;

	status = shutdown( *connection->sock, how );

	return status;
}

/*******************************************************
*
* NAME:             Shutdown_Sock_NW
*
* FUNCTION:         shuts down data transfer, partially
*                   or completely, on an actively connected
*                   TCP socket
*
* NOTE:
*
* RETURNS:                              int
* *******************************************************/
static int Shutdown_Sock_NW (TCP_CONNECTION_INFO *connection, int how )
{
	int status;

	status = shutdown_nw ( *connection->sock
						 , how
						 , connection->tag );

	return status;
}

/*******************************************************
*
* NAME:                 Close_Sock
*
* FUNCTION:             closes the socket/fd. & sets it
*                       back to NULL
*
* NOTE:
*
* RETURNS:                 int
* *******************************************************/
static int Close_Sock (TCP_CONNECTION_INFO *connection )
{
	int status;

	if ( !connection->sock )
	{
		*connection->sock = 0;
		memset ( &connection->sock
			   , 0
			   , sizeof ( *connection->sock ) );

		status = 0;
		return status;
	}
	status = FILE_CLOSE_ ( ( signed short ) *connection->sock ); /* We use the nonstop call here you can use close(), but sometimes its finickey*/
	
	memset ( &connection->sock
		   , 0
		   , sizeof ( *connection->sock ) );

	return status;
}

/*******************************************************
*
* NAME:                                 Get_Sock_Name
*
* FUNCTION:                         .
*
* NOTE:
*
* RETURNS:                              int
* *******************************************************/
static int Get_Sock_Name (TCP_CONNECTION_INFO *connection )
{
	int status;

	status = getsockname ( *connection->sock
						 , ( struct sockaddr * ) connection->sockaddr
						 , &connection->sockaddr_len );

	return status;
}

/*******************************************************
*
* NAME:                            Get_Sock_Name_NW
*
* FUNCTION:                         .
*
* NOTE:
*
* RETURNS:                              int
* *******************************************************/
static int Get_Sock_Name_NW ( TCP_CONNECTION_INFO *connection )
{
	int status;

	status = getsockname_nw ( *connection->sock
							, ( struct sockaddr * ) connection->sockaddr
							, &connection->sockaddr_len
							, connection->tag );

	return status;
}

/******************************************************************************************
*
* NAME:                 Clean_Conn_Info
*
* FUNCTION:             Clean up memory allocated for sockaddr structure and socket.
*                       Will also zero out some other elements, for the next transaction.
*
* RETURNS:              Nadda
*
******************************************************************************************/
static void Clean_Conn_Info ( TCP_CONNECTION_INFO *connection )
{
	if (connection->sockaddr != 0)
	{
		free(connection->sockaddr);
		connection->sockaddr = 0;
	}
	if (connection->sock != 0)
	{
		free(connection->sock);
		connection->sock = 0;
	}

	/* cleanup all data which is set each time a socket is created */
	connection->queue_len = '\0';
	connection->flags = '\0';
	connection->sockaddr_len = '\0';
	connection->tag = '\0';
}

#pragma PAGE "init_tcpip"
/******************************************************************************************
*
* NAME:                 initializeTCPIP
*
* FUNCTION:             This function will allocate a chunk of memory for the size of the
*                       TCPIP structure. Then all propreitary IP function definitions
*                       will inherit the properties of each standard socket / nonstop socket
*                       function call. Aka will look at the address of those standard functions.
*
* RETURNS:              Nadda
*
*****************************************************************************************/
TCP* intialize_tcp ( )
{
	TCP               *tcp;
	BOOLEAN            status;
	ERROR_MESSAGE      error_text;

	/* allocate memory */
	tcp = (TCP *) malloc ( sizeof ( TCP ) );

	/* redirect function calls to address of tcpip calls*/
	tcp->set_proc = Set_Proc;
	tcp->get_sock = Get_Sock;
	tcp->get_sock_nw = Get_Sock_NW;
	tcp->set_bind = Set_Bind;
	tcp->set_bind_nw = Set_Bind_NW;
	tcp->make_connect = Make_Connect;
	tcp->make_connect_nw = Make_Connect_NW;
	tcp->set_listen = Set_Listen;
	tcp->new_accept = New_Accept;
	tcp->new_accept_nw = New_Accept_NW;
	tcp->new_accept_nw1 = New_Accept_NW1;
	tcp->new_accept_nw2 = New_Accept_NW2;
	tcp->new_accept_nw3 = New_Accept_NW3;
	tcp->new_send = New_Send;
	tcp->new_send_nw = New_Send_NW;
	tcp->new_recv = New_Recv;
	tcp->new_recv_nw = New_Recv_NW;
	tcp->shutdown_sock = Shutdown_Sock;
	tcp->shutdown_sock_nw = Shutdown_Sock_NW;
	tcp->close_sock = Close_Sock;
	tcp->get_sock_name = Get_Sock_Name;
	tcp->get_sock_name_nw = Get_Sock_Name_NW;
	tcp->clean_conn_info = Clean_Conn_Info;
	tcp->set_addtionals = Tcp_Set_Additionals;
	tcp->set_sockaddr = Set_SockAddr;

	/* allocate memory for connection structure */
	tcp->tcp_connect = ( TCP_CONNECTION_INFO * ) malloc ( sizeof ( TCP_CONNECTION_INFO ) );

	return tcp;
}

/******************************************************************************************
*
* NAME:                 Tcp_Set_Additionals
*
* FUNCTION:             Set's the rest of the addition elements for later use in calling
*                       process
*
* RETURNS:              Nadda
*
*****************************************************************************************/
static void Tcp_Set_Additionals ( TCP_CONNECTION_INFO *connection, int flags
	, int queue_length, long tag, long sockaddr_len)
{
	/* pre-set some additional data for later use in calling process */
	connection->flags = flags;
	connection->queue_len = queue_length;
	connection->sockaddr_len = sockaddr_len;
	connection->tag = tag;
}


#ifdef __cplusplus
}
#endif