#pragma once
#include <string>
#include <stdint.h>

namespace SohUtils {
    int32_t GetItemIdIconFromRandomizerGet(int32_t randomizerGet);

    const char* GetIconNameFromItemID(int32_t itemId);

    const std::string& GetSceneName(int32_t scene);

    const std::string& GetItemName(int32_t item);

    const std::string& GetQuestItemName(int32_t item);

    // Copies a string and ensures the destination is null terminated if the source string is larger than size
    // Only up to size-1 characters are copied from the source string
    void CopyStringToCharArray(char* destination, std::string source, size_t size);

    std::string Sanitize(std::string stringValue);
} // namespace SohUtils
