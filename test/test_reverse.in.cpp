//*****************************************************************************
// Copyright 2018-2020 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//*****************************************************************************

#include "he_op_annotations.hpp"
#include "ngraph/ngraph.hpp"
#include "seal/he_seal_backend.hpp"
#include "test_util.hpp"
#include "util/all_close.hpp"
#include "util/ndarray.hpp"
#include "util/test_control.hpp"
#include "util/test_tools.hpp"

static std::string s_manifest = "${MANIFEST}";

namespace ngraph::runtime::he {

auto reverse_test = [](const Shape& shape_a, const AxisSet& axis_set,
                       const std::vector<float>& input,
                       const std::vector<float>& output,
                       const bool arg1_encrypted, const bool complex_packing,
                       const bool packed) {
  auto backend = runtime::Backend::create("${BACKEND_NAME}");
  auto he_backend = static_cast<HESealBackend*>(backend.get());

  if (complex_packing) {
    he_backend->update_encryption_parameters(
        HESealEncryptionParameters::default_complex_packing_parms());
  }

  auto a = std::make_shared<op::Parameter>(element::f32, shape_a);
  auto t = std::make_shared<op::Reverse>(a, axis_set);
  auto f = std::make_shared<Function>(t, ParameterVector{a});

  const auto& arg1_config =
      test::config_from_flags(false, arg1_encrypted, packed);

  std::string error_str;
  he_backend->set_config({{a->get_name(), arg1_config}}, error_str);

  auto t_a =
      test::tensor_from_flags(*he_backend, shape_a, arg1_encrypted, packed);
  auto t_result = test::tensor_from_flags(*he_backend, t->get_shape(),
                                          arg1_encrypted, packed);

  copy_data(t_a, input);

  auto handle = backend->compile(f);
  handle->call_with_validate({t_result}, {t_a});
  EXPECT_TRUE(test::all_close(read_vector<float>(t_result), output, 1e-3f));
};

NGRAPH_TEST(${BACKEND_NAME}, reverse_0d) {
  for (bool arg1_encrypted : std::vector<bool>{false, true}) {
    for (bool complex_packing : std::vector<bool>{false, true}) {
      for (bool packing : std::vector<bool>{false}) {
        reverse_test(Shape{}, AxisSet{}, std::vector<float>{6},
                     std::vector<float>{6}, arg1_encrypted, complex_packing,
                     packing);
      }
    }
  }
}

NGRAPH_TEST(${BACKEND_NAME}, reverse_1d_nochange) {
  for (bool arg1_encrypted : std::vector<bool>{false, true}) {
    for (bool complex_packing : std::vector<bool>{false, true}) {
      for (bool packing : std::vector<bool>{false}) {
        reverse_test(Shape{8}, AxisSet{},
                     std::vector<float>{0, 1, 2, 3, 4, 5, 6, 7},
                     std::vector<float>{0, 1, 2, 3, 4, 5, 6, 7}, arg1_encrypted,
                     complex_packing, packing);
      }
    }
  }
}

NGRAPH_TEST(${BACKEND_NAME}, reverse_1d_0) {
  for (bool arg1_encrypted : std::vector<bool>{false, true}) {
    for (bool complex_packing : std::vector<bool>{false, true}) {
      for (bool packing : std::vector<bool>{false}) {
        reverse_test(Shape{8}, AxisSet{0},
                     std::vector<float>{0, 1, 2, 3, 4, 5, 6, 7},
                     std::vector<float>{7, 6, 5, 4, 3, 2, 1, 0}, arg1_encrypted,
                     complex_packing, packing);
      }
    }
  }
}

NGRAPH_TEST(${BACKEND_NAME}, reverse_2d_nochange) {
  for (bool arg1_encrypted : std::vector<bool>{false, true}) {
    for (bool complex_packing : std::vector<bool>{false, true}) {
      for (bool packing : std::vector<bool>{false}) {
        reverse_test(Shape{4, 3}, AxisSet{},
                     ngraph::test::NDArray<float, 2>(
                         {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}})
                         .get_vector(),
                     ngraph::test::NDArray<float, 2>(
                         {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}})
                         .get_vector(),
                     arg1_encrypted, complex_packing, packing);
      }
    }
  }
}

NGRAPH_TEST(${BACKEND_NAME}, reverse_2d_0) {
  for (bool arg1_encrypted : std::vector<bool>{false, true}) {
    for (bool complex_packing : std::vector<bool>{false, true}) {
      for (bool packing : std::vector<bool>{false}) {
        reverse_test(Shape{4, 3}, AxisSet{0},
                     ngraph::test::NDArray<float, 2>(
                         {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}})
                         .get_vector(),
                     ngraph::test::NDArray<float, 2>(
                         {{9, 10, 11}, {6, 7, 8}, {3, 4, 5}, {0, 1, 2}})
                         .get_vector(),
                     arg1_encrypted, complex_packing, packing);
      }
    }
  }
}

