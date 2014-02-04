#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include <stdbool.h>
#include <stdint.h>
#define MATH_INT64_NATIVE_IF_AVAILABLE
#include "perl_math_int64.h"
#include "perl_math_int128.h"

#define MMDBW_RECORD_TYPE_EMPTY (0)
#define MMDBW_RECORD_TYPE_DATA (1)
#define MMDBW_RECORD_TYPE_NODE (2)
#define MMDBW_RECORD_TYPE_ALIAS (3)

#define FLIP_NETWORK_BIT(network, max_depth0, depth) \
    ((network) | ((uint128_t)1 << ((max_depth0) - (depth))))

#if MMDBW_UINT128_USING_MODE
typedef unsigned int mmdbw_uint128_t __attribute__ ((__mode__(TI)));
#else
typedef unsigned __int128 mmdbw_uint128_t;
#endif

typedef struct MMDBW_record_s {
    int type;
    union {
        SV *key;
        struct MMDBW_node_s *node;
    } value;
} MMDBW_record_s;

/* This is a linked list so we can loop through all the nodes ever allocated
 * when freeing the tree. It's possible that we have allocated nodes that are
 * no longer reachable in the tree, but we still need to free them and
 * decrement the ref counts for any SVs they refer to. */
typedef struct MMDBW_node_s {
    uint64_t number;
    MMDBW_record_s left_record;
    MMDBW_record_s right_record;
    struct MMDBW_node_s *next_node;
} MMDBW_node_s;

typedef struct MMDBW_tree_s {
    uint8_t ip_version;
    uint8_t record_size;
    bool merge_record_collisions;
    HV *data_hash;
    MMDBW_node_s *root_node;
    MMDBW_node_s *last_node;
    uint32_t node_count;
    bool is_finalized;
    void *iteration_args;
} MMDBW_tree_s;

typedef struct MMDBW_network_s {
    uint8_t *bytes;
    uint8_t mask_length;
    uint8_t max_depth0;
    int family;
    const char *address_string;
    const char *as_string;
} MMDBW_network_s;

    /* *INDENT-OFF* */
    /* --prototypes automatically generated by dev-bin/regen-prototypes.pl - don't remove this comment */
    extern MMDBW_tree_s *new_tree(uint8_t ip_version, uint8_t record_size,
                                  bool merge_record_collisions);
    extern int insert_network(MMDBW_tree_s *tree, char *ipstr, uint8_t mask_length,
                              SV *key, SV *data);
    extern void delete_reserved_networks(MMDBW_tree_s *tree);
    extern void alias_ipv4_networks(MMDBW_tree_s *tree);
    extern SV *merge_hashes_for_keys(MMDBW_tree_s *tree, SV *key_from, SV *key_into,
                                     MMDBW_network_s *network);
    extern SV *lookup_ip_address(MMDBW_tree_s *tree, char *ipstr);
    extern MMDBW_node_s *new_node(MMDBW_tree_s *tree);
    extern void finalize_tree(MMDBW_tree_s *tree);
    extern void write_search_tree(MMDBW_tree_s *tree, SV *output, bool alias_ipv6,
                                  SV *root_data_type, SV *serializer);
    extern void start_iteration(MMDBW_tree_s *tree,
                                void(callback) (MMDBW_tree_s * tree,
                                                MMDBW_node_s * node,
                                                mmdbw_uint128_t network,
                                                uint8_t depth));
    extern SV *data_for_key(MMDBW_tree_s *tree, SV *key);
    extern void free_tree(MMDBW_tree_s *tree);
    extern char *record_type_name(int record_type);
    extern void dwarn(SV *thing);
    extern void warn_hex(uint8_t digest[16], char *where);
    extern char *md5_as_hex(uint8_t digest[16]);
    /* --prototypes end - don't remove this comment-- */
    /* *INDENT-ON* */
