//
// Created by zeitgeist on 12.04.24.
//
#pragma once

#include <string>

class Compound {
public:
    Compound(std::string name, std::size_t sizeInSqm);
private:
    std::string m_name;
    std::size_t m_sizeInSqm;
};
