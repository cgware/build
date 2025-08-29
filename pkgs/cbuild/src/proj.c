#include "proj.h"

#include "log.h"
#include "path.h"

proj_t *proj_init(proj_t *proj, uint pkgs_cap, uint targets_cap, alloc_t alloc)
{
	if (proj == NULL) {
		return NULL;
	}

	if (strvbuf_init(&proj->strs, 4 * pkgs_cap + 1 * targets_cap, 16, alloc) == NULL ||
	    arr_init(&proj->pkgs, pkgs_cap, sizeof(pkg_t), alloc) == NULL ||
	    list_init(&proj->targets, targets_cap, sizeof(target_t), alloc) == NULL ||
	    list_init(&proj->deps, targets_cap, sizeof(list_node_t), alloc) == NULL) {
		return NULL;
	}

	return proj;
}

void proj_free(proj_t *proj)
{
	if (proj == NULL) {
		return;
	}

	list_free(&proj->deps);
	list_free(&proj->targets);
	arr_free(&proj->pkgs);
	strvbuf_free(&proj->strs);
}

pkg_t *proj_get_pkg(const proj_t *proj, uint id)
{
	if (proj == NULL) {
		return NULL;
	}

	pkg_t *pkg = arr_get(&proj->pkgs, id);
	if (pkg == NULL) {
		log_error("cbuild", "proj", NULL, "failed to get package");
		return NULL;
	}

	return pkg;
}

target_t *proj_get_target(const proj_t *proj, uint id)
{
	if (proj == NULL) {
		return NULL;
	}

	target_t *target = list_get(&proj->targets, id);
	if (target == NULL) {
		log_error("cbuild", "proj", NULL, "failed to get target");
		return NULL;
	}

	return target;
}

int proj_get_deps(const proj_t *proj, list_node_t target, arr_t *deps)
{
	if (proj == NULL) {
		return 1;
	}

	if (proj_get_target(proj, target) == NULL) {
		log_error("cbuild", "proj", NULL, "failed to get target dependencies: %d", target);
		return 1;
	}

	if (proj->deps.cnt == 0) {
		return 0;
	}

	arr_t visited = {0};
	arr_init(&visited, proj->targets.cnt, sizeof(uint8_t), ALLOC_STD);
	for (uint i = 0; i < proj->targets.cnt; i++) {
		*(uint8_t *)arr_add(&visited, NULL) = 0;
	}

	arr_t queue = {0};
	arr_init(&queue, proj->deps.cnt * 2, sizeof(list_node_t), ALLOC_STD);
	*(list_node_t *)arr_add(&queue, NULL) = target;

	uint front = 0;

	while (front < queue.cnt) {
		list_node_t current = *(list_node_t *)arr_get(&queue, front++);
		uint8_t *v	    = arr_get(&visited, current);

		if (*v) {
			continue;
		}
		*v = 1;

		if (current != target) {
			arr_addu(deps, &current, NULL);
		}

		const target_t *tgt = list_get(&proj->targets, current);
		if (!tgt->has_deps) {
			continue;
		}

		const list_node_t *dep_target_id;
		list_node_t i = tgt->deps;
		list_foreach(&proj->deps, i, dep_target_id)
		{
			uint8_t *visited_to = arr_get(&visited, *dep_target_id);
			if (!*visited_to) {
				*(uint *)arr_add(&queue, NULL) = *dep_target_id;
			}
		}
	}

	arr_free(&visited);
	arr_free(&queue);
	return 0;
}

