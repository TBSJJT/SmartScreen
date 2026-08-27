#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// 定义HTTP响应数据结构
typedef struct {
    char *data;  // 存储响应数据的缓冲区
    size_t size; // 响应数据的实际大小
} http_resp_data_t;

/**
 * @brief 组装HTTP请求URL
 * @param host 服务器主机地址（如"http://example.com"）
 * @param path 资源路径（如"/api/data"）
 * @param out_url 输出参数：用于存储拼接后的URL（需调用者手动释放）
 * @return 0: 成功；-1: 内存分配失败；-2: 输入参数无效
 */
static int assemble_url(const char *host, const char *path, char **out_url)
{
    //总长度：host长度 + path长度 + 1（字符串结束符'\0'）
    *out_url = malloc(strlen(host) + strlen(path) + 1);
    strcpy(*out_url, host); // 复制host到缓冲区
    strcat(*out_url, path); // 追加path到缓冲区
    return 0;
}

/**
 * @brief CURL数据接收回调函数
 * @param data 本次接收的数据
 * @param size 单个数据单元大小（通常为1）
 * @param nmemb 数据单元数量
 * @param userp 指向http_resp_data_t的指针，用于存储数据
 * @return 实际处理的字节数，0表示失败
 */
static size_t write_callback(void *data, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    http_resp_data_t *mem = (http_resp_data_t *)userp;

    // 计算新缓冲区大小并重新分配内存
    // 为什么不用malloc？因为write_callback回调函数需要持续接收并拼接多批次的数据，realloc能在保留已有数据的基础上扩容，malloc则做不到。
    char *ptr = realloc(mem->data, mem->size + realsize + 1);
    if (!ptr) return 0; // 内存分配失败
    // 存储新接收的数据
    mem->data = ptr;
    memcpy(mem->data + mem->size, data, realsize);
    mem->size += realsize;
    mem->data[mem->size] = '\0';
    return realsize;
}

/**
 * @brief 发送HTTP请求
 * @param host 服务器主机地址（如"http://example.com"）
 * @param path 资源路径（如"/api/data"）
 * @param method 请求方法（GET/POST等）
 * @param request_json POST请求时的提交数据（NULL表示无）
 * @param[out] response_json 输出参数：用于返回响应数据（需调用者手动释放）
 */
void http_request_method(const char *host, const char *path, const char *method, const char *request_json, char **response_json)
{
    printf("Request: %s%s, Method: %s\n", host, path, method);
    //创建一个传输会话的句柄
    CURL *curl = curl_easy_init();
    // 组装拼接url
    char *url = NULL;
    assemble_url(host, path, &url);
    // 设置URL
    curl_easy_setopt(curl, CURLOPT_URL, url);
    // 通用配置
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);           // 调试模式：启用详细输出模式
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);          // 设置请求超时时间（单位：秒），20L表示超过20秒无响应则终止请求
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);    // 跳过SSL证书验证（0L表示关闭），跳过对服务器SSL证书的有效性检查
    // 设置响应处理
    http_resp_data_t response_data = {0};
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);  // 注册响应数据接收回调函数
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);      // 指定回调函数的用户数据
    // POST方法设置
    if (strcmp(method, "POST") == 0)
    {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_json);
    }
    // 执行请求
    CURLcode code = curl_easy_perform(curl);
    // 处理响应
    if (code == CURLE_OK)
    {
        // printf("Response len: %ld, data: %s\n", response_data.size, response_data.data);
        *response_json = response_data.data; // 转移内存所有权
    }
    else
    {
        printf("Request failed: %s (%d)\n", curl_easy_strerror(code), code);
        free(response_data.data); // 失败时释放内存
    }
    // 资源清理
    free(url);
    curl_easy_cleanup(curl);
}

// 测试http get请求
void test_http_get_time(){
    //初始化底层库和依赖（如网络、SSL）
    curl_global_init(CURL_GLOBAL_DEFAULT);
    char *response;
    http_request_method("https://tptm.hd.mi.com/","gettimestamp", "GET", "",  &response);
    if (response != NULL)
    {
        printf("%s\n",response);
        free(response); // 记得释放内存！
    }
    // 清理libcurl
    curl_global_cleanup();
}

// 测试http get请求
void test_http_get_weather(){
    //libcurl初始化
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    //用户KEY，从心知天气网站注册后，管理后台获取自己的KEY替换
    char weather_key[] = "S8uW9BlypsENoMKBL";
    //城市名称
    char weather_city[] = "Yangling";
    //构建请求内容
    char get_path[100];
    //api说明参考：天气实况 · 心知科技
    snprintf(get_path, sizeof(get_path), "/v3/weather/now.json?key=%s&location=%s&language=zh-Hans&unit=c", weather_key,weather_city);
    
    //发送请求
    char *response;
    http_request_method("https://api.seniverse.com",get_path, "GET", "",  &response);
    if (response != NULL)
    {
        printf("response ---> %s\n",response);
        free(response); // 记得释放内存！
    }
    
    // 清理libcurl
    curl_global_cleanup();
}

