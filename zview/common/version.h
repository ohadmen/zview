#pragma once
#include <array>
#include <QString>

class Version
{
public:
    typedef std::array<std::uint8_t,3> DATA_TYPE;

    constexpr Version(std::uint8_t major,std::uint8_t minor,std::uint8_t fix):data_{major,minor,fix}{};

    QString toQString() const
    {
        return QString::number(data_[0])+"."+ QString::number(data_[1])+"."+ QString::number(data_[2]);
    }
    DATA_TYPE toArray()const {return data_;}
private:
    const DATA_TYPE data_;

    
};

