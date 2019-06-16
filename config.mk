# Compile options

VERBOSE=n
OPT=g
USE_NANO=y
SEMIHOST=n
USE_FPU=y

# Libraries

USE_LPCOPEN=y
USE_SAPI=y
USE_FATFS=y
USE_FREERTOS=y
FREERTOS_HEAP_TYPE=1
LOAD_INRAM=n

DEFINES+=DEBUG_ENABLE

#SOURCE FILES
PROJECT_C_FILES := $(wildcard $(PROJECT)/src/*.c)
PROJECT_ASM_FILES := $(wildcard $(PROJECT)/src/*.s)
