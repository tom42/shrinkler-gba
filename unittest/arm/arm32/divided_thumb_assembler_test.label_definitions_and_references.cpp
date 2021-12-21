// MIT License
//
// lzasm: a runtime assembler
// Copyright 2021 Thomas Mathys
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <boost/test/unit_test.hpp>
#include "lzasm/arm/arm32/divided_thumb_assembler.hpp"
#include "assembler_test_utilities.hpp"
#include "test_utilities.hpp"

namespace lzasm_unittest
{

using namespace ::lzasm::arm::arm32;

BOOST_AUTO_TEST_SUITE(divided_thumb_assembler_test)

    BOOST_AUTO_TEST_SUITE(label_definitions_and_references)

        BOOST_AUTO_TEST_SUITE(TSymbolName_is_int)

            BOOST_AUTO_TEST_CASE(definition)
            {
                basic_divided_thumb_assembler<int> a;
                a.label(symbol(0));
            }

            BOOST_AUTO_TEST_CASE(reference)
            {
                basic_divided_thumb_assembler<int> a;

                a.swi(123);
                a.swi(symbol(123));
                a.label(symbol(123));

                CHECK_PROGRAM(a, 0, H(0xdf7b, 0xdf04));
            }

        BOOST_AUTO_TEST_SUITE_END()

        BOOST_AUTO_TEST_SUITE(TSymbolName_is_std_string)

            BOOST_AUTO_TEST_CASE(definition_with_explicitly_constructed_symbol)
            {
                divided_thumb_assembler a;
                a.label(symbol(std::string("name")));
            }

            BOOST_AUTO_TEST_CASE(definition_with_implicit_conversion_from_std_string)
            {
                divided_thumb_assembler a;
                a.label(std::string("name"));
            }

            BOOST_AUTO_TEST_CASE(reference)
            {
                divided_thumb_assembler a;

                a.swi(symbol(std::string("123")));
                a.swi(std::string("123"));
                a.label(std::string("123"));

                CHECK_PROGRAM(a, 0, H(0xdf04, 0xdf04));
            }

        BOOST_AUTO_TEST_SUITE_END()

    BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

}
