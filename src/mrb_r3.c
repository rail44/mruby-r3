/*
** mrb_r3.c - R3 class
**
** Copyright (c) AMEMIYA Satoshi 2015
**
** See Copyright Notice in LICENSE
*/

#include "mruby.h"
#include "mruby/data.h"
#include "mrb_r3.h"

#define DONE mrb_gc_arena_restore(mrb, 0);

typedef struct {
  char *str;
  int len;
} mrb_r3_data;

static const struct mrb_data_type mrb_r3_data_type = {
  "mrb_r3_data", mrb_free,
};

static mrb_value mrb_r3_init(mrb_state *mrb, mrb_value self)
{
  mrb_r3_data *data;
  char *str;
  int len;

  data = (mrb_r3_data *)DATA_PTR(self);
  if (data) {
    mrb_free(mrb, data);
  }
  DATA_TYPE(self) = &mrb_r3_data_type;
  DATA_PTR(self) = NULL;

  mrb_get_args(mrb, "s", &str, &len);
  data = (mrb_r3_data *)mrb_malloc(mrb, sizeof(mrb_r3_data));
  data->str = str;
  data->len = len;
  DATA_PTR(self) = data;

  return self;
}

static mrb_value mrb_r3_hello(mrb_state *mrb, mrb_value self)
{
  mrb_r3_data *data = DATA_PTR(self);

  return mrb_str_new(mrb, data->str, data->len);
}

static mrb_value mrb_r3_hi(mrb_state *mrb, mrb_value self)
{
  return mrb_str_new_cstr(mrb, "hi!!");
}

void mrb_mruby_r3_gem_init(mrb_state *mrb)
{
    struct RClass *r3;
    r3 = mrb_define_class(mrb, "R3", mrb->object_class);
    mrb_define_method(mrb, r3, "initialize", mrb_r3_init, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, r3, "hello", mrb_r3_hello, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, r3, "hi", mrb_r3_hi, MRB_ARGS_NONE());
    DONE;
}

void mrb_mruby_r3_gem_final(mrb_state *mrb)
{
}

