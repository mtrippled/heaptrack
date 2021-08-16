/*
    symbolcache.h

    SPDX-FileCopyrightText: 2022 Klarälvdalens Datakonsult AB a KDAB Group company <info@kdab.com>
    SPDX-FileContributor: Milian Wolff <milian.wolff@kdab.com>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SYMBOLCACHE_H
#define SYMBOLCACHE_H

#include <tsl/robin_map.h>

#include <string>
#include <vector>

#include <elfutils/libdwfl.h>

class SymbolCache
{
public:
    struct SymbolCacheEntry
    {
        SymbolCacheEntry() = default;
        SymbolCacheEntry(uint64_t offset, uint64_t value, uint64_t size, std::string symname)
            : offset(offset)
            , value(value)
            , size(size)
            , symname(std::move(symname))
        {
        }

        bool isValid() const
        {
            return !symname.empty();
        }

        // adjusted/absolute st_value, see documentation of the `addr` arg in `dwfl_module_getsym_info`
        uint64_t offset = 0;
        // unadjusted/relative st_value
        uint64_t value = 0;
        uint64_t size = 0;
        std::string symname;
        bool demangled = false;
    };
    using Symbols = std::vector<SymbolCacheEntry>;

    /// check if @c setSymbolCache was called for @p filePath already
    bool hasSymbols(const std::string& filePath) const;
    /// take @p cache, sort it and use it for symbol lookups in @p filePath
    void setSymbols(const std::string& filePath, Symbols symbols);
    /// find the symbol that encompasses @p relAddr in @p filePath
    /// if the found symbol wasn't yet demangled, it will be demangled now
    SymbolCacheEntry findSymbol(const std::string& filePath, uint64_t relAddr);

    /// extract all symbols in @p module into a structure suitable to be passed to @p setSymbols
    static Symbols extractSymbols(Dwfl_Module* module, uint64_t elfStart, bool isArmArch);

private:
    tsl::robin_map<std::string, Symbols> m_symbolCache;
};

#endif // SYMBOLCACHE_H
