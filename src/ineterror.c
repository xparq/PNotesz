// Copyright (C) 2010 Andrey Gruber (aka lamer)

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

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winbase.h>
#include <wchar.h>
#include <wininet.h>
#include "ineterror.h"

void GetInternetErrorDescriptionW(wchar_t * lpErrDesc, int nErr){
	if(nErr == 0)
		nErr = GetLastError();

	lpErrDesc[0] = '\0';
	switch(nErr){
		case ERROR_FTP_DROPPED:
			wcscpy(lpErrDesc, L"The FTP operation was not completed because the session was aborted."); break;
		case ERROR_FTP_NO_PASSIVE_MODE:
			wcscpy(lpErrDesc, L"Passive mode is not available on the server."); break;
		case ERROR_FTP_TRANSFER_IN_PROGRESS:
			wcscpy(lpErrDesc, L"The requested operation cannot be made on the FTP session handle because an operation is already in progress."); break;
		case ERROR_GOPHER_ATTRIBUTE_NOT_FOUND:
			wcscpy(lpErrDesc, L"The requested attribute could not be located."); break;
		case ERROR_GOPHER_DATA_ERROR:
			wcscpy(lpErrDesc, L"An error was detected while receiving data from the Gopher server."); break;
		case ERROR_GOPHER_END_OF_DATA:
			wcscpy(lpErrDesc, L"The end of the data has been reached."); break;
		case ERROR_GOPHER_INCORRECT_LOCATOR_TYPE:
			wcscpy(lpErrDesc, L"The type of the locator is not correct for this operation."); break;
		case ERROR_GOPHER_INVALID_LOCATOR:
			wcscpy(lpErrDesc, L"The supplied locator is not valid."); break;
		case ERROR_GOPHER_NOT_FILE:
			wcscpy(lpErrDesc, L"The request must be made for a file locator."); break;
		case ERROR_GOPHER_NOT_GOPHER_PLUS:
			wcscpy(lpErrDesc, L"The requested operation can be made only against a Gopher+ server, or with a locator that specifies a Gopher+ operation."); break;
		case ERROR_GOPHER_PROTOCOL_ERROR:
			wcscpy(lpErrDesc, L"An error was detected while parsing data returned from the Gopher server."); break;
		case ERROR_GOPHER_UNKNOWN_LOCATOR:
			wcscpy(lpErrDesc, L"The locator type is unknown."); break;
		case ERROR_HTTP_COOKIE_DECLINED:
			wcscpy(lpErrDesc, L"The HTTP cookie was declined by the server."); break;
		case ERROR_HTTP_COOKIE_NEEDS_CONFIRMATION:
			wcscpy(lpErrDesc, L"The HTTP cookie requires confirmation."); break;
		case ERROR_HTTP_DOWNLEVEL_SERVER:
			wcscpy(lpErrDesc, L"The server did not return any headers."); break;
		case ERROR_HTTP_HEADER_ALREADY_EXISTS:
			wcscpy(lpErrDesc, L"The header could not be added because it already exists."); break;
		case ERROR_HTTP_HEADER_NOT_FOUND:
			wcscpy(lpErrDesc, L"The requested header could not be located."); break;
		case ERROR_HTTP_INVALID_HEADER:
			wcscpy(lpErrDesc, L"The supplied header is invalid."); break;
		case ERROR_HTTP_INVALID_QUERY_REQUEST:
			wcscpy(lpErrDesc, L"The request made to HttpQueryInfo is invalid."); break;
		case ERROR_HTTP_INVALID_SERVER_RESPONSE:
			wcscpy(lpErrDesc, L"The server response could not be parsed."); break;
		case ERROR_HTTP_NOT_REDIRECTED:
			wcscpy(lpErrDesc, L"The HTTP request was not redirected."); break;
		case ERROR_HTTP_REDIRECT_FAILED:
			wcscpy(lpErrDesc, L"The redirection failed because either the scheme changed (for example, HTTP to FTP) or all attempts made to redirect failed (default is five attempts)."); break;
		case ERROR_HTTP_REDIRECT_NEEDS_CONFIRMATION:
			wcscpy(lpErrDesc, L"The redirection requires user confirmation."); break;
		case ERROR_INTERNET_ASYNC_THREAD_FAILED:
			wcscpy(lpErrDesc, L"The application could not start an asynchronous thread."); break;
		case ERROR_INTERNET_BAD_AUTO_PROXY_SCRIPT:
			wcscpy(lpErrDesc, L"There was an error in the automatic proxy configuration script."); break;
		case ERROR_INTERNET_BAD_OPTION_LENGTH:
			wcscpy(lpErrDesc, L"The length of an option supplied to InternetQueryOption or InternetSetOption is incorrect for the type of option specified."); break;
		case ERROR_INTERNET_BAD_REGISTRY_PARAMETER:
			wcscpy(lpErrDesc, L"A required registry value was located but is an incorrect type or has an invalid value."); break;
		case ERROR_INTERNET_CANNOT_CONNECT:
			wcscpy(lpErrDesc, L"The attempt to connect to the server failed."); break;
		case ERROR_INTERNET_CHG_POST_IS_NON_SECURE:
			wcscpy(lpErrDesc, L"The application is posting and attempting to change multiple lines of text on a server that is not secure."); break;
		case ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED:
			wcscpy(lpErrDesc, L"The server is requesting client authentication."); break;
		case ERROR_INTERNET_CLIENT_AUTH_NOT_SETUP:
			wcscpy(lpErrDesc, L"Client authorization is not set up on this computer."); break;
		case ERROR_INTERNET_CONNECTION_ABORTED:
			wcscpy(lpErrDesc, L"The connection with the server has been terminated."); break;
		case ERROR_INTERNET_CONNECTION_RESET:
			wcscpy(lpErrDesc, L"The connection with the server has been reset."); break;
		// case ERROR_INTERNET_DECODING_FAILED:
			// wcscpy(lpErrDesc, L"WinINet failed to perform content decoding on the response. For more information, see the Content Encoding topic."); break;
		case ERROR_INTERNET_DIALOG_PENDING:
			wcscpy(lpErrDesc, L"Another thread has a password dialog box in progress."); break;
		case ERROR_INTERNET_DISCONNECTED:
			wcscpy(lpErrDesc, L"The Internet connection has been lost."); break;
		case ERROR_INTERNET_EXTENDED_ERROR:
			wcscpy(lpErrDesc, L"An extended error was returned from the server."); break;
		case ERROR_INTERNET_FAILED_DUETOSECURITYCHECK:
			wcscpy(lpErrDesc, L"The function failed due to a security check."); break;
		case ERROR_INTERNET_FORCE_RETRY:
			wcscpy(lpErrDesc, L"The function needs to redo the request."); break;
		case ERROR_INTERNET_FORTEZZA_LOGIN_NEEDED:
			wcscpy(lpErrDesc, L"The requested resource requires Fortezza authentication."); break;
		case ERROR_INTERNET_HANDLE_EXISTS:
			wcscpy(lpErrDesc, L"The request failed because the handle already exists."); break;
		case ERROR_INTERNET_HTTP_TO_HTTPS_ON_REDIR:
			wcscpy(lpErrDesc, L"The application is moving from a non-SSL to an SSL connection because of a redirect."); break;
		case ERROR_INTERNET_HTTPS_HTTP_SUBMIT_REDIR:
			wcscpy(lpErrDesc, L"The data being submitted to an SSL connection is being redirected to a non-SSL connection."); break;
		case ERROR_INTERNET_HTTPS_TO_HTTP_ON_REDIR:
			wcscpy(lpErrDesc, L"The application is moving from an SSL to an non-SSL connection because of a redirect."); break;
		case ERROR_INTERNET_INCORRECT_FORMAT:
			wcscpy(lpErrDesc, L"The format of the request is invalid."); break;
		case ERROR_INTERNET_INCORRECT_HANDLE_STATE:
			wcscpy(lpErrDesc, L"The requested operation cannot be carried out because the handle supplied is not in the correct state."); break;
		case ERROR_INTERNET_INCORRECT_HANDLE_TYPE:
			wcscpy(lpErrDesc, L"The type of handle supplied is incorrect for this operation."); break;
		case ERROR_INTERNET_INCORRECT_PASSWORD:
			wcscpy(lpErrDesc, L"The request to connect and log on to an FTP server could not be completed because the supplied password is incorrect."); break;
		case ERROR_INTERNET_INCORRECT_USER_NAME:
			wcscpy(lpErrDesc, L"The request to connect and log on to an FTP server could not be completed because the supplied user name is incorrect."); break;
		case ERROR_INTERNET_INSERT_CDROM:
			wcscpy(lpErrDesc, L"The request requires a CD-ROM to be inserted in the CD-ROM drive to locate the resource requested."); break;
		case ERROR_INTERNET_INTERNAL_ERROR:
			wcscpy(lpErrDesc, L"An internal error has occurred."); break;
		case ERROR_INTERNET_INVALID_CA:
			wcscpy(lpErrDesc, L"The function is unfamiliar with the Certificate Authority that generated the server's certificate."); break;
		case ERROR_INTERNET_INVALID_OPERATION:
			wcscpy(lpErrDesc, L"The requested operation is invalid."); break;
		case ERROR_INTERNET_INVALID_OPTION:
			wcscpy(lpErrDesc, L"A request to InternetQueryOption or InternetSetOption specified an invalid option value."); break;
		case ERROR_INTERNET_INVALID_PROXY_REQUEST:
			wcscpy(lpErrDesc, L"The request to the proxy was invalid."); break;
		case ERROR_INTERNET_INVALID_URL:
			wcscpy(lpErrDesc, L"The URL is invalid."); break;
		case ERROR_INTERNET_ITEM_NOT_FOUND:
			wcscpy(lpErrDesc, L"The requested item could not be located."); break;
		case ERROR_INTERNET_LOGIN_FAILURE:
			wcscpy(lpErrDesc, L"The request to connect and log on to an FTP server failed."); break;
		case ERROR_INTERNET_LOGIN_FAILURE_DISPLAY_ENTITY_BODY:
			wcscpy(lpErrDesc, L"The MS-Logoff digest header has been returned from the Web site. This header specifically instructs the digest package to purge credentials for the associated realm. This error will only be returned if INTERNET_ERROR_MASK_LOGIN_FAILURE_DISPLAY_ENTITY_BODY has been set."); break;
		case ERROR_INTERNET_MIXED_SECURITY:
			wcscpy(lpErrDesc, L"The content is not entirely secure. Some of the content being viewed may have come from unsecured servers."); break;
		case ERROR_INTERNET_NAME_NOT_RESOLVED:
			wcscpy(lpErrDesc, L"The server name could not be resolved."); break;
		case ERROR_INTERNET_NEED_MSN_SSPI_PKG:
			wcscpy(lpErrDesc, L"Not currently implemented."); break;
		case ERROR_INTERNET_NEED_UI:
			wcscpy(lpErrDesc, L"A user interface or other blocking operation has been requested."); break;
		case ERROR_INTERNET_NO_CALLBACK:
			wcscpy(lpErrDesc, L"An asynchronous request could not be made because a callback function has not been set."); break;
		case ERROR_INTERNET_NO_CONTEXT:
			wcscpy(lpErrDesc, L"An asynchronous request could not be made because a zero context value was supplied."); break;
		case ERROR_INTERNET_NO_DIRECT_ACCESS:
			wcscpy(lpErrDesc, L"Direct network access cannot be made at this time."); break;
		case ERROR_INTERNET_NOT_INITIALIZED:
			wcscpy(lpErrDesc, L"Initialization of the WinINet API has not occurred. Indicates that a higher-level function, such as InternetOpen, has not been called yet."); break;
		case ERROR_INTERNET_NOT_PROXY_REQUEST:
			wcscpy(lpErrDesc, L"The request cannot be made via a proxy."); break;
		case ERROR_INTERNET_OPERATION_CANCELLED:
			wcscpy(lpErrDesc, L"The operation was canceled, usually because the handle on which the request was operating was closed before the operation completed."); break;
		case ERROR_INTERNET_OPTION_NOT_SETTABLE:
			wcscpy(lpErrDesc, L"The requested option cannot be set, only queried."); break;
		case ERROR_INTERNET_OUT_OF_HANDLES:
			wcscpy(lpErrDesc, L"No more handles could be generated at this time."); break;
		case ERROR_INTERNET_POST_IS_NON_SECURE:
			wcscpy(lpErrDesc, L"The application is posting data to a server that is not secure."); break;
		case ERROR_INTERNET_PROTOCOL_NOT_FOUND:
			wcscpy(lpErrDesc, L"The requested protocol could not be located."); break;
		case ERROR_INTERNET_PROXY_SERVER_UNREACHABLE:
			wcscpy(lpErrDesc, L"The designated proxy server cannot be reached."); break;
		case ERROR_INTERNET_REDIRECT_SCHEME_CHANGE:
			wcscpy(lpErrDesc, L"The function could not handle the redirection, because the scheme changed (for example, HTTP to FTP)."); break;
		case ERROR_INTERNET_REGISTRY_VALUE_NOT_FOUND:
			wcscpy(lpErrDesc, L"A required registry value could not be located."); break;
		case ERROR_INTERNET_REQUEST_PENDING:
			wcscpy(lpErrDesc, L"The required operation could not be completed because one or more requests are pending."); break;
		case ERROR_INTERNET_RETRY_DIALOG:
			wcscpy(lpErrDesc, L"The dialog box should be retried."); break;
		case ERROR_INTERNET_SEC_CERT_CN_INVALID:
			wcscpy(lpErrDesc, L"SSL certificate common name (host name field) is incorrect—for example, if you entered www.server.com and the common name on the certificate says www.different.com."); break;
		case ERROR_INTERNET_SEC_CERT_DATE_INVALID:
			wcscpy(lpErrDesc, L"SSL certificate date that was received from the server is bad. The certificate is expired."); break;
		case ERROR_INTERNET_SEC_CERT_ERRORS:
			wcscpy(lpErrDesc, L"The SSL certificate contains errors."); break;
		case ERROR_INTERNET_SEC_CERT_NO_REV:
			wcscpy(lpErrDesc, L"SSL certificate was not revoked."); break;
		case ERROR_INTERNET_SEC_CERT_REV_FAILED:
			wcscpy(lpErrDesc, L"SSL certificate revoking failed."); break;
		case ERROR_INTERNET_SEC_CERT_REVOKED:
			wcscpy(lpErrDesc, L"SSL certificate was revoked."); break;
		case ERROR_INTERNET_SEC_INVALID_CERT:
			wcscpy(lpErrDesc, L"SSL certificate is invalid."); break;
		case ERROR_INTERNET_SECURITY_CHANNEL_ERROR:
			wcscpy(lpErrDesc, L"The application experienced an internal error loading the SSL libraries."); break;
		case ERROR_INTERNET_SERVER_UNREACHABLE:
			wcscpy(lpErrDesc, L"The Web site or server indicated is unreachable."); break;
		case ERROR_INTERNET_SHUTDOWN:
			wcscpy(lpErrDesc, L"WinINet support is being shut down or unloaded."); break;
		case ERROR_INTERNET_TCPIP_NOT_INSTALLED:
			wcscpy(lpErrDesc, L"The required protocol stack is not loaded and the application cannot start WinSock."); break;
		case ERROR_INTERNET_TIMEOUT:
			wcscpy(lpErrDesc, L"The request has timed out."); break;
		case ERROR_INTERNET_UNABLE_TO_CACHE_FILE:
			wcscpy(lpErrDesc, L"The function was unable to cache the file."); break;
		case ERROR_INTERNET_UNABLE_TO_DOWNLOAD_SCRIPT:
			wcscpy(lpErrDesc, L"The automatic proxy configuration script could not be downloaded. The INTERNET_FLAG_MUST_CACHE_REQUEST flag was set."); break;
		case ERROR_INTERNET_UNRECOGNIZED_SCHEME:
			wcscpy(lpErrDesc, L"The URL scheme could not be recognized, or is not supported."); break;
		case ERROR_INVALID_HANDLE:
			wcscpy(lpErrDesc, L"The handle that was passed to the API has been either invalidated or closed."); break;
		case ERROR_MORE_DATA:
			wcscpy(lpErrDesc, L"More data is available."); break;
		case ERROR_NO_MORE_FILES:
			wcscpy(lpErrDesc, L"No more files have been found."); break;
		case ERROR_NO_MORE_ITEMS:
			wcscpy(lpErrDesc, L"No more items have been found."); break;
		default:{
			LPVOID lpMsgBuf;
 
			FormatMessageW( 
			    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			    NULL,
			    nErr,
			    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			    (wchar_t *) &lpMsgBuf,
			    0,
			    NULL 
			);
			wcscpy(lpErrDesc, (wchar_t *)lpMsgBuf);
			// Free the buffer.
			LocalFree( lpMsgBuf );
			break;
		}
	}
}

