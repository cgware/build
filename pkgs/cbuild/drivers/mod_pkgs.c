#include "mod.h"

#include "config_fs.h"
#include "mem.h"
#include "path.h"

static int mod_pkgs_config_fs(mod_t *mod, config_t *config, config_t *tmp, registry_t *registry, fs_t *fs, proc_t *proc, strv_t proj_path,
			      strv_t cur_path, strv_t name, str_t *buf, alloc_t alloc, dst_t dst)
{
	(void)mod;
	(void)name;

	path_t path = {0};
	path_init(&path, proj_path);
	path_push(&path, cur_path);
	size_t path_len = path.len;
	path_push(&path, STRV("pkgs"));
	size_t pkgs_path_len = path.len;

	int ret = 0;

	if (fs_isdir(fs, STRVS(path))) {
		strbuf_t pkgs = {0};
		strbuf_init(&pkgs, 8, 16, alloc);

		fs_lsdir(fs, STRVS(path), &pkgs);

		uint i = 0;
		strv_t subdir;
		strbuf_foreach(&pkgs, i, subdir)
		{
			path_push(&path, subdir);
			ret |= config_fs(config,
					 tmp,
					 registry,
					 fs,
					 proc,
					 proj_path,
					 STRVN(&path.data[path_len], path.len - path_len),
					 subdir,
					 buf,
					 alloc,
					 dst);
			path.len = pkgs_path_len;
		}

		strbuf_free(&pkgs);
	}

	return ret;
}

static int mod_pkgs_proj_cfg(mod_t *mod, proj_t *proj)
{
	(void)mod;
	(void)proj;

	return 0;
}

static int mod_pkgs_init(mod_t *mod, uint cap, alloc_t alloc)
{
	(void)mod;
	(void)cap;
	(void)alloc;
	return 0;
}

static int mod_pkgs_free(mod_t *mod)
{
	(void)mod;
	return 0;
}

static mod_t mod_pkgs = {
	.name	   = STRVT("mod_pkgs"),
	.init	   = mod_pkgs_init,
	.free	   = mod_pkgs_free,
	.config_fs = mod_pkgs_config_fs,
	.proj_cfg  = mod_pkgs_proj_cfg,
};

MOD(mod_pkgs, &mod_pkgs);
