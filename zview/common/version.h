#pragma once
#include <array>
#include <string>

class Version
{
public:
    typedef std::array<std::uint8_t,3> DATA_TYPE;

    constexpr Version(std::uint8_t major,std::uint8_t minor,std::uint8_t fix):data_{major,minor,fix}{};

    std::string toQString() const
    {
        return std::to_string(data_[0])+"."+ std::to_string(data_[1])+"."+ std::to_string(data_[2]);
    }
    DATA_TYPE toArray()const {return data_;}
private:
    const DATA_TYPE data_;

    
};

