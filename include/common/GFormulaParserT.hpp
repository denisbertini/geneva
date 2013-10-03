/**
 * @file GFormulaParserT.hpp
 */

/*
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * This file is part of the Geneva library collection.
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: contact [at] gemfony (dot) eu
 *
 * Geneva was developed with kind support from Karlsruhe Institute of
 * Technology (KIT) and Steinbuch Centre for Computing (SCC). Further
 * information about KIT and SCC can be found at http://www.kit.edu/english
 * and http://scc.kit.edu .
 *
 * Geneva is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Boost Software License for more details.
 *
 * For further information on Gemfony scientific and Geneva, visit
 * http://www.gemfony.eu .
 */

/*
 * This code is based on a number of examples shipped with the Boost.Spirit
 * library, particularly "calc6.cpp" from Boost 1.54. Consequently, the following
 * license applies to the code in this file:
 *
 *******************************************************************************
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 *******************************************************************************
 *
 * NOTE THAT THE BOOST-LICENSE DOES NOT APPLY TO ANY OTHER FILES OF THE
 * GENEVA LIBRARY, UNLESS THIS IS EXPLICITLY STATED IN THE CORRESPONDING FILE.
 */

// Standard headers go here
#include <iostream>
#include <string>
#include <stack>
#include <map>
#include <cmath>

// Boost headers go here
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/foreach.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/optional.hpp>
#include <boost/function.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/variant.hpp>
#include <boost/function.hpp>
#include <boost/utility.hpp>

#ifndef GFORMULAPARSERT_HPP_
#define GFORMULAPARSERT_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif

// Geneva headers go here
#include "common/GExceptions.hpp"
#include "common/GMathHelperFunctions.hpp"

namespace Gem {
namespace Common {

/******************************************************************************/
// The Abstract Syntax Tree + access functions

struct nil {};
struct signed_;
struct unary_function_;
struct binary_function_;
struct ast_expression;

typedef boost::variant<
   nil
   , float
   , double
   , boost::recursive_wrapper<signed_>
   , boost::recursive_wrapper<unary_function_>
   , boost::recursive_wrapper<binary_function_>
   , boost::recursive_wrapper<ast_expression>
>
operand;

struct signed_ {
   char sign;
   operand operand_;
};

struct operation {
   char operator_;
   operand operand_;
};

struct unary_function_ {
   std::string fname_;
   operand operand_;
};

struct binary_function_ {
   std::string fname_;
   operand operand1_;
   operand operand2_;
};

struct ast_expression {
   operand first;
   std::list<operation> rest;
};

/** @brief print function for debugging */
inline std::ostream& operator<<(std::ostream& out, nil) { out << "nil"; return out; }

} /* namespace Common */
} /* namespace Gem */

