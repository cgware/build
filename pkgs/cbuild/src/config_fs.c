#include "config_fs.h"

#include "log.h"
#include "mod.h"
#include "path.h"

config_sync_plan_t *config_sync_plan_init(config_sync_plan_t *plan, uint cap, alloc_t alloc)
{
	if (plan == NULL) {
		return NULL;
	}

	if (arr_init(&plan->items, cap, sizeof(config_sync_item_t), alloc) == NULL ||
	    strbuf_init(&plan->strs, cap * 3, 16, alloc) == NULL) {
		arr_free(&plan->items);
		return NULL;
	}

	return plan;
}

void config_sync_plan_free(config_sync_plan_t *plan)
{
	if (plan == NULL) {
		return;
	}

	arr_free(&plan->items);
	strbuf_free(&plan->strs);
}

static int config_sync_plan_add(config_sync_plan_t *plan, config_sync_kind_t kind, strv_t uri, strv_t path, strv_t name)
{
	if (plan == NULL) {
		return 1;
	}

	uint uri_id  = -1;
	uint path_id = -1;
	uint name_id = -1;
	if (uri.data && strbuf_add(&plan->strs, uri, &uri_id)) {
		return 1;
	}
	if (path.data && strbuf_add(&plan->strs, path, &path_id)) {
		return 1;
	}
	if (name.data && strbuf_add(&plan->strs, name, &name_id)) {
		return 1;
	}

	config_sync_item_t *item = arr_add(&plan->items, NULL);
	if (item == NULL) {
		return 1;
	}

	item->kind = kind;
	item->uri  = uri_id;
	item->path = path_id;
	item->name = name_id;

	return 0;
}

int config_sync_plan_add_ext(config_sync_plan_t *plan, strv_t uri, strv_t name)
{
	return config_sync_plan_add(plan, CONFIG_SYNC_KIND_EXT, uri, STRV_NULL, name);
}

int config_sync_plan_add_dir(config_sync_plan_t *plan, strv_t path, strv_t name)
{
	uint i = 0;
	const config_sync_item_t *item;
	arr_foreach(&plan->items, i, item)
	{
		if (item->kind == CONFIG_SYNC_KIND_DIR && item->path != (uint)-1 && strv_eq(strbuf_get(&plan->strs, item->path), path)) {
			return 0;
		}
	}

	return config_sync_plan_add(plan, CONFIG_SYNC_KIND_DIR, STRV_NULL, path, name);
}

static int ensure_tmp(fs_t *fs, strv_t dir)
{
	path_t path = {0};
	path_init(&path, dir);

	path_push(&path, STRV("tmp"));
	if (!fs_isdir(fs, STRVS(path))) {
		fs_mkdir(fs, STRVS(path));
	}

	path_push(&path, STRV(".gitignore"));
	if (!fs_isfile(fs, STRVS(path))) {
		void *f;
		fs_open(fs, STRVS(path), "w", &f);
		fs_writes(fs, f, STRV("*"));
		fs_close(fs, f);
	}

	return 0;
}

static strv_t config_sync_plan_get_str(const config_sync_plan_t *plan, uint id)
{
	if (id == (uint)-1) {
		return STRV_NULL;
	}

	return strbuf_get(&plan->strs, id);
}

static int config_sync_plan_add_develop_uri(str_t *buf, strv_t uri)
{
	strv_t scheme = STRV("https://");
	strv_t suffix = STRV(".git");

	if (uri.len <= scheme.len + suffix.len || strv_cmpn(uri, scheme, scheme.len) ||
	    strv_cmpn(STRVN(&uri.data[uri.len - suffix.len], suffix.len), suffix, suffix.len)) {
		return 1;
	}

	strv_t host = STRV_NULL;
	strv_t path = STRV_NULL;
	strv_lsplit(STRVN(&uri.data[scheme.len], uri.len - scheme.len), '/', &host, &path);
	if (host.len == 0 || path.data == NULL || path.len == 0) {
		return 1;
	}

	str_cat(buf, STRV("git@"));
	str_cat(buf, host);
	str_cat(buf, STRV(":"));
	str_cat(buf, path);

	return 0;
}

