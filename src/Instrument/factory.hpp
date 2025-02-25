#ifndef FACTORY_HPP
#define FACTORY_HPP

#include <map>
#include <memory>
#include <string>
#include <functional>
#include "instrument.hpp"  // Clase base Instrument

class Factory {
public:
    using Builder = std::function<std::unique_ptr<Instrument>(const std::string&)>;
    
    static Factory& instance(); // Singleton
    
    void registerBuilder(const std::string& type, Builder builder);
    std::unique_ptr<Instrument> create(const std::string& type) const;

private:
    Factory() = default;
    std::map<std::string, Builder> builders_;
};

#endif // FACTORY_HPP