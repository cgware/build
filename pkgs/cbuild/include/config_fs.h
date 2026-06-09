#ifndef CONFIG_FS
#define CONFIG_FS

#include "config.h"
#include "fs.h"
#include "proc.h"
#include "strbuf.h"

typedef enum config_sync_kind_e {
	CONFIG_SYNC_KIND_UNKNOWN,
	CONFIG_SYNC_KIND_DIR,
	CONFIG_SYNC_KIND_EXT,
} config_sync_kind_t;

typedef struct config_sync_item_s {
	config_sync_kind_t kind;
	uint uri;
	uint path;
	uint name;
} config_sync_item_t;

typedef struct config_sync_plan_s {
	arr_t items;
	strbuf_t strs;
} config_sync_plan_t;

config_sync_plan_t *config_sync_plan_init(config_sync_plan_t *plan, uint cap, alloc_t alloc);
void config_sync_plan_free(config_sync_plan_t *plan);
int config_sync_plan_add_ext(config_sync_plan_t *plan, strv_t uri, strv_t name);
int config_sync_plan_add_dir(config_sync_plan_t *plan, strv_t path, strv_t name);

int config_fs(config_t *config, config_t *tmp, const config_schema_t *schema, registry_t *registry, fs_t *fs, proc_t *proc,
	      strv_t proj_path, strv_t cur_path, strv_t name, str_t *buf, alloc_t alloc, dst_t dst);

#endif
