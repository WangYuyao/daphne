/*
 * Copyright 2021 The DAPHNE Consortium
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
 */

#ifdef USE_CUDA


#endif // USE_CUDA

#include "run_tests.h"

#include <api/cli/DaphneUserConfig.h>
#include <runtime/local/datagen/GenGivenVals.h>
#include <runtime/local/datastructures/DenseMatrix.h>
#include "runtime/local/kernels/BatchNorm2DTrainForward.h"

#include <cassert>
#include <catch.hpp>
#include <tags.h>

template<class DT>
void check(const DT* in, const DT* gamma, const DT* beta, const DT* ema_mean, const DT* ema_var, const DT* exp,
        DaphneContext* dctx)
{
    DT* res = nullptr;
    DT* new_emaMean = nullptr;
    DT* new_emaVar = nullptr;
    DT* Mean = nullptr;
    DT* invVar = nullptr;
    typename DT::VT epsilon = 1e-5;
    typename DT::VT mu = 1;
    Batch2DTrainForward<DT, DT>::apply(res, new_emaMean, new_emaVar, Mean, invVar, in, gamma, beta, ema_mean, ema_var, epsilon, mu, dctx);
    CHECK(Approx(*(res->getValues())).epsilon(epsilon) == *(exp->getValues()));
}

TEMPLATE_PRODUCT_TEST_CASE("batch_norm_train_fwd", TAG_DNN, (DenseMatrix), (float, double)) { // NOLINT(cert-err58-cpp)
    auto dctx = setupContextAndLogger();
    using DT = TestType;

    auto input = genGivenVals<DT>(2, { -3, -2, -1, 0, 1, 2, 3, 4, 5,
                                    -3, -2, -1, 0, 1, 2, 3, 4, 5,
                                    -3, -2, -1, 0, 1, 2, 3, 4, 5,
                                    -3, -2, -1, 0, 1, 2, 3, 4, 5,
                                    -3, -2, -1, 0, 1, 2, 3, 4, 5,
                                    -3, -2, -1, 0, 1, 2, 3, 4, 5});
    auto gamma = genGivenVals<DT>(3, { 1, 1, 1 });
    auto beta = genGivenVals<DT>(3, { 0, 0, 0 });
    auto ema_mean = genGivenVals<DT>(3, { 0, 0, 0 });
    auto ema_var = genGivenVals<DT>(1, { 1, 1, 1 });

    auto result = genGivenVals<DT>(2, { -1.5492, -1.1619, -0.7746,
                                    -0.3873,  0.0000,  0.3873,
                                    0.7746,  1.1619,  1.5492,
                                    -1.5492, -1.1619, -0.7746,
                                    -0.3873,  0.0000,  0.3873,
                                    0.7746,  1.1619,  1.5492,
                                    -1.5492, -1.1619, -0.7746,
                                    -0.3873,  0.0000,  0.3873,
                                    0.7746,  1.1619,  1.5492,
                                    -1.5492, -1.1619, -0.7746,
                                    -0.3873,  0.0000,  0.3873,
                                    0.7746,  1.1619,  1.5492,
                                    -1.5492, -1.1619, -0.7746,
                                    -0.3873,  0.0000,  0.3873,
                                    0.7746,  1.1619,  1.5492,
                                    -1.5492, -1.1619, -0.7746,
                                    -0.3873,  0.0000,  0.3873,
                                    0.7746,  1.1619,  1.5492});

    check(input, gamma, beta, ema_mean, ema_var, result, dctx.get());

    DataObjectFactory::destroy(input);
    DataObjectFactory::destroy(result);
}