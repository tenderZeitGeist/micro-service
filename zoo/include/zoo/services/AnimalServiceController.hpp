//
// Created by abentler on 13.04.24.
//

#pragma once

#include <core/repository/ServiceControllerInterface.hpp>

namespace zoo {

class Compound;
class AnimalRepository;
class CompoundRepository;

class AnimalServiceController
    : public core::repository::ServiceControllerInterface {

    struct DeleteResourceId {
        std::string compoundName;
        std::string animalName;
    };

public:
    explicit AnimalServiceController(std::unique_ptr<AnimalRepository> animalsService, std::unique_ptr<CompoundRepository> compoundService);
    [[nodiscard]] rest::Response getAllCompounds() const override;
    [[nodiscard]] rest::Response getCompoundByName(const rest::Request& r) const override;
    [[nodiscard]] rest::Response getAnimalByName(const rest::Request& r) const override;
    [[nodiscard]] rest::Response addAnimalToCompound(const rest::Request& r) override;
    [[nodiscard]] rest::Response deleteAnimalFromCompound(const rest::Request& r) override;
    [[nodiscard]] rest::Response getAllAnimalsBySpecies(const rest::Request& r) override;

private:
    [[nodiscard]] std::string parse(const std::string& baseUri, const std::vector<std::reference_wrapper<const Compound>>& compounds) const;
    [[nodiscard]] std::optional<std::size_t> tryAdd(std::string_view body) const;
    [[nodiscard]] std::optional<DeleteResourceId> tryDelete(std::string_view body) const;

    std::unique_ptr<AnimalRepository> m_animalService;
    std::unique_ptr<CompoundRepository> m_compoundService;
};

}
