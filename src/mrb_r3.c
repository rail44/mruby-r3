/*
** mrb_r3.c - R3 class
**
** Copyright (c) AMEMIYA Satoshi 2015
**
** See Copyright Notice in LICENSE
*/

#define HAVE_STRNDUP
#define HAVE_STRDUP

#include "mruby.h"
#include "mruby/array.h"
#include "mruby/string.h"
#include "mruby/data.h"
#include "mruby/hash.h"
#include "mrb_r3.h"
#include "r3.h"

#define DONE mrb_gc_arena_restore(mrb, 0);

mrb_value mrb_r3_tree_initialize(mrb_state *mrb, mrb_value self)
{
    mrb_value cap;
    DATA_PTR(self) = NULL;
    mrb_get_args(mrb, "i", &cap);
    DATA_PTR(self) = r3_tree_create(mrb_fixnum(cap));

    return self;
}

mrb_value mrb_r3_tree_insert_route(mrb_state *mrb, mrb_value self)
{
    mrb_value method, path, data;
    node *tree = DATA_PTR(self);
    mrb_get_args(mrb, "iSo", &method, &path, &data);

    r3_tree_insert_route(tree,
                         mrb_fixnum(method),
                         mrb_str_to_cstr(mrb, path),
                         mrb_ptr(data));
    return mrb_nil_value();
}

mrb_value mrb_r3_tree_compile(mrb_state *mrb, mrb_value self)
{
    char *errstr = NULL;
    node *tree = DATA_PTR(self);

    r3_tree_compile(tree, &errstr);
    return mrb_str_new_cstr(mrb, errstr);
}

mrb_value mrb_r3_tree_match(mrb_state *mrb, mrb_value self)
{
    int i;
    mrb_value method,
              path,
              data_key,
              match,
              params_key,
              params_value;
    match_entry *entry;
    route *matched_route;
    node *tree = DATA_PTR(self);
    mrb_get_args(mrb, "iS", &method, &path);

    match = mrb_hash_new(mrb);
    params_value = mrb_ary_new(mrb);
    data_key = mrb_symbol_value(mrb_intern_cstr(mrb, "data"));
    params_key = mrb_symbol_value(mrb_intern_cstr(mrb, "params"));
    entry = match_entry_create(mrb_str_to_cstr(mrb, path));
    entry->request_method = mrb_fixnum(method);

    matched_route = r3_tree_match_route(tree, entry);
    if (matched_route != NULL) {
        mrb_hash_set(mrb, match, data_key, mrb_obj_value(matched_route->data));
    }
    for (i = 0; i < entry->vars->len; i++) {
        mrb_ary_push(mrb, params_value, mrb_str_new_cstr(mrb, entry->vars->tokens[i]));
    }
    match_entry_free(entry);
    mrb_hash_set(mrb, match, params_key, params_value);

    return match;
}

void mrb_mruby_r3_gem_init(mrb_state *mrb)
{
    struct RClass *r3, *method, *tree;

    r3 = mrb_define_module(mrb, "R3");

    method = mrb_define_module_under(mrb, r3, "Method");
    mrb_define_const(mrb, method, "GET", mrb_fixnum_value(METHOD_GET));
    mrb_define_const(mrb, method, "POST", mrb_fixnum_value(METHOD_POST));
    mrb_define_const(mrb, method, "PUT", mrb_fixnum_value(METHOD_PUT));
    mrb_define_const(mrb, method, "DELETE", mrb_fixnum_value(METHOD_DELETE));
    mrb_define_const(mrb, method, "PATCH", mrb_fixnum_value(METHOD_PATCH));
    mrb_define_const(mrb, method, "HEAD", mrb_fixnum_value(METHOD_HEAD));
    mrb_define_const(mrb, method, "OPTIONS", mrb_fixnum_value(METHOD_OPTIONS));

    tree = mrb_define_class_under(mrb, r3, "Tree", mrb->object_class);
    mrb_define_method(mrb, tree, "initialize", mrb_r3_tree_initialize, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, tree, "compile", mrb_r3_tree_compile, MRB_ARGS_NONE());
    mrb_define_method(mrb, tree, "insert_route", mrb_r3_tree_insert_route, MRB_ARGS_REQ(3));
    mrb_define_method(mrb, tree, "match", mrb_r3_tree_match, MRB_ARGS_REQ(2));

    DONE;
}

void mrb_mruby_r3_gem_final(mrb_state *mrb)
{
}