int proj_get_pkg_build_order(const proj_t *proj, arr_t *order, alloc_t alloc)
{
	if (proj == NULL || order == NULL) {
		return 1;
	}

	arr_t indegrees = {0};
	arr_init(&indegrees, proj->targets.cnt, sizeof(uint), alloc);

	uint i;
	for (i = 0; i < proj->targets.cnt; i++) {
		*(uint *)arr_add(&indegrees, NULL) = 0;
	}

	uint *indegree;
	list_node_t *dep;
	i = 0;
	list_foreach_all(&proj->deps, i, dep)
	{
		indegree = arr_get(&indegrees, *dep);
		(*indegree)++;
	}

	arr_t queue = {0};
	arr_init(&queue, proj->targets.cnt, sizeof(list_node_t), alloc);

	i = 0;
	arr_foreach(&indegrees, i, indegree)
	{
		if (*indegree == 0) {
			*(list_node_t *)arr_add(&queue, NULL) = i;
		}
	}

	list_node_t front = 0;

	order->cnt = 0;
	uint cnt   = 0;

	while (front < queue.cnt) {
		list_node_t node = *(list_node_t *)arr_get(&queue, front++);

		const target_t *target = proj_get_target(proj, node);
		arr_addu(order, &target->pkg, NULL);
		cnt++;

		if (!target->has_deps) {
			continue;
		}

		const list_node_t *dep_target_id;
		list_node_t i = target->deps;
		list_foreach(&proj->deps, i, dep_target_id)
		{
			indegree = arr_get(&indegrees, *dep_target_id);
			if (--*indegree == 0) {
				*(uint *)arr_add(&queue, NULL) = *dep_target_id;
			}
		}
	}

	arr_free(&queue);
	arr_free(&indegrees);

	if (cnt != proj->targets.cnt) {
		log_error("cbuild", "proj", NULL, "failed to get package build order: cycle detected");
		return 1;
	}

	return 0;
}

static pkg_t *find_pkg(const proj_t *proj, strv_t name, uint *id)
{
	uint i = 0;
	pkg_t *pkg;
	arr_foreach(&proj->pkgs, i, pkg)
	{
		if (strv_eq(strvbuf_get(&proj->strs, pkg->name), name)) {
			id ? *id = i : (uint)0;
			return pkg;
		}
	}

	return NULL;
}

int proj_set_uri(proj_t *proj, pkg_t *pkg, strv_t uri)
{
	if (proj == NULL || pkg == NULL) {
		return 1;
	}

	strv_t proto = {0};
	strv_t host  = {0};
	strv_t path  = {0};
	strv_t file  = {0};
	strv_t name  = {0};
	strv_t ext   = {0};

	if (strv_lsplit(uri, ':', &proto, &host) || strv_rsplit(uri, '/', &path, &file) || strv_lsplit(file, '.', &name, &ext)) {
		log_error("cbuild", "proj", NULL, "failed to resolve uri: '%.*s'", uri.len, uri.data);
		return 1;
	}

	if (strv_cmpn(host, STRV("//github.com"), 12) == 0) {
		strv_t repo    = {0};
		strv_t archive = {0};
		strv_t refs    = {0};
		strv_t cat     = {0};

		if (strv_rsplit(path, '/', &path, &cat)) {
			log_error("cbuild", "proj", NULL, "failed to resolve uri cat: '%.*s'", uri.len, uri.data);
			return 1;
		}

		if (strv_rsplit(path, '/', &path, &refs) || !strv_eq(refs, STRV("refs"))) {
			log_error("cbuild", "proj", NULL, "failed to resolve uri refs: '%.*s'", uri.len, uri.data);
			return 1;
		}

		if (strv_rsplit(path, '/', &path, &archive) || !strv_eq(archive, STRV("archive"))) {
			log_error("cbuild", "proj", NULL, "failed to resolve uri archive: '%.*s'", uri.len, uri.data);
			return 1;
		}

		if (strv_rsplit(path, '/', &path, &repo)) {
			log_error("cbuild", "proj", NULL, "failed to resolve uri archive: '%.*s'", uri.len, uri.data);
			return 1;
		}

		strvbuf_add(&proj->strs, repo, &pkg->name);
		if (strv_eq(cat, STRV("heads"))) {
			str_t buf = strz(16);
			str_cat(&buf, repo);
			str_cat(&buf, STRV("-"));
			str_cat(&buf, name);
			str_cat(&buf, STRV(SEP));
			strvbuf_add(&proj->strs, STRVS(buf), &pkg->uri_dir);
			str_free(&buf);
		}
	}

	if (strv_eq(proto, STRV("https"))) {
		pkg->uri.proto = PKG_URI_PROTO_HTTPS;
	} else {
		log_error("cbuild", "proj", NULL, "not supported protocol: '%.*s'", proto.len, proto.data);
		return 1;
	}

	if (strv_eq(ext, STRV("git"))) {
		pkg->uri.proto = PKG_URI_PROTO_GIT;
		pkg->uri.ext   = PKG_URI_EXT_NONE;
	} else if (strv_eq(ext, STRV("zip"))) {
		pkg->uri.ext = PKG_URI_EXT_ZIP;
	} else {
		pkg->uri.ext = PKG_URI_EXT_NONE;
	}

	return strvbuf_add(&proj->strs, uri, &pkg->uri_str) || strvbuf_add(&proj->strs, name, &pkg->uri_name);
}

