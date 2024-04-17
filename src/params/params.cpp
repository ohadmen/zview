#include "params.h"
namespace zview{
    Params& Params::i()
    {
        static Params instance;
        return instance;
    }
    void Params::load(std::string fn)
    {
    }

}