static void config_sync_plan_add_clone_uri(str_t *buf, strv_t uri, int develop)
{
	if (develop && config_sync_plan_add_develop_uri(buf, uri) == 0) {
		return;
	}

	str_cat(buf, uri);
}

static int config_sync_plan_sync_ext(const config_sync_plan_t *plan, const config_sync_item_t *item, config_sync_plan_t *queue, fs_t *fs,
				     proc_t *proc, strv_t proj_path, int develop, str_t *buf)
{
	strv_t uri  = config_sync_plan_get_str(plan, item->uri);
	strv_t name = config_sync_plan_get_str(plan, item->name);

	ensure_tmp(fs, proj_path);

	path_t dir = {0};
	path_init(&dir, STRV("tmp"));
	path_push(&dir, STRV("ext"));
	path_push(&dir, name);
	path_push(&dir, STRV(""));

	path_t path = {0};
	path_init(&path, proj_path);
	path_push(&path, STRVS(dir));
	size_t path_len = path.len;

	if (!fs_isdir(fs, STRVS(path))) {
		fs_mkpath(fs, proj_path, STRVS(dir));
	}

	path_push(&path, STRV(".git"));
	if (!fs_isdir(fs, STRVS(path))) {
		path.len = path_len;
		if (buf) {
			buf->len = 0;
			str_cat(buf, STRV("git clone "));
			size_t uri_start = buf->len;
			config_sync_plan_add_clone_uri(buf, uri, develop);
			size_t uri_len = buf->len - uri_start;
			str_cat(buf, STRV(" "));
			str_cat(buf, STRVS(path));
			if (proc) {
				log_info("cbuild", "config", NULL, "cloning package: %.*s", uri_len, &buf->data[uri_start]);
				proc_cmd(proc, STRVS(*buf));
			}
		}
	}

	if (config_sync_plan_add_dir(queue, STRVS(dir), name)) {
		return 1;
	}

	return 0;
}

int config_fs(config_t *config, config_t *tmp, const config_schema_t *schema, registry_t *registry, fs_t *fs, proc_t *proc,
	      strv_t proj_path, strv_t cur_path, strv_t name, int develop, str_t *buf, alloc_t alloc, dst_t dst)
{
	if (config == NULL || tmp == NULL || schema == NULL || registry == NULL || fs == NULL) {
		return 0;
	}

	int ret			 = 0;
	config_sync_plan_t queue = {0};
	config_sync_plan_init(&queue, 4, alloc);
	config_sync_plan_add_dir(&queue, cur_path, name);

	for (uint idx = 0; idx < queue.items.cnt; idx++) {
		const config_sync_item_t *work = arr_get(&queue.items, idx);
		strv_t work_path	       = config_sync_plan_get_str(&queue, work->path);
		strv_t work_name	       = config_sync_plan_get_str(&queue, work->name);

		switch (work->kind) {
		case CONFIG_SYNC_KIND_DIR:
			log_info("cbuild", "config", NULL, "loading directory: '%.*s'", work_path.len, work_path.data);

			for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
				if (i->type != DRIVER_TYPE_MOD) {
					continue;
				}

				mod_t *mod = i->data;
				ret |= mod->config_fs(
					mod, config, tmp, schema, registry, &queue, fs, proj_path, work_path, work_name, buf, alloc, dst);
			}
			break;
		case CONFIG_SYNC_KIND_EXT:
			ret |= config_sync_plan_sync_ext(&queue, work, &queue, fs, proc, proj_path, develop, buf);
			break;
		default:
			ret = 1;
			break;
		}
	}

	config_sync_plan_free(&queue);

	return ret;
}