int proj_config(proj_t *proj, const config_t *config)
{
	if (proj == NULL) {
		return 1;
	}

	int ret = 0;
	uint i;

	strvbuf_add(&proj->strs, STRV("bin/${ARCH}-${CONFIG}/"), &proj->outdir);

	i = 0;
	const config_dir_t *dir;
	arr_foreach(&config->dirs, i, dir)
	{
		strv_t name = strvbuf_get(&config->strs, dir->name);
		strv_t path = strvbuf_get(&config->strs, dir->path);
		strv_t src  = strvbuf_get(&config->strs, dir->src);
		strv_t inc  = strvbuf_get(&config->strs, dir->inc);
		strv_t test = strvbuf_get(&config->strs, dir->test);

		uint pkg_id;
		pkg_t *pkg	 = NULL;
		target_t *target = NULL;

		if (src.len > 0 || inc.len > 0 || test.len > 0) {
			pkg = arr_add(&proj->pkgs, &pkg_id);

			pkg->has_targets = 0;

			strvbuf_add(&proj->strs, name, &pkg->name);
			strvbuf_add(&proj->strs, path, &pkg->path);
			strvbuf_add(&proj->strs, src, &pkg->src);
			strvbuf_add(&proj->strs, inc, &pkg->inc);
			strvbuf_add(&proj->strs, test, &pkg->test);
			strvbuf_add(&proj->strs, STRV_NULL, &pkg->uri_str);
			strvbuf_add(&proj->strs, STRV_NULL, &pkg->uri_name);
			strvbuf_add(&proj->strs, STRV_NULL, &pkg->uri_dir);

			if (src.len > 0 || inc.len > 0) {
				list_node_t target_id;
				target = list_node(&proj->targets, &target_id);
				strvbuf_add(&proj->strs, name, &target->name);
				strvbuf_add(&proj->strs, STRV_NULL, &target->cmd);
				strvbuf_add(&proj->strs, STRV_NULL, &target->out);
				target->has_deps = 0;
				target->pkg	 = pkg_id;

				if (pkg->has_targets) {
					list_app(&proj->targets, pkg->targets, target_id);
				} else {
					pkg->targets	 = target_id;
					pkg->has_targets = 1;
				}

				if (src.len > 0) {
					target->type = TARGET_TYPE_EXE;
				}

				if (inc.len > 0) {
					target->type = TARGET_TYPE_LIB;
				}
			}

			if (test.len > 0) {
				list_node_t target_id;
				target = list_node(&proj->targets, &target_id);
				strvbuf_add(&proj->strs, name, &target->name);
				strvbuf_app(&proj->strs, target->name, STRV("_test"));
				strvbuf_add(&proj->strs, STRV_NULL, &target->cmd);
				strvbuf_add(&proj->strs, STRV_NULL, &target->out);
				target->has_deps = 0;
				target->pkg	 = pkg_id;

				if (pkg->has_targets) {
					list_app(&proj->targets, pkg->targets, target_id);
				} else {
					pkg->targets	 = target_id;
					pkg->has_targets = 1;
				}

				target->type = TARGET_TYPE_TST;
			}
		}

		if (dir->has_pkgs) {
			config_pkg_t *cfg_pkg;
			list_node_t pkgs = dir->pkgs;
			list_foreach(&config->pkgs, pkgs, cfg_pkg)
			{
				strv_t uri = strvbuf_get(&config->strs, cfg_pkg->uri);

				if (pkg == NULL || uri.len > 0) {
					pkg = arr_add(&proj->pkgs, &pkg_id);

					pkg->has_targets = 0;

					strvbuf_add(&proj->strs, path, &pkg->path);
					strvbuf_add(&proj->strs, src, &pkg->src);
					strvbuf_add(&proj->strs, inc, &pkg->inc);
					strvbuf_add(&proj->strs, test, &pkg->test);

					if (uri.len > 0) {
						ret |= proj_set_uri(proj, pkg, uri);
						name = strvbuf_get(&proj->strs, pkg->name);
					} else {
						strvbuf_add(&proj->strs, name, &pkg->name);
						strvbuf_add(&proj->strs, STRV_NULL, &pkg->uri_str);
						strvbuf_add(&proj->strs, STRV_NULL, &pkg->uri_name);
						strvbuf_add(&proj->strs, STRV_NULL, &pkg->uri_dir);
					}
				}

				cfg_pkg->pkg = pkg_id;

				if (cfg_pkg->has_targets) {
					config_target_t *cfg_target;
					list_node_t targets = cfg_pkg->targets;

					list_node_t target_id = pkg->targets;

					target = pkg->has_targets ? list_get(&proj->targets, pkg->targets) : NULL;

					int created = 0;

					list_foreach(&config->targets, targets, cfg_target)
					{
						strv_t cmd = strvbuf_get(&config->strs, cfg_target->cmd);
						strv_t out = strvbuf_get(&config->strs, cfg_target->out);

						if (target == NULL || uri.len > 0) {
							target = list_node(&proj->targets, &target_id);
							strvbuf_add(&proj->strs, name, &target->name);
							target->has_deps = 0;
							target->pkg	 = pkg_id;

							if (pkg->has_targets) {
								list_app(&proj->targets, pkg->targets, target_id);
							} else {
								pkg->targets	 = target_id;
								pkg->has_targets = 1;
							}

							created = 1;
						}

						strvbuf_add(&proj->strs, cmd, &target->cmd);
						strvbuf_add(&proj->strs, out, &target->out);

						if (uri.len > 0) {
							target->type = TARGET_TYPE_EXT;
						}

						if (!created) {
							target = list_get_next(&proj->targets, target_id, &target_id);
						}
					}
				}

				pkg = NULL;
			}
		}
	}

	i = 0;
	config_pkg_t *cfg_pkg;
	list_foreach_all(&config->pkgs, i, cfg_pkg)
	{
		if (cfg_pkg->has_deps) {
			const size_t *dep;
			list_node_t deps = cfg_pkg->deps;
			list_foreach(&config->deps, deps, dep)
			{
				strv_t dep_str = strvbuf_get(&config->strs, *dep);
				uint dep_pkg_id;
				pkg_t *dep_pkg = find_pkg(proj, dep_str, &dep_pkg_id);
				if (dep_pkg == NULL) {
					log_error("cbuild", "proj_cfg", NULL, "package not found: %.*s", dep_str.len, dep_str.data);
					continue;
				}

				uint dep_target_id;
				uint found = 0;
				if (dep_pkg->has_targets) {
					target_t *target;
					list_node_t j = dep_pkg->targets;
					list_foreach(&proj->targets, j, target)
					{
						dep_target_id = j;
						found	      = 1;
						break;
					}
				}

				if (found) {
					pkg_t *pkg = arr_get(&proj->pkgs, cfg_pkg->pkg);
					if (pkg->has_targets) {
						target_t *target;
						list_node_t j = pkg->targets;
						list_foreach(&proj->targets, j, target)
						{
							list_node_t node;
							uint *data = list_node(&proj->deps, &node);
							*data	   = dep_target_id;
							if (target->has_deps) {
								list_app(&proj->deps, target->deps, node);
							} else {
								target->deps	 = node;
								target->has_deps = 1;
							}
						}
					}
				}
			}
		}
	}

	i = 0;
	const pkg_t *pkg;
	arr_foreach(&proj->pkgs, i, pkg)
	{
		if (pkg->has_targets) {
			target_t *target;
			list_node_t j = pkg->targets;
			list_foreach(&proj->targets, j, target)
			{
				if (target->type != TARGET_TYPE_TST) {
					continue;
				}

				const target_t *dep_target;
				list_node_t k = pkg->targets;
				list_foreach(&proj->targets, k, dep_target)
				{
					if (dep_target->type == TARGET_TYPE_LIB) {
						list_node_t node;
						list_node_t *data = list_node(&proj->deps, &node);
						*data		  = k;
						if (target->has_deps) {
							list_app(&proj->deps, target->deps, node);
						} else {
							target->deps	 = node;
							target->has_deps = 1;
						}
						break;
					}
				}
			}
		}
	}

	return ret;
}

