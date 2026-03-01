#include "config_fs.h"

#include "log.h"
#include "mod.h"

int config_fs(config_t *config, config_t *tmp, registry_t *registry, fs_t *fs, proc_t *proc, strv_t proj_path, strv_t cur_path, strv_t name,
	      str_t *buf, alloc_t alloc, dst_t dst)
{
	log_info("cbuild", "config", NULL, "loading directory: '%.*s'", cur_path.len, cur_path.data);

	int ret = 0;

	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != DRIVER_TYPE_MOD) {
			continue;
		}

		mod_t *mod = i->data;
		ret |= mod->config_fs(mod, config, tmp, registry, fs, proc, proj_path, cur_path, name, buf, alloc, dst);
	}

	return ret;
}