NGRAPH_TEST(${BACKEND_NAME}, reverse_2d_1) {
  for (bool arg1_encrypted : std::vector<bool>{false, true}) {
    for (bool complex_packing : std::vector<bool>{false, true}) {
      for (bool packing : std::vector<bool>{false}) {
        reverse_test(Shape{4, 3}, AxisSet{1},
                     ngraph::test::NDArray<float, 2>(
                         {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}})
                         .get_vector(),
                     ngraph::test::NDArray<float, 2>(
                         {{2, 1, 0}, {5, 4, 3}, {8, 7, 6}, {11, 10, 9}})
                         .get_vector(),
                     arg1_encrypted, complex_packing, packing);
      }
    }
  }
}

NGRAPH_TEST(${BACKEND_NAME}, reverse_2d_01) {
  for (bool arg1_encrypted : std::vector<bool>{false, true}) {
    for (bool complex_packing : std::vector<bool>{false, true}) {
      for (bool packing : std::vector<bool>{false}) {
        reverse_test(Shape{4, 3}, AxisSet{0, 1},
                     ngraph::test::NDArray<float, 2>(
                         {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}})
                         .get_vector(),
                     ngraph::test::NDArray<float, 2>(
                         {{11, 10, 9}, {8, 7, 6}, {5, 4, 3}, {2, 1, 0}})
                         .get_vector(),
                     arg1_encrypted, complex_packing, packing);
      }
    }
  }
}

NGRAPH_TEST(${BACKEND_NAME}, reverse_3d_nochange) {
  for (bool arg1_encrypted : std::vector<bool>{false, true}) {
    for (bool complex_packing : std::vector<bool>{false, true}) {
      for (bool packing : std::vector<bool>{false}) {
        reverse_test(
            Shape{2, 4, 3}, AxisSet{},
            ngraph::test::NDArray<float, 3>(
                {{{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}},
                 {{12, 13, 14}, {15, 16, 17}, {18, 19, 20}, {21, 22, 23}}})
                .get_vector(),
            ngraph::test::NDArray<float, 3>(
                {{{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}},
                 {{12, 13, 14}, {15, 16, 17}, {18, 19, 20}, {21, 22, 23}}})
                .get_vector(),
            arg1_encrypted, complex_packing, packing);
      }
    }
  }
}

NGRAPH_TEST(${BACKEND_NAME}, reverse_3d_0) {
  for (bool arg1_encrypted : std::vector<bool>{false, true}) {
    for (bool complex_packing : std::vector<bool>{false, true}) {
      for (bool packing : std::vector<bool>{false}) {
        reverse_test(
            Shape{2, 4, 3}, AxisSet{0},
            ngraph::test::NDArray<float, 3>(
                {{{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}},
                 {{12, 13, 14}, {15, 16, 17}, {18, 19, 20}, {21, 22, 23}}})
                .get_vector(),
            ngraph::test::NDArray<float, 3>(
                {{{12, 13, 14}, {15, 16, 17}, {18, 19, 20}, {21, 22, 23}},
                 {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}}})
                .get_vector(),
            arg1_encrypted, complex_packing, packing);
      }
    }
  }
}

NGRAPH_TEST(${BACKEND_NAME}, reverse_3d_1) {
  for (bool arg1_encrypted : std::vector<bool>{false, true}) {
    for (bool complex_packing : std::vector<bool>{false, true}) {
      for (bool packing : std::vector<bool>{false}) {
        reverse_test(
            Shape{2, 4, 3}, AxisSet{1},
            ngraph::test::NDArray<float, 3>(
                {{{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}},
                 {{12, 13, 14}, {15, 16, 17}, {18, 19, 20}, {21, 22, 23}}})
                .get_vector(),
            ngraph::test::NDArray<float, 3>(
                {{{9, 10, 11}, {6, 7, 8}, {3, 4, 5}, {0, 1, 2}},
                 {{21, 22, 23}, {18, 19, 20}, {15, 16, 17}, {12, 13, 14}}})
                .get_vector(),
            arg1_encrypted, complex_packing, packing);
      }
    }
  }
}

