#include "config.h"

#include "log.h"

config_t *config_init(config_t *config, uint dirs_cap, uint pkgs_cap, uint targets_cap, alloc_t alloc)
{
	if (config == NULL) {
		return NULL;
	}

	if (strbuf_init(&config->strs,
			__CONFIG_DIR_STR_CNT * dirs_cap + __CONFIG_PKG_STR_CNT * pkgs_cap + __CONFIG_TARGET_STR_CNT * targets_cap,
			16,
			alloc) == NULL ||
	    arr_init(&config->dirs, dirs_cap, sizeof(config_dir_t), alloc) == NULL ||
	    list_init(&config->pkgs, pkgs_cap, sizeof(config_pkg_t), alloc) == NULL ||
	    list_init(&config->targets, targets_cap, sizeof(config_target_t), alloc) == NULL ||
	    list_init(&config->deps, pkgs_cap, sizeof(uint), alloc) == NULL) {
		return NULL;
	}

	return config;
}

void config_free(config_t *config)
{
	if (config == NULL) {
		return;
	}

	list_free(&config->deps);
	list_free(&config->targets);
	list_free(&config->pkgs);
	arr_free(&config->dirs);
	strbuf_free(&config->strs);
}

config_dir_t *config_add_dir(config_t *config, uint *id)
{
	if (config == NULL) {
		return NULL;
	}

	uint tmp;
	config_dir_t *dir = arr_add(&config->dirs, &tmp);

	uint strs_cnt = config->strs.off.cnt;

	for (uint i = 0; i < __CONFIG_DIR_STR_CNT; i++) {
		if (strbuf_add(&config->strs, STRV_NULL, NULL)) {
			arr_reset(&config->dirs, tmp);
			strbuf_reset(&config->strs, strs_cnt);
			log_error("cbuild", "config", NULL, "failed to add directory strings");
			return NULL;
		}
	}

	dir->strs     = strs_cnt;
	dir->has_pkgs = 0;
	dir->has_main = 0;

	id ? *id = tmp : (uint)0;

	return dir;
}

config_dir_t *config_get_dir(config_t *config, uint id)
{
	if (config == NULL) {
		return NULL;
	}

	config_dir_t *dir = arr_get(&config->dirs, id);
	if (dir == NULL) {
		log_error("cbuild", "config", NULL, "failed to get directory");
		return NULL;
	}

	return dir;
}

config_pkg_t *config_add_pkg(config_t *config, list_node_t dir, list_node_t *id)
{
	if (config == NULL) {
		return NULL;
	}

	config_dir_t *d = arr_get(&config->dirs, dir);
	if (d == NULL) {
		log_error("cbuild", "config", NULL, "failed to get directory");
		return NULL;
	}

	list_node_t tmp;
	config_pkg_t *pkg = list_node(&config->pkgs, &tmp);

	uint strs_cnt = config->strs.off.cnt;

	for (uint i = 0; i < __CONFIG_PKG_STR_CNT; i++) {
		if (strbuf_add(&config->strs, STRV_NULL, NULL)) {
			list_reset(&config->pkgs, tmp);
			strbuf_reset(&config->strs, strs_cnt);
			log_error("cbuild", "config", NULL, "failed to add package strings");
			return NULL;
		}
	}

	pkg->strs	 = strs_cnt;
	pkg->has_targets = 0;
	pkg->has_deps	 = 0;

	if (d->has_pkgs) {
		list_app(&config->pkgs, d->pkgs, tmp);
	} else {
		d->pkgs	    = tmp;
		d->has_pkgs = 1;
	}

	id ? *id = tmp : (uint)0;

	return pkg;
}

config_pkg_t *config_get_pkg(config_t *config, list_node_t id)
{
	if (config == NULL) {
		return NULL;
	}

	config_pkg_t *pkg = list_get(&config->pkgs, id);
	if (pkg == NULL) {
		log_error("cbuild", "config", NULL, "failed to get package");
		return NULL;
	}

	return pkg;
}

config_target_t *config_add_target(config_t *config, list_node_t pkg, list_node_t *id)
{
	if (config == NULL) {
		return NULL;
	}

	config_pkg_t *p = list_get(&config->pkgs, pkg);
	if (p == NULL) {
		log_error("cbuild", "config", NULL, "failed to get package");
		return NULL;
	}

	list_node_t tmp;
	config_target_t *target = list_node(&config->targets, &tmp);
	if (target == NULL) {
		log_error("cbuild", "config", NULL, "failed to add target");
		return NULL;
	}

	uint strs_cnt = config->strs.off.cnt;

	for (uint i = 0; i < __CONFIG_TARGET_STR_CNT; i++) {
		if (strbuf_add(&config->strs, STRV_NULL, NULL)) {
			list_reset(&config->targets, tmp);
			strbuf_reset(&config->strs, strs_cnt);
			log_error("cbuild", "config", NULL, "failed to add target strings");
			return NULL;
		}
	}

	target->strs = strs_cnt;
	target->out_type = CONFIG_TARGET_OUT_TYPE_UNKNOWN;

	if (p->has_targets) {
		list_app(&config->targets, p->targets, tmp);
	} else {
		p->targets     = tmp;
		p->has_targets = 1;
	}

	id ? *id = tmp : (uint)0;

	return target;
}

config_target_t *config_get_target(config_t *config, list_node_t id)
{
	if (config == NULL) {
		return NULL;
	}

	config_target_t *target = list_get(&config->targets, id);
	if (target == NULL) {
		log_error("cbuild", "config", NULL, "failed to get target");
		return NULL;
	}

	return target;
}

