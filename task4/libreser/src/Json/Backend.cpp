#include "LibReser/Json/Backend.hpp"

#include "LibReser/Json/AST.hpp"
#include "LibReser/Json/IRConverter.hpp"
#include "LibReser/Json/Parser.hpp"
#include "LibReser/Json/Printer.hpp"

namespace reser {
namespace json {
static FormatBackendImpl<JsonNode, json::Printer, json::Parser, json::IRConverter> BackendImpl;
FormatBackend &Backend = BackendImpl;
};  // namespace json
};  // namespace reser
