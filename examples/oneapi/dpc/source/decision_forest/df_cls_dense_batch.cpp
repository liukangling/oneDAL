/*******************************************************************************
* Copyright 2020 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#define ONEAPI_DAL_DATA_PARALLEL
#include "oneapi/dal/algo/decision_forest.hpp"

#include "example_util/utils.hpp"
#include "oneapi/dal/exceptions.hpp"

using namespace oneapi;
namespace df = oneapi::dal::decision_forest;

void run(sycl::queue &queue) {
  constexpr std::int64_t row_count_train = 6;
  constexpr std::int64_t row_count_test = 3;
  constexpr std::int64_t column_count = 2;

  constexpr std::int64_t class_count = 2;

  const float x_train_host[] = {
      -2.f, -1.f, -1.f, -1.f, -1.f, -2.f, +1.f, +1.f, +1.f, +2.f, +2.f, +1.f,
  };
  const float y_train_host[] = {
      0.f, 0.f, 0.f, 1.f, 1.f, 1.f,
  };

  const float x_test_host[] = {
      -1.f, -1.f, 2.f, 2.f, 3.f, 2.f,
  };
  const float y_test_host[] = {
      0.f,
      1.f,
      1.f,
  };

  auto x_train = sycl::malloc_shared<float>(row_count_train * column_count, queue);
  queue.memcpy(x_train, x_train_host, sizeof(float) * row_count_train * column_count).wait();
  const auto x_train_table = dal::homogen_table{queue, x_train, row_count_train, column_count, dal::empty_delete<const float>()};

  auto y_train = sycl::malloc_shared<float>(row_count_train, queue);
  queue.memcpy(y_train, y_train_host, sizeof(float) * row_count_train).wait();
  const auto y_train_table = dal::homogen_table{queue, y_train, row_count_train, 1, dal::empty_delete<const float>()};

  const auto x_test_table = dal::homogen_table{x_test_host, row_count_test, column_count, dal::empty_delete<const float>()};
  const auto y_test_table = dal::homogen_table{y_test_host, row_count_test, 1, dal::empty_delete<const float>()};

  const auto df_train_desc = df::descriptor<float, df::task::classification, df::method::hist>{}
          .set_class_count(class_count)
          .set_tree_count(10)
          .set_features_per_node(1)
          .set_min_observations_in_leaf_node(1)
          .set_error_metric_mode(df::error_metric_mode::out_of_bag_error)
          .set_variable_importance_mode(df::variable_importance_mode::mdi);

  const auto df_infer_desc = df::descriptor<float, df::task::classification, df::method::dense>{}
          .set_class_count(class_count)
          .set_infer_mode(df::infer_mode::class_labels | df::infer_mode::class_probabilities)
          .set_voting_mode(df::voting_mode::weighted);

  try {
    const auto result_train = dal::train(queue, df_train_desc, x_train_table, y_train_table);

    std::cout << "Variable importance results:" << std::endl
              << result_train.get_var_importance() << std::endl;

    std::cout << "OOB error: " << result_train.get_oob_err() << std::endl;

    const auto result_infer = dal::infer(df_infer_desc, result_train.get_model(), x_test_table);

    std::cout << "Prediction results:" << std::endl
              << result_infer.get_labels() << std::endl;
    std::cout << "Probabilities results:" << std::endl
              << result_infer.get_probabilities() << std::endl;

    std::cout << "Ground truth:" << std::endl << y_test_table << std::endl;
  } catch (oneapi::dal::unimplemented_error &e) {
    std::cout << "  " << e.what() << std::endl;
    return;
  }
}

int main(int argc, char const *argv[]) {
  for (auto device : list_devices()) {
    std::cout << "Running on " << device.get_info<sycl::info::device::name>()
              << std::endl << std::endl;
    auto queue = sycl::queue{device};
    run(queue);
  }
  return 0;
}