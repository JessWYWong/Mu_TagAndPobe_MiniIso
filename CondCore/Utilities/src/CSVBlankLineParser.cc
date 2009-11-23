#include "CondCore/Utilities/interface/CSVBlankLineParser.h"
#include <boost/spirit//include/classic_core.hpp>
using namespace boost::spirit;

bool CSVBlankLineParser::isBlank( const std::string& inputLine){
  boost::spirit::rule<> blankparser=*blank_p;
  return boost::spirit::parse(inputLine.c_str(),blankparser).full;
}
