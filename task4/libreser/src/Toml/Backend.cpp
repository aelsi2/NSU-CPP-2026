#include "LibReser/Toml/Backend.hpp"

#include "LibReser/Toml/AST.hpp"
#include "LibReser/Toml/IRConverter.hpp"
#include "LibReser/Toml/Parser.hpp"
#include "LibReser/Toml/Printer.hpp"

namespace reser {
namespace toml {
static FormatBackendImpl<TomlNode, toml::Printer, toml::Parser, toml::IRConverter> BackendImpl;
FormatBackend &Backend = BackendImpl;
};  // namespace toml
};  // namespace reser
