#ifndef KERNEL_RUNNER_OPENCL_UGLY_ERROR_HANDLING_HPP_
#define KERNEL_RUNNER_OPENCL_UGLY_ERROR_HANDLING_HPP_

#include <common_types.hpp>

#define CL_ERR_TO_STR(err) case err: return #err

char const *
clGetErrorString(cl_int const err)
{
  switch(err)
    {
      CL_ERR_TO_STR(CL_SUCCESS);
      CL_ERR_TO_STR(CL_DEVICE_NOT_FOUND);
      CL_ERR_TO_STR(CL_DEVICE_NOT_AVAILABLE);
      CL_ERR_TO_STR(CL_COMPILER_NOT_AVAILABLE);
      CL_ERR_TO_STR(CL_MEM_OBJECT_ALLOCATION_FAILURE);
      CL_ERR_TO_STR(CL_OUT_OF_RESOURCES);
      CL_ERR_TO_STR(CL_OUT_OF_HOST_MEMORY);
      CL_ERR_TO_STR(CL_PROFILING_INFO_NOT_AVAILABLE);
      CL_ERR_TO_STR(CL_MEM_COPY_OVERLAP);
      CL_ERR_TO_STR(CL_IMAGE_FORMAT_MISMATCH);
      CL_ERR_TO_STR(CL_IMAGE_FORMAT_NOT_SUPPORTED);
      CL_ERR_TO_STR(CL_BUILD_PROGRAM_FAILURE);
      CL_ERR_TO_STR(CL_MAP_FAILURE);
      CL_ERR_TO_STR(CL_MISALIGNED_SUB_BUFFER_OFFSET);
      CL_ERR_TO_STR(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
      CL_ERR_TO_STR(CL_COMPILE_PROGRAM_FAILURE);
      CL_ERR_TO_STR(CL_LINKER_NOT_AVAILABLE);
      CL_ERR_TO_STR(CL_LINK_PROGRAM_FAILURE);
      CL_ERR_TO_STR(CL_DEVICE_PARTITION_FAILED);
      CL_ERR_TO_STR(CL_KERNEL_ARG_INFO_NOT_AVAILABLE);
      CL_ERR_TO_STR(CL_INVALID_VALUE);
      CL_ERR_TO_STR(CL_INVALID_DEVICE_TYPE);
      CL_ERR_TO_STR(CL_INVALID_PLATFORM);
      CL_ERR_TO_STR(CL_INVALID_DEVICE);
      CL_ERR_TO_STR(CL_INVALID_CONTEXT);
      CL_ERR_TO_STR(CL_INVALID_QUEUE_PROPERTIES);
      CL_ERR_TO_STR(CL_INVALID_COMMAND_QUEUE);
      CL_ERR_TO_STR(CL_INVALID_HOST_PTR);
      CL_ERR_TO_STR(CL_INVALID_MEM_OBJECT);
      CL_ERR_TO_STR(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
      CL_ERR_TO_STR(CL_INVALID_IMAGE_SIZE);
      CL_ERR_TO_STR(CL_INVALID_SAMPLER);
      CL_ERR_TO_STR(CL_INVALID_BINARY);
      CL_ERR_TO_STR(CL_INVALID_BUILD_OPTIONS);
      CL_ERR_TO_STR(CL_INVALID_PROGRAM);
      CL_ERR_TO_STR(CL_INVALID_PROGRAM_EXECUTABLE);
      CL_ERR_TO_STR(CL_INVALID_KERNEL_NAME);
      CL_ERR_TO_STR(CL_INVALID_KERNEL_DEFINITION);
      CL_ERR_TO_STR(CL_INVALID_KERNEL);
      CL_ERR_TO_STR(CL_INVALID_ARG_INDEX);
      CL_ERR_TO_STR(CL_INVALID_ARG_VALUE);
      CL_ERR_TO_STR(CL_INVALID_ARG_SIZE);
      CL_ERR_TO_STR(CL_INVALID_KERNEL_ARGS);
      CL_ERR_TO_STR(CL_INVALID_WORK_DIMENSION);
      CL_ERR_TO_STR(CL_INVALID_WORK_GROUP_SIZE);
      CL_ERR_TO_STR(CL_INVALID_WORK_ITEM_SIZE);
      CL_ERR_TO_STR(CL_INVALID_GLOBAL_OFFSET);
      CL_ERR_TO_STR(CL_INVALID_EVENT_WAIT_LIST);
      CL_ERR_TO_STR(CL_INVALID_EVENT);
      CL_ERR_TO_STR(CL_INVALID_OPERATION);
      CL_ERR_TO_STR(CL_INVALID_GL_OBJECT);
      CL_ERR_TO_STR(CL_INVALID_BUFFER_SIZE);
      CL_ERR_TO_STR(CL_INVALID_MIP_LEVEL);
      CL_ERR_TO_STR(CL_INVALID_GLOBAL_WORK_SIZE);
      CL_ERR_TO_STR(CL_INVALID_PROPERTY);
      CL_ERR_TO_STR(CL_INVALID_IMAGE_DESCRIPTOR);
      CL_ERR_TO_STR(CL_INVALID_COMPILER_OPTIONS);
      CL_ERR_TO_STR(CL_INVALID_LINKER_OPTIONS);
      CL_ERR_TO_STR(CL_INVALID_DEVICE_PARTITION_COUNT);
//      CL_ERR_TO_STR(CL_INVALID_PIPE_SIZE);
//      CL_ERR_TO_STR(CL_INVALID_DEVICE_QUEUE);

    default:
      return "UNKNOWN ERROR CODE";
    }
}

#undef CL_ERR_TO_STR

#endif /* KERNEL_RUNNER_OPENCL_UGLY_ERROR_HANDLING_HPP_ */
