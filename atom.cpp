#include "atom.hpp"

Atom::Atom() {
  m_type = Type::NoneKind;
}

Atom::Atom(double value){
  setNumber(value);
}

Atom::Atom(std::complex<double> value): Atom() {
  setComplex(value);
}

Atom::Atom(const Token & token): Atom(){

  std::istringstream iss(token.asString());
  double temp;
  if(iss >> temp){
    // is token a number?
    // check for trailing characters if >> succeeds
    if(iss.rdbuf()->in_avail() == 0){
      setNumber(temp);
    }
  }
  else if(!std::isdigit(token.asString()[0]) ){ 
      // else assume symbol
      setSymbol(token.asString());
  }
}

Atom::Atom(const std::string & value): Atom() {
  setSymbol(value);
}

Atom::Atom(const Atom & x): Atom(){

  if(x.isNumber()){
    setNumber(x.numberValue);
  }
  else if(x.isSymbol() || x.isString()){
    setSymbol(x.stringValue);
  }
  else if(x.isComplex()){
    setComplex(x.complexValue);
  }
}

Atom & Atom::operator=(const Atom & x){

  if(this != &x){
    if(x.m_type == NoneKind){
      m_type = NoneKind;
    }
    else if(x.m_type == NumberKind){
      setNumber(x.numberValue);
    }
    else if(x.m_type == SymbolKind){
      setSymbol(x.stringValue);
    }
    else if(x.m_type == ComplexKind){
      setComplex(x.complexValue);
    }
  }
  return *this;
}

Atom::~Atom(){

  //ensure the destructor of the symbol string is called
  if(m_type == SymbolKind){
    stringValue.~basic_string();
  }
}

bool Atom::isNone() const noexcept{
  return m_type == NoneKind;
}

bool Atom::isNumber() const noexcept{
  return m_type == NumberKind;
}

bool Atom::isSymbol() const noexcept{
  return (m_type == SymbolKind && stringValue[0] != '"');
}

bool Atom::isComplex() const noexcept{
  return m_type == ComplexKind;
}

bool Atom::isString() const noexcept{
  return m_type == SymbolKind && stringValue[0] == '"';
}

void Atom::setNumber(double value){

  m_type = NumberKind;
  numberValue = value;
}

void Atom::setSymbol(const std::string & value){

  // we need to ensure the destructor of the symbol string is called
  if(m_type == SymbolKind){
    stringValue.~basic_string();
  }

  m_type = SymbolKind;

  // copy construct in place
  new (&stringValue) std::string(value);
}

void Atom::setComplex(const std::complex<double> & value){
  m_type = ComplexKind;
  complexValue = value;
}

double Atom::asNumber() const noexcept{
  if (m_type == ComplexKind) {
    return complexValue.real();
  }
  return (m_type == NumberKind) ? numberValue : 0.0;
}


std::string Atom::asSymbol() const noexcept{

  std::string s;

  if(m_type == SymbolKind){
    s = stringValue;
    s.erase(remove( s.begin(), s.end(), '\"' ),s.end());
  }

  return s;
}

std::string Atom::asString() const noexcept{

  std::ostringstream os;

  if(m_type == SymbolKind){
    os << stringValue;
  }
  else if (m_type == NumberKind){
    os << numberValue;
  }
  else if (m_type == ComplexKind){
    os << complexValue;
  }

  return os.str();
}

std::complex<double> Atom::asComplex() const noexcept{
  if (m_type == NumberKind) {
    std::complex<double> number2complex(numberValue, 0.0);
    return number2complex;
  }
  return (m_type == ComplexKind) ? complexValue : (std::complex<double>)(0);
}

bool Atom::operator==(const Atom & right) const noexcept{

  if(m_type != right.m_type) return false;

  switch(m_type){
    case NoneKind:
      if(right.m_type != NoneKind) return false;
      break;
    case NumberKind:
      {
        if(right.m_type != NumberKind) return false;
        double dleft = numberValue;
        double dright = right.numberValue;
        double diff = fabs(dleft - dright);
        if(std::isnan(diff) || (diff > (std::numeric_limits<double>::epsilon()*2) ) )
          return false;
      }
      break;
    case SymbolKind:
      {
        if(right.m_type != SymbolKind) return false;
        return stringValue == right.stringValue;
      }
      break;
    case ComplexKind:
    {
      if(right.m_type != ComplexKind) return false;
      std::complex<double> diff;
      diff = (complexValue - right.complexValue);
      double realPart = std::fabs(diff.real());
      double imagPart = std::fabs(diff.imag());
      if(realPart > std::numeric_limits<double>::epsilon()*2 || imagPart > std::numeric_limits<double>::epsilon()*2)
        return false;
    }
    break;
  }

  return true;
}

bool operator!=(const Atom & left, const Atom & right) noexcept{

  return !(left == right);
}


std::ostream & operator<<(std::ostream & out, const Atom & a){

  if(a.isNumber()){
    out << a.asNumber();
  }
  if(a.isSymbol() || a.isString()){
    out << a.asString();
  }
  if(a.isComplex()){
    out << a.asComplex();
  }
  return out;
}
