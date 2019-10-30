/*
* enum_converter
* Copyright (C) 2019 Lenewt
*
* This program is free software: you can redistribute it and/or modify
*         it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
*         but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LENEWT_ENUMCONVERTER_HPP_
#define LENEWT_ENUMCONVERTER_HPP_

#include <utility>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <string>
#include <algorithm>

namespace lenewt {

    template<typename InternalType, typename ExternalType>
    class enum_converter {
    public:
        class equiv {
        public:
            enum class type_t {
                Equiv,
                Internal,
                External
            };

            equiv(const InternalType&& internal, const ExternalType&& external) :
                    equiv{std::forward<const InternalType>(internal), std::forward<const ExternalType>(external), type_t::Equiv} {}

            const InternalType internal;
            const ExternalType external;
            const type_t type;

        protected:
            equiv(const InternalType&& internal, const ExternalType&& external, const type_t&& type) :
                    internal{internal}, external{external}, type{type} {}
        };
        class external_proj : public equiv {
        public:
            external_proj(const InternalType&& internal, const ExternalType&& external) :
            equiv{std::forward<const InternalType>(internal), std::forward<const ExternalType>(external), equiv::type_t::External} {}
        };
        class internal_proj : public equiv {
        public:
            internal_proj(const InternalType&& internal, const ExternalType&& external) :
            equiv{std::forward<const InternalType>(internal), std::forward<const ExternalType>(external), equiv::type_t::Internal} {}
        };

        class duplicate_enum_error : public std::invalid_argument {
        public:
            explicit duplicate_enum_error(const std::string&& what) noexcept : std::invalid_argument{what} {}
        };
        class undefined_enum_error : public std::invalid_argument {
        public:
            explicit undefined_enum_error(const std::string &&what) noexcept : std::invalid_argument{what} {}
        };


        explicit enum_converter(const std::initializer_list<equiv>&& equivList) {
            std::for_each(std::cbegin(equivList), std::cend(equivList), [this](const auto& equiv) {
               _insert_equivalent(equiv); });
        }

        ExternalType cast(const InternalType& type) const {
            const auto& externalOpt = try_cast(type);

            if (!externalOpt)
                throw undefined_enum_error("No valid cast value is defined for internal value");
            return *externalOpt;
        }

        InternalType cast(const ExternalType& type) const {
            const auto& internalOpt = try_cast(type);

            if (!internalOpt)
                throw undefined_enum_error("No valid cast value is defined for external value");
            return *internalOpt;
        }

        std::optional<ExternalType> try_cast(const InternalType& type) const noexcept {
            return _try_cast<InternalType, ExternalType>(type, _internalExternalMap, _intToExtProjections);
        }

        std::optional<InternalType> try_cast(const ExternalType& type) const noexcept {
            return _try_cast<ExternalType, InternalType>(type, _externalInternalMap, _extToIntProjections);
        }

    private:
        template <typename In, typename Out, typename EquivMap, typename ProjMap>
        std::optional<Out> _try_cast(const In& in, const EquivMap& equivalents, const ProjMap& projections) const noexcept {
            const auto& equivItr = equivalents.find(in);
            if (equivItr != std::cend(equivalents)) {
                return equivItr->second;
            }

            const auto& projectionItr = projections.find(in);
            if (projectionItr != std::cend(projections)) {
                return projectionItr->second;
            }

            return std::nullopt;
        }

        void _insert_equivalent(const equiv &equiv) {
            switch (equiv.type) {
                case equiv::type_t::External:
                    if (_is_projection_defined(_extToIntProjections, equiv.external)) {
                        throw duplicate_enum_error{"Same enum external projection defined multiple times."};
                    }
                    _extToIntProjections[equiv.external] = equiv.internal;
                    break;
                case equiv::type_t::Internal:
                    if (_is_projection_defined(_intToExtProjections, equiv.internal)) {
                        throw duplicate_enum_error{"Same enum internal projection defined multiple times."};
                    }
                    _intToExtProjections[equiv.internal] = equiv.external;
                    break;
                case equiv::type_t::Equiv:
                    if (_is_equiv_defined(_externalInternalMap, equiv.external, equiv.internal)) {
                        throw duplicate_enum_error{"Same enum equivalent defined multiple times."};

                    }
                    _externalInternalMap[equiv.external] = equiv.internal;
                    _internalExternalMap[equiv.internal] = equiv.external;
                    break;
            }
        }

        template<typename Map, typename Key>
        bool _is_projection_defined(const Map &map, const Key &key) const noexcept {
            return std::any_of(std::cbegin(map), std::cend(map),
                               [&key](const auto& pair) {
                            return  key == pair.first;
                        });
        }

        template<typename Map, typename Key, typename Value>
        bool _is_equiv_defined(const Map &map, const Key &key, const Value &value) const noexcept {
            return std::any_of(std::cbegin(map), std::cend(map),
                               [&key, &value](const auto& pair) {
                                   return  key == pair.first || value == pair.second;
                               });
        }

        std::unordered_map<InternalType, ExternalType> _internalExternalMap;
        std::unordered_map<ExternalType, InternalType> _externalInternalMap;
        std::unordered_map<InternalType, ExternalType> _intToExtProjections;
        std::unordered_map<ExternalType, InternalType> _extToIntProjections;
    };
}

#endif