BOOST_FUSION_ADAPT_STRUCT(
   Gem::Common::signed_,
   (char, sign)
   (Gem::Common::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
   Gem::Common::operation,
   (char, operator_)
   (Gem::Common::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
   Gem::Common::unary_function_,
   (std::string, fname_)
   (Gem::Common::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
   Gem::Common::binary_function_,
   (std::string, fname_)
   (Gem::Common::operand, operand1_)
   (Gem::Common::operand, operand2_)
)

BOOST_FUSION_ADAPT_STRUCT(
   Gem::Common::ast_expression,
   (Gem::Common::operand, first)
   (std::list<Gem::Common::operation>, rest)
)

namespace Gem {
namespace Common {

/******************************************************************************/
/**
 * This class allows to parse and evaluate simple mathematical formulas of the
 * "type (sin(3.)*sqrt(5.) - (2*pi))^2". Formulas may optionally contain place
 * holders for variables, e.g. "(sin({{var1}})*sqrt({{var2}}) - ({{var3}}*pi))^2".
 * Formulas are provided in string form to the constructor. The evaluate()
 * function will then replace the place-holders with the corresponding entries of
 * a std::map<std::string, fp_type>. An object of this class may deal with a single
 * formula only, which is given to it through the constructor. When a formula cannot
 * be parsed, an exception will be thrown. Note that the class currently only handles
 * floating point values (float and double). The class is based on a number of
 * examples taken from the Boost.Spirit code base, particularly calc6.cpp from
 * Boost version 1.54.
 */
/*****************************************************************************/
/**
 * The actual formula parser
 */
template <typename fp_type>
class GFormulaParserT
   : public boost::spirit::qi::grammar<std::string::const_iterator, ast_expression(), boost::spirit::ascii::space_type>
{
   // Make sure, fp_type is a floating point value
   BOOST_MPL_ASSERT((boost::is_floating_point<fp_type>));

public:
   /*****************************************************************************/
   /**
    * Specifies the operations the parser must know about
    */
   enum byte_code {
      op_trap   = 0,      // triggers an exception --> boost::variant<int,fp_type>() == 0
      op_neg    = 1,      // negate the top stack entry
      op_add    = 2,      // add top two stack entries
      op_sub    = 3,      // subtract top two stack entries
      op_mul    = 4,      // multiply top two stack entries
      op_div    = 5,      // divide top two stack entries
      op_acos   = 7,      // Calculates the acos value of the top-most stack-entry
      op_asin   = 8,      // Calculates the asin value of the top-most stack-entry
      op_atan   = 9,      // Calculates the atan value of the top-most stack-entry
      op_ceil   = 10,     // Calculates the ceil value of the top-most stack-entry
      op_cos    = 11,     // Calculates the cos value of the top-most stack-entry
      op_cosh   = 12,     // Calculates the cosh value of the top-most stack-entry
      op_exp    = 13,     // Calculates the exp value of the top-most stack-entry
      op_fabs   = 14,     // Calculates the fabs value of the top-most stack-entry
      op_floor  = 15,     // Calculates the floor value of the top-most stack-entry
      op_log    = 16,     // Calculates the log value of the top-most stack-entry
      op_log10  = 17,     // Calculates the log10 value of the top-most stack-entry
      op_sin    = 18,     // Calculates the sin value of the top-most stack-entry
      op_sinh   = 19,     // Calculates the sinh value of the top-most stack-entry
      op_sqrt   = 20,     // Calculates the sqrt value of the top-most stack-entry
      op_tan    = 21,     // Calculates the tan value of the top-most stack-entry
      op_tanh   = 22,     // Calculates the tanh value of the top-most stack-entry
      op_pow    = 23,     // Calculates the pow value of the two top-most stack-entries
      op_hypot  = 24,     // Calculates the hypot value of the two top-most stack-entries
      op_min    = 25,     // Calculates the min value of the two top-most stack-entries
      op_max    = 26,     // Calculates the max value of the two top-most stack-entries
      op_fp     = 27,     // Pushes a fp_type onto the stack
   };

   typedef void result_type; // Needed for the operator() and apply_visitor
   typedef boost::variant<byte_code,fp_type> codeEntry;

   /***************************************************************************/
   /**
    * The standard constructor
    */
   explicit GFormulaParserT(
     const std::string& formula
     , const std::map<std::string, fp_type>& user_constants = std::map<std::string, fp_type>()
   )
   : GFormulaParserT::base_type(expression_rule_)
   , raw_formula_(formula)
   , stack_(4096)
   , stack_ptr_(stack_.begin())
   , error_handler(error_handler_())
   , printCode_(false)
   {
      boost::spirit::qi::char_type char_;
      boost::spirit::qi::string_type string_;
      boost::spirit::qi::_2_type _2;
      boost::spirit::qi::_3_type _3;
      boost::spirit::qi::_4_type _4;

      using boost::spirit::qi::on_error;
      using boost::spirit::qi::fail;

      //---------------------------------------------------------------------------
      // Define a number of mathematical constants
      constants_.add
      ("e" , boost::math::constants::e<fp_type>() )
      ("pi", boost::math::constants::pi<fp_type>() )
      ;

      // Add user-defined constants
      typename std::map<std::string, fp_type>::const_iterator cit;
      for(cit=user_constants.begin(); cit!=user_constants.end(); ++cit) {
         constants_.add(cit->first, cit->second);
      }

      //---------------------------------------------------------------------------
      // Define the actual grammar
      expression_rule_ =
            term_rule_ >> *((char_('+') > term_rule_) | (char_('-') > term_rule_));

      term_rule_ =
            factor_rule_ >> *( (char_('*') > factor_rule_) | (char_('/') > factor_rule_));

      unary_function_rule_ =
            (string_("acos")    > '(' > expression_rule_ > ')')
            | (string_("asin")    > '(' > expression_rule_ > ')')
            | (string_("atan")    > '(' > expression_rule_ > ')')
            | (string_("ceil")    > '(' > expression_rule_ > ')')
            | (string_("cosh")    > '(' > expression_rule_ > ')')
            | (string_("cos")     > '(' > expression_rule_ > ')')
            | (string_("exp")     > '(' > expression_rule_ > ')')
            | (string_("fabs")    > '(' > expression_rule_ > ')')
            | (string_("floor")   > '(' > expression_rule_ > ')')
            | (string_("log10")   > '(' > expression_rule_ > ')')
            | (string_("log")     > '(' > expression_rule_ > ')')
            | (string_("sinh")    > '(' > expression_rule_ > ')')
            | (string_("sin")     > '(' > expression_rule_ > ')')
            | (string_("sqrt")    > '(' > expression_rule_ > ')')
            | (string_("tanh")    > '(' > expression_rule_ > ')')
            | (string_("tan")     > '(' > expression_rule_ > ')')
            ;

      binary_function_rule_ =
            (string_("min")     > '(' > expression_rule_ > ',' > expression_rule_ > ')')
            | (string_("max")     > '(' > expression_rule_ > ',' > expression_rule_ > ')')
            | (string_("pow")     > '(' > expression_rule_ > ',' > expression_rule_ > ')')
            | (string_("hypot")   > '(' > expression_rule_ > ',' > expression_rule_ > ')')
            ;

      factor_rule_ =
            real
            | ('(' > expression_rule_ > ')')
            | (char_('-') > factor_rule_)
            | (char_('+') > factor_rule_)
            | unary_function_rule_
            | binary_function_rule_
            | constants_
            ;

      //---------------------------------------------------------------------------
      // Debugging and error handling and reporting support.
      BOOST_SPIRIT_DEBUG_NODES(
            (expression_rule_)
            (term_rule_)
            (unary_function_rule_)
            (binary_function_rule_)
            (factor_rule_)
      );

      // Error handling
      on_error<fail>(expression_rule_, error_handler(_4, _3, _2));
   }

   /***************************************************************************/
   /**
    * When set to true, the code-vector will be printed prior to the evaluation
    */
   void setPrintCode(bool printCode) {
      printCode_ = printCode;
   }

   /***************************************************************************/
   /**
    * Retrieves the processed formula (after replacement of place-holders)
    *
    * @param placeHolders A list of place-holders for variable values
    * @return A string containing the processed formula
    */
   std::string getFormula(const std::map<std::string, fp_type>& vm) const {
      return this->replacePlaceHolders(vm);
   }

   /***************************************************************************/
   /**
    * Evaluates a formula after replacing place holders with values
    */
   fp_type evaluate(const std::map<std::string, fp_type>& vm) const {
      // Clear local data structures
      code_.clear();
      stack_ptr_ = stack_.begin();

      ast_expression ast; ///< The abstract syntax tree

      // Replace place holders with values taken from the map
      std::string formula = this->replacePlaceHolders(vm);

      // Do the actual parsing of the formula
      std::string::const_iterator iter = formula.begin();
      std::string::const_iterator end = formula.end();
      boost::spirit::ascii::space_type space;
      bool r = boost::spirit::qi::phrase_parse(iter, end, *this, space, ast);

      if (r && iter == end) {
         this->compile(ast);
         this->execute();
      } else {
         std::string rest(iter, end);
         std::cout << "Parsing failed" << std::endl;
      }

      return stack_.at(0);
   }

   /*****************************************************************************/
   /**
    * Ease of access to the evaluate function
    */
   fp_type operator()(const std::map<std::string, fp_type>& vm) const {
      return this->evaluate(vm);
   }

   /*****************************************************************************/
   // Code for the compilation of the AST
   void operator()(nil) const { BOOST_ASSERT(0); }
   void operator()(const fp_type& fp_val) const {
      code_.push_back(codeEntry(op_fp));
      code_.push_back(codeEntry(fp_val));
   }

   void operator()(const operation& x) const {
      boost::apply_visitor(*this, x.operand_);

      if(x.operator_ == '+') code_.push_back(codeEntry(op_add));
      else if(x.operator_ == '-') code_.push_back(codeEntry(op_sub));
      else if(x.operator_ == '*') code_.push_back(codeEntry(op_mul));
      else if(x.operator_ == '/') code_.push_back(codeEntry(op_div));
      else BOOST_ASSERT(0);
   }

   void operator()(const unary_function_& f) const {
      boost::apply_visitor(*this, f.operand_);

      if(f.fname_ == "acos") code_.push_back(codeEntry(op_acos));
      else if(f.fname_ == "asin") code_.push_back(codeEntry(op_asin));
      else if(f.fname_ == "atan") code_.push_back(codeEntry(op_atan));
      else if(f.fname_ == "ceil") code_.push_back(codeEntry(op_ceil));
      else if(f.fname_ == "cos") code_.push_back(codeEntry(op_cos));
      else if(f.fname_ == "cosh") code_.push_back(codeEntry(op_cosh));
      else if(f.fname_ == "exp") code_.push_back(codeEntry(op_exp));
      else if(f.fname_ == "fabs") code_.push_back(codeEntry(op_fabs));
      else if(f.fname_ == "floor") code_.push_back(codeEntry(op_floor));
      else if(f.fname_ == "log") code_.push_back(codeEntry(op_log));
      else if(f.fname_ == "log10") code_.push_back(codeEntry(op_log10));
      else if(f.fname_ == "sin") code_.push_back(codeEntry(op_sin));
      else if(f.fname_ == "sinh") code_.push_back(codeEntry(op_sinh));
      else if(f.fname_ == "sqrt") code_.push_back(codeEntry(op_sqrt));
      else if(f.fname_ == "tan") code_.push_back(codeEntry(op_tan));
      else if(f.fname_ == "tanh") code_.push_back(codeEntry(op_tanh));
      else BOOST_ASSERT(0);
   }

   void operator()(const binary_function_& f) const {
      boost::apply_visitor(*this, f.operand1_);
      boost::apply_visitor(*this, f.operand2_);

      if(f.fname_ == "min") code_.push_back(codeEntry(op_min));
      else if(f.fname_ == "max") code_.push_back(codeEntry(op_max));
      else if(f.fname_ == "pow") code_.push_back(codeEntry(op_pow));
      else if(f.fname_ == "hypot") code_.push_back(codeEntry(op_hypot));
      else BOOST_ASSERT(0);
   }

   void operator()(const signed_& x) const {
      boost::apply_visitor(*this, x.operand_);
      if(x.sign == '-') code_.push_back(codeEntry(op_neg));
      else if(x.sign == '+'){ /* nothing */ }
      else BOOST_ASSERT(0);
   }

   void operator()(const ast_expression& x) const {
      boost::apply_visitor(*this, x.first);
      BOOST_FOREACH(const operation& oper, x.rest) {
         (*this)(oper);
      }
   }

private:
   /***************************************************************************/
   /**
    * Replaces place holders with corresponding parameter values
    *
    * @param vm A std::map of name-value pairs, holding place-holders to be replaced with values
    */
   std::string replacePlaceHolders(const std::map<std::string,fp_type>& vm) const {
      using namespace boost::xpressive;

      std::string formula = raw_formula_;
      std::string key, value;
      sregex re;

      typename std::map<std::string,fp_type>::const_iterator cit;
      for(cit=vm.begin(); cit!=vm.end(); ++cit) {
         key = cit->first;
         value = boost::lexical_cast<std::string>(cit->second);
         re = as_xpr("{{" + key + "}}");

         formula = regex_replace(formula, re, value);
      }

      return formula;
   }

   /***************************************************************************/
   /**
    * Compiles the AST into byte code
    */
   void compile(const ast_expression& x) const {
      (*this)(x);
   }

   /***************************************************************************/
   /**
    * The actual calculations
    */
   void execute() const {
      // Position pointers for stack and code
      typename std::vector<codeEntry>::const_iterator code_ptr = code_.begin();
      stack_ptr_ = stack_.begin();

      // When requested by the user, print a copy of the code-vector
      if(printCode_) printCode();

      while (code_ptr != code_.end()) {
         switch (boost::get<byte_code>(*code_ptr++)) { // Read out code_ptr, then switch it to the next position
            case op_trap:
            BOOST_ASSERT(0);
            break;

            case op_neg:
            stack_ptr_[-1] = -stack_ptr_[-1];
            break;

            case op_add:
            --stack_ptr_;
            stack_ptr_[-1] += stack_ptr_[0];
            break;

            case op_sub:
            --stack_ptr_;
            stack_ptr_[-1] -= stack_ptr_[0];
            break;

            case op_mul:
            --stack_ptr_;
            stack_ptr_[-1] *= stack_ptr_[0];
            break;

            case op_div:
            --stack_ptr_;
            stack_ptr_[-1] /= stack_ptr_[0];
            break;

            case op_min:
            --stack_ptr_;
            stack_ptr_[-1] = std::min(stack_ptr_[-1], stack_ptr_[0]);
            break;

            case op_max:
            --stack_ptr_;
            stack_ptr_[-1] = std::max(stack_ptr_[-1], stack_ptr_[0]);
            break;

            case op_pow:
            --stack_ptr_;
            stack_ptr_[-1] = std::pow(stack_ptr_[-1], stack_ptr_[0]);
            break;

            case op_hypot:
            --stack_ptr_;
            stack_ptr_[-1] = hypot(stack_ptr_[-1], stack_ptr_[0]);
            break;

            case op_acos:
            stack_ptr_[-1] = std::acos(stack_ptr_[-1]);
            break;

            case op_asin:
            stack_ptr_[-1] = std::asin(stack_ptr_[-1]);
            break;

            case op_atan:
            stack_ptr_[-1] = std::atan(stack_ptr_[-1]);
            break;

            case op_ceil:
            stack_ptr_[-1] = std::ceil(stack_ptr_[-1]);
            break;

            case op_cos:
            stack_ptr_[-1] = std::cos(stack_ptr_[-1]);
            break;

            case op_cosh:
            stack_ptr_[-1] = std::cosh(stack_ptr_[-1]);
            break;

            case op_exp:
            stack_ptr_[-1] = std::exp(stack_ptr_[-1]);
            break;

            case op_fabs:
            stack_ptr_[-1] = std::fabs(stack_ptr_[-1]);
            break;

            case op_floor:
            stack_ptr_[-1] = std::floor(stack_ptr_[-1]);
            break;

            case op_log:
            stack_ptr_[-1] = std::log(stack_ptr_[-1]);
            break;

            case op_log10:
            stack_ptr_[-1] = std::log10(stack_ptr_[-1]);
            break;

            case op_sin:
            stack_ptr_[-1] = std::sin(stack_ptr_[-1]);
            break;

            case op_sinh:
            stack_ptr_[-1] = std::sinh(stack_ptr_[-1]);
            break;

            case op_sqrt:
            stack_ptr_[-1] = std::sqrt(stack_ptr_[-1]);
            break;

            case op_tan:
            stack_ptr_[-1] = std::tan(stack_ptr_[-1]);
            break;

            case op_tanh:
            stack_ptr_[-1] = std::tanh(stack_ptr_[-1]);
            break;

            case op_fp:
            *stack_ptr_++=boost::get<fp_type>(*code_ptr++);
            break;

            default:
            BOOST_ASSERT(0);
            break;
         }
      }
   }

   /***************************************************************************/
   /**
    * Prints the stack until it encounters a 0 entry or the end of the list
    */
   void printStack() const {
      if(stack_.empty()) {
         std::cout << "Stack is empty!" << std::endl;
         return;
      }

      typename std::vector<fp_type>::const_iterator it = stack_.begin();
      std::cout << "Stack: ";
      while(it!=stack_.end() && *it != 0.) {
         std::cout << *it << " " << std::flush;
         ++it;
      }
      std::cout << std::endl;
   }

   /***************************************************************************/
   /**
    * Prints the code
    */
   void printCode() const {
      if(code_.empty()) {
         std::cout << "Code is empty!" << std::endl;
         return;
      }

      typename std::vector<codeEntry>::const_iterator it = code_.begin();
      std::cout << "Code: ";
      for(it=code_.begin(); it!=code_.end(); ++it) {
         std::cout << *it << " " << std::flush;
      }
      std::cout << std::endl;
   }

   /*****************************************************************************/
   /**
    * The error handler
    */
   struct error_handler_{
      template <typename, typename, typename>
      struct result { typedef void type; };

      void operator()(
            boost::spirit::qi::info const& what
            , std::string::const_iterator err_pos
            , std::string::const_iterator last
      ) const {
         std::cout
         << "Error! Expecting "
         << what                         // what failed?
         << " here: \""
         << std::string(err_pos, last)   // iterators to error-pos, end
         << "\""
         << std::endl
         ;
      }
   };

   /***************************************************************************/
   // Local data and empty functions

   /** @brief The default constructor -- intentionally private and undefined */
   GFormulaParserT();

   std::string raw_formula_; ///< Holds the formula with place holders

   boost::spirit::qi::rule<std::string::const_iterator, ast_expression(), boost::spirit::ascii::space_type> expression_rule_;
   boost::spirit::qi::rule<std::string::const_iterator, ast_expression(), boost::spirit::ascii::space_type> term_rule_;
   boost::spirit::qi::rule<std::string::const_iterator, unary_function_(), boost::spirit::ascii::space_type> unary_function_rule_;
   boost::spirit::qi::rule<std::string::const_iterator, binary_function_(), boost::spirit::ascii::space_type> binary_function_rule_;
   boost::spirit::qi::rule<std::string::const_iterator, operand(), boost::spirit::ascii::space_type> factor_rule_;

   boost::spirit::qi::real_parser<fp_type,boost::spirit::qi::real_policies<fp_type> > real;

   boost::spirit::qi::symbols<std::iterator_traits<std::string::const_iterator>::value_type, fp_type> constants_; ///< Holds mathematical- and user-defined constants

   mutable std::vector<fp_type> stack_; ///< Holds the data needed as input for each operation
   mutable std::vector<codeEntry> code_; ///< Holds the "compiled" code

   mutable typename std::vector<fp_type>::iterator stack_ptr_;

   boost::phoenix::function<error_handler_> const error_handler;

   bool printCode_; ///< When set, the code will be printed prior to the evaluation
};

/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */

#endif /* GFORMULAPARSERT_HPP_ */
