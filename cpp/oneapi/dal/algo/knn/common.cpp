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

#include "oneapi/dal/algo/knn/common.hpp"
#include "oneapi/dal/algo/knn/backend/model_impl.hpp"
#include "oneapi/dal/exceptions.hpp"

namespace oneapi::dal::knn {
namespace detail {
namespace v1 {

template <typename Task>
class descriptor_impl : public base {
public:
    std::int64_t class_count = 2;
    std::int64_t neighbor_count = 1;
};

template <typename Task>
descriptor_base<Task>::descriptor_base() : impl_(new descriptor_impl<Task>{}) {}

template <typename Task>
std::int64_t descriptor_base<Task>::get_class_count() const {
    return impl_->class_count;
}

template <typename Task>
void descriptor_base<Task>::set_class_count_impl(std::int64_t value) {
    if (value < 2) {
        throw domain_error(dal::detail::error_messages::class_count_leq_one());
    }
    impl_->class_count = value;
}

template <typename Task>
std::int64_t descriptor_base<Task>::get_neighbor_count() const {
    return impl_->neighbor_count;
}

template <typename Task>
void descriptor_base<Task>::set_neighbor_count_impl(std::int64_t value) {
    if (value < 1) {
        throw domain_error(dal::detail::error_messages::neighbor_count_lt_one());
    }
    impl_->neighbor_count = value;
}

template class ONEDAL_EXPORT descriptor_base<task::classification>;

} // namespace v1
} // namespace detail

namespace v1 {

using detail::v1::model_impl;

template <typename Task>
model<Task>::model() : impl_(new model_impl<Task>{}) {}

template <typename Task>
model<Task>::model(const std::shared_ptr<detail::model_impl<Task>>& impl) : impl_(impl) {}

template class ONEDAL_EXPORT model<task::classification>;

} // namespace v1
} // namespace oneapi::dal::knn
