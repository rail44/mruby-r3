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
#include "mruby/data.h"
#include "mruby/string.h"
#include "mruby/class.h"
#include "mruby/array.h"
#include "mrb_r3.h"
#include "r3.h"

#define DONE mrb_gc_arena_restore(mrb, 0);

void mrb_r3_match_free(mrb_state *mrb, void *match);
const static struct mrb_data_type mrb_r3_match_type = { "R3::Match", mrb_free };

typedef struct {
    mrb_value params;
    mrb_value data;
} mrb_r3_match_struct;

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
    mrb_value method, path;
    route *matched_route;
    mrb_r3_match_struct *match = (mrb_r3_match_struct *) mrb_malloc(mrb, sizeof(mrb_r3_match_struct));
    node *tree = DATA_PTR(self);
    match_entry *entry;
    mrb_get_args(mrb, "iS", &method, &path);

    entry =  match_entry_create(mrb_str_to_cstr(mrb, path));
    entry->request_method = mrb_fixnum(method);

    matched_route = r3_tree_match_route(tree, entry);
    if (matched_route == NULL) {
        match->data = mrb_nil_value();
    } else { 
        match->data = mrb_obj_value(matched_route->data);
    }
    match->params = mrb_ary_new(mrb);
    for (i = 0; i < entry->vars->len; i++) {
        mrb_ary_push(mrb, match->params, mrb_str_new_cstr(mrb, entry->vars->tokens[i]));
    }
    match_entry_free(entry);

    return mrb_obj_value(Data_Wrap_Struct(
            mrb,
            mrb_class_get_under(mrb, mrb_module_get(mrb, "R3"), "Match"),
            &mrb_r3_match_type,
            match));
}

mrb_value mrb_r3_tree_dump(mrb_state *mrb, mrb_value self)
{
    node *tree = DATA_PTR(self);
    r3_tree_dump(tree, 0);
    return mrb_nil_value();
}

mrb_value mrb_r3_match_initialize(mrb_state *mrb, mrb_value self)
{
    mrb_r3_match_struct *match = (mrb_r3_match_struct *) mrb_malloc(mrb, sizeof(mrb_r3_match_struct));
    DATA_TYPE(self) = &mrb_r3_match_type;
    DATA_PTR(self) = match;
    return self;
}

mrb_value mrb_r3_match_params(mrb_state *mrb, mrb_value self)
{
    mrb_r3_match_struct *match = DATA_PTR(self);
    return match->params;
}

mrb_value mrb_r3_match_data(mrb_state *mrb, mrb_value self)
{
    mrb_r3_match_struct *match = DATA_PTR(self);
    return match->data;
}

void mrb_mruby_r3_gem_init(mrb_state *mrb)
{
    struct RClass *r3, *method, *tree, *match;

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
    mrb_define_method(mrb, tree, "dump", mrb_r3_tree_dump, MRB_ARGS_NONE());
    mrb_define_method(mrb, tree, "insert_route", mrb_r3_tree_insert_route, MRB_ARGS_REQ(3));
    mrb_define_method(mrb, tree, "match", mrb_r3_tree_match, MRB_ARGS_REQ(2));

    match = mrb_define_class_under(mrb, r3, "Match", mrb->object_class);
    MRB_SET_INSTANCE_TT(match, MRB_TT_DATA);
    mrb_define_method(mrb, match, "params", mrb_r3_match_params, MRB_ARGS_NONE());
    mrb_define_method(mrb, match, "data", mrb_r3_match_data, MRB_ARGS_NONE());

    DONE;
}

void mrb_mruby_r3_gem_final(mrb_state *mrb)
{
}
