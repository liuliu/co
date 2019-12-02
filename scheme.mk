.PHONY: debug undef asan

# Debug Scheme

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -g -fno-omit-frame-pointer -O0
	LDFLAGS += -g -fno-omit-frame-pointer -O0
endif

debug: CFLAGS += -g -fno-omit-frame-pointer -O0
debug: LDFLAGS += -g -fno-omit-frame-pointer -O0
debug: export DEBUG = 1
debug: all

# Asan Scheme

ASAN ?= 0
ifeq ($(ASAN), 1)
	CFLAGS += -g -fno-omit-frame-pointer -fsanitize=address
	LDFLAGS += -g -fno-omit-frame-pointer -fsanitize=address
endif

asan: CFLAGS += -g -fno-omit-frame-pointer -fsanitize=address
asan: LDFLAGS += -g -fno-omit-frame-pointer -fsanitize=address
asan: export ASAN = 1
asan: all

# Undefined Scheme

UNDEF ?= 0
ifeq ($(UNDEF), 1)
	CFLAGS += -g -fno-omit-frame-pointer -O0 -fsanitize=address -fsanitize=undefined
	LDFLAGS += -g -fno-omit-frame-pointer -O0 -fsanitize=address -fsanitize=undefined
endif

undef: CFLAGS += -g -fno-omit-frame-pointer -O0 -fsanitize=address -fsanitize=undefined
undef: LDFLAGS += -g -fno-omit-frame-pointer -O0 -fsanitize=address -fsanitize=undefined
undef: export UNDEF = 1
undef: all

