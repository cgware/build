#ifndef CONFIG_H
#define CONFIG_H

#include "fs.h"
#include "list.h"
#include "proc.h"
#include "strvbuf.h"

typedef struct config_target_s {
	size_t name;
	size_t cmd;
	size_t out;
} config_target_t;

typedef struct config_pkg_s {
	size_t name;
	list_node_t targets;
	list_node_t deps;
	uint pkg;
	size_t uri;
	uint has_targets : 1;
	uint has_deps : 1;
} config_pkg_t;

typedef struct config_dir_s {
	size_t name;
	size_t path;
	size_t src;
	size_t inc;
	size_t test;
	list_node_t pkgs;
	uint has_pkgs;
} config_dir_t;

typedef struct config_s {
	strvbuf_t strs;
	arr_t dirs;
	list_t pkgs;
	list_t targets;
	list_t deps;
} config_t;

config_t *config_init(config_t *config, uint dirs_cap, uint pkgs_cap, uint targets_cap, alloc_t alloc);
void config_free(config_t *config);

int config_load(config_t *config, fs_t *fs, proc_t *proc, strv_t base_path, strv_t dir_path, strv_t name, str_t *buf, alloc_t alloc,
		dst_t dst);

size_t config_print(const config_t *config, dst_t dst);

#endif
