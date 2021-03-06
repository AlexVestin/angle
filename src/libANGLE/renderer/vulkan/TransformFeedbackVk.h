//
// Copyright 2016 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// TransformFeedbackVk.h:
//    Defines the class interface for TransformFeedbackVk, implementing TransformFeedbackImpl.
//

#ifndef LIBANGLE_RENDERER_VULKAN_TRANSFORMFEEDBACKVK_H_
#define LIBANGLE_RENDERER_VULKAN_TRANSFORMFEEDBACKVK_H_

#include "libANGLE/renderer/TransformFeedbackImpl.h"
#include "libANGLE/renderer/vulkan/vk_helpers.h"

namespace gl
{
class ProgramState;
}  // namespace gl

namespace rx
{
namespace vk
{
class DescriptorSetLayoutDesc;
}

// Cached buffer properties for faster descriptor set update and offset calculation.
struct TransformFeedbackBufferRange
{
    // Offset as provided by OffsetBindingPointer.
    gl::TransformFeedbackBuffersArray<VkDeviceSize> offsets;
    // Size as provided by OffsetBindingPointer.
    gl::TransformFeedbackBuffersArray<VkDeviceSize> sizes;
    // Aligned offset usable for VkDescriptorBufferInfo.  This value could be smaller than
    // offset.
    gl::TransformFeedbackBuffersArray<VkDeviceSize> alignedOffsets;
};

class TransformFeedbackVk : public TransformFeedbackImpl
{
  public:
    TransformFeedbackVk(const gl::TransformFeedbackState &state);
    ~TransformFeedbackVk() override;
    void onDestroy(const gl::Context *context) override;

    angle::Result begin(const gl::Context *context, gl::PrimitiveMode primitiveMode) override;
    angle::Result end(const gl::Context *context) override;
    angle::Result pause(const gl::Context *context) override;
    angle::Result resume(const gl::Context *context) override;

    angle::Result bindIndexedBuffer(const gl::Context *context,
                                    size_t index,
                                    const gl::OffsetBindingPointer<gl::Buffer> &binding) override;

    void updateDescriptorSetLayout(ContextVk *contextVk,
                                   const gl::ProgramState &programState,
                                   vk::DescriptorSetLayoutDesc *descSetLayoutOut) const;
    void initDescriptorSet(ContextVk *contextVk,
                           size_t xfbBufferCount,
                           vk::BufferHelper *emptyBuffer,
                           VkDescriptorSet descSet) const;
    void updateDescriptorSet(ContextVk *contextVk,
                             const gl::ProgramState &programState,
                             VkDescriptorSet descSet) const;
    void getBufferOffsets(ContextVk *contextVk,
                          const gl::ProgramState &programState,
                          GLint drawCallFirstVertex,
                          int32_t *offsetsOut,
                          size_t offsetsSize) const;

    void unsetTransformFeedbackBufferRebindState() { mRebindTransformFeedbackBuffer = false; }

    bool getTransformFeedbackBufferRebindState() const { return mRebindTransformFeedbackBuffer; }

    const TransformFeedbackBufferRange &getTransformFeedbackBufferRange() const
    {
        return mTransformFeedbackBufferRange;
    }

    const gl::TransformFeedbackBuffersArray<VkBuffer> &getCounterBufferHandles() const
    {
        return mCounterBufferHandles;
    }

  private:
    angle::Result onTransformFeedbackStateChanged(ContextVk *contextVk);
    void writeDescriptorSet(ContextVk *contextVk,
                            size_t xfbBufferCount,
                            VkDescriptorBufferInfo *pBufferInfo,
                            VkDescriptorSet descSet) const;

    // This member variable is set when glBindTransformFeedbackBuffers/glBeginTransformFeedback
    // is called and unset in dirty bit handler for transform feedback state change. If this
    // value is true, vertex shader will record transform feedback varyings from the beginning
    // of the buffer.
    bool mRebindTransformFeedbackBuffer;
    TransformFeedbackBufferRange mTransformFeedbackBufferRange;
    // Counter buffer used for pause and resume.
    gl::TransformFeedbackBuffersArray<vk::BufferHelper> mCounterBuffer;
    gl::TransformFeedbackBuffersArray<VkBuffer> mCounterBufferHandles;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_VULKAN_TRANSFORMFEEDBACKVK_H_
