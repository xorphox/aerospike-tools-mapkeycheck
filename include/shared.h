/*
 * Aerospike Shared Includes
 *
 * Copyright (c) 2008-2017 Aerospike, Inc. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"

#include <citrusleaf/alloc.h>
#include <citrusleaf/cf_b64.h>
#include <citrusleaf/cf_clock.h>

#include <aerospike/aerospike.h>
#include <aerospike/aerospike_info.h>
#include <aerospike/aerospike_key.h>
#include <aerospike/as_arraylist.h>
#include <aerospike/as_bin.h>
#include <aerospike/as_cluster.h>
#include <aerospike/as_hashmap.h>
#include <aerospike/as_info.h>
#include <aerospike/as_key.h>
#include <aerospike/as_log_macros.h>
#include <aerospike/as_nil.h>
#include <aerospike/as_node.h>
#include <aerospike/as_policy.h>
#include <aerospike/as_record.h>
#include <aerospike/as_scan.h>

#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wsign-conversion"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <getopt.h>
#include <libgen.h>
#include <math.h>
#include <pthread.h>
#include <regex.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined __APPLE__
#include <sys/syscall.h>
#else
#include <syscall.h>
#endif
#include <time.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>

#define VERSION_1_1 "1.1"               ///< Indicates a new-style backup file.

#define MAX_META_LINE 1000              ///< The maximal length of a meta data line in a backup
                                        ///  file.
#define META_PREFIX "# "                ///< Every meta data line starts with this prefix.
#define META_FIRST_FILE "first-file"    ///< The meta data tag that marks the backup file that was
                                        ///  written first FIXME: Remove
#define META_NAMESPACE "namespace"      ///< The meta data tag that specifies the namespace from
                                        ///  which this backup file was created.

#define GLOBAL_PREFIX "* "              ///< Every global data (= secondary index information and
                                        ///  UDF files) line starts with this prefix.
#define RECORD_META_PREFIX "+ "         ///< Every record meta data (= digest, generation, etc.)
                                        ///  line starts with this prefix.
#define RECORD_BIN_PREFIX "- "          ///< Every record bin line starts with this prefix.

#define MAX_TOKEN_SIZE 1000             ///< The maximal length of an individual syntactic token in
                                        ///  the backup file.

#define DEFAULT_HOST "127.0.0.1"        ///< The default host to connect to.
#define DEFAULT_PORT 3000               ///< The default port to connect to.

#define TIMEOUT 10000                   ///< The timeout for all operations (in ms).

///
/// The data type of a path expression.
///
typedef enum {
	PATH_TYPE_INVALID,  ///< Invalid.
	PATH_TYPE_STRING,   ///< The path results in a string.
	PATH_TYPE_NUMERIC,  ///< The path results in an integer.
	PATH_TYPE_GEOJSON   ///< The path results in a geojson value.
} path_type;

///
/// Represents a path expression and its data type.
///
typedef struct {
	char *path;     ///< The path expression.
	path_type type; ///< The data type.
} path_param;

///
/// Identifies the TLS client command line options.
///
typedef enum {
	TLS_OPT_ENABLE = 1000,      ///< The `--tls-enable` option.
	TLS_OPT_ENCRYPT_ONLY,       ///< The `--tls-encrypt-only` option.
	TLS_OPT_CA_FILE,            ///< The `--tls-cafile` option.
	TLS_OPT_CA_PATH,            ///< The `--tls-capath` option.
	TLS_OPT_PROTOCOLS,          ///< The `--tls-protocols` option.
	TLS_OPT_CIPHER_SUITE,       ///< The `--tls-cipher-suite` option.
	TLS_OPT_CRL_CHECK,          ///< The `--tls-crl-check` option.
	TLS_OPT_CRL_CHECK_ALL,      ///< The `--tls-crl-checkall` option.
	TLS_OPT_CERT_BLACK_LIST,    ///< The `--tls-cert-blacklist` option.
	TLS_OPT_LOG_SESSION_INFO,   ///< The `--tlsLogSessionInfo` option.
	TLS_OPT_KEY_FILE,           ///< The `--tls-keyfile` option.
	TLS_OPT_KEY_FILE_PASSWORD,  ///< The `--tls-keyfile-password` option.
	TLS_OPT_CERT_FILE           ///< The `--tls-certfile` option.
} tls_opt;

///
/// Identifies the config file command line options.
///
typedef enum {
	CONFIG_FILE_OPT_FILE = 2000,
	CONFIG_FILE_OPT_INSTANCE, 
	CONFIG_FILE_OPT_NO_CONFIG_FILE,
	CONFIG_FILE_OPT_ONLY_CONFIG_FILE,
} cfgfile_opt;

#define CDT_FIX_OPT 3000
#define CDT_PRINT 3001

typedef struct {
	void *conf;                                 ///< The global configuration and stats.
	char (*node_names)[][AS_NODE_NAME_SIZE];    ///< The cluster nodes to be backed up.
	uint32_t n_node_names;                      ///< The number of cluster nodes to be backed up.
	FILE *mach_fd;                              ///< The file descriptor for the machine-readable
} counter_thread_args;
