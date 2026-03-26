#include <string>
#include <algorithm>
#include "http/curl/curl_http_client.h"
#include "http/http_request.h"
#include "utils.h"
#include "outcome.h"
#include "resource_wrappers.h"

namespace aliyun
{
namespace datahub
{
namespace http
{

#if 0
static void dump(const char *text,
        FILE *stream, unsigned char *ptr, size_t size)
{
    size_t i;
    size_t c;
    unsigned int width=0x10;

    fprintf(stream, "%s, %10.10ld bytes (0x%8.8lx)\n",
            text, (long)size, (long)size);

    for(i=0; i<size; i+= width)
    {
        fprintf(stream, "%4.4lx: ", (long)i);

        /* show hex to the left */
        for(c = 0; c < width; c++)
        {
            if(i+c < size)
                fprintf(stream, "%02x ", ptr[i+c]);
            else
                fputs("   ", stream);
        }

        /* show data on the right */
        for(c = 0; (c < width) && (i+c < size); c++)
        {
            char x = (ptr[i+c] >= 0x20 && ptr[i+c] < 0x80) ? ptr[i+c] : '.';
            fputc(x, stream);
        }

        fputc('\n', stream); /* newline */
    }
}

static int my_trace(CURL *handle, curl_infotype type,
        char *data, size_t size, void *userp)
{
    const char *text;
    (void)handle; /* prevent compiler warning */

    switch (type)
    {
    case CURLINFO_TEXT:
        fprintf(stderr, "== Info: %s", data);
    default: /* in case a new one is introduced to shock us */
        return 0;

    case CURLINFO_HEADER_OUT:
        text = "=> Send header";
        break;
    case CURLINFO_DATA_OUT:
        text = "=> Send data";
        break;
    case CURLINFO_SSL_DATA_OUT:
        text = "=> Send SSL data";
        break;
    case CURLINFO_HEADER_IN:
        text = "<= Recv header";
        break;
    case CURLINFO_DATA_IN:
        text = "<= Recv data";
        break;
    case CURLINFO_SSL_DATA_IN:
        text = "<= Recv SSL data";
        break;
    }

    dump(text, stderr, (unsigned char *)data, size);
    return 0;
}
#endif

struct CurlWriteCallbackContext
{
    CurlWriteCallbackContext(HttpResponse* response) :
        m_response(response)
    {}

    HttpResponse* m_response;
};

struct CurlReadCallbackContext
{
    CurlReadCallbackContext(HttpRequest* request) :
        mCurrentPos(0),
        m_request(request)
    {}

