#include "message_writer.h"
#include "thread_pool.h"
#include "datahub/datahub_exception.h"

namespace aliyun
{
namespace datahub
{

MessageWriter::MessageWriter(int32_t threadNum, int32_t queueLimit)
{
    mThreadPoolPtr = std::make_shared<HashThreadPool>(threadNum, queueLimit, "MessageWriter");
    mThreadPoolPtr->Start();
}

MessageWriter::~MessageWriter()
{
    mThreadPoolPtr->Shutdown();
}

bool MessageWriter::SubmitTask(int32_t key, std::function<void()> func)
{
    return mThreadPoolPtr->Submit(key, func);
}

} // namespace datahub
} // namespace aliyun
