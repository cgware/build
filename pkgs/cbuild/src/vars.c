#include "vars.h"

#include "log.h"

#define MIN_VAR_NAME_LEN 2

typedef struct svar_s {
	strv_t name;
	strv_t val;
} svar_t;

#define PDVAR(_name)	   [_name] = {STRVT(#_name), {0}}
#define PSVAR(_name, _val) [_name] = {STRVT(#_name), STRVT(_val)}

static const svar_t s_vars[__VARS_CNT] = {
	PDVAR(ARCH),
	PDVAR(CONFIG),
	PDVAR(PN),
	PDVAR(TN),
	// PROJ
	PSVAR(DIR_PROJ, ""),
	PSVAR(DIR_BUILD, ""),
	// PKG
	PSVAR(PKG_DIR, "${${PN}_DIR}"),
	PSVAR(PKG_URI, "${${PN}_URI}"),
	PSVAR(PKG_URI_FILE, "${${PN}_URI_FILE}"),
	PSVAR(PKG_URI_ROOT, "${${PN}_URI_ROOT}"),
	// TGT
	PSVAR(TGT_CMD, "${${PN}_${TN}_CMD}"),
	PSVAR(TGT_OUT, "${${PN}_${TN}_OUT}"),
	PSVAR(TGT_DST, "${${PN}_${TN}_DST}"),
	// DIR_TMP
	PSVAR(DIR_TMP, "${DIR_PROJ}tmp/"),
	PSVAR(DIR_TMP_EXT, "${DIR_TMP}ext/"),
	PSVAR(DIR_TMP_EXT_PKG, "${DIR_TMP_EXT}${PKG_DIR}"),
	PSVAR(DIR_TMP_EXT_PKG_ROOT, "${DIR_TMP_EXT_PKG}${PKG_URI_ROOT}"),
	PSVAR(DIR_TMP_EXT_PKG_ROOT_OUT, "${DIR_TMP_EXT_PKG_ROOT}${TGT_OUT}"),
	PSVAR(DIR_TMP_REP, "${DIR_TMP}report/"),
	PSVAR(DIR_TMP_COV, "${DIR_TMP_REP}cov/"),
	PSVAR(DIR_TMP_DL, "${DIR_TMP}dl/"),
	PSVAR(DIR_TMP_DL_PKG, "${DIR_TMP_DL}${PKG_DIR}"),
	// DIR_PKG
	PSVAR(DIR_PKG, "${DIR_PROJ}${PKG_DIR}"),
	PSVAR(DIR_PKG_SRC, "${DIR_PKG}src/"),
	PSVAR(DIR_PKG_INC, "${DIR_PKG}include/"),
	PSVAR(DIR_PKG_DRV, "${DIR_PKG}drivers/"),
	PSVAR(DIR_PKG_DRV_C, "${DIR_PKG_DRV}*.c"),
	PSVAR(DIR_PKG_TST, "${DIR_PKG}test/"),
	PDVAR(PN_DRIVERS),
	// DIR_OUT
	PSVAR(DIR_OUT, "${DIR_PROJ}bin/${ARCH}-${CONFIG}/"),
	PSVAR(DIR_OUT_INT, "${DIR_OUT}int/"),
	PSVAR(DIR_OUT_INT_SRC, "${DIR_OUT_INT}${PN}/src/"),
	PSVAR(DIR_OUT_INT_TST, "${DIR_OUT_INT}${PN}/test/"),
	PSVAR(DIR_OUT_LIB, "${DIR_OUT}lib/"),
	PSVAR(DIR_OUT_LIB_FILE, "${DIR_OUT_LIB}${PN}.a"),
	PSVAR(DIR_OUT_DRV, "${DIR_OUT}drivers/"),
	PSVAR(DIR_OUT_DRV_PKG, "${DIR_OUT_DRV}${PN}/"),
	PSVAR(DIR_OUT_BIN, "${DIR_OUT}bin/"),
	PSVAR(DIR_OUT_BIN_FILE, "${DIR_OUT_BIN}${PN}"),
	PSVAR(DIR_OUT_EXT, "${DIR_OUT}ext/"),
	PSVAR(DIR_OUT_EXT_PKG, "${DIR_OUT_EXT}${PN}/"),
	PSVAR(ABS_DIR_OUT_EXT_PKG, "${DIR_OUT_EXT_PKG}"),
	PSVAR(DIR_OUT_EXT_FILE, "${DIR_OUT_EXT_PKG}${TGT_DST}"),
	PSVAR(DIR_OUT_TST, "${DIR_OUT}test/"),
	PSVAR(DIR_OUT_TST_FILE, "${DIR_OUT_TST}${PN}"),
};

