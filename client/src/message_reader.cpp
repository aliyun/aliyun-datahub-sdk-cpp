#include "message_reader.h"
#include "thread_pool.h"
#include "datahub/datahub_exception.h"

namespace aliyun
{
namespace datahub
{

MessageReader::MessageReader(int32_t threadNum, int32_t queueLimit)
{
    mThreadPoolPtr = std::make_shared<ThreadPool>(threadNum, queueLimit, "MessageReader");
    mThreadPoolPtr->Start();
}

MessageReader::~MessageReader()
{
    mThreadPoolPtr->Shutdown();
}

bool MessageReader::SubmitTask(std::function<void()> func)
{
    return mThreadPoolPtr->Submit(func);
}

} // namespace datahub
} // namespace aliyun