int config_add_dep(config_t *config, list_node_t pkg, strv_t dep)
{
	if (config == NULL) {
		return 1;
	}

	config_pkg_t *p = list_get(&config->pkgs, pkg);
	if (p == NULL) {
		log_error("cbuild", "config", NULL, "failed to get package");
		return 1;
	}

	list_node_t dep_id;
	uint *data = list_node(&config->deps, &dep_id);
	if (data == NULL) {
		return 1;
	}

	if (p->has_deps) {
		list_app(&config->deps, p->deps, dep_id);
	} else {
		p->deps	    = dep_id;
		p->has_deps = 1;
	}

	strbuf_add(&config->strs, dep, data);

	return 0;
}

int config_set_str(config_t *config, uint id, strv_t val)
{
	if (config == NULL) {
		return 1;
	}

	if (strbuf_set(&config->strs, id, val)) {
		log_error("cbuild", "config", NULL, "failed to set string: %d: '%.*s'", id, val.len, val.data);
		return 1;
	}

	return 0;
}

strv_t config_get_str(const config_t *config, uint id)
{
	if (config == NULL) {
		return STRV_NULL;
	}

	return strbuf_get(&config->strs, id);
}

size_t config_print(const config_t *config, dst_t dst)
{
	if (config == NULL) {
		return 0;
	}

	size_t off = dst.off;

	uint i = 0;
	const config_dir_t *dir;
	arr_foreach(&config->dirs, i, dir)
	{
		strv_t dir_name = config_get_str(config, dir->strs + CONFIG_DIR_NAME);
		strv_t dir_path = config_get_str(config, dir->strs + CONFIG_DIR_PATH);
		strv_t dir_src	= config_get_str(config, dir->strs + CONFIG_DIR_SRC);
		strv_t dir_inc	= config_get_str(config, dir->strs + CONFIG_DIR_INC);
		strv_t dir_drv	= config_get_str(config, dir->strs + CONFIG_DIR_DRV);
		strv_t dir_tst	= config_get_str(config, dir->strs + CONFIG_DIR_TST);

		dst.off += dputf(dst, "[dir]\n");

		dst.off += dputf(dst,
				 "NAME: %.*s\n"
				 "PATH: %.*s\n"
				 "SRC: %.*s\n"
				 "MAIN: %d\n"
				 "INC: %.*s\n"
				 "DRV: %.*s\n"
				 "TEST: %.*s\n",
				 dir_name.len,
				 dir_name.data,
				 dir_path.len,
				 dir_path.data,
				 dir_src.len,
				 dir_src.data,
				 dir->has_main,
				 dir_inc.len,
				 dir_inc.data,
				 dir_drv.len,
				 dir_drv.data,
				 dir_tst.len,
				 dir_tst.data);

		dst.off += dputf(dst, "\n");

		if (dir->has_pkgs) {
			const config_pkg_t *pkg;
			list_node_t pkgs = dir->pkgs;
			list_foreach(&config->pkgs, pkgs, pkg)
			{
				dst.off += dputf(dst, "[pkg]\n");
				strv_t pkg_name = config_get_str(config, pkg->strs + CONFIG_PKG_NAME);
				strv_t uri	= config_get_str(config, pkg->strs + CONFIG_PKG_URI);
				strv_t inc	= config_get_str(config, pkg->strs + CONFIG_PKG_INC);
				dst.off += dputf(dst,
						 "NAME: %.*s\n"
						 "URI: %.*s\n"
						 "INC: %.*s\n",
						 pkg_name.len,
						 pkg_name.data,
						 uri.len,
						 uri.data,
						 inc.len,
						 inc.data);
				dst.off += dputf(dst, "DEPS:");
				if (pkg->has_deps) {
					const uint *dep;
					list_node_t deps = pkg->deps;
					list_foreach(&config->deps, deps, dep)
					{
						strv_t dep_str = config_get_str(config, *dep);
						dst.off += dputf(dst, " %.*s", dep_str.len, dep_str.data);
					}
				}
				dst.off += dputf(dst, "\n\n");

				if (pkg->has_targets) {
					const config_target_t *target;
					list_node_t targets = pkg->targets;
					list_foreach(&config->targets, targets, target)
					{
						dst.off += dputf(dst, "[target]\n");
						strv_t tgt_name = config_get_str(config, target->strs + CONFIG_TARGET_NAME);
						strv_t prep	= config_get_str(config, target->strs + CONFIG_TARGET_PREP);
						strv_t conf	= config_get_str(config, target->strs + CONFIG_TARGET_CONF);
						strv_t comp	= config_get_str(config, target->strs + CONFIG_TARGET_COMP);
						strv_t inst	= config_get_str(config, target->strs + CONFIG_TARGET_INST);
						strv_t tgt_out	= config_get_str(config, target->strs + CONFIG_TARGET_OUT);
						dst.off += dputf(dst,
								 "NAME: %.*s\n"
								 "PREP: %.*s\n"
								 "CONF: %.*s\n"
								 "COMP: %.*s\n"
								 "INST: %.*s\n"
								 "OUT: %.*s\n"
								 "TYPE: %d\n",
								 tgt_name.len,
								 tgt_name.data,
								 prep.len,
								 prep.data,
								 conf.len,
								 conf.data,
								 comp.len,
								 comp.data,
								 inst.len,
								 inst.data,
								 tgt_out.len,
								 tgt_out.data,
								 target->out_type);
						dst.off += dputf(dst, "\n");
					}
				}
			}
		}
	}

	return dst.off - off;
}
