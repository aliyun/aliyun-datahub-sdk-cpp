#ifndef INCLUDE_DATAHUB_CLIENT_MESSAGE_READER_H
#define INCLUDE_DATAHUB_CLIENT_MESSAGE_READER_H

#include <memory>
#include <functional>


namespace aliyun
{
namespace datahub
{

class ThreadPool;
typedef std::shared_ptr<ThreadPool> ThreadPoolPtr;

class MessageReader
{
public:
    MessageReader(int32_t threadNum, int32_t queueLimit);
    ~MessageReader();

    bool SubmitTask(std::function<void()> func);

private:
    ThreadPoolPtr mThreadPoolPtr;
};

typedef std::shared_ptr<MessageReader> MessageReaderPtr;

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_MESSAGE_READER_H
