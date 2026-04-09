#include <unistd.h>
#include <fcntl.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

#include "frontend/lexer.hpp"
#include "frontend/parser.hpp"
#include "frontend/semantic/semantic_resolver.hpp"
#include "utils/diagnostic.hpp"

using namespace Spark;
using namespace Spark::FrontEnd;

static void printUsage(std::ostream& os, std::string_view exe) {
    os << "usage: " << exe << " [-l <lua_path>] <file>\n";
}

static int run(const std::string& luaPath, const std::string& source) {
    // Creates a temporary file
    char tmpl[] = "/tmp/sparkXXXXXX";
    int fd = mkstemp(tmpl);
    if (fd == -1) {
        std::cerr << "error: failed to create temp file\n";
        return false;
    }

    // Adds `.lua` extension
    std::string filename = std::string{tmpl} + ".lua";
    if (rename(tmpl, filename.c_str()) != 0) {
        std::cerr << "error: failed to rename temp file\n";
        close(fd);
        return false;
    }

    // Writes source
    ssize_t written = write(fd, source.data(), source.size());
    if (written < 0 || static_cast<size_t>(written) != source.size()) {
        std::cerr << "error: failed to write temp file\n";
        close(fd);
        return false;
    }

    // Closes temp file
    close(fd);

    // Runs Lua
    std::string cmd = luaPath + " \"" + filename + "\"";
    int result = std::system(cmd.c_str());

    // Cleans up
    unlink(filename.c_str());

    return result;
}

int main(int argc, char* argv[]) {
    // Checks argument count (at least 1)
    if (argc < 2) {
        printUsage(std::cerr, argv[0]);
        return EXIT_FAILURE;
    }

    // Argument parsing
    std::string luaPath = "lua";
    bool hasFilePath = false;
    std::string filePath;
    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];
        if (arg == "-l") {
            if (i + 1 >= argc) {
                std::cerr << "error: -l requires an argument\n";
                return EXIT_FAILURE;
            }
            luaPath = argv[++i];
        } else {
            if (hasFilePath) {
                printUsage(std::cerr, argv[0]);
                return EXIT_FAILURE;
            }
            filePath = argv[i];
            hasFilePath = true;
        }
    }
    if (!hasFilePath) {
        printUsage(std::cerr, argv[0]);
        return EXIT_FAILURE;
    }

    // Check file extension
    if (filePath.size() < 6 || filePath.substr(filePath.size() - 6) != ".spark") {
        std::cerr << "error: source file must have `.spark` extension\n";
        return EXIT_FAILURE;
    }

    // Gets path and open file
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "error: cannot open file: " << filePath << "\n";
        return EXIT_FAILURE;
    }

    // Handle BOM
    {
        char bom[3];
        file.read(bom, 3);
        if (!(file.gcount() == 3 &&
            (unsigned char)bom[0] == 0xEF && (unsigned char)bom[1] == 0xBB && (unsigned char)bom[2] == 0xBF)) {
            file.clear();
            file.seekg(0, std::ios::beg);
        }
    }

    // Parse
    Lexer lexer(file);
    yy::parser::location_type loc;
    AST ast;
    Diagnostics parserDiags;
    yy::parser parser(lexer.scanner(), &loc, ast, parserDiags);
    if (parser.parse() == 2) {
        throw std::bad_alloc();
    }
    if (parserDiags.hasError()) {
        parserDiags.render(std::cout, filePath);
        return EXIT_FAILURE;
    }

    // Semantic pass / codegen
    Diagnostics semanticDiags;
    SemanticResolver resolver{semanticDiags};
    std::string luaSource = resolver.emit(ast.root);
    if (semanticDiags.hasError()) {
        semanticDiags.render(std::cout, filePath);
        return EXIT_FAILURE;
    }

    std::cout << "source: \n" << luaSource << "\n\nresult: \n";

    // Execution
    return run(luaPath, luaSource);
}
