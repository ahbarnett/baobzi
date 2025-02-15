#ifndef BAOBZI_HEADER_H
#define BAOBZI_HEADER_H

#define BAOBZI_HEADER_VERSION 1

typedef double (*baobzi_input_func_t)(const double *, const void *);

typedef struct {
    baobzi_input_func_t func;
    void *data;
    int dim;
    int order;
    double tol;
} baobzi_input_t;

#ifdef BAOBZI_TEMPLATE_HPP
#include <msgpack.hpp>
struct baobzi_header_t {
    int dim;
    int order;
    int version;
    MSGPACK_DEFINE(dim, order, version);
};
#else
/// @brief header for serialization
typedef struct {
    int dim;     ///< Dimension of function
    int order;   ///< Order of polynomial
    int version; ///< Version of output format (BAOBZI_HEADER_VERSION)
} baobzi_header_t;
#endif

#endif
