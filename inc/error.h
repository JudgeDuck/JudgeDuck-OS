/* See COPYRIGHT for copyright information. */

#ifndef JOS_INC_ERROR_H
#define JOS_INC_ERROR_H

enum {
	// Kernel error codes -- keep in sync with list in lib/printfmt.c.
	E_UNSPECIFIED	= 1,	// Unspecified or unknown problem
	E_INVAL		,	// Invalid parameter
	E_NOMEM	,	// Request failed due to memory shortage
	E_FAULT		,	// Memory fault

	MAXERROR
};

#endif	// !JOS_INC_ERROR_H */
