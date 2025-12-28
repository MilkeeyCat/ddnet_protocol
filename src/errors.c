#include <ddnet_protocol/errors.h>

const char *ddnet_error_str(DDNetError err) {
	switch(err) {
#define X(err) \
	case err: return #err;
		DDNET_ERRORS
#undef X
	default:
		__builtin_unreachable();
	}
}
