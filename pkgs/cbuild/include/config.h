#ifndef CONFIG_H
#define CONFIG_H

#include "list.h"
#include "strbuf.h"

typedef enum config_target_str_e {
	CONFIG_TARGET_NAME,
	CONFIG_TARGET_PREP,
	CONFIG_TARGET_CONF,
	CONFIG_TARGET_COMP,
	CONFIG_TARGET_INST,
	CONFIG_TARGET_DST,
	__CONFIG_TARGET_STR_CNT,
} config_target_str_t;

typedef struct config_target_s {
	uint strs;
} config_target_t;

typedef enum config_pkg_str_e {
	CONFIG_PKG_NAME,
	CONFIG_PKG_URI,
	CONFIG_PKG_INC,
	__CONFIG_PKG_STR_CNT,
} config_pkg_str_t;

typedef struct config_pkg_s {
	uint strs;
	list_node_t targets;
	list_node_t deps;
	uint pkg;
	uint has_targets : 1;
	uint has_deps : 1;
} config_pkg_t;

typedef enum config_dir_str_e {
	CONFIG_DIR_NAME,
	CONFIG_DIR_PATH,
	CONFIG_DIR_SRC,
	CONFIG_DIR_INC,
	CONFIG_DIR_DRV,
	CONFIG_DIR_TST,
	__CONFIG_DIR_STR_CNT,
} config_dir_str_t;

typedef struct config_dir_s {
	uint strs;
	list_node_t pkgs;
	uint has_pkgs : 1;
	uint has_main : 1;
} config_dir_t;

typedef struct config_s {
	strbuf_t strs;
	arr_t dirs;
	list_t pkgs;
	list_t targets;
	list_t deps;
} config_t;

config_t *config_init(config_t *config, uint dirs_cap, uint pkgs_cap, uint targets_cap, alloc_t alloc);
void config_free(config_t *config);

config_dir_t *config_add_dir(config_t *config, uint *id);
config_dir_t *config_get_dir(config_t *config, uint id);

config_pkg_t *config_add_pkg(config_t *config, list_node_t dir, list_node_t *id);
config_pkg_t *config_get_pkg(config_t *config, list_node_t id);

config_target_t *config_add_target(config_t *config, list_node_t pkg, list_node_t *id);
config_target_t *config_get_target(config_t *config, list_node_t id);

int config_add_dep(config_t *config, list_node_t pkg, strv_t dep);

int config_set_str(config_t *config, uint id, strv_t val);
strv_t config_get_str(const config_t *config, uint id);

size_t config_print(const config_t *config, dst_t dst);

#endif
