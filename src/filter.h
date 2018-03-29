/* We start with 4 dimensions for the decision tree */
typedef struct filter {
        long src_ip;
        long dst_ip;
        short src_port;
        short dst_port;
} filter_t;