static const char *target_type_str[] = {
	[TARGET_TYPE_UNKNOWN] = "UNKNOWN",
	[TARGET_TYPE_EXE]     = "EXE",
	[TARGET_TYPE_LIB]     = "LIB",
	[TARGET_TYPE_EXT]     = "EXT",
	[TARGET_TYPE_TST]     = "TEST",
};

size_t proj_print(const proj_t *proj, dst_t dst)
{
	if (proj == NULL) {
		return 0;
	}

	int off = dst.off;

	uint i = 0;
	const pkg_t *pkg;
	arr_foreach(&proj->pkgs, i, pkg)
	{
		if (i != 0) {
			dst.off += dputf(dst, "\n");
		}

		dst.off += dputf(dst, "[pkg]\n");

		strv_t pkg_name = strvbuf_get(&proj->strs, pkg->name);
		strv_t path	= strvbuf_get(&proj->strs, pkg->path);
		strv_t src	= strvbuf_get(&proj->strs, pkg->src);
		strv_t inc	= strvbuf_get(&proj->strs, pkg->inc);
		strv_t test	= strvbuf_get(&proj->strs, pkg->test);
		strv_t uri_str	= strvbuf_get(&proj->strs, pkg->uri_str);
		strv_t uri_name = strvbuf_get(&proj->strs, pkg->uri_name);
		strv_t uri_dir	= strvbuf_get(&proj->strs, pkg->uri_dir);

		dst.off += dputf(dst,
				 "NAME: %.*s\n"
				 "PATH: %.*s\n"
				 "SRC: %.*s\n"
				 "INC: %.*s\n"
				 "TEST: %.*s\n"
				 "URI_STR: %.*s\n"
				 "URI_NAME: %.*s\n"
				 "URI_DIR: %.*s\n",
				 pkg_name.len,
				 pkg_name.data,
				 path.len,
				 path.data,
				 src.len,
				 src.data,
				 inc.len,
				 inc.data,
				 test.len,
				 test.data,
				 uri_str.len,
				 uri_str.data,
				 uri_name.len,
				 uri_name.data,
				 uri_dir.len,
				 uri_dir.data);

		if (pkg->has_targets) {
			const target_t *target;
			list_node_t j = pkg->targets;
			list_foreach(&proj->targets, j, target)
			{
				dst.off += dputf(dst, "\n[target]\n");
				strv_t target_name = strvbuf_get(&proj->strs, target->name);
				strv_t cmd	   = strvbuf_get(&proj->strs, target->cmd);
				strv_t out	   = strvbuf_get(&proj->strs, target->out);
				dst.off += dputf(dst,
						 "NAME: %.*s\n"
						 "TYPE: %s\n"
						 "CMD: %.*s\n"
						 "OUT: %.*s\n",
						 target_name.len,
						 target_name.data,
						 target_type_str[target->type],
						 cmd.len,
						 cmd.data,
						 out.len,
						 out.data);
				dst.off += dputf(dst, "DEPS:");

				if (target->has_deps) {
					const list_node_t *dep_target_id;
					list_node_t j = target->deps;
					list_foreach(&proj->deps, j, dep_target_id)
					{
						const target_t *dep_tgt = list_get(&proj->targets, *dep_target_id);
						const pkg_t *dep_pkg	= arr_get(&proj->pkgs, dep_tgt->pkg);

						strv_t dep_tgt_name = strvbuf_get(&proj->strs, dep_tgt->name);
						strv_t dep_pkg_name = strvbuf_get(&proj->strs, dep_pkg->name);
						dst.off += dputf(dst,
								 " %.*s:%.*s",
								 dep_pkg_name.len,
								 dep_pkg_name.data,
								 dep_tgt_name.len,
								 dep_tgt_name.data);
					}
				}
				dst.off += dputf(dst, "\n");
			}
		}
	}

	return off - dst.off;
}
