EXTENSION = bzip
EXTVERSION = 0.1.0

all: sql/$(EXTENSION)--$(EXTVERSION).sql $(EXTENSION).control

sql/$(EXTENSION)--$(EXTVERSION).sql: sql/$(EXTENSION).sql
	cp $< $@

$(EXTENSION).control:
	sed "s/@EXTVERSION@/$(EXTVERSION)/g" $(EXTENSION).control.in > $(EXTENSION).control

DATA = $(wildcard sql/*--*.sql)

MODULE_big = $(EXTENSION)
OBJS = src/pg_bzip.o

TESTS = $(wildcard test/sql/*.sql)
REGRESS = $(patsubst test/sql/%.sql,%,$(TESTS))
REGRESS_OPTS = --inputdir=test

PG_CONFIG = pg_config
SHLIB_LINK = -lbz2

PG_CFLAGS = -std=c99 -Wno-declaration-after-statement -Wall -Werror -Wshadow

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
