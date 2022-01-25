#ifndef BAOBZI_HPP
#define BAOBZI_HPP

#include <baobzi.h>
#include <string>

namespace baobzi {
/// Wrapper class for C library (which is a wrapper for the template library. oof)
class Baobzi {
  private:
    baobzi_t obj_ = nullptr; ///< Pointer to C baobzi struct this class wraps
  public:
    /// @brief Construct Baobzi object from input function
    /// @param[in] input pointer to baobzi_input_t object
    /// @param[in] center [dim] center of the domain
    /// @param[in] half_length [dim] half the size of the domain in each dimension
    Baobzi(const baobzi_input_t *input, const double *center, const double *half_length)
        : obj_(baobzi_init(input, center, half_length)) {}

    /// @brief Restore baobzi object from serialized version
    /// @param[in] input_file path to file of serialized function
    Baobzi(const std::string &input_file) : obj_(baobzi_restore(input_file.c_str())) {}

    /// @brief Destroy Baobzi object and associated memory
    ~Baobzi() { obj_ = baobzi_free(obj_); }

    /// @brief Save Baobzi object to file
    /// @param[in] output_file path of file to serialize object to
    void save(const std::string &output_file) { baobzi_save(obj_, output_file.c_str()); }

    /// @brief Evaluate Baobzi object at point
    /// @param[in] x [dim]
    /// @return approximate value of function at point x
    double operator()(const double *x) const { return baobzi_eval(obj_, x); }
};
} // namespace baobzi
#endif
