//
// Created by abentler on 13.04.24.
//

#pragma once

#include "ServiceControllerInterface.hpp"
#include "ServiceInterface.hpp"

namespace zoo {

class Compound;
class AnimalService;
class CompoundService;

class ServiceController
    : public ServiceControllerInterface {
public:
    explicit ServiceController(std::unique_ptr<AnimalService> animalsService, std::unique_ptr<CompoundService> compoundService);
    [[nodiscard]] rest::Response getAllCompounds() const override;
    [[nodiscard]] rest::Response getCompoundByName(const rest::Request& r) const override;
    [[nodiscard]] rest::Response getAnimalByName(const rest::Request& r) const override;
    [[nodiscard]] rest::Response addAnimalToCompound(const rest::Request& r) override;
    [[nodiscard]] rest::Response deleteAnimalFromCompound(const rest::Request& r) override;
    [[nodiscard]] rest::Response getAllAnimalsBySpecies(const rest::Request& r) override;

private:
    [[nodiscard]] std::string parse(const std::vector<std::reference_wrapper<const Compound>>& compounds) const;
    [[nodiscard]] std::optional<std::size_t> tryAdd(std::string_view body) const;
    [[nodiscard]] std::optional<std::size_t> tryDelete(std::string_view body) const;

    std::unique_ptr<AnimalService> m_animalService;
    std::unique_ptr<CompoundService> m_compoundService;
};

}
