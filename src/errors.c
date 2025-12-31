#include <ddnet_protocol/errors.h>

const char *ddproto_error_str(DDProtoError err) {
	switch(err) {
#define X(err) \
	case err: return #err;
		DDPROTO_ERRORS
#undef X
	default:
		__builtin_unreachable();
	}
}
