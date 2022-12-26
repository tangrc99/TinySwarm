//
// Created by 唐仁初 on 2022/9/5.
//

#ifndef TINYSWARM_RESULT_H
#define TINYSWARM_RESULT_H

#include <string>
#include <utility>

namespace manager {

    struct PodDescriptor;

    /// Class CreateResult describes a result of pod creation.
    class CreateResult {
    public:

        /// Is result failed.
        /// \return Is result failed
        [[nodiscard]] bool isFail() const {
            return fail_;
        }

        /// Get failed reason
        /// \return Failed reason
        std::string reason() {
            return reason_;
        }

        /// If succeed, get pod descriptor.
        /// \return Created pod descriptor
        PodDescriptor *podDescriptor() {
            return pod_;
        }

        /// Constructor describes a successful create process.
        /// \param pod Created pod descriptor
        explicit CreateResult(PodDescriptor *pod) : fail_(false), pod_(pod) {}

        /// Constructor describes a failed create process.
        /// \param reason Failed reason
        explicit CreateResult(std::string reason) : fail_(true), reason_(std::move(reason)), pod_(nullptr) {}

    private:
        bool fail_;
        std::string reason_;
        PodDescriptor *pod_;
    };
}

#endif //TINYSWARM_RESULT_H
