/*
 * Copyright (c) 2022, Alibaba Group Holding Limited;
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
#ifndef FUTURE_SIMPLE_IO_EXECUTOR_H
#define FUTURE_SIMPLE_IO_EXECUTOR_H

#include <async_simple/IOExecutor.h>
#include <libaio.h>
#include <sys/syscall.h> /* For SYS_xxx definitions */
#include <unistd.h>
#include <thread>

namespace async_simple {

namespace executors {

// This is a demo IOExecutor.
//  submitIO and submitIOV should be implemented.
class SimpleIOExecutor : public IOExecutor {
public:
    static constexpr int kMaxAio = 8;

public:
    SimpleIOExecutor() {}
    virtual ~SimpleIOExecutor() {}

    SimpleIOExecutor(const IOExecutor&) = delete;
    SimpleIOExecutor& operator=(const IOExecutor&) = delete;

public:
    class Task {
    public:
        Task(AIOCallback& func) : _func(func) {}
        ~Task() {}

    public:
        void process(io_event& event) { _func(event); }

    private:
        AIOCallback _func;
    };

public:
    bool init() {
        auto r = io_setup(kMaxAio, &_ioContext);
        if (r < 0) {
            return false;
        }
        _loopThread = std::thread([this]() mutable { this->loop(); });
        return true;
    }

    void destroy() {
        _shutdown = true;
        if (_loopThread.joinable()) {
            _loopThread.join();
        }
        io_destroy(_ioContext);
    }

    void loop() {
        while (!_shutdown) {
            io_event events[kMaxAio];
            struct timespec timeout = {0, 1000 * 300};
            auto n = io_getevents(_ioContext, 1, kMaxAio, events, &timeout);
            if (n < 0) {
                continue;
            }
            for (auto i = 0; i < n; ++i) {
                auto task = reinterpret_cast<Task*>(events[i].data);
                task->process(events[i]);
                delete task;
            }
        }
    }

public:
    void submitIO(int fd, iocb_cmd cmd, void* buffer, size_t length,
                  off_t offset, AIOCallback cbfn) override {
        iocb io;
        memset(&io, 0, sizeof(iocb));
        io.aio_fildes = fd;
        io.aio_lio_opcode = cmd;
        io.u.c.buf = buffer;
        io.u.c.offset = offset;
        io.u.c.nbytes = length;
        io.data = new Task(cbfn);
        struct iocb* iocbs[] = {&io};
        auto r = io_submit(_ioContext, 1, iocbs);
        if (r < 0) {
            auto task = reinterpret_cast<Task*>(iocbs[0]->data);
            io_event event;
            event.res = r;
            task->process(event);
            delete task;
            return;
        }
    }
    void submitIOV(int fd, iocb_cmd cmd, const iovec* iov, size_t count,
                   off_t offset, AIOCallback cbfn) override {
        iocb io;
        memset(&io, 0, sizeof(iocb));
        io.aio_fildes = fd;
        io.aio_lio_opcode = cmd;
        io.u.c.buf = (void*)iov;
        io.u.c.offset = offset;
        io.u.c.nbytes = count;
        io.data = new Task(cbfn);
        struct iocb* iocbs[] = {&io};
        auto r = io_submit(_ioContext, 1, iocbs);
        if (r < 0) {
            auto task = reinterpret_cast<Task*>(iocbs[0]->data);
            io_event event;
            event.res = r;
            task->process(event);
            delete task;
            return;
        }
    }

private:
    volatile bool _shutdown = false;
    io_context_t _ioContext = 0;
    std::thread _loopThread;
};

}  // namespace executors

}  // namespace async_simple

#endif  // FUTURE_SIMPLE_IO_EXECUTOR_H