    size_t mCurrentPos;
    HttpRequest* m_request;
};

void SetOptCodeForHttpMethod(CURL* requestHandle, const HttpRequest& request)
{
    switch (request.GetMethod())
    {
        case HTTP_GET:
            curl_easy_setopt(requestHandle, CURLOPT_HTTPGET, 1L);
            break;
        case HTTP_POST:

            if (!request.HasHeader(http::CONTENT_LENGTH_HEADER))
            {
                curl_easy_setopt(requestHandle, CURLOPT_CUSTOMREQUEST, "POST");
            }
            else
            {
                curl_easy_setopt(requestHandle, CURLOPT_POST, 1L);
            }
            break;
        case HTTP_PUT:
            if (!request.HasHeader(http::CONTENT_LENGTH_HEADER))
            {
                curl_easy_setopt(requestHandle, CURLOPT_CUSTOMREQUEST, "PUT");
            }
            else
            {
                curl_easy_setopt(requestHandle, CURLOPT_PUT, 1L);
                // fix Transfer-Encoding:chunked in request headers, body is empty
                // see:https://stackoverflow.com/questions/7569826/send-string-in-put-request-with-libcurl
                // fix:https://curl.haxx.se/libcurl/c/httpput.html
                curl_easy_setopt(requestHandle, CURLOPT_UPLOAD, 1L);
                curl_easy_setopt(requestHandle, CURLOPT_INFILESIZE,
                        (curl_off_t)std::stol(request.GetContentLength()));
            }
            break;
        case HTTP_HEAD:
            curl_easy_setopt(requestHandle, CURLOPT_HTTPGET, 1L);
            curl_easy_setopt(requestHandle, CURLOPT_NOBODY, 1L);
            break;
        case HTTP_PATCH:
            curl_easy_setopt(requestHandle, CURLOPT_CUSTOMREQUEST, "PATCH");
            break;
        case HTTP_DELETE:
            curl_easy_setopt(requestHandle, CURLOPT_CUSTOMREQUEST, "DELETE");
            curl_easy_setopt(requestHandle, CURLOPT_NOBODY, 1L);
            break;
        default:
            curl_easy_setopt(requestHandle, CURLOPT_CUSTOMREQUEST, "GET");
            break;
    }
}

CurlHttpClient::CurlHttpClient(unsigned maxPoolSize, long requestTimeout, long connectTimeout) :
    mCurlHandleContainer(maxPoolSize, requestTimeout, connectTimeout)
{
}


Outcome CurlHttpClient::MakeRequest(HttpRequest& request) const
{
    URI uri = request.GetUri();
    std::string url = uri.GetURIString();

    struct curl_slist* headers = NULL;

    const std::map<std::string, std::string>& requestHeaders = request.GetHeaders();

    std::map<std::string, std::string>::const_iterator iter;
    for (iter = requestHeaders.begin(); iter != requestHeaders.end(); ++iter)
    {
        std::string headerString;
        headerString.append(iter->first).append(": ").append(iter->second);
        headers = curl_slist_append(headers, headerString.c_str());
    }

    // [fix #15473949] https://aone.alibaba-inc.com/issue/15473949
    headers = curl_slist_append(headers, "Expect:");
    headers = curl_slist_append(headers, "Transfer-Encoding:");

    std::shared_ptr<HttpResponse> response;
    CurlHandleWrapperPtr handleWrapper = mCurlHandleContainer.AcquireCurlHandle();
    CURL* connectionHandle = handleWrapper->GetHandle();
    if (!connectionHandle)
    {
        return Outcome(false, "connectionHandle is invalid", response);
    }

    if (headers)
    {
        curl_easy_setopt(connectionHandle, CURLOPT_HTTPHEADER, headers);
    }

    response.reset(new HttpResponse());
    CurlWriteCallbackContext writeContext(response.get());
    CurlReadCallbackContext readContext(&request);

    SetOptCodeForHttpMethod(connectionHandle, request);

    curl_easy_setopt(connectionHandle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(connectionHandle, CURLOPT_WRITEFUNCTION, &CurlHttpClient::WriteData);
    curl_easy_setopt(connectionHandle, CURLOPT_WRITEDATA, &writeContext);
    curl_easy_setopt(connectionHandle, CURLOPT_HEADERFUNCTION, &CurlHttpClient::WriteHeader);
    curl_easy_setopt(connectionHandle, CURLOPT_HEADERDATA, response.get());
    curl_easy_setopt(connectionHandle, CURLOPT_TCP_NODELAY, 1L);

    curl_easy_setopt(connectionHandle, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(connectionHandle, CURLOPT_SSL_VERIFYHOST, 2L);

    //curl_easy_setopt(connectionHandle, CURLOPT_DEBUGFUNCTION, my_trace);
    //curl_easy_setopt(connectionHandle, CURLOPT_VERBOSE, 1);
    //curl_easy_setopt(connectionHandle, CURLOPT_FOLLOWLOCATION, 1L);

    if (!request.GetContentBody().empty())
    {
        curl_easy_setopt(connectionHandle, CURLOPT_READFUNCTION, &CurlHttpClient::ReadBody);
        curl_easy_setopt(connectionHandle, CURLOPT_READDATA, &readContext);
    }

    CURLcode curlResponseCode = curl_easy_perform(connectionHandle);
    curl_slist_free_all(headers);

    if (curlResponseCode != CURLE_OK)
    {
        response.reset();
        std::string ss;
        ss.append("curl code is ").append(std::to_string(curlResponseCode));
        return Outcome(false, ss, response);
    }

    long responseCode;
    curl_easy_getinfo(connectionHandle, CURLINFO_RESPONSE_CODE, &responseCode);
    response->SetResponseCode(static_cast<HttpResponseCode>(responseCode));

    return Outcome(response);
}


size_t CurlHttpClient::WriteData(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    if (ptr)
    {
        CurlWriteCallbackContext* context = reinterpret_cast<CurlWriteCallbackContext*>(userdata);

        HttpResponse* response = context->m_response;
        size_t sizeToWrite = size * nmemb;
        response->GetResponseBody()->append(ptr, sizeToWrite);

        return sizeToWrite;
    }
    return 0;
}

size_t CurlHttpClient::WriteHeader(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    if (ptr)
    {
        HttpResponse* response = (HttpResponse*) userdata;
        std::string headerLine(ptr, size * nmemb);
        const std::vector<std::string>& keyValuePair = Utils::Split(headerLine, ':');

        if (keyValuePair.size() > 1 && !keyValuePair[0].empty())
        {
            std::string value = keyValuePair[1];
            Utils::Trim(value);
            response->AddHeader(keyValuePair[0], value);
        }
        return size * nmemb;
    }
    return 0;
}

size_t CurlHttpClient::ReadBody(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    CurlReadCallbackContext* context = reinterpret_cast<CurlReadCallbackContext*>(userdata);
    if (context == NULL)
    {
	    return 0;
    }

    HttpRequest* request = context->m_request;
    size_t& currentPos = context->mCurrentPos;
    const std::string& body = request->GetContentBody();

    if (size * nmemb > 0)
    {
        size_t length = body.size();
        size_t amountToRead = std::min<size_t>(length - currentPos, size * nmemb);
        body.copy(ptr, amountToRead, currentPos);
        currentPos += amountToRead;
        return amountToRead;
    }

    return 0;
}

} // namespace http
} // namespace datahub
} // namespace aliyun
