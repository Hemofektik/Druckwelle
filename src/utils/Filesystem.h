#if HAS_FILE_SYSTEM
#include <filesystem>
#else
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#endif

#if HAS_FILE_SYSTEM
using namespace std::experimental::filesystem::v1;
#define fsend(x) end(x)
#else
using namespace boost::filesystem;
typedef boost::system::error_code error_code;
static directory_iterator fs_end_iter;
#define fsend(x) fs_end_iter
#endif
