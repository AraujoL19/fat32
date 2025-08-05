#include "fat_manager.hpp"
#include "shell.hpp"
#include <iostream>
#include <filesystem>

int main() {
    if (!std::filesystem::exists(FAT_FILENAME)) {
        std::cout << "Inicializando novo sistema de arquivos...\n";
        init_fs();
    } else {
        load_fat();
    }

    shell_loop();
    return 0;
}
