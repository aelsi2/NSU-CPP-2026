#include "LibReser/Xml/Backend.hpp"

#include "LibReser/Xml/AST.hpp"
#include "LibReser/Xml/IRConverter.hpp"
#include "LibReser/Xml/Parser.hpp"
#include "LibReser/Xml/Printer.hpp"

namespace reser {
namespace xml {
static FormatBackendImpl<XmlNode, xml::Printer, xml::Parser, xml::IRConverter> BackendImpl;
FormatBackend &Backend = BackendImpl;
};  // namespace xml
};  // namespace reser
