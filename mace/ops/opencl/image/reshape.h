// Copyright 2018 The MACE Authors. All Rights Reserved.
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

#ifndef MACE_OPS_OPENCL_IMAGE_RESHAPE_H_
#define MACE_OPS_OPENCL_IMAGE_RESHAPE_H_

#include "mace/ops/opencl/reshape.h"

#include <vector>
#include <memory>

#include "mace/core/ops/operator.h"
#include "mace/runtimes/opencl/core/opencl_helper.h"
#include "mace/runtimes/opencl/transform/buffer_transform_kernel.h"
#include "mace/ops/opencl/transpose.h"

namespace mace {
namespace ops {
namespace opencl {
namespace image {

class ReshapeKernel : public OpenCLReshapeKernel {
 public:
  ReshapeKernel(OpConstructContext *context,
                FrameworkType framework = TENSORFLOW,
                int has_data_format = 0);

  MaceStatus Compute(OpContext *context,
                     const Tensor *input,
                     const std::vector<index_t> &new_shape,
                     Tensor *output) override;

 private:
  std::unique_ptr<OpenCLBufferTransformKernel> i2bkernel_;
  std::unique_ptr<Tensor> nchw_inter_buffer_;
  std::unique_ptr<OpenCLBufferTransformKernel> b2ikernel_;
  std::unique_ptr<Tensor> inter_buffer_;
  std::unique_ptr<ops::OpenCLTransposeKernel> nhwc2nchw_kernel_;
  std::unique_ptr<ops::OpenCLTransposeKernel> nchw2nhwc_kernel_;
  FrameworkType framework_;
  int has_data_format_;
};

}  // namespace image
}  // namespace opencl
}  // namespace ops
}  // namespace mace

#endif  // MACE_OPS_OPENCL_IMAGE_RESHAPE_H_
