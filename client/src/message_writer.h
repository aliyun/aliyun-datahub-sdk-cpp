#ifndef INCLUDE_DATAHUB_CLIENT_MESSAGE_WRITER_H
#define INCLUDE_DATAHUB_CLIENT_MESSAGE_WRITER_H

#include <memory>
#include <functional>


namespace aliyun
{
namespace datahub
{

class HashThreadPool;
typedef std::shared_ptr<HashThreadPool> HashThreadPoolPtr;

class MessageWriter
{
public:
    MessageWriter(int32_t threadNum, int32_t queueLimit);
    ~MessageWriter();

    bool SubmitTask(int32_t key, std::function<void()> func);

private:
    HashThreadPoolPtr mThreadPoolPtr;
};

typedef std::shared_ptr<MessageWriter> MessageWriterPtr;

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_MESSAGE_WRITER_H
