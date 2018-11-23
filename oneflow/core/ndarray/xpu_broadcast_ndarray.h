#ifndef ONEFLOW_CORE_NDARRAY_XPU_BROADCAST_NDARRAY_H_
#define ONEFLOW_CORE_NDARRAY_XPU_BROADCAST_NDARRAY_H_

#include "oneflow/core/ndarray/xpu_var_ndarray.h"

namespace oneflow {

template<typename T, int NDIMS>
struct XpuBroadcastNdarrayUtil;

template<typename T>
class XpuBroadcastNdarray final {
 public:
  OF_DEVICE_FUNC XpuBroadcastNdarray(const ExecShape& shape, const XpuVarNdarray<T>& var)
      : shape_(shape), var_(var) {}
  OF_DEVICE_FUNC ~XpuBroadcastNdarray() = default;

  template<int NDIMS>
  OF_DEVICE_FUNC T Get(int64_t offset) const {
    int64_t coord[NDIMS];
    shape_.template Offset2Coordinate<NDIMS>(offset, coord);
    XpuBroadcastNdarrayUtil<T, NDIMS>::SrcCoordinate(var_.shape(), coord);
    return var_.template Get<NDIMS>(coord);
  }

  OF_DEVICE_FUNC const ExecShape& shape() const { return shape_; }
  OF_DEVICE_FUNC const XpuVarNdarray<T>& var() const { return var_; }

 private:
  const ExecShape& shape_;
  const XpuVarNdarray<T>& var_;
};

#define IMPLACE_SET_SRC_COORD(i) coord[i] %= src_shape.At(i);
#define SPECIALIZE_XPU_BROADCAST_NDARRAY_UTIL(n)                                                 \
  template<typename T>                                                                           \
  struct XpuBroadcastNdarrayUtil<T, n + 1> final {                                               \
    OF_DEVICE_FUNC static void SrcCoordinate(const ExecShape& src_shape, int64_t coord[n + 1]) { \
      OF_PP_FOR_EACH_TUPLE(IMPLACE_SET_SRC_COORD, GET_SEQ(n));                                   \
    }                                                                                            \
  }
SPECIALIZE_XPU_BROADCAST_NDARRAY_UTIL(0);
SPECIALIZE_XPU_BROADCAST_NDARRAY_UTIL(1);
SPECIALIZE_XPU_BROADCAST_NDARRAY_UTIL(2);
SPECIALIZE_XPU_BROADCAST_NDARRAY_UTIL(3);
SPECIALIZE_XPU_BROADCAST_NDARRAY_UTIL(4);
#undef SPECIALIZE_XPU_BROADCAST_NDARRAY_UTIL
#undef IMPLACE_SET_SRC_COORD

}  // namespace oneflow

#endif  // ONEFLOW_CORE_NDARRAY_XPU_BROADCAST_NDARRAY_H_