NGRAPH_TEST(${BACKEND_NAME}, reverse_3d_2) {
  for (bool arg1_encrypted : std::vector<bool>{false, true}) {
    for (bool complex_packing : std::vector<bool>{false, true}) {
      for (bool packing : std::vector<bool>{false}) {
        reverse_test(
            Shape{2, 4, 3}, AxisSet{2},
            ngraph::test::NDArray<float, 3>(
                {{{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}},
                 {{12, 13, 14}, {15, 16, 17}, {18, 19, 20}, {21, 22, 23}}})
                .get_vector(),
            ngraph::test::NDArray<float, 3>(
                {{{2, 1, 0}, {5, 4, 3}, {8, 7, 6}, {11, 10, 9}},
                 {{14, 13, 12}, {17, 16, 15}, {20, 19, 18}, {23, 22, 21}}})
                .get_vector(),
            arg1_encrypted, complex_packing, packing);
      }
    }
  }
}

NGRAPH_TEST(${BACKEND_NAME}, reverse_3d_01) {
  for (bool arg1_encrypted : std::vector<bool>{false, true}) {
    for (bool complex_packing : std::vector<bool>{false, true}) {
      for (bool packing : std::vector<bool>{false}) {
        reverse_test(
            Shape{2, 4, 3}, AxisSet{0, 1},
            ngraph::test::NDArray<float, 3>(
                {{{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}},
                 {{12, 13, 14}, {15, 16, 17}, {18, 19, 20}, {21, 22, 23}}})
                .get_vector(),
            ngraph::test::NDArray<float, 3>(
                {{{21, 22, 23}, {18, 19, 20}, {15, 16, 17}, {12, 13, 14}},
                 {{9, 10, 11}, {6, 7, 8}, {3, 4, 5}, {0, 1, 2}}})
                .get_vector(),
            arg1_encrypted, complex_packing, packing);
      }
    }
  }
}

NGRAPH_TEST(${BACKEND_NAME}, reverse_3d_02) {
  for (bool arg1_encrypted : std::vector<bool>{false, true}) {
    for (bool complex_packing : std::vector<bool>{false, true}) {
      for (bool packing : std::vector<bool>{false}) {
        reverse_test(
            Shape{2, 4, 3}, AxisSet{0, 2},
            ngraph::test::NDArray<float, 3>(
                {{{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}},
                 {{12, 13, 14}, {15, 16, 17}, {18, 19, 20}, {21, 22, 23}}})
                .get_vector(),
            ngraph::test::NDArray<float, 3>(
                {{{14, 13, 12}, {17, 16, 15}, {20, 19, 18}, {23, 22, 21}},
                 {{2, 1, 0}, {5, 4, 3}, {8, 7, 6}, {11, 10, 9}}})
                .get_vector(),
            arg1_encrypted, complex_packing, packing);
      }
    }
  }
}

NGRAPH_TEST(${BACKEND_NAME}, reverse_3d_12) {
  for (bool arg1_encrypted : std::vector<bool>{false, true}) {
    for (bool complex_packing : std::vector<bool>{false, true}) {
      for (bool packing : std::vector<bool>{false}) {
        reverse_test(
            Shape{2, 4, 3}, AxisSet{1, 2},
            ngraph::test::NDArray<float, 3>(
                {{{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}},
                 {{12, 13, 14}, {15, 16, 17}, {18, 19, 20}, {21, 22, 23}}})
                .get_vector(),
            ngraph::test::NDArray<float, 3>(
                {{{11, 10, 9}, {8, 7, 6}, {5, 4, 3}, {2, 1, 0}},
                 {{23, 22, 21}, {20, 19, 18}, {17, 16, 15}, {14, 13, 12}}})
                .get_vector(),
            arg1_encrypted, complex_packing, packing);
      }
    }
  }
}

NGRAPH_TEST(${BACKEND_NAME}, reverse_3d_012) {
  for (bool arg1_encrypted : std::vector<bool>{false, true}) {
    for (bool complex_packing : std::vector<bool>{false, true}) {
      for (bool packing : std::vector<bool>{false}) {
        reverse_test(
            Shape{2, 4, 3}, AxisSet{0, 1, 2},
            ngraph::test::NDArray<float, 3>(
                {{{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}},
                 {{12, 13, 14}, {15, 16, 17}, {18, 19, 20}, {21, 22, 23}}})
                .get_vector(),
            ngraph::test::NDArray<float, 3>(
                {{{23, 22, 21}, {20, 19, 18}, {17, 16, 15}, {14, 13, 12}},
                 {{11, 10, 9}, {8, 7, 6}, {5, 4, 3}, {2, 1, 0}}})
                .get_vector(),
            arg1_encrypted, complex_packing, packing);
      }
    }
  }
}

}  // namespace ngraph::runtime::he
