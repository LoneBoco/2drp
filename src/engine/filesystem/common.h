#ifdef __GNUC__
#include <experimental/filesystem>
#elif _MSC_VER
#include <filesystem>
#endif

#if defined(_MSC_VER) || defined(__GNUC__)
namespace filesystem = std::experimental::filesystem;
#else
namespace filesystem = std::filesystem;
#endif