void GetInternetErrorDescription(char * lpErrDesc, int nErr){
	if(nErr == 0)
		nErr = GetLastError();

	lpErrDesc[0] = '\0';
	switch(nErr){
		case ERROR_FTP_DROPPED:
			strcpy(lpErrDesc, "The FTP operation was not completed because the session was aborted."); break;
		case ERROR_FTP_NO_PASSIVE_MODE:
			strcpy(lpErrDesc, "Passive mode is not available on the server."); break;
		case ERROR_FTP_TRANSFER_IN_PROGRESS:
			strcpy(lpErrDesc, "The requested operation cannot be made on the FTP session handle because an operation is already in progress."); break;
		case ERROR_GOPHER_ATTRIBUTE_NOT_FOUND:
			strcpy(lpErrDesc, "The requested attribute could not be located."); break;
		case ERROR_GOPHER_DATA_ERROR:
			strcpy(lpErrDesc, "An error was detected while receiving data from the Gopher server."); break;
		case ERROR_GOPHER_END_OF_DATA:
			strcpy(lpErrDesc, "The end of the data has been reached."); break;
		case ERROR_GOPHER_INCORRECT_LOCATOR_TYPE:
			strcpy(lpErrDesc, "The type of the locator is not correct for this operation."); break;
		case ERROR_GOPHER_INVALID_LOCATOR:
			strcpy(lpErrDesc, "The supplied locator is not valid."); break;
		case ERROR_GOPHER_NOT_FILE:
			strcpy(lpErrDesc, "The request must be made for a file locator."); break;
		case ERROR_GOPHER_NOT_GOPHER_PLUS:
			strcpy(lpErrDesc, "The requested operation can be made only against a Gopher+ server, or with a locator that specifies a Gopher+ operation."); break;
		case ERROR_GOPHER_PROTOCOL_ERROR:
			strcpy(lpErrDesc, "An error was detected while parsing data returned from the Gopher server."); break;
		case ERROR_GOPHER_UNKNOWN_LOCATOR:
			strcpy(lpErrDesc, "The locator type is unknown."); break;
		case ERROR_HTTP_COOKIE_DECLINED:
			strcpy(lpErrDesc, "The HTTP cookie was declined by the server."); break;
		case ERROR_HTTP_COOKIE_NEEDS_CONFIRMATION:
			strcpy(lpErrDesc, "The HTTP cookie requires confirmation."); break;
		case ERROR_HTTP_DOWNLEVEL_SERVER:
			strcpy(lpErrDesc, "The server did not return any headers."); break;
		case ERROR_HTTP_HEADER_ALREADY_EXISTS:
			strcpy(lpErrDesc, "The header could not be added because it already exists."); break;
		case ERROR_HTTP_HEADER_NOT_FOUND:
			strcpy(lpErrDesc, "The requested header could not be located."); break;
		case ERROR_HTTP_INVALID_HEADER:
			strcpy(lpErrDesc, "The supplied header is invalid."); break;
		case ERROR_HTTP_INVALID_QUERY_REQUEST:
			strcpy(lpErrDesc, "The request made to HttpQueryInfo is invalid."); break;
		case ERROR_HTTP_INVALID_SERVER_RESPONSE:
			strcpy(lpErrDesc, "The server response could not be parsed."); break;
		case ERROR_HTTP_NOT_REDIRECTED:
			strcpy(lpErrDesc, "The HTTP request was not redirected."); break;
		case ERROR_HTTP_REDIRECT_FAILED:
			strcpy(lpErrDesc, "The redirection failed because either the scheme changed (for example, HTTP to FTP) or all attempts made to redirect failed (default is five attempts)."); break;
		case ERROR_HTTP_REDIRECT_NEEDS_CONFIRMATION:
			strcpy(lpErrDesc, "The redirection requires user confirmation."); break;
		case ERROR_INTERNET_ASYNC_THREAD_FAILED:
			strcpy(lpErrDesc, "The application could not start an asynchronous thread."); break;
		case ERROR_INTERNET_BAD_AUTO_PROXY_SCRIPT:
			strcpy(lpErrDesc, "There was an error in the automatic proxy configuration script."); break;
		case ERROR_INTERNET_BAD_OPTION_LENGTH:
			strcpy(lpErrDesc, "The length of an option supplied to InternetQueryOption or InternetSetOption is incorrect for the type of option specified."); break;
		case ERROR_INTERNET_BAD_REGISTRY_PARAMETER:
			strcpy(lpErrDesc, "A required registry value was located but is an incorrect type or has an invalid value."); break;
		case ERROR_INTERNET_CANNOT_CONNECT:
			strcpy(lpErrDesc, "The attempt to connect to the server failed."); break;
		case ERROR_INTERNET_CHG_POST_IS_NON_SECURE:
			strcpy(lpErrDesc, "The application is posting and attempting to change multiple lines of text on a server that is not secure."); break;
		case ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED:
			strcpy(lpErrDesc, "The server is requesting client authentication."); break;
		case ERROR_INTERNET_CLIENT_AUTH_NOT_SETUP:
			strcpy(lpErrDesc, "Client authorization is not set up on this computer."); break;
		case ERROR_INTERNET_CONNECTION_ABORTED:
			strcpy(lpErrDesc, "The connection with the server has been terminated."); break;
		case ERROR_INTERNET_CONNECTION_RESET:
			strcpy(lpErrDesc, "The connection with the server has been reset."); break;
		// case ERROR_INTERNET_DECODING_FAILED:
			// strcpy(lpErrDesc, "WinINet failed to perform content decoding on the response. For more information, see the Content Encoding topic."); break;
		case ERROR_INTERNET_DIALOG_PENDING:
			strcpy(lpErrDesc, "Another thread has a password dialog box in progress."); break;
		case ERROR_INTERNET_DISCONNECTED:
			strcpy(lpErrDesc, "The Internet connection has been lost."); break;
		case ERROR_INTERNET_EXTENDED_ERROR:
			strcpy(lpErrDesc, "An extended error was returned from the server."); break;
		case ERROR_INTERNET_FAILED_DUETOSECURITYCHECK:
			strcpy(lpErrDesc, "The function failed due to a security check."); break;
		case ERROR_INTERNET_FORCE_RETRY:
			strcpy(lpErrDesc, "The function needs to redo the request."); break;
		case ERROR_INTERNET_FORTEZZA_LOGIN_NEEDED:
			strcpy(lpErrDesc, "The requested resource requires Fortezza authentication."); break;
		case ERROR_INTERNET_HANDLE_EXISTS:
			strcpy(lpErrDesc, "The request failed because the handle already exists."); break;
		case ERROR_INTERNET_HTTP_TO_HTTPS_ON_REDIR:
			strcpy(lpErrDesc, "The application is moving from a non-SSL to an SSL connection because of a redirect."); break;
		case ERROR_INTERNET_HTTPS_HTTP_SUBMIT_REDIR:
			strcpy(lpErrDesc, "The data being submitted to an SSL connection is being redirected to a non-SSL connection."); break;
		case ERROR_INTERNET_HTTPS_TO_HTTP_ON_REDIR:
			strcpy(lpErrDesc, "The application is moving from an SSL to an non-SSL connection because of a redirect."); break;
		case ERROR_INTERNET_INCORRECT_FORMAT:
			strcpy(lpErrDesc, "The format of the request is invalid."); break;
		case ERROR_INTERNET_INCORRECT_HANDLE_STATE:
			strcpy(lpErrDesc, "The requested operation cannot be carried out because the handle supplied is not in the correct state."); break;
		case ERROR_INTERNET_INCORRECT_HANDLE_TYPE:
			strcpy(lpErrDesc, "The type of handle supplied is incorrect for this operation."); break;
		case ERROR_INTERNET_INCORRECT_PASSWORD:
			strcpy(lpErrDesc, "The request to connect and log on to an FTP server could not be completed because the supplied password is incorrect."); break;
		case ERROR_INTERNET_INCORRECT_USER_NAME:
			strcpy(lpErrDesc, "The request to connect and log on to an FTP server could not be completed because the supplied user name is incorrect."); break;
		case ERROR_INTERNET_INSERT_CDROM:
			strcpy(lpErrDesc, "The request requires a CD-ROM to be inserted in the CD-ROM drive to locate the resource requested."); break;
		case ERROR_INTERNET_INTERNAL_ERROR:
			strcpy(lpErrDesc, "An internal error has occurred."); break;
		case ERROR_INTERNET_INVALID_CA:
			strcpy(lpErrDesc, "The function is unfamiliar with the Certificate Authority that generated the server's certificate."); break;
		case ERROR_INTERNET_INVALID_OPERATION:
			strcpy(lpErrDesc, "The requested operation is invalid."); break;
		case ERROR_INTERNET_INVALID_OPTION:
			strcpy(lpErrDesc, "A request to InternetQueryOption or InternetSetOption specified an invalid option value."); break;
		case ERROR_INTERNET_INVALID_PROXY_REQUEST:
			strcpy(lpErrDesc, "The request to the proxy was invalid."); break;
		case ERROR_INTERNET_INVALID_URL:
			strcpy(lpErrDesc, "The URL is invalid."); break;
		case ERROR_INTERNET_ITEM_NOT_FOUND:
			strcpy(lpErrDesc, "The requested item could not be located."); break;
		case ERROR_INTERNET_LOGIN_FAILURE:
			strcpy(lpErrDesc, "The request to connect and log on to an FTP server failed."); break;
		case ERROR_INTERNET_LOGIN_FAILURE_DISPLAY_ENTITY_BODY:
			strcpy(lpErrDesc, "The MS-Logoff digest header has been returned from the Web site. This header specifically instructs the digest package to purge credentials for the associated realm. This error will only be returned if INTERNET_ERROR_MASK_LOGIN_FAILURE_DISPLAY_ENTITY_BODY has been set."); break;
		case ERROR_INTERNET_MIXED_SECURITY:
			strcpy(lpErrDesc, "The content is not entirely secure. Some of the content being viewed may have come from unsecured servers."); break;
		case ERROR_INTERNET_NAME_NOT_RESOLVED:
			strcpy(lpErrDesc, "The server name could not be resolved."); break;
		case ERROR_INTERNET_NEED_MSN_SSPI_PKG:
			strcpy(lpErrDesc, "Not currently implemented."); break;
		case ERROR_INTERNET_NEED_UI:
			strcpy(lpErrDesc, "A user interface or other blocking operation has been requested."); break;
		case ERROR_INTERNET_NO_CALLBACK:
			strcpy(lpErrDesc, "An asynchronous request could not be made because a callback function has not been set."); break;
		case ERROR_INTERNET_NO_CONTEXT:
			strcpy(lpErrDesc, "An asynchronous request could not be made because a zero context value was supplied."); break;
		case ERROR_INTERNET_NO_DIRECT_ACCESS:
			strcpy(lpErrDesc, "Direct network access cannot be made at this time."); break;
		case ERROR_INTERNET_NOT_INITIALIZED:
			strcpy(lpErrDesc, "Initialization of the WinINet API has not occurred. Indicates that a higher-level function, such as InternetOpen, has not been called yet."); break;
		case ERROR_INTERNET_NOT_PROXY_REQUEST:
			strcpy(lpErrDesc, "The request cannot be made via a proxy."); break;
		case ERROR_INTERNET_OPERATION_CANCELLED:
			strcpy(lpErrDesc, "The operation was canceled, usually because the handle on which the request was operating was closed before the operation completed."); break;
		case ERROR_INTERNET_OPTION_NOT_SETTABLE:
			strcpy(lpErrDesc, "The requested option cannot be set, only queried."); break;
		case ERROR_INTERNET_OUT_OF_HANDLES:
			strcpy(lpErrDesc, "No more handles could be generated at this time."); break;
		case ERROR_INTERNET_POST_IS_NON_SECURE:
			strcpy(lpErrDesc, "The application is posting data to a server that is not secure."); break;
		case ERROR_INTERNET_PROTOCOL_NOT_FOUND:
			strcpy(lpErrDesc, "The requested protocol could not be located."); break;
		case ERROR_INTERNET_PROXY_SERVER_UNREACHABLE:
			strcpy(lpErrDesc, "The designated proxy server cannot be reached."); break;
		case ERROR_INTERNET_REDIRECT_SCHEME_CHANGE:
			strcpy(lpErrDesc, "The function could not handle the redirection, because the scheme changed (for example, HTTP to FTP)."); break;
		case ERROR_INTERNET_REGISTRY_VALUE_NOT_FOUND:
			strcpy(lpErrDesc, "A required registry value could not be located."); break;
		case ERROR_INTERNET_REQUEST_PENDING:
			strcpy(lpErrDesc, "The required operation could not be completed because one or more requests are pending."); break;
		case ERROR_INTERNET_RETRY_DIALOG:
			strcpy(lpErrDesc, "The dialog box should be retried."); break;
		case ERROR_INTERNET_SEC_CERT_CN_INVALID:
			strcpy(lpErrDesc, "SSL certificate common name (host name field) is incorrect—for example, if you entered www.server.com and the common name on the certificate says www.different.com."); break;
		case ERROR_INTERNET_SEC_CERT_DATE_INVALID:
			strcpy(lpErrDesc, "SSL certificate date that was received from the server is bad. The certificate is expired."); break;
		case ERROR_INTERNET_SEC_CERT_ERRORS:
			strcpy(lpErrDesc, "The SSL certificate contains errors."); break;
		case ERROR_INTERNET_SEC_CERT_NO_REV:
			strcpy(lpErrDesc, "SSL certificate was not revoked."); break;
		case ERROR_INTERNET_SEC_CERT_REV_FAILED:
			strcpy(lpErrDesc, "SSL certificate revoking failed."); break;
		case ERROR_INTERNET_SEC_CERT_REVOKED:
			strcpy(lpErrDesc, "SSL certificate was revoked."); break;
		case ERROR_INTERNET_SEC_INVALID_CERT:
			strcpy(lpErrDesc, "SSL certificate is invalid."); break;
		case ERROR_INTERNET_SECURITY_CHANNEL_ERROR:
			strcpy(lpErrDesc, "The application experienced an internal error loading the SSL libraries."); break;
		case ERROR_INTERNET_SERVER_UNREACHABLE:
			strcpy(lpErrDesc, "The Web site or server indicated is unreachable."); break;
		case ERROR_INTERNET_SHUTDOWN:
			strcpy(lpErrDesc, "WinINet support is being shut down or unloaded."); break;
		case ERROR_INTERNET_TCPIP_NOT_INSTALLED:
			strcpy(lpErrDesc, "The required protocol stack is not loaded and the application cannot start WinSock."); break;
		case ERROR_INTERNET_TIMEOUT:
			strcpy(lpErrDesc, "The request has timed out."); break;
		case ERROR_INTERNET_UNABLE_TO_CACHE_FILE:
			strcpy(lpErrDesc, "The function was unable to cache the file."); break;
		case ERROR_INTERNET_UNABLE_TO_DOWNLOAD_SCRIPT:
			strcpy(lpErrDesc, "The automatic proxy configuration script could not be downloaded. The INTERNET_FLAG_MUST_CACHE_REQUEST flag was set."); break;
		case ERROR_INTERNET_UNRECOGNIZED_SCHEME:
			strcpy(lpErrDesc, "The URL scheme could not be recognized, or is not supported."); break;
		case ERROR_INVALID_HANDLE:
			strcpy(lpErrDesc, "The handle that was passed to the API has been either invalidated or closed."); break;
		case ERROR_MORE_DATA:
			strcpy(lpErrDesc, "More data is available."); break;
		case ERROR_NO_MORE_FILES:
			strcpy(lpErrDesc, "No more files have been found."); break;
		case ERROR_NO_MORE_ITEMS:
			strcpy(lpErrDesc, "No more items have been found."); break;
	}
}
