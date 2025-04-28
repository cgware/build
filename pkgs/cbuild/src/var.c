#include "var.h"

#include "log.h"

#define MIN_VAR_NAME_LEN 4

static const strv_t var_names[__VAR_CNT] = {
	[VAR_ARCH]   = STRVT("ARCH"),
	[VAR_CONFIG] = STRVT("CONFIG"),
};

int var_replace(str_t *str, const strv_t *values)
{
	if (str == NULL || str->data == NULL || values == NULL) {
		return 1;
	}

	for (size_t s = 0; str->len >= MIN_VAR_NAME_LEN + 3 && s <= str->len - MIN_VAR_NAME_LEN - 3; s++) {
		if (str->data[s] != '$' || str->data[s + 1] != '{') {
			continue;
		}

		size_t e = s + 2;
		while (e < str->len && str->data[e] != '}') {
			e++;
		}

		if (e >= str->len) {
			break;
		}

		strv_t name = STRVN(&str->data[s + 2], e - (s + 2));

		size_t v = 0;
		while (v < __VAR_CNT && !strv_eq(name, var_names[v])) {
			v++;
		}

		if (v >= __VAR_CNT) {
			continue;
		}

		if (str_subreplace(str, s, e + 1, values[v])) {
			log_error("build",
				  "var",
				  NULL,
				  "failed to replace %.*s with value: '%.*s'",
				  var_names[v].len,
				  var_names[v].data,
				  values[v].len,
				  values[v].data);
			return 1;
		}
		s--;
	}

	((char *)str->data)[str->len] = '\0';
	return 0;
}
