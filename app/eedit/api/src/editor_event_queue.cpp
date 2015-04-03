#include <queue>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "editor_event_queue.h"

namespace eedit
{

/* FIXME:
 * implement like a ring buffer with a gap
 * do the slots clearing when we push ...
 * this will avoid locks in get(...)
 */
template <class T>
class event_queue
{
public:

    size_t size() const
    {
        return m_queue.size();
    }

    inline bool push(T msg)
    {
        std::lock_guard<std::mutex> lock(m_queue_mtx);
        m_queue.push(msg);
        m_queue_cond_var.notify_one();
        return true;
    }

    inline size_t wait(size_t wait_time)
    {
        if (m_queue.size())
            return m_queue.size();

        std::unique_lock<std::mutex> lk(m_queue_mtx);
        auto now = std::chrono::system_clock::now();
        m_queue_cond_var.wait_until(lk, now + std::chrono::milliseconds(wait_time));
        return m_queue.size();
    }

    inline T get()
    {
        // FIXME: race condition here if multiple get
        if (m_queue.empty()) {
            return nullptr;
        }

        std::lock_guard<std::mutex> lock(m_queue_mtx);
        T msg = m_queue.front();
        m_queue.pop();

        return msg;
    }

private:
    std::mutex              m_queue_mtx;
    std::queue<T>           m_queue;
    std::condition_variable m_queue_cond_var;
};

////////////////////////////////////////////////////////////////////////////////


}


#ifdef __cplusplus
extern "C" {
#endif


struct editor_event_queue_s * editor_event_queue_new(void)
{
    struct editor_event_queue_s * q = (struct editor_event_queue_s *)new eedit::event_queue<void *>();
    return q;
}

void editor_event_queue_delete(struct editor_event_queue_s * q_)
{
    eedit::event_queue<void *> * q = (eedit::event_queue<void *> *)q_;
    delete q;
}

size_t editor_event_queue_size(struct editor_event_queue_s * q_)
{
    eedit::event_queue<void *> * q = (eedit::event_queue<void *> *)q_;

    return q->size();
}

bool editor_event_queue_push(struct editor_event_queue_s * q_, struct editor_event_s *ev)
{
    if (!q_)
        return false;

    eedit::event_queue<void *> * q = (eedit::event_queue<void *> *)q_;
    return q->push(ev);
}


size_t editor_event_queue_wait(struct editor_event_queue_s * q_, size_t wait_time)
{
    eedit::event_queue<void *> * q = (eedit::event_queue<void *> *)q_;
    return q->wait(wait_time);
}

struct editor_event_s * editor_event_queue_get(struct editor_event_queue_s * q_)
{
    eedit::event_queue<void *> * q = (eedit::event_queue<void *> *)q_;
    return (struct editor_event_s *)q->get();
}


#ifdef __cplusplus
}
#endif
