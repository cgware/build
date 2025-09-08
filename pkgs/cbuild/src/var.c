#include "var.h"

#include "log.h"

#define MIN_VAR_NAME_LEN 4

#define PVAR_NULL(_name)	      [_name] = {STRVT(#_name)}
#define PVAR(_name, _val, _pkg, _tgt) [_name] = {STRVT(#_name), STRVT(_val), _pkg, _tgt}

const var_t g_vars[__VARS_CNT] = {
	PVAR_NULL(ARCH),
	PVAR_NULL(CONFIG),
	// PROJ
	PVAR(DIR_PROJ, "", 0, 0),
	PVAR(DIR_BUILD, "", 0, 0),
	// PKG
	PVAR(PKG_DIR, "${${PN}_DIR}", 1, 0),
	PVAR(PKG_URI, "${${PN}_URI}", 1, 0),
	PVAR(PKG_DLFILE, "${${PN}_DLFILE}", 1, 0),
	PVAR(PKG_DLROOT, "${${PN}_DLROOT}", 1, 0),
	// TGT
	PVAR(TGT_CMD, "${${PN}_${TN}_CMD}", 1, 1),
	PVAR(TGT_OUT, "${${PN}_${TN}_OUT}", 1, 1),
	// DIR_TMP
	PVAR(DIR_TMP, "${DIR_PROJ}tmp/", 0, 0),
	PVAR(DIR_TMP_EXT, "${DIR_TMP}ext/", 0, 0),
	PVAR(DIR_TMP_EXT_PKG, "${DIR_TMP_EXT}${PKG_DIR}", 1, 0),
	PVAR(DIR_TMP_EXT_PKG_ROOT, "${DIR_TMP_EXT_PKG}${PKG_DLROOT}", 1, 0),
	PVAR(DIR_TMP_EXT_PKG_ROOT_OUT, "${DIR_TMP_EXT_PKG_ROOT}${TGT_OUT}", 1, 1),
	PVAR(DIR_TMP_REP, "${DIR_TMP}report/", 0, 0),
	PVAR(DIR_TMP_COV, "${DIR_TMP_REP}cov/", 0, 0),
	PVAR(DIR_TMP_DL, "${DIR_TMP}dl/", 0, 0),
	PVAR(DIR_TMP_DL_PKG, "${DIR_TMP_DL}${PKG_DIR}", 1, 0),
	// DIR_PKG
	PVAR(DIR_PKG, "${DIR_PROJ}${PKG_DIR}", 1, 0),
	PVAR(DIR_PKG_SRC, "${DIR_PKG}src/", 1, 0),
	PVAR(DIR_PKG_INC, "${DIR_PKG}include/", 1, 0),
	PVAR(DIR_PKG_DRV, "${DIR_PKG}drivers/", 1, 0),
	PVAR(DIR_PKG_TST, "${DIR_PKG}test/", 1, 0),
	// DIR_OUT
	PVAR(DIR_OUT, "${DIR_PROJ}bin/${ARCH}-${CONFIG}/", 0, 0),
	PVAR(DIR_OUT_INT, "${DIR_OUT}int/", 0, 0),
	PVAR(DIR_OUT_INT_SRC, "${DIR_OUT_INT}${PN}/src/", 1, 0),
	PVAR(DIR_OUT_INT_DRV, "${DIR_OUT_INT}${PN}/drivers/", 1, 0),
	PVAR(DIR_OUT_INT_TST, "${DIR_OUT_INT}${PN}/test/", 1, 0),
	PVAR(DIR_OUT_LIB, "${DIR_OUT}lib/", 0, 0),
	PVAR(DIR_OUT_LIB_FILE, "${DIR_OUT_LIB}${PN}.a", 1, 0),
	PVAR(DIR_OUT_BIN, "${DIR_OUT}bin/", 0, 0),
	PVAR(DIR_OUT_BIN_FILE, "${DIR_OUT_BIN}${PN}", 1, 0),
	PVAR(DIR_OUT_EXT, "${DIR_OUT}ext/", 0, 0),
	PVAR(DIR_OUT_EXT_PKG, "${DIR_OUT_EXT}${PN}/", 1, 0),
	PVAR(DIR_OUT_EXT_FILE, "${DIR_OUT_EXT_PKG}${TN}", 1, 1),
	PVAR(DIR_OUT_TST, "${DIR_OUT}test/", 0, 0),
	PVAR(DIR_OUT_TST_FILE, "${DIR_OUT_TST}${PN}", 1, 0),
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
		while (v < __VARS_CNT && !strv_eq(name, g_vars[v].name)) {
			v++;
		}

		if (v >= __VARS_CNT) {
			continue;
		}

		if (values[v].data == NULL) {
			continue;
		}

		if (strv_eq(STRVN(&str->data[s], e + 1 - s), values[v])) {
			continue;
		}

		if (str_subreplace(str, s, e + 1, values[v])) {
			log_error("build",
				  "var",
				  NULL,
				  "failed to replace %.*s with value: '%.*s'",
				  g_vars[v].name.len,
				  g_vars[v].name.data,
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
