//
// Created by 唐仁初 on 2022/9/2.
//

#ifndef TINYSWARM_SCHEDULER_H
#define TINYSWARM_SCHEDULER_H

#include "../WorkerDescriptor.h"

namespace manager::scheduler {

    /// Scheduler is the base class of scheduling module. Different type of scheduling strategies should derived from this
    /// class and override interface getBestWorker.
    class Scheduler {
    public:

        /// Constructor of base class. Class owns a reference of current worker list.
        /// \param workers worker list
        explicit Scheduler(const std::list<WorkerDescriptor *> &workers) : workers_(workers) {}

        virtual ~Scheduler() = default;

        /// Interface to get scheduled worker.
        /// \param pod Information of created pod
        /// \return Descriptor of selected worker
        virtual WorkerDescriptor *getBestWorker(PodDescriptor *pod) = 0;

    protected:
        const std::list<WorkerDescriptor *> &workers_;  // reference of manger's workers
    };


    /// RoundRobin Scheduling module impl.
    class RoundRobin final : public Scheduler {
    public:

        /// Construct a RoundRobin scheduler.
        /// \param workers Usable worker list
        explicit RoundRobin(const std::list<WorkerDescriptor *> &workers) : Scheduler(workers) {}

        /// Select a worker to create pod using round-robin strategy.
        /// \param pod Pod Descriptor
        /// \return Descriptor of selected worker
        [[nodiscard]] WorkerDescriptor *getBestWorker(PodDescriptor *pod) override {

            int i = last.fetch_add(1);
            i = i % static_cast<int>(workers_.size());

            for (auto it = workers_.begin(); it != workers_.end(); i++) {
                i--;
                if (i == 0 && (*it)->alive && (*it)->portAvailable(pod->port_))
                    return *it;
            }

            return nullptr;
        }

    private:
        std::atomic<int> last = 0;
    };
}

#endif //TINYSWARM_SCHEDULER_H
