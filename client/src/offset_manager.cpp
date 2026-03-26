#include "offset_manager.h"
#include "log4cpp/Category.hh"
#include "util.h"
#include "timer.h"
#include "logger.h"
#include "constant.h"
#include "datahub_client_factory.h"
#include "datahub/datahub_client.h"
#include "datahub/datahub_exception.h"
#include <functional>

namespace aliyun
{
namespace datahub
{

OffsetManager::OffsetManager(const std::string& projectName, const std::string& topicName, const std::string& subId, const Configuration& conf)
    : mClosed(false), mProjectName(projectName), mTopicName(topicName), mSubId(subId)
{
    mLoggerPtr = Logger::GetInstance().GenLogger("commit");
    GenUtil::GenKey(mUniqKey, {mProjectName, mTopicName, mSubId});
    mTimer.reset(new Timer(OFFSET_COMMIT_TIMEOUT));
    mClientPtr = DatahubClientFactory::GetInstance().GetDatahubClient(conf);
    Start();
}

OffsetManager::~OffsetManager()
{
    mClosed = true;
    mTimer->NotifyAll();
    mLastOffsetMap.clear();
    if (mCommitThread.joinable())
    {
        mCommitThread.join();
    }
    LOG_INFO(mLoggerPtr, "Close OffsetManager success. key: %s", mUniqKey.c_str());
}

void OffsetManager::Start()
{
    std::function<void()> func = std::bind(&OffsetManager::CommitOffsetTask, this);
    mCommitThread = std::thread(func);
}

void OffsetManager::UpdateOffsetMeta(const std::string& shardId, int64_t version, int64_t sessionId)
{
    std::unique_lock<std::mutex> lock(mMutex);
    if (mOffsetMetaMap.count(shardId) == 0)
    {
        mOffsetMetaMap[shardId] = OffsetMeta(version, sessionId);
    }
    else
    {
        auto& tmp = mOffsetMetaMap.at(shardId);
        tmp.SetVersion(version);
        tmp.SetSessionId(sessionId);
    }
    mOffsetRequestMap[shardId] = std::queue<MessageKeyPtr>();
    LOG_INFO(mLoggerPtr, "UpdateOffsetMeta success. shardId: %s, version: %ld, sessionId: %ld", shardId.c_str(), version, sessionId);
}

void OffsetManager::SendOffsetRequest(MessageKeyPtr messageKeyPtr)
{
    std::unique_lock<std::mutex> lock(mMutex);
    const auto& shardId = messageKeyPtr->GetShardId();
    if (mOffsetRequestMap.count(shardId) > 0)       // 如果OffsetRequestMap中没有该shardId,说明在中间态,当前shard在释放中
    {
        mOffsetRequestMap.at(shardId).push(messageKeyPtr);
    }
}

void OffsetManager::OnShardRealese(const StringVec& shardIds)
{
    ForceCommitOffset(shardIds);
    std::unique_lock<std::mutex> lock(mMutex);
    for (auto it = shardIds.begin(); it != shardIds.end(); it++)
    {
        mLastOffsetMap.erase(*it);
        mOffsetRequestMap.erase(*it);
        mOffsetMetaMap.erase(*it);
    }
    LOG_INFO(mLoggerPtr, "On shardRealese. shardIds: %s", PrintUtil::GetMsg(shardIds).c_str());
}

void OffsetManager::OnOffsetReset()
{
    std::unique_lock<std::mutex> lock(mMutex);
    mLastOffsetMap.clear();
    mOffsetRequestMap.clear();
    mOffsetMetaMap.clear();
}

void OffsetManager::CommitOffsetTask()
{
    LOG_INFO(mLoggerPtr, "OffsetManager start. key: %s", mUniqKey.c_str());
    while (!mClosed)
    {
        if (mTimer->IsExpired())
        {
            std::unique_lock<std::mutex> lock(mMutex);
            SyncOffset();
            CommitOffset();
            mTimer->Reset();
        }
        else
        {
            try
            {
                mTimer->WaitExpire(OFFSET_CHECK_TIMEOUT);
            }
            catch (const std::exception& e)
            {
                LOG_WARN(mLoggerPtr, "Offset Manager wait fail. key: %s, %s", mUniqKey.c_str(), e.what());
            }
        }
    }
    {
        std::unique_lock<std::mutex> lock(mMutex);
        SyncOffset();
        CommitOffset();
    }
    LOG_INFO(mLoggerPtr, "OffsetManager stop. key: %s", mUniqKey.c_str());
}

void OffsetManager::SyncOffset()
{
    try
    {
        for (auto it = mOffsetRequestMap.begin(); it != mOffsetRequestMap.end(); it++)
        {
            std::queue<MessageKeyPtr>& requestQueue = it->second;
            MessageKeyPtr oldesetMessageKey = nullptr;
            while (!requestQueue.empty() && requestQueue.front()->IsReady())
            {
                oldesetMessageKey = requestQueue.front();
                requestQueue.pop();
            }

            if (oldesetMessageKey != nullptr && !oldesetMessageKey->GetShardId().empty())
            {
                const auto& offset = oldesetMessageKey->GetOffset();
                mLastOffsetMap[it->first] = SubscriptionOffset(offset.GetTimestamp(), offset.GetSequence(), offset.GetBatchIndex());
            }
        }
    }
    catch (const std::exception& e)
    {
        LOG_WARN(mLoggerPtr, "Sync offset fail. key: %s, %s", mUniqKey.c_str(), e.what());
    }
    LOG_DEBUG(mLoggerPtr, "Sync offset success. key: %s, minOffset: %ld", mUniqKey.c_str(), GetMinTimestamp());
}

void OffsetManager::CommitOffset()
{
    if (!mLastOffsetMap.empty())
    {
        uint32_t retryNum = 0;
        while (true)
        {
            try
            {
                for (auto it = mLastOffsetMap.begin(); it != mLastOffsetMap.end(); it++)
                {
                    const auto& offsetMapIt = mOffsetMetaMap.find(it->first);
                    if (offsetMapIt != mOffsetMetaMap.end() || offsetMapIt->second.GetVersion() != -1l || offsetMapIt->second.GetSessionId() != -1l)
                    {
                        it->second.SetVersion(offsetMapIt->second.GetVersion());
                        it->second.SetSessionId(offsetMapIt->second.GetSessionId());
                    }
                    else
                    {
                        LOG_WARN(mLoggerPtr, "OffsetMeta not found OR Version or sessionId error. key: %s, shardId: %s, version: %ld, sessionId: %ld",
                                mUniqKey.c_str(), it->first.c_str(), offsetMapIt->second.GetVersion(), offsetMapIt->second.GetSessionId());
                        throw DatahubException(LOCAL_ERROR_CODE, "OffsetMeta not found OR Version or sessionId error");
                    }
                }

                mClientPtr->UpdateSubscriptionOffset(mProjectName, mTopicName, mSubId, mLastOffsetMap);
                LOG_INFO(mLoggerPtr, "Commit offset once success. key: %s, minOffset: %ld", mUniqKey.c_str(), GetMinTimestamp());
                mLastOffsetMap.clear();
                return ;
            }
            catch (const DatahubException& e)
            {
                LOG_WARN(mLoggerPtr, "Commit offset fail. key: %s, minOffset: %ld, DatahubException: %s", mUniqKey.c_str(), GetMinTimestamp(), e.GetErrorMessage().c_str());
                if (++retryNum >= MAX_COMMIT_OFFSET_RETRY_TIMES)
                {
                    throw ;
                }
            }
            catch (const std::exception& e)
            {
                LOG_WARN(mLoggerPtr, "Commit offset fail. key: %s, minOffset: %ld, %s", mUniqKey.c_str(), GetMinTimestamp(), e.what());
                throw;
            }
        }
    }
}

void OffsetManager::ForceCommitOffset(const StringVec& shardIds)
{
    try
    {
        Timer waitTimer(FORCE_COMMIT_TIMEOUT_MS);
        CommitOnceRightNow();
        while (!waitTimer.IsExpired() && !CheckRequestQueueIsEmpty(shardIds))
        {
            CommitOnceRightNow();
            waitTimer.WaitExpire(10);
        }
    }
    catch(const std::exception& e)
    {
        LOG_WARN(mLoggerPtr, "Force commit offset fail. key: %s, %s", mUniqKey.c_str(), e.what());
    }
}

bool OffsetManager::CheckRequestQueueIsEmpty(const StringVec& shardIds)
{
    std::unique_lock<std::mutex> lock(mMutex);
    if (!mLastOffsetMap.empty())
    {
        return false;
    }
    for (auto it = shardIds.begin(); it != shardIds.end(); it++)
    {
        if (mOffsetRequestMap.count(*it) > 0 && !mOffsetRequestMap.at(*it).empty())
        {
            return false;
        }
    }
    return true;
}

void OffsetManager::CommitOnceRightNow()
{
    mTimer->ResetDeadline();
    mTimer->NotifyAll();
}

int64_t OffsetManager::GetMinTimestamp()
{
    int64_t minTimestamp = INT64_MAX;
    for (auto it = mLastOffsetMap.begin(); it != mLastOffsetMap.end(); it++)
    {
        minTimestamp = std::min(minTimestamp, it->second.GetTimestamp());
    }
    return minTimestamp == INT64_MAX ? -1l : minTimestamp;
}

} // namespace datahub
} // namespace aliyun