static var_name_t get_var(const vars_t *vars, strv_t str, size_t s)
{
	if (str.data[s] != '$' || str.data[s + 1] != '{') {
		return __VARS_CNT;
	}

	size_t e = s + 2;
	while (e < str.len && str.data[e] != '}') {
		e++;
	}

	if (e >= str.len) {
		return __VARS_CNT;
	}

	strv_t name = STRVN(&str.data[s + 2], e - (s + 2));

	var_name_t v = 0;
	while (v < __VARS_CNT && !strv_eq(name, vars->vars[v].name)) {
		v++;
	}

	return v;
}

vars_t *vars_init(vars_t *vars)
{
	if (vars == NULL) {
		return NULL;
	}

	for (uint i = 0; i < __VARS_CNT; i++) {
		vars->vars[i].name = s_vars[i].name;
		vars->vars[i].val  = s_vars[i].val;
		vars->vars[i].deps = 0;

		for (size_t s = 0; vars->vars[i].val.len >= MIN_VAR_NAME_LEN + 3 && s <= vars->vars[i].val.len - MIN_VAR_NAME_LEN - 3;
		     s++) {
			var_name_t v = get_var(vars, vars->vars[i].val, s);
			if (v >= __VARS_CNT) {
				continue;
			}

			vars->vars[i].deps |= 1ULL << v;
			vars->vars[i].deps |= vars->vars[v].deps;
		}
	}

	return vars;
}

int vars_replace(const vars_t *vars, str_t *str, const strv_t *values)
{
	if (str == NULL || str->data == NULL || values == NULL) {
		return 1;
	}

	for (size_t s = 0; str->len >= MIN_VAR_NAME_LEN + 3 && s <= str->len - MIN_VAR_NAME_LEN - 3; s++) {
		var_name_t v = get_var(vars, STRVS(*str), s);
		if (v >= __VARS_CNT) {
			continue;
		}

		if (values[v].data == NULL) {
			continue;
		}

		if (strv_eq(STRVN(&str->data[s], vars->vars[v].name.len + 3), values[v])) {
			continue;
		}

		if (str_subreplace(str, s, s + vars->vars[v].name.len + 3, values[v])) {
			log_error("build",
				  "var",
				  NULL,
				  "failed to replace %.*s with value: '%.*s'",
				  vars->vars[v].name.len,
				  vars->vars[v].name.data,
				  values[v].len,
				  values[v].data);
			return 1;
		}
		s--;
	}

	((char *)str->data)[str->len] = '\0';
	return 0;
}

int var_convert(str_t *str, char froms, char frome, char tos, char toe)
{
	if (str == NULL) {
		return 1;
	}

	for (size_t s = 0; str->len >= 3 && s < str->len - 2; s++) {
		if (str->data[s] != '$' || str->data[s + 1] != froms) {
			continue;
		}

		int cnt = 1;

		for (size_t e = s + 2; e < str->len; e++) {
			if (e + 1 < str->len && str->data[e] == '$' && str->data[e + 1] == froms) {
				cnt++;
				e++;
				continue;
			}

			if (str->data[e] == frome) {
				cnt--;
				if (cnt == 0) {
					str->data[s + 1] = tos;
					str->data[e]	 = toe;
					break;
				}
			}
		}
	}

	return 0;
}
