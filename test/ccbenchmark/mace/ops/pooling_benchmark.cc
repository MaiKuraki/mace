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

#include "mace/benchmark_utils/test_benchmark.h"
#include "mace/ops/common/conv_pool_2d_util.h"
#include "mace/ops/common/pooling_type.h"
#include "mace/ops/ops_test_util.h"

namespace mace {
namespace ops {
namespace test {

namespace {
template <RuntimeType D, typename T>
void Pooling(int iters,
             int batch,
             int channels,
             int height,
             int width,
             int kernel,
             int stride,
             Padding padding,
             PoolingType pooling_type) {
  mace::testing::StopTiming();

  OpsTestNet net;

  // Add input data
  if (D == RuntimeType::RT_CPU) {
    if (DataTypeToEnum<T>::value != DT_UINT8) {
      net.AddRandomInput<D, float>(
          "Input", {batch, channels, height, width});
    } else {
      net.AddRandomInput<RuntimeType::RT_CPU, uint8_t>(
          "Input", {batch, height, width, channels});
    }
  } else if (D == RuntimeType::RT_OPENCL) {
    net.AddRandomInput<D, float>("Input",
                                 {batch, height, width, channels});
  } else {
    MACE_NOT_IMPLEMENTED;
  }

  OpDefBuilder("Pooling", "PoolingTest")
      .Input("Input")
      .Output("Output")
      .AddIntArg("pooling_type", pooling_type)
      .AddIntsArg("kernels", {kernel, kernel})
      .AddIntsArg("strides", {stride, stride})
      .AddIntArg("padding", padding)
      .AddIntsArg("dilations", {1, 1})
      .AddIntArg("T", static_cast<int>(DataTypeToEnum<T>::value))
      .Finalize(net.NewOperatorDef());

  // Warm-up
  net.Setup(D);
  for (int i = 0; i < 5; ++i) {
    net.Run();
  }
  net.Sync();

  mace::testing::StartTiming();
  while (iters--) {
    net.Run();
  }
  net.Sync();
}
}  // namespace

#define MACE_BM_POOLING_MACRO(N, C, H, W, KE, STRIDE, PA, PO, TYPE, DEVICE)    \
  static void                                                                  \
      MACE_BM_POOLING_##N##_##C##_##H##_##W##_K##KE##S##STRIDE##_##PA##_##PO##_\
        ##TYPE##_##DEVICE(                                                     \
          int iters) {                                                         \
    const int64_t tot = static_cast<int64_t>(iters) * N * C * H * W;           \
    mace::testing::BytesProcessed(tot *(sizeof(TYPE)));                        \
    Pooling<DEVICE, TYPE>(iters, N, C, H, W, KE, STRIDE, Padding::PA,          \
                    PoolingType::PO);                                          \
  }                                                                            \
  MACE_BENCHMARK(                                                              \
      MACE_BM_POOLING_##N##_##C##_##H##_##W##_K##KE##S##STRIDE##_##PA##_##PO##_\
        ##TYPE##_##DEVICE)

#if defined(MACE_ENABLE_OPENCL) && defined(MACE_ENABLE_QUANTIZE)
#define MACE_BM_POOLING(N, C, H, W, K, S, PA, PO)       \
  MACE_BM_POOLING_MACRO(N, C, H, W, K, S, PA, PO, float, RT_CPU); \
  MACE_BM_POOLING_MACRO(N, C, H, W, K, S, PA, PO, float, RT_OPENCL); \
  MACE_BM_POOLING_MACRO(N, C, H, W, K, S, PA, PO, half, RT_OPENCL); \
  MACE_BM_POOLING_MACRO(N, C, H, W, K, S, PA, PO, uint8_t, RT_CPU)
#elif defined(MACE_ENABLE_OPENCL)
#define MACE_BM_POOLING(N, C, H, W, K, S, PA, PO)       \
  MACE_BM_POOLING_MACRO(N, C, H, W, K, S, PA, PO, float, RT_CPU); \
  MACE_BM_POOLING_MACRO(N, C, H, W, K, S, PA, PO, float, RT_OPENCL); \
  MACE_BM_POOLING_MACRO(N, C, H, W, K, S, PA, PO, half, RT_OPENCL)
#elif defined(MACE_ENABLE_QUANTIZE)
#define MACE_BM_POOLING(N, C, H, W, K, S, PA, PO)       \
  MACE_BM_POOLING_MACRO(N, C, H, W, K, S, PA, PO, float, RT_CPU); \
  MACE_BM_POOLING_MACRO(N, C, H, W, K, S, PA, PO, uint8_t, RT_CPU)
#else
#define MACE_BM_POOLING(N, C, H, W, K, S, PA, PO)       \
  MACE_BM_POOLING_MACRO(N, C, H, W, K, S, PA, PO, float, RT_CPU)
#endif


MACE_BM_POOLING(1, 3, 129, 129, 2, 2, SAME, MAX);
MACE_BM_POOLING(1, 3, 257, 257, 2, 2, SAME, MAX);
MACE_BM_POOLING(1, 3, 513, 513, 2, 2, SAME, MAX);
MACE_BM_POOLING(1, 3, 1025, 1025, 2, 2, SAME, MAX);
MACE_BM_POOLING(1, 32, 480, 640, 480, 640, VALID, AVG);
MACE_BM_POOLING(1, 1024, 7, 7, 7, 1, VALID, AVG);

}  // namespace test
}  // namespace ops
}  // namespace mace
