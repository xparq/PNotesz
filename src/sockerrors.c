// Copyright (C) 2011 Andrey Gruber (aka lamer)

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include <winerror.h>
#include <winsock2.h>
#include <wchar.h>

void GetSockErrorDescription(int errCode, wchar_t * lpDesc){
	switch(errCode){
		case WSA_INVALID_HANDLE: wcscpy(lpDesc, L"Specified event object handle is invalid."); break;
		case WSA_NOT_ENOUGH_MEMORY: wcscpy(lpDesc, L"Insufficient memory available."); break;
		case WSA_INVALID_PARAMETER: wcscpy(lpDesc, L"One or more parameters are invalid."); break;
		case WSA_OPERATION_ABORTED: wcscpy(lpDesc, L"Overlapped operation aborted."); break;
		case WSA_IO_INCOMPLETE: wcscpy(lpDesc, L"Overlapped I/O event object not in signaled state."); break;
		case WSA_IO_PENDING: wcscpy(lpDesc, L"Overlapped operations will complete later."); break;
		case WSAEINTR: wcscpy(lpDesc, L"Interrupted function call."); break;
		case WSAEBADF: wcscpy(lpDesc, L"File handle is not valid."); break;
		case WSAEACCES: wcscpy(lpDesc, L"Permission denied."); break;
		case WSAEFAULT: wcscpy(lpDesc, L"Bad address."); break;
		case WSAEINVAL: wcscpy(lpDesc, L"Invalid argument."); break;
		case WSAEMFILE: wcscpy(lpDesc, L"Too many open files."); break;
		case WSAEWOULDBLOCK: wcscpy(lpDesc, L"Resource temporarily unavailable."); break;
		case WSAEINPROGRESS: wcscpy(lpDesc, L"Operation now in progress."); break;
		case WSAEALREADY: wcscpy(lpDesc, L"Operation already in progress."); break;
		case WSAENOTSOCK: wcscpy(lpDesc, L"Socket operation on nonsocket."); break;
		case WSAEDESTADDRREQ: wcscpy(lpDesc, L"Destination address required."); break;
		case WSAEMSGSIZE: wcscpy(lpDesc, L"Message too long."); break;
		case WSAEPROTOTYPE: wcscpy(lpDesc, L"Protocol wrong type for socket."); break;
		case WSAENOPROTOOPT: wcscpy(lpDesc, L"Bad protocol option."); break;
		case WSAEPROTONOSUPPORT: wcscpy(lpDesc, L"Protocol not supported."); break;
		case WSAESOCKTNOSUPPORT: wcscpy(lpDesc, L"Socket type not supported."); break;
		case WSAEOPNOTSUPP: wcscpy(lpDesc, L"Operation not supported."); break;
		case WSAEPFNOSUPPORT: wcscpy(lpDesc, L"Protocol family not supported."); break;
		case WSAEAFNOSUPPORT: wcscpy(lpDesc, L"Address family not supported by protocol family."); break;
		case WSAEADDRINUSE: wcscpy(lpDesc, L"Address already in use."); break;
		case WSAEADDRNOTAVAIL: wcscpy(lpDesc, L"Cannot assign requested address."); break;
		case WSAENETDOWN: wcscpy(lpDesc, L"Network is down."); break;
		case WSAENETUNREACH: wcscpy(lpDesc, L"Network is unreachable."); break;
		case WSAENETRESET: wcscpy(lpDesc, L"Network dropped connection on reset."); break;
		case WSAECONNABORTED: wcscpy(lpDesc, L"Software caused connection abort."); break;
		case WSAECONNRESET: wcscpy(lpDesc, L"Connection reset by peer."); break;
		case WSAENOBUFS: wcscpy(lpDesc, L"No buffer space available."); break;
		case WSAEISCONN: wcscpy(lpDesc, L"Socket is already connected."); break;
		case WSAENOTCONN: wcscpy(lpDesc, L"Socket is not connected."); break;
		case WSAESHUTDOWN: wcscpy(lpDesc, L"Cannot send after socket shutdown."); break;
		case WSAETOOMANYREFS: wcscpy(lpDesc, L"Too many references."); break;
		case WSAETIMEDOUT: wcscpy(lpDesc, L"Connection timed out."); break;
		case WSAECONNREFUSED: wcscpy(lpDesc, L"Connection refused."); break;
		case WSAELOOP: wcscpy(lpDesc, L"Cannot translate name."); break;
		case WSAENAMETOOLONG: wcscpy(lpDesc, L"Name too long."); break;
		case WSAEHOSTDOWN: wcscpy(lpDesc, L"Host is down."); break;
		case WSAEHOSTUNREACH: wcscpy(lpDesc, L"No route to host."); break;
		case WSAENOTEMPTY: wcscpy(lpDesc, L"Directory not empty."); break;
		case WSAEPROCLIM: wcscpy(lpDesc, L"Too many processes."); break;
		case WSAEUSERS: wcscpy(lpDesc, L"User quota exceeded."); break;
		case WSAEDQUOT: wcscpy(lpDesc, L"Disk quota exceeded."); break;
		case WSAESTALE: wcscpy(lpDesc, L"Stale file handle reference."); break;
		case WSAEREMOTE: wcscpy(lpDesc, L"Item is remote."); break;
		case WSASYSNOTREADY: wcscpy(lpDesc, L"Network subsystem is unavailable."); break;
		case WSAVERNOTSUPPORTED: wcscpy(lpDesc, L"Winsock.dll version out of range."); break;
		case WSANOTINITIALISED: wcscpy(lpDesc, L"Successful WSAStartup not yet performed."); break;
		case WSAEDISCON: wcscpy(lpDesc, L"Graceful shutdown in progress."); break;
		case WSAENOMORE: wcscpy(lpDesc, L"No more results."); break;
		case WSAECANCELLED: wcscpy(lpDesc, L"Call has been canceled."); break;
		case WSAEINVALIDPROCTABLE: wcscpy(lpDesc, L"Procedure call table is invalid."); break;
		case WSAEINVALIDPROVIDER: wcscpy(lpDesc, L"Service provider is invalid."); break;
		case WSAEPROVIDERFAILEDINIT: wcscpy(lpDesc, L"Service provider failed to initialize."); break;
		case WSASYSCALLFAILURE: wcscpy(lpDesc, L"System call failure."); break;
		case WSASERVICE_NOT_FOUND: wcscpy(lpDesc, L"Service not found."); break;
		case WSATYPE_NOT_FOUND: wcscpy(lpDesc, L"Class type not found."); break;
		case WSA_E_NO_MORE: wcscpy(lpDesc, L"No more results."); break;
		case WSA_E_CANCELLED: wcscpy(lpDesc, L"Call was canceled."); break;
		case WSAEREFUSED: wcscpy(lpDesc, L"Database query was refused."); break;
		case WSAHOST_NOT_FOUND: wcscpy(lpDesc, L"Host not found."); break;
		case WSATRY_AGAIN: wcscpy(lpDesc, L"Nonauthoritative host not found."); break;
		case WSANO_RECOVERY: wcscpy(lpDesc, L"This is a nonrecoverable error."); break;
		case WSANO_DATA: wcscpy(lpDesc, L"Valid name, no data record of requested type."); break;
		case WSA_QOS_RECEIVERS: wcscpy(lpDesc, L"QOS receivers."); break;
		case WSA_QOS_SENDERS: wcscpy(lpDesc, L"QOS senders."); break;
		case WSA_QOS_NO_SENDERS: wcscpy(lpDesc, L"No QOS senders."); break;
		case WSA_QOS_NO_RECEIVERS: wcscpy(lpDesc, L"QOS no receivers."); break;
		case WSA_QOS_REQUEST_CONFIRMED: wcscpy(lpDesc, L"QOS request confirmed."); break;
		case WSA_QOS_ADMISSION_FAILURE: wcscpy(lpDesc, L"QOS admission error."); break;
		case WSA_QOS_POLICY_FAILURE: wcscpy(lpDesc, L"QOS policy failure."); break;
		case WSA_QOS_BAD_STYLE: wcscpy(lpDesc, L"QOS bad style."); break;
		case WSA_QOS_BAD_OBJECT: wcscpy(lpDesc, L"QOS bad object."); break;
		case WSA_QOS_TRAFFIC_CTRL_ERROR: wcscpy(lpDesc, L"QOS traffic control error."); break;
		case WSA_QOS_GENERIC_ERROR: wcscpy(lpDesc, L"QOS generic error."); break;
		case WSA_QOS_ESERVICETYPE: wcscpy(lpDesc, L"QOS service type error."); break;
		case WSA_QOS_EFLOWSPEC: wcscpy(lpDesc, L"QOS flowspec error."); break;
		case WSA_QOS_EPROVSPECBUF: wcscpy(lpDesc, L"Invalid QOS provider buffer."); break;
		case WSA_QOS_EFILTERSTYLE: wcscpy(lpDesc, L"Invalid QOS filter style."); break;
		case WSA_QOS_EFILTERTYPE: wcscpy(lpDesc, L"Invalid QOS filter type."); break;
		case WSA_QOS_EFILTERCOUNT: wcscpy(lpDesc, L"Incorrect QOS filter count."); break;
		case WSA_QOS_EOBJLENGTH: wcscpy(lpDesc, L"Invalid QOS object length."); break;
		case WSA_QOS_EFLOWCOUNT: wcscpy(lpDesc, L"Incorrect QOS flow count."); break;
		case WSA_QOS_EUNKOWNPSOBJ: wcscpy(lpDesc, L"Unrecognized QOS object."); break;
		case WSA_QOS_EPOLICYOBJ: wcscpy(lpDesc, L"Invalid QOS policy object."); break;
		case WSA_QOS_EFLOWDESC: wcscpy(lpDesc, L"Invalid QOS flow descriptor."); break;
		case WSA_QOS_EPSFLOWSPEC: wcscpy(lpDesc, L"Invalid QOS provider-specific flowspec."); break;
		case WSA_QOS_EPSFILTERSPEC: wcscpy(lpDesc, L"Invalid QOS provider-specific filterspec."); break;
		case WSA_QOS_ESDMODEOBJ: wcscpy(lpDesc, L"Invalid QOS shape discard mode object."); break;
		case WSA_QOS_ESHAPERATEOBJ: wcscpy(lpDesc, L"Invalid QOS shaping rate object."); break;
		case WSA_QOS_RESERVED_PETYPE: wcscpy(lpDesc, L"Reserved policy QOS element type."); break;
		default:{
			LPVOID lpMsgBuf;
 
			FormatMessageW( 
			    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			    NULL,
			    errCode,
			    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			    (wchar_t *) &lpMsgBuf,
			    0,
			    NULL 
			);
			wcscpy(lpDesc, (wchar_t *)lpMsgBuf);
			// Free the buffer.
			LocalFree( lpMsgBuf );
			break;
		}
	}
